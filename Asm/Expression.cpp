 /*
   Copyright (C) 2022 Thomas DiModica <ricinwich@yahoo.com>

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
#include "Expression.h"

int Multiply::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) * rhs->evaluate(context);
 }

int Divide::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) / rhs->evaluate(context);
 }

int Mod::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) % rhs->evaluate(context);
 }

int ShiftLeft::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) << rhs->evaluate(context);
 }

int ShiftRight::evaluate (const SymbolTable& context) const
 {
   return ((short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }

int UnsignedShiftRight::evaluate (const SymbolTable& context) const
 {
   return ((unsigned short)lhs->evaluate(context)) >> rhs->evaluate(context);
 }

int Plus::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) + rhs->evaluate(context);
 }

int Minus::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) - rhs->evaluate(context);
 }

int BitAnd::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) & rhs->evaluate(context);
 }

int BitOr::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) | rhs->evaluate(context);
 }

int BitXor::evaluate (const SymbolTable& context) const
 {
   return lhs->evaluate(context) ^ rhs->evaluate(context);
 }

int Equals::evaluate (const SymbolTable& context) const
 {
   if (lhs->evaluate(context) == rhs->evaluate(context))
      return 1;

   return 0;
 }

int NotEquals::evaluate (const SymbolTable& context) const
 {
   if (lhs->evaluate(context) != rhs->evaluate(context))
      return 1;

   return 0;
 }

int Greater::evaluate (const SymbolTable& context) const
 {
   if (lhs->evaluate(context) > rhs->evaluate(context))
      return 1;

   return 0;
 }

int Less::evaluate (const SymbolTable& context) const
 {
   if (lhs->evaluate(context) < rhs->evaluate(context))
      return 1;

   return 0;
 }

int GEQ::evaluate (const SymbolTable& context) const
 {
   if (lhs->evaluate(context) >= rhs->evaluate(context))
      return 1;

   return 0;
 }

int LEQ::evaluate (const SymbolTable& context) const
 {
   if (lhs->evaluate(context) <= rhs->evaluate(context))
      return 1;

   return 0;
 }

int ShortOr::evaluate (const SymbolTable& context) const
 {
   if ((0 != lhs->evaluate(context)) ||
       (0 != rhs->evaluate(context)))
      return 1;

   return 0;
 }

int ShortAnd::evaluate (const SymbolTable& context) const
 {
   if ((0 != lhs->evaluate(context)) &&
       (0 != rhs->evaluate(context)))
      return 1;

   return 0;
 }

int ValueOf::evaluate (const SymbolTable& context) const
 {
   return (arg->evaluate(context) + location) & 0x1FFF;
 }

int Abs::evaluate (const SymbolTable& context) const
 {
   int temp = arg->evaluate(context);
   return temp < 0 ? -temp : temp;
 }

int Negate::evaluate (const SymbolTable& context) const
 {
   return -arg->evaluate(context);
 }

int BitNot::evaluate (const SymbolTable& context) const
 {
   return ~arg->evaluate(context);
 }

int HighByte::evaluate (const SymbolTable& context) const
 {
   return arg->evaluate(context) >> 8;
 }

int LowByte::evaluate (const SymbolTable& context) const
 {
   return arg->evaluate(context) & 0xFF;
 }

int Not::evaluate (const SymbolTable& context) const
 {
   if (0 == arg->evaluate(context))
      return 1;

   return 0;
 }
