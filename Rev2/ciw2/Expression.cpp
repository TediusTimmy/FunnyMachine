 /*
   Copyright (C) 2014 Thomas DiModica <ricinwich@yahoo.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Expression.hpp"
#include "SymbolTable.hpp"

#include <iostream>

void DB_panic (const std::string &, size_t) __attribute__ ((__noreturn__));

void beltVal(short val)
 {
   if ((val > 0x7FF) || (val < -0x800))
    {
      if (0 != (val & 0xFFF))
       {
         std::cout << "        LDI " << (val & 0xFFF) << std::endl;
         std::cout << "        LHN 0, " << ((val >> 12) & 0xF) << std::endl;
       }
      else
       {
         std::cout << "        LHN 15, " << ((val >> 12) & 0xF) << std::endl;
       }
    }
   else
    {
      std::cout << "        LDI " << val << std::endl;
    }
 }

void beltVal(short val, const std::string& name)
 {
   if ((val > 0x7FF) || (val < -0x800))
    {
      if (0 != (val & 0xFFF))
       {
         std::cout << "        LDI " << (val & 0xFFF) << std::endl;
         std::cout << " @" << name << " LHN 0, " << ((val >> 12) & 0xF) << std::endl;
       }
      else
       {
         std::cout << " @" << name << " LHN 15, " << ((val >> 12) & 0xF) << std::endl;
       }
    }
   else
    {
      std::cout << " @" << name << " LDI " << val << std::endl;
    }
 }

void VS_pushAddr (short addr)
 {
   beltVal(addr);
   std::cout << "        LD  0" << std::endl;
   std::cout << "        SDO4 0" << std::endl;
 }

void VS_pushVal (short val)
 {
   if (0 != val)
    {
      beltVal(val);
      std::cout << "        SDO4 0" << std::endl;
    }
   else
    {
         // I don't like this, because, based on how the architecture is defined,
         // we are telling the SDO to store an invalid belt location.
         // However ... it's going to save us instructions.
      std::cout << "        SDO4 15" << std::endl;
    }
 }


void Expression::emitPush(const CallingContext& context, GlobalData& data) const
 {
   if (true == canEvaluate(context))
    {
      int result = evaluate(context);
      std::cout << "    ; Folded constant sub-expression to " << result << " : " << lineNo << std::endl;
      if (0 != result)
       {
         beltVal(result);
         std::cout << "        SDO4 0" << std::endl;
       }
      else
       {
            // See comment in VS_pushVal
         std::cout << "        SDO4 15" << std::endl;
       }
      return;
    }
   if (true == canBelt(context))
    {
      std::string result = emitBelt(context, data);
      std::cout << "        SDO4 " << result << std::endl;
    }
   else
    {
      emitStack(context, data);
    }
 }

std::string Expression::emitNext(const CallingContext& context, GlobalData& data) const
 {
   if (true == canEvaluate(context))
    {
      int value = evaluate(context);
      std::cout << "    ; Folded constant sub-expression to " << value << " : " << lineNo << std::endl;
      if (0 != value)
       {
         std::string result = data.getNextResult();
         beltVal(value, result);
         return result;
       }
      return "15";
    }
   return emitBelt(context, data);
 }

std::string Expression::emitResult(const CallingContext& context, GlobalData& data) const
 {
   if (true == canEvaluate(context))
    {
      int value = evaluate(context);
      std::cout << "    ; Folded constant sub-expression to " << value << " : " << lineNo << std::endl;
      if (0 != value)
       {
         std::string result = data.getNextResult();
         beltVal(value, result);
         return result;
       }
      return "15";
    }
   if (true == canBelt(context))
    {
      return emitBelt(context, data);
    }
   else
    {
      emitStack(context, data);
      std::string result = data.getNextResult();
      std::cout << " @" << result << " SDO5" << std::endl;
      return result;
    }
 }


void Constant::emitStack(const CallingContext&, GlobalData&) const
 {
   std::cout << "    ; Constant " << value << " : " << lineNo << std::endl;
   VS_pushVal(value);
 }

std::string Constant::emitBelt(const CallingContext&, GlobalData& data) const
 {
   std::cout << "    ; Constant " << value << " : " << lineNo << std::endl;
   std::string result = data.getNextResult();
   beltVal(value, result);
   return result;
 }


void Variable::emitStack(const CallingContext&, GlobalData&) const
 {
   std::cout << "    ; Variable " << referent << " : " << lineNo << std::endl;
   if (location < 128)
    {
      beltVal(location & ~1);
      std::cout << "        LDS 1" << std::endl;
      std::cout << "        ADD 0, 1" << std::endl;
      if (0 == (location & 1))
       {
         std::cout << "        LD  0" << std::endl;
       }
      std::cout << "        SDO4 0" << std::endl;
    }
   else
    {
      if (0 == (location & 1))
       {
         VS_pushAddr(location & ~1);
       }
      else
       {
         VS_pushVal(location & ~1);
       }
    }
 }

std::string Variable::emitBelt(const CallingContext&, GlobalData& data) const
 {
   std::cout << "    ; Variable " << referent << " : " << lineNo << std::endl;
   std::string result = data.getNextResult();
   if (location < 128)
    {
      beltVal(location & ~1);
      std::cout << "        LDS 1" << std::endl;
      if (0 == (location & 1))
       {
         std::cout << "        ADD 0, 1" << std::endl;
         std::cout << " @" << result << " LD  0" << std::endl;
       }
      else
       {
         std::cout << " @" << result << " ADD 0, 1" << std::endl;
       }
    }
   else
    {
      if (0 == (location & 1))
       {
         beltVal(location & ~1);
         std::cout << " @" << result << " LD  0" << std::endl;
       }
      else
       {
         beltVal(location & ~1, result);
       }
    }
   return result;
 }

int Variable::beltResults(const CallingContext&) const
 {
   return valueResults(location & ~1) + ((location < 128) ? 2 : 0) + ((0 == (location & 1)) ? 1 : 0);
 }

int Variable::evaluate(const CallingContext& context) const
 {
   return context.getValue(referent, lineNo);
 }

bool Variable::canEvaluate (const CallingContext&) const
 {
   if ((location >= 0xE000) && (location <= 0xFFFF))
    {
      return true;
    }
   return false;
 }


void OrOp::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; \\/ " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        OR  0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string OrOp::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; \\/ " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " OR  " << LHS << ", " << RHS << std::endl;
   return result;
 }

int OrOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) | rhs->evaluate(context);
 }


void AndOp::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; /\\ " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        AND 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string AndOp::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; /\\ " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " AND " << LHS << ", " << RHS << std::endl;
   return result;
 }

int AndOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) & rhs->evaluate(context);
 }


void XorOp::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; ? " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        XOR 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string XorOp::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; ? " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " XOR " << LHS << ", " << RHS << std::endl;
   return result;
 }

int XorOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) ^ rhs->evaluate(context);
 }


void ShortOr::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   std::cout << "    ; || first arg " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::string dest = data.getNextLabel() + "_or_else";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        RETC 1, 0" << std::endl;
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @nsp   ADD sp, two" << std::endl;
   std::cout << "        STS nsp, 0" << std::endl;
   rhs->emitPush(context, data);
   std::cout << dest << ":" << std::endl;
 }

std::string ShortOr::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::cout << "    ; || first arg " << lineNo << std::endl;
   std::string dest = data.getNextLabel() + "_or_else";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        ADD 15, " << LHS << std::endl;
   std::cout << "        RETC 0, 1" << std::endl;
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "        ADD 15, " << RHS << std::endl;
   std::string result = data.getNextResult();
   std::cout << dest << ":\n @" << result << " ADD 0, 15" << std::endl;
   return result;
 }

int ShortOr::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 5;
 }

int ShortOr::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) || (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


void ShortAnd::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   std::cout << "    ; && first arg " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::string dest = data.getNextLabel() + "_and_then";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        RETZ 1, 0" << std::endl;
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @nsp   ADD sp, two" << std::endl;
   std::cout << "        STS nsp, 0" << std::endl;
   rhs->emitPush(context, data);
   std::cout << dest << ":" << std::endl;
 }

std::string ShortAnd::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::cout << "    ; && first arg " << lineNo << std::endl;
   std::string dest = data.getNextLabel() + "_and_then";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        ADD 15, " << LHS << std::endl;
   std::cout << "        RETZ 0, 1" << std::endl;
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "        ADD 15, " << RHS << std::endl;
   std::string result = data.getNextResult();
   std::cout << dest << ":\n @" << result << " ADD 0, 15" << std::endl;
   return result;
 }

int ShortAnd::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 5;
 }

int ShortAnd::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) && (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


void Equals::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; = " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKZ 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string Equals::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; = " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << " @" << result << " PICKZ 1, 0, 15" << std::endl;
   return result;
 }

int Equals::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 3;
 }

int Equals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) == rhs->evaluate(context)) ? -1 : 0);
 }


void NotEquals::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICK 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string NotEquals::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << " @" << result << " PICK 1, 0, 15" << std::endl;
   return result;
 }

int NotEquals::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 3;
 }

int NotEquals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) != rhs->evaluate(context)) ? -1 : 0);
 }


void Greater::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; > " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKP 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string Greater::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << " @" << result << " PICKP 1, 0, 15" << std::endl;
   return result;
 }

int Greater::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 3;
 }

int Greater::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) > rhs->evaluate(context)) ? -1 : 0);
 }


void Less::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; < " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKN 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string Less::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << " @" << result << " PICKN 1, 0, 15" << std::endl;
   return result;
 }

int Less::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 3;
 }

int Less::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) < rhs->evaluate(context)) ? -1 : 0);
 }


void GEQ::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; >= " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKZP 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string GEQ::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << " @" << result << " PICKZP 1, 0, 15" << std::endl;
   return result;
 }

int GEQ::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 3;
 }

int GEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) >= rhs->evaluate(context)) ? -1 : 0);
 }


void LEQ::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; <= " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKZN 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string LEQ::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << " @" << result << " PICKZN 1, 0, 15" << std::endl;
   return result;
 }

int LEQ::beltResults(const CallingContext& context) const
 {
   if (true == canEvaluate(context))
    {
      return valueResults(evaluate(context));
    }
   return lhs->beltResults(context) + rhs->beltResults(context) + 3;
 }

int LEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) <= rhs->evaluate(context)) ? -1 : 0);
 }


void ShiftLeft::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; << " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SHL 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string ShiftLeft::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; << " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SHL " << LHS << ", " << RHS << std::endl;
   return result;
 }

int ShiftLeft::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) << rhs->evaluate(context);
 }


void ShiftRight::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; >> " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ASR 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string ShiftRight::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; >> " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " ASR " << LHS << ", " << RHS << std::endl;
   return result;
 }

int ShiftRight::evaluate(const CallingContext& context) const
 {
   return ((short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void UnsignedShiftRight::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; >>> " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SHR 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string UnsignedShiftRight::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; >>> " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SHR " << LHS << ", " << RHS << std::endl;
   return result;
 }

int UnsignedShiftRight::evaluate(const CallingContext& context) const
 {
   return ((unsigned short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void RotateLeft::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; <<> " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ROL 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string RotateLeft::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; <<> " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " ROL " << LHS << ", " << RHS << std::endl;
   return result;
 }

int RotateLeft::evaluate(const CallingContext& context) const
 {
   unsigned short A1 = lhs->evaluate(context);
   unsigned short A2 = rhs->evaluate(context);
   return ((unsigned short)(A1 << (A2 & 15)) | (A1 >> (-A2 & 15)));
 }


void RotateRight::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; >>< " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        NEG 1" << std::endl;
   std::cout << "        ROL 1, 0" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string RotateRight::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; <<> " << lineNo << std::endl;
   std::cout << "        NEG " << RHS << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " ROL " << LHS << ", 0" << std::endl;
   return result;
 }

int RotateRight::evaluate(const CallingContext& context) const
 {
   unsigned short A1 = lhs->evaluate(context);
   unsigned short A2 = rhs->evaluate(context);
   return ((unsigned short)(A1 >> (A2 & 15)) | (A1 << (-A2 & 15)));
 }


void Plus::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; + " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ADD 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string Plus::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; + " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " ADD " << LHS << ", " << RHS << std::endl;
   return result;
 }

int Plus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) + rhs->evaluate(context);
 }


void Minus::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; - " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string Minus::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; - " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SUB " << LHS << ", " << RHS << std::endl;
   return result;
 }

int Minus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) - rhs->evaluate(context);
 }


void Multiply::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; * " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SDO2 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string Multiply::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; * " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SDO2 " << LHS << ", " << RHS << std::endl;
   return result;
 }

int Multiply::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) * rhs->evaluate(context);
 }


void Divide::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; / " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SDO3 0, 1" << std::endl;
   std::cout << "        SDO1 1" << std::endl;
 }

std::string Divide::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; / " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SDO3 " << LHS << ", " << RHS << std::endl;
   std::cout << " @" << result << " ADD 1, 15" << std::endl;
   return result;
 }

int Divide::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) / rhs->evaluate(context);
 }


void Remainder::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; % " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SDO3 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string Remainder::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; % " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SDO3 " << LHS << ", " << RHS << std::endl;
   return result;
 }

int Remainder::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) % rhs->evaluate(context);
 }


void DerefVar::emitStack(const CallingContext& context, GlobalData& data) const
 {
   lhs->emitPush(context, data);
   rhs->emitPush(context, data);
   std::cout << "    ; [] " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ADD 1, 1" << std::endl;
   std::cout << "        ADD 0, 1" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

std::string DerefVar::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emitNext(context, data);
   std::string RHS = rhs->emitNext(context, data);
   std::cout << "    ; [] " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        ADD " << RHS << ", " << RHS << std::endl;
   std::cout << "        ADD 0, " << LHS << std::endl;
   std::cout << " @" << result << " LD  0" << std::endl;
   return result;
 }

int DerefVar::evaluate(const CallingContext& context) const
 {
   if (nullptr != context.m_constantData)
    {
      int array = lhs->evaluate(context);
      int index = rhs->evaluate(context);
      if ((array < 0xE000) || (array > 0xFFFF))
       {
         DB_panic("Array Dereference not a constant.", lineNo);
       }
      int mem = (index * 2 + (array - 0xE000)) / 2;
      if ((mem < 0) || (static_cast<size_t>(mem) > context.m_constantData->size()))
       {
         DB_panic("Array Dereference not in defined constants: " + std::to_string(array) + " " + std::to_string(index) + " " + std::to_string(mem), lineNo);
       }
      return (*context.m_constantData)[mem];
    }
   else
    {
      DB_panic("Array Dereference not implemented for constants.", lineNo);
    }
 }

bool DerefVar::canEvaluate (const CallingContext& context) const
 {
   if ((true == lhs->canEvaluate(context)) && (true == rhs->canEvaluate(context)))
    {
      int array = lhs->evaluate(context);
      int index = rhs->evaluate(context);
      int mem = (index * 2 + (array - 0xE000)) / 2;
      // If this is an out-of-bound read, just say that it can't be done compile-time.
      if ((mem < 0) || (static_cast<size_t>(mem) > context.m_constantData->size()))
       {
         return false;
       }
      return true;
    }
   return false;
 }


void Abs::emitStack(const CallingContext& context, GlobalData& data) const
 {
   arg->emitPush(context, data);
   std::cout << "    ; +u " << lineNo << std::endl;
   // Source: https://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs
   //  OR : http://web.archive.org/web/19961201174141/www.x86.org/ftp/articles/pentopt/PENTOPT.TXT
   // This example of branchless programming relies on two's complement.
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << " @arg   LD  sp" << std::endl;
   std::cout << "        LDI 15" << std::endl;
   std::cout << "        ASR arg, 0" << std::endl;
   std::cout << "        ADD arg, 0" << std::endl;
   std::cout << "        XOR 0, 1" << std::endl;
   std::cout << "        ST  0, sp" << std::endl;
 }

std::string Abs::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string ARG = arg->emitNext(context, data);
   std::cout << "    ; +u " << lineNo << std::endl;
   std::string result = data.getNextResult();
   // Source: https://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs
   //  OR : http://web.archive.org/web/19961201174141/www.x86.org/ftp/articles/pentopt/PENTOPT.TXT
   //    Branchless programming is frustratingly necessary on a belt.
   std::cout << "        LDI 15" << std::endl;
   std::cout << "        ASR " << ARG << ", 0" << std::endl;
   std::cout << "        ADD " << ARG << ", 0" << std::endl;
   std::cout << " @" << result << " XOR 0, 1" << std::endl;
   return result;
 }

int Abs::evaluate(const CallingContext& context) const
 {
   int temp = arg->evaluate(context);
   return temp < 0 ? -temp : temp;
 }


void Negate::emitStack(const CallingContext& context, GlobalData& data) const
 {
   arg->emitPush(context, data);
   std::cout << "    ; -u " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        NEG 0" << std::endl;
   std::cout << "        ST  0, sp" << std::endl;
 }

std::string Negate::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string ARG = arg->emitNext(context, data);
   std::cout << "    ; -u " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " NEG " << ARG << std::endl;
   return result;
 }

int Negate::evaluate(const CallingContext& context) const
 {
   return -arg->evaluate(context);
 }


void Not::emitStack(const CallingContext& context, GlobalData& data) const
 {
   arg->emitPush(context, data);
   std::cout << "    ; ! " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        NOT 0" << std::endl;
   std::cout << "        ST  0, sp" << std::endl;
 }

std::string Not::emitBelt(const CallingContext& context, GlobalData& data) const
 {
   std::string ARG = arg->emitNext(context, data);
   std::cout << "    ; ! " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " NOT " << ARG << std::endl;
   return result;
 }

int Not::evaluate(const CallingContext& context) const
 {
   return ~arg->evaluate(context);
 }


/*
   Call Stack:
      Arguments, left-to-right
      Return Address
      Previous Base Pointer
      Locals
*/

void FunctionCall::emitStack(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; Function call " << name << " : return value " << lineNo << std::endl;
   VS_pushVal(0);
   std::cout << "    ; Function call " << name << " : arguments " << lineNo << std::endl;
   for (auto arg : args)
    {
      arg->emitPush(context, data);
    }
   std::cout << "    ; Function call " << name << " : call " << lineNo << std::endl;
   std::string dest = data.getNextLabel() + "_ret";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        SDO4 0" << std::endl;
   std::cout << "        LRA function_" << name << std::endl;
   std::cout << "        RET 0" << std::endl;
   std::cout << dest << ":" << std::endl;
 }

std::string FunctionCall::emitBelt(const CallingContext&, GlobalData&) const
 {
   DB_panic("Programming error: tried to emit belt code for function call.", lineNo);
 }

int FunctionCall::beltResults(const CallingContext&) const
 {
   DB_panic("Programming error: tried to count belt results for function call.", lineNo);
 }

int FunctionCall::evaluate(const CallingContext&) const
 {
   DB_panic("Function Calls not implemented for constants.", lineNo);
 }
