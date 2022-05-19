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
      std::cout << "        LDI " << ((val >> 4) & 0xFFF) << std::endl;
      std::cout << "        LDI 4" << std::endl;
      std::cout << "        SHL 1, 0" << std::endl;
      if (0 != (val & 0xF))
       {
         std::cout << "        LDI " << (val & 0xF) << std::endl;
         std::cout << "        OR  1, 0" << std::endl;
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
      std::cout << "        LDI " << ((val >> 4) & 0xFFF) << std::endl;
      std::cout << "        LDI 4" << std::endl;
      if (0 != (val & 0xF))
       {
         std::cout << "        SHL 1, 0" << std::endl;
         std::cout << "        LDI " << (val & 0xF) << std::endl;
         std::cout << " @" << name << " OR  1, 0" << std::endl;
       }
      else
       {
         std::cout << " @" << name << " SHL 1, 0" << std::endl;
       }
   }
   else
   {
      std::cout << " @" << name << " LDI " << val << std::endl;
   }
 }

std::string Constant::emit(const CallingContext&, GlobalData& data) const
 {
   std::cout << "    ; Constant " << value << " : " << lineNo << std::endl;
   std::string result = data.getNextResult();
   beltVal(value, result);
   return result;
 }


std::string Variable::emit(const CallingContext&, GlobalData& data) const
 {
   std::cout << "    ; Variable " << referent << " : " << lineNo << std::endl;
   std::string result = data.getNextResult();
   if (location < 128)
    {
      beltVal(location & ~1);
      std::cout << "        LDI 4" << std::endl;
      std::cout << "        LD  0" << std::endl;
      if (0 == (location & 1))
       {
         std::cout << "        ADD 0, 2" << std::endl;
         std::cout << " @" << result << " LD  0" << std::endl;
       }
      else
       {
         std::cout << " @" << result << " ADD 0, 2" << std::endl;
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


std::string OrOp::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; \\/ " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " OR  " << LHS << ", " << RHS << std::endl;
   return result;
 }

int OrOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) | rhs->evaluate(context);
 }


std::string AndOp::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; /\\ " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " AND " << LHS << ", " << RHS << std::endl;
   return result;
 }

int AndOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) & rhs->evaluate(context);
 }


std::string XorOp::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; ? " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " XOR " << LHS << ", " << RHS << std::endl;
   return result;
 }

int XorOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) ^ rhs->evaluate(context);
 }


std::string ShortOr::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::cout << "    ; || first arg " << lineNo << std::endl;
   std::cout << "        ADD 15, " << LHS << std::endl;
   std::string dest = data.getNextLabel() + "_or_else";
   std::cout << "        BR  0, " << dest << std::endl;
   std::string RHS = rhs->emit(context, data);
   std::cout << "        ADD 15, " << RHS << std::endl;
   std::string result = data.getNextResult();
   std::cout << dest << ": @" << result << " ADD 0, 15" << std::endl;
   return result;
 }

int ShortOr::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) || (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


std::string ShortAnd::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::cout << "    ; && first arg " << lineNo << std::endl;
   std::cout << "        ADD 15, " << LHS << std::endl;
   std::string dest = data.getNextLabel() + "_and_then";
   std::cout << "        BRZ 0, " << dest << std::endl;
   std::string RHS = rhs->emit(context, data);
   std::cout << "        ADD 15, " << RHS << std::endl;
   std::string result = data.getNextResult();
   std::cout << dest << ": @" << result << " ADD 0, 15" << std::endl;
   return result;
 }

int ShortAnd::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) && (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


std::string Equals::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; = " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        BRZ 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << " @" << result << " NOT 15" << std::endl; // Assembler isn't smart enough to follow both branches.
   return result;
 }

int Equals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) == rhs->evaluate(context)) ? -1 : 0);
 }


std::string NotEquals::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        BRZ 0, 2" << std::endl;
   std::cout << " @" << result << " NOT 15" << std::endl;
   return result;
 }

int NotEquals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) != rhs->evaluate(context)) ? -1 : 0);
 }


std::string Greater::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; > " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        BRP 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << " @" << result << " NOT 15" << std::endl;
   return result;
 }

int Greater::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) > rhs->evaluate(context)) ? -1 : 0);
 }


std::string Less::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; < " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        BRN 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << " @" << result << " NOT 15" << std::endl;
   return result;
 }

int Less::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) < rhs->evaluate(context)) ? -1 : 0);
 }


std::string GEQ::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; >= " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        BRZP 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << " @" << result << " NOT 15" << std::endl;
   return result;
 }

int GEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) >= rhs->evaluate(context)) ? -1 : 0);
 }


std::string LEQ::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; <= " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        SUB " << LHS << ", " << RHS << std::endl;
   std::cout << "        BRZN 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << " @" << result << " NOT 15" << std::endl;
   return result;
 }

int LEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) <= rhs->evaluate(context)) ? -1 : 0);
 }


std::string ShiftLeft::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; << " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SHL " << LHS << ", " << RHS << std::endl;
   return result;
 }

int ShiftLeft::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) << rhs->evaluate(context);
 }


std::string ShiftRight::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; >> " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " ASR " << LHS << ", " << RHS << std::endl;
   return result;
 }

int ShiftRight::evaluate(const CallingContext& context) const
 {
   return ((short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


std::string UnsignedShiftRight::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; >>> " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SHR " << LHS << ", " << RHS << std::endl;
   return result;
 }

int UnsignedShiftRight::evaluate(const CallingContext& context) const
 {
   return ((unsigned short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


std::string RotateLeft::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
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


std::string RotateRight::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; >>< " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " ROR " << LHS << ", " << RHS << std::endl;
   return result;
 }

int RotateRight::evaluate(const CallingContext& context) const
 {
   unsigned short A1 = lhs->evaluate(context);
   unsigned short A2 = rhs->evaluate(context);
   return ((unsigned short)(A1 >> (A2 & 15)) | (A1 << (-A2 & 15)));
 }


std::string Plus::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; + " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " ADD " << LHS << ", " << RHS << std::endl;
   return result;
 }

int Plus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) + rhs->evaluate(context);
 }


std::string Minus::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; - " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " SUB " << LHS << ", " << RHS << std::endl;
   return result;
 }

int Minus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) - rhs->evaluate(context);
 }


std::string Multiply::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; * " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        MUL " << LHS << ", " << RHS << std::endl;
   std::cout << " @" << result << " ADD 1, 15" << std::endl;
   return result;
 }

int Multiply::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) * rhs->evaluate(context);
 }


std::string Divide::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; / " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        DIV " << LHS << ", " << RHS << std::endl;
   std::cout << " @" << result << " ADD 1, 15" << std::endl;
   return result;
 }

int Divide::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) / rhs->evaluate(context);
 }


std::string Remainder::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; % " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " DIV " << LHS << ", " << RHS << std::endl;
   return result;
 }

int Remainder::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) % rhs->evaluate(context);
 }


std::string DerefVar::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string LHS = lhs->emit(context, data);
   std::string RHS = rhs->emit(context, data);
   std::cout << "    ; [] " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << "        LDI 1" << std::endl;
   std::cout << "        SHL " << RHS << ", 0" << std::endl;
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
         DB_panic("Array Dereference not a constants.", lineNo);
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


std::string Abs::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string ARG = arg->emit(context, data);
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


std::string Negate::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string ARG = arg->emit(context, data);
   std::cout << "    ; -u " << lineNo << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " NEG " << ARG << std::endl;
   return result;
 }

int Negate::evaluate(const CallingContext& context) const
 {
   return -arg->evaluate(context);
 }


std::string Not::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string ARG = arg->emit(context, data);
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

std::string FunctionCall::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; Function call " << name << " : return value " << lineNo << std::endl;
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << " @nsp   SUB sp, two" << std::endl;
   std::cout << "        ST  nsp, two" << std::endl;
   std::cout << "        ST  15, nsp" << std::endl;
   std::cout << "    ; Function call " << name << " : arguments " << lineNo << std::endl;
   for (auto arg : args)
    {
      std::string ARG = arg->emit(context, data);
      std::cout << " @two   LDI 2" << std::endl;
      std::cout << " @sp    LD  0" << std::endl;
      std::cout << " @nsp   SUB sp, two" << std::endl;
      std::cout << "        ST  " << ARG << ", nsp" << std::endl;
      std::cout << "        ST  nsp, two" << std::endl;
    }
   std::cout << "    ; Function call " << name << " : call " << lineNo << std::endl;
   std::cout << " @sa    LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << " @ra    SUB 0, 1" << std::endl;
   std::cout << " @bp    SUB 0, 2" << std::endl;
   std::cout << " @ba    LDI 4" << std::endl;
   std::cout << " @obp   LD  0" << std::endl;
   std::cout << "        ST  obp, bp" << std::endl;
   std::cout << "        ST  bp, ba" << std::endl;
   beltVal(context.getNumLocals(name, lineNo));
   std::cout << " @sp    SUB bp, 0" << std::endl;
   std::cout << "        ST  sp, sa" << std::endl;
   std::string dest = data.getNextLabel() + "_ret";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        ST  0, ra" << std::endl;
   std::cout << "        LRA function_" << name << std::endl;
   std::cout << "        RET 0" << std::endl;
   std::cout << dest << ":" << std::endl;
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << " @nsp   SUB sp, two" << std::endl;
   std::cout << "        ST  nsp, two" << std::endl;
   std::string result = data.getNextResult();
   std::cout << " @" << result << " LD  sp" << std::endl;
   return result;
 }

int FunctionCall::evaluate(const CallingContext&) const
 {
   DB_panic("Function Calls not implemented for constants.", lineNo);
 }
