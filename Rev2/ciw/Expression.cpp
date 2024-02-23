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
      std::cout << "        LDI " << (val & 0xFFF) << std::endl;
      std::cout << "        LHN 0, " << ((val >> 12) & 0xF) << std::endl;
   }
   else
   {
      std::cout << "        LDI " << val << std::endl;
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
   beltVal(val);
   std::cout << "        SDO4 0" << std::endl;
 }

void Constant::emit(const CallingContext&, GlobalData&) const
 {
   std::cout << "    ; Constant " << value << " : " << lineNo << std::endl;
   VS_pushVal(value);
 }


void Variable::emit(const CallingContext&, GlobalData&) const
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


void OrOp::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; \\/ " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        OR  0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int OrOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) | rhs->evaluate(context);
 }


void AndOp::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; /\\ " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        AND 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int AndOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) & rhs->evaluate(context);
 }


void XorOp::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; ? " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        XOR 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int XorOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) ^ rhs->evaluate(context);
 }


void ShortOr::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   std::cout << "    ; || first arg " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::string dest = data.getNextLabel() + "_or_else";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        RETC 1, 0" << std::endl;
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @nsp   ADD sp, two" << std::endl;
   std::cout << "        STS nsp, 0" << std::endl;
   rhs->emit(context, data);
   std::cout << dest << ":" << std::endl;
 }

int ShortOr::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) || (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


void ShortAnd::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   std::cout << "    ; && first arg " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::string dest = data.getNextLabel() + "_and_then";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        RETZ 1, 0" << std::endl;
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @nsp   ADD sp, two" << std::endl;
   std::cout << "        STS nsp, 0" << std::endl;
   rhs->emit(context, data);
   std::cout << dest << ":" << std::endl;
 }

int ShortAnd::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) && (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


void Equals::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; = " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKZ 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int Equals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) == rhs->evaluate(context)) ? -1 : 0);
 }


void NotEquals::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; # " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICK 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int NotEquals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) != rhs->evaluate(context)) ? -1 : 0);
 }


void Greater::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; > " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKP 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int Greater::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) > rhs->evaluate(context)) ? -1 : 0);
 }


void Less::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; < " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKN 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int Less::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) < rhs->evaluate(context)) ? -1 : 0);
 }


void GEQ::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; >= " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKZP 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int GEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) >= rhs->evaluate(context)) ? -1 : 0);
 }


void LEQ::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; <= " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        PICKZN 1, 0, 15" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int LEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) <= rhs->evaluate(context)) ? -1 : 0);
 }


void ShiftLeft::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; << " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SHL 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int ShiftLeft::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) << rhs->evaluate(context);
 }


void ShiftRight::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; >> " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ASR 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int ShiftRight::evaluate(const CallingContext& context) const
 {
   return ((short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void UnsignedShiftRight::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; >>> " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SHR 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int UnsignedShiftRight::evaluate(const CallingContext& context) const
 {
   return ((unsigned short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void RotateLeft::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; <<> " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ROL 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int RotateLeft::evaluate(const CallingContext& context) const
 {
   unsigned short A1 = lhs->evaluate(context);
   unsigned short A2 = rhs->evaluate(context);
   return ((unsigned short)(A1 << (A2 & 15)) | (A1 >> (-A2 & 15)));
 }


void RotateRight::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; >>< " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ROR 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int RotateRight::evaluate(const CallingContext& context) const
 {
   unsigned short A1 = lhs->evaluate(context);
   unsigned short A2 = rhs->evaluate(context);
   return ((unsigned short)(A1 >> (A2 & 15)) | (A1 << (-A2 & 15)));
 }


void Plus::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; + " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        ADD 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int Plus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) + rhs->evaluate(context);
 }


void Minus::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; - " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int Minus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) - rhs->evaluate(context);
 }


void Multiply::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; * " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SDO2 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int Multiply::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) * rhs->evaluate(context);
 }


void Divide::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; / " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SDO3 0, 1" << std::endl;
   std::cout << "        SDO1 1" << std::endl;
 }

int Divide::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) / rhs->evaluate(context);
 }


void Remainder::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; % " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        SDO3 0, 1" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
 }

int Remainder::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) % rhs->evaluate(context);
 }


void DerefVar::emit(const CallingContext& context, GlobalData& data) const
 {
   lhs->emit(context, data);
   rhs->emit(context, data);
   std::cout << "    ; [] " << lineNo << std::endl;
   std::cout << "        SDO0" << std::endl;
   std::cout << "        LDI 1" << std::endl;
   std::cout << "        SHL 2, 0" << std::endl;
   std::cout << "        ADD 0, 2" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::cout << "        SDO1 0" << std::endl;
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


void Abs::emit(const CallingContext& context, GlobalData& data) const
 {
   arg->emit(context, data);
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

int Abs::evaluate(const CallingContext& context) const
 {
   int temp = arg->evaluate(context);
   return temp < 0 ? -temp : temp;
 }


void Negate::emit(const CallingContext& context, GlobalData& data) const
 {
   arg->emit(context, data);
   std::cout << "    ; -u " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        NEG 0" << std::endl;
   std::cout << "        ST  0, sp" << std::endl;
 }

int Negate::evaluate(const CallingContext& context) const
 {
   return -arg->evaluate(context);
 }


void Not::emit(const CallingContext& context, GlobalData& data) const
 {
   arg->emit(context, data);
   std::cout << "    ; ! " << lineNo << std::endl;
   std::cout << " @sp    LDS 0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        NOT 0" << std::endl;
   std::cout << "        ST  0, sp" << std::endl;
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

void FunctionCall::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; Function call " << name << " : return value " << lineNo << std::endl;
   VS_pushVal(0);
   std::cout << "    ; Function call " << name << " : arguments " << lineNo << std::endl;
   for (auto arg : args)
    {
      arg->emit(context, data);
    }
   std::cout << "    ; Function call " << name << " : call " << lineNo << std::endl;
   std::string dest = data.getNextLabel() + "_ret";
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        SDO4 0" << std::endl;
   std::cout << "        LRA function_" << name << std::endl;
   std::cout << "        RET 0" << std::endl;
   std::cout << dest << ":" << std::endl;
 }

int FunctionCall::evaluate(const CallingContext&) const
 {
   DB_panic("Function Calls not implemented for constants.", lineNo);
 }
