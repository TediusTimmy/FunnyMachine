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
#include "Statement.hpp"
#include "Expression.hpp"

#include <iostream>

void DB_panic (const std::string &) __attribute__ ((__noreturn__));

void RecAssignState::emit(const CallingContext&) const
 {
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

void Assignment::emit(const CallingContext& context) const
 {
   if (nullptr == index)
    {
      rhs->emit(context);
      if (location < 256)
       {
         VS_LDR(location);
         VS_pop();
         std::cout << "        LD  sp" << std::endl;
         std::cout << "        ST  ldr, 0" << std::endl;
       }
      else
       {
         VS_popAddr(location);
       }
    }
   else
    {
      std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
    }
 }

void IfStatement::emit(const CallingContext&) const
 {
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

void DoStatement::emit(const CallingContext&) const
 {
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

void BreakStatement::emit(const CallingContext&) const
 {
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

void ReturnStatement::emit(const CallingContext& context) const
 {
   value->emit(context);
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << " @four  LDI 4" << std::endl;
   std::cout << " @bp    LD  four" << std::endl;
   std::cout << "        ST  bp, two" << std::endl;
   std::cout << "        SUB bp, two" << std::endl;
   std::cout << "        ST  0, two" << std::endl;
 }

void TailCallStatement::emit(const CallingContext&) const
 {
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }

void CallStatement::emit(const CallingContext&) const
 {
   std::cout << "In " << __PRETTY_FUNCTION__ << std::endl;
 }
