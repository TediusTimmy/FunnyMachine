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

void Constant::emit(const CallingContext&) const
 {
   std::cout << "Push " << value << " to stack." << std::endl;
 }


void Variable::emit(const CallingContext&) const
 {
   std::cout << "Push value of " << referent << " to stack." << std::endl;
 }

int Variable::evaluate(const CallingContext& context) const
 {
   return context.getValue(referent, lineNo);
 }


void OrOp::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int OrOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) | rhs->evaluate(context);
 }


void AndOp::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int AndOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) & rhs->evaluate(context);
 }


void XorOp::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int XorOp::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) ^ rhs->evaluate(context);
 }


void ShortOr::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__  << " lhs" << std::endl;
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__  << " rhs" << std::endl;
 }

int ShortOr::evaluate(const CallingContext& context) const
 {
   if ((0 != lhs->evaluate(context)) || (0 != rhs->evaluate(context))) return -1;
   return 0;
 }


void ShortAnd::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__  << " lhs" << std::endl;
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__  << " rhs" << std::endl;
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
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Equals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) == rhs->evaluate(context)) ? -1 : 0);
 }


void NotEquals::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int NotEquals::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) != rhs->evaluate(context)) ? -1 : 0);
 }


void Greater::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Greater::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) > rhs->evaluate(context)) ? -1 : 0);
 }


void Less::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Less::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) < rhs->evaluate(context)) ? -1 : 0);
 }


void GEQ::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int GEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) >= rhs->evaluate(context)) ? -1 : 0);
 }


void LEQ::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int LEQ::evaluate(const CallingContext& context) const
 {
   return ((lhs->evaluate(context) <= rhs->evaluate(context)) ? -1 : 0);
 }


void ShiftLeft::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int ShiftLeft::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) << rhs->evaluate(context);
 }


void ShiftRight::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int ShiftRight::evaluate(const CallingContext& context) const
 {
   return ((short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void UnsignedShiftRight::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int UnsignedShiftRight::evaluate(const CallingContext& context) const
 {
   return ((unsigned short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }


void RotateLeft::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
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
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
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
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Plus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) + rhs->evaluate(context);
 }


void Minus::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Minus::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) - rhs->evaluate(context);
 }


void Multiply::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Multiply::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) * rhs->evaluate(context);
 }


void Divide::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Divide::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) / rhs->evaluate(context);
 }


void Remainder::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Remainder::evaluate(const CallingContext& context) const
 {
   return lhs->evaluate(context) % rhs->evaluate(context);
 }


void DerefVar::emit(const CallingContext& context) const
 {
   lhs->emit(context);
   rhs->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int DerefVar::evaluate(const CallingContext& context) const
 {
   DB_panic("Array Dereference not implemented for constants.", context, lineNo);
 }


void Abs::emit(const CallingContext& context) const
 {
   arg->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Abs::evaluate(const CallingContext& context) const
 {
   int temp = arg->evaluate(context);
   return temp < 0 ? -temp : temp;
 }


void Negate::emit(const CallingContext& context) const
 {
   arg->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Negate::evaluate(const CallingContext& context) const
 {
   return -arg->evaluate(context);
 }


void Not::emit(const CallingContext& context) const
 {
   arg->emit(context);
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

int Not::evaluate(const CallingContext& context) const
 {
   return ~arg->evaluate(context);
 }


void FunctionCall::emit(const CallingContext& context) const
 {
   for (auto arg : args)
    {
      arg->emit(context);
    }
   std::cout << "Call function " << name << std::endl;
 }

int FunctionCall::evaluate(const CallingContext& context) const
 {
   DB_panic("Function Calls not implemented for constants.", context, lineNo);
 }
