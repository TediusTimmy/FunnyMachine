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

void DB_panic (const std::string &, const CallingContext &, size_t) __attribute__ ((__noreturn__));

void beltVal(short val)
 {
   if ((val > 0x7FF) || (val < -0x7FF))
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

void VS_pushAddr (short addr)
 {
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << " @nsp   SUB sp, two" << std::endl;
   std::cout << "        ST  nsp, two" << std::endl;
   beltVal(addr);
   std::cout << "        LD  0" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

void VS_pushVal (short val)
 {
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << " @nsp   SUB sp, two" << std::endl;
   std::cout << "        ST  nsp, two" << std::endl;
   beltVal(val);
   std::cout << "        ST  0, nsp" << std::endl;
 }

void VS_LDR (short addr)
 {
   std::cout << " @four  LDI 4" << std::endl;
   std::cout << " @bp    LD  0" << std::endl;
   beltVal(addr);
   std::cout << " @ldr   ADD bp, 0" << std::endl;
 }

void VS_popAddr (short addr) // pop value and store in addr
 {
   VS_pop();
   beltVal(addr);
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        ST  0, 1" << std::endl;
 }

void VS_pop (void)
 {
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << " @nsp   ADD sp, two" << std::endl;
   std::cout << "        ST  nsp, two" << std::endl;
 }

void Constant::emit(const CallingContext&) const
 {
   std::cout << "    ; Constant " << value << " : " << lineNo << std::endl;
   VS_pushVal(value);
 }


void Variable::emit(const CallingContext&) const
 {
   std::cout << "    ; Variable " << referent << " : " << lineNo << std::endl;
   if (location < 128)
    {
      VS_pushAddr(4);
      VS_pushVal(location & ~1);
      VS_pop();
      std::cout << "        LD  sp" << std::endl;
      std::cout << "        LD  nsp" << std::endl;
      std::cout << "        ADD 0, 1" << std::endl;
      if (0 == (location & 1))
       {
         std::cout << "        LD  0" << std::endl;
       }
      std::cout << "        ST  0, nsp" << std::endl;
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


void OrOp::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; \\/ " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        OR  0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int OrOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) | rhs->evaluate(context);
 }


void AndOp::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; /\\ " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        AND 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int AndOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) & rhs->evaluate(context);
 }


void XorOp::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; ? " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        XOR 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int XorOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) ^ rhs->evaluate(context);
 }


void ShortOr::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   std::cout << "    ; || first arg " << lineNo << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::string dest = CallingContext::getNextLabel();
   std::cout << "        BR  0, " << dest << std::endl;
   VS_pop();
   rhs->emit(context);
   std::cout << dest << ":" << std::endl;
 }

int ShortOr::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) || (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


void ShortAnd::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   std::cout << "    ; && first arg " << lineNo << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::string dest = CallingContext::getNextLabel();
   std::cout << "        BRZ 0, " << dest << std::endl;
   VS_pop();
   rhs->emit(context);
   std::cout << dest << ":" << std::endl;
 }

int ShortAnd::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) && (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


void Equals::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; = " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        BRZ 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        ST  0, nsp - 1" << std::endl;
 }

int Equals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) == rhs->evaluate(context)) ? -1 : 0);
 }


void NotEquals::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; # " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        BRZ 0, 2 ; Invalidates belt references" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << "        ST  2, sp" << std::endl;
 }

int NotEquals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) != rhs->evaluate(context)) ? -1 : 0);
 }


void Greater::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; > " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        BRP 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        ST  0, nsp - 1" << std::endl;
 }

int Greater::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) > rhs->evaluate(context)) ? -1 : 0);
 }


void Less::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; < " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        BRN 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        ST  0, nsp - 1" << std::endl;
 }

int Less::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) < rhs->evaluate(context)) ? -1 : 0);
 }


void GEQ::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; >= " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        BRZP 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        ST  0, nsp - 1" << std::endl;
 }

int GEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) >= rhs->evaluate(context)) ? -1 : 0);
 }


void LEQ::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; <= " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        BRZN 0, 4" << std::endl;
   std::cout << "        ADD 15, 15" << std::endl;
   std::cout << "        BRA 2" << std::endl;
   std::cout << "        NOT 15" << std::endl;
   std::cout << "        ST  0, nsp - 1" << std::endl;
 }

int LEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) <= rhs->evaluate(context)) ? -1 : 0);
 }


void ShiftLeft::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; << " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SHL 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int ShiftLeft::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) << rhs->evaluate(context);
 }


void ShiftRight::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; >> " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        ASR 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int ShiftRight::evaluate(const CallingContext& context) const
 {
   return ((short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void UnsignedShiftRight::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; >>> " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SHR 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int UnsignedShiftRight::evaluate(const CallingContext& context) const
 {
   return ((unsigned short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void RotateLeft::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; <<> " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        ROL 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int RotateLeft::evaluate(const CallingContext& context) const
 {
   unsigned short A1 = lhs->evaluate(context);
   unsigned short A2 = rhs->evaluate(context);
   return ((unsigned short)(A1 << (A2 & 15)) | (A1 >> (-A2 & 15)));
 }


void RotateRight::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; >>< " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        ROR 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int RotateRight::evaluate(const CallingContext& context) const
 {
   unsigned short A1 = lhs->evaluate(context);
   unsigned short A2 = rhs->evaluate(context);
   return ((unsigned short)(A1 >> (A2 & 15)) | (A1 << (-A2 & 15)));
 }


void Plus::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; + " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        ADD 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int Plus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) + rhs->evaluate(context);
 }


void Minus::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; - " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int Minus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) - rhs->evaluate(context);
 }


void Multiply::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; * " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        MUL 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int Multiply::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) * rhs->evaluate(context);
 }


void Divide::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; / " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        DIV 0, 1" << std::endl;
   std::cout << "        ST  nsp, 1" << std::endl;
 }

int Divide::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) / rhs->evaluate(context);
 }


void Remainder::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; % " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        DIV 0, 1" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int Remainder::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) % rhs->evaluate(context);
 }


void DerefVar::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "    ; [] " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        LDI 1" << std::endl;
   std::cout << "        SHL 1, 0" << std::endl;
   std::cout << "        LD  nsp" << std::endl;
   std::cout << "        ADD 0, 1" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::cout << "        ST  0, nsp" << std::endl;
 }

int DerefVar::evaluate(const CallingContext& context) const
 {
   DB_panic("Array Dereference not implemented for constants.", context, lineNo);
 }


void Abs::emit(const CallingContext& context) const
 {
   arg->emit(context);
   std::cout << "    ; +u " << lineNo << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        BRZP 0, 2 ; Invalidates result references" << std::endl;
   std::cout << " @res   NEG 0" << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << "        ST  res, sp" << std::endl;
 }

int Abs::evaluate(const CallingContext& context) const
 {
   int temp = arg->evaluate(context);
   return temp < 0 ? -temp : temp;
 }


void Negate::emit(const CallingContext& context) const
 {
   arg->emit(context);
   std::cout << "    ; -u " << lineNo << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
   std::cout << "        LD  sp" << std::endl;
   std::cout << "        NEG 0" << std::endl;
   std::cout << "        ST  0, sp" << std::endl;
 }

int Negate::evaluate(const CallingContext& context) const
 {
   return -arg->evaluate(context);
 }


void Not::emit(const CallingContext& context) const
 {
   arg->emit(context);
   std::cout << "    ; ! " << lineNo << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << " @sp    LD  0" << std::endl;
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

void FunctionCall::emit(const CallingContext& context) const
 {
   std::cout << "    ; Function call " << name << " : return value " << lineNo << std::endl;
   VS_pushVal(0);
   std::cout << "    ; Function call " << name << " : arguments " << lineNo << std::endl;
   for (auto arg : args)
    {
      arg->emit(context);
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
   std::string dest = CallingContext::getNextLabel();
   std::cout << "        LRA " << dest << std::endl;
   std::cout << "        ST  0, ra" << std::endl;
   std::cout << "        LRA function_" << name << std::endl;
   std::cout << "        RET 0" << std::endl;
   std::cout << dest << ":" << std::endl;
 }

int FunctionCall::evaluate(const CallingContext& context) const
 {
   DB_panic("Function Calls not implemented for constants.", context, lineNo);
 }
