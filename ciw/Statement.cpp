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
#include "SymbolTable.hpp"

#include <iostream>

void DB_panic (const std::string &) __attribute__ ((__noreturn__));
void DB_panic (const std::string &, size_t) __attribute__ ((__noreturn__));

void RecAssignState::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; Assignment [] " << std::endl;
   index->emit(context, data); // Thankfully, I'm not supporting a full "Recursive Assignment Statement".
 }

void Assignment::emit(const CallingContext& context, GlobalData& data) const
 {
   if (nullptr == index)
    {
      rhs->emit(context, data);
      std::cout << "    ; Assignment to " << lhs << " " << lineNo << std::endl;
      if (location < 128)
       {
         VS_LDR(location & ~1);
         VS_pop();
         std::cout << "        LD  sp" << std::endl;
         std::cout << "        ST  0, ldr" << std::endl;
       }
      else
       {
         VS_popAddr(location & ~1);
       }
    }
   else
    {
      rhs->emit(context, data);
      index->emit(context, data);
      std::cout << "    ; Assignment [] to " << lhs << " " << lineNo << std::endl;
      if (location < 128)
       {
         VS_LDR(location & ~1);
         std::cout << " @two   LDI 2" << std::endl;
         std::cout << " @sp    LD  0" << std::endl;
         std::cout << " @nsp   SUB sp, two" << std::endl;
         std::cout << "        ST  nsp, two" << std::endl;
         if (location & 1) // If an array, the name is the address.
          {
            std::cout << "        ST  ldr, nsp" << std::endl;
          }
         else
          {
            std::cout << "        LD  ldr" << std::endl;
            std::cout << "        ST  0, nsp" << std::endl;
          }
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
      VS_pop();
      std::cout << "        LD  nsp" << std::endl;
      std::cout << "        LDI 1" << std::endl;
      std::cout << "        SHL 1, 0" << std::endl;
      std::cout << "        LD  sp" << std::endl;
      std::cout << "        ADD 0, 1" << std::endl;
      std::cout << "        ST  0, nsp" << std::endl;
      VS_pop();
      std::cout << "        LD  nsp" << std::endl;
      std::cout << "        LD  sp" << std::endl;
      std::cout << "        ST  1, 0" << std::endl;
    }
 }

void IfStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   condition->emit(context, data);
   std::cout << "    ; If " << lineNo << std::endl;
   VS_pop();
   std::cout << "        LD  sp" << std::endl;
   std::string toElse = data.getNextLabel() + "_else";
   std::cout << "        LRA " << toElse << std::endl;
   std::cout << "        RETZ 1, 0" << std::endl;
   if (nullptr != thenSeq)
    {
      thenSeq->emit(context, data);
    }
   if (nullptr != elseSeq)
    {
      std::string toEnd = data.getNextLabel() + "_fi";
      std::cout << "        LRA " << toEnd << std::endl;
      std::cout << "        RET 0" << std::endl;
      std::cout << toElse << ":" << std::endl;
      elseSeq->emit(context, data);
      std::cout << toEnd << ":" << std::endl;
    }
   else
    {
      std::cout << toElse << ":" << std::endl;
    }
 }

void DoStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string topLabel = data.getNextLabel() + label + "_top";
   std::string bottomLabel = data.getNextLabel() + label + "_bottom";
   std::string realTopLabel = data.getNextLabel() + label;
   std::cout << "    ; While " << lineNo << std::endl;
   std::cout << realTopLabel << ":" << std::endl;
   if (nullptr == postCondition)
    {
      std::cout << topLabel << ":" << std::endl;
    }
   if (nullptr != preCondition)
    {
      preCondition->emit(context, data);
      VS_pop();
      std::cout << "        LD  sp" << std::endl;
      std::cout << "        LRA " << bottomLabel << std::endl;
      std::cout << "        RETZ 1, 0" << std::endl;
    }
   data.labels.push_back(std::make_pair(label, std::make_pair(topLabel, bottomLabel)));
   seq->emit(context, data); // For DO, this is not null.
   data.labels.pop_back();
   if (nullptr != postCondition)
    {
      std::cout << topLabel << ":" << std::endl;
      postCondition->emit(context, data);
      VS_pop();
      std::cout << "        LD  sp" << std::endl;
      std::cout << "        LRA " << bottomLabel << std::endl;
      std::cout << "        RETZ 1, 0" << std::endl;
    }
   std::cout << "        LRA " << realTopLabel << std::endl;
   std::cout << "        RET 0" << std::endl;
   std::cout << bottomLabel << ":" << std::endl;
 }

void BreakStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; Break/Continue/While/Until " << lineNo << std::endl;
   std::string dest = data.getNextLabel() + "_cond";
   if (nullptr != condition)
    {
      condition->emit(context, data);
      VS_pop();
      std::cout << "        LD  sp" << std::endl;
      std::cout << "        BRZ 0, " << dest << std::endl;
    }
   size_t index = data.labels.size();
   if ("" != label)
    {
      do
       {
         if (0 == index)
          {
            DB_panic("Can't find label " + label + " but we got here.", lineNo);
          }
         --index;
       }
      while (label != data.labels[index].first);
    }
   if (true == toContinue)
    {
      std::cout << "        LRA " << data.labels[index].second.first << std::endl;
    }
   else
    {
      std::cout << "        LRA " << data.labels[index].second.second << std::endl;
    }
   std::cout << "        RET 0" << std::endl;
   if (nullptr != condition)
    {
      std::cout << dest << ":" << std::endl;
    }
 }

void ReturnStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   value->emit(context, data);
   std::cout << "    ; Return " << lineNo << std::endl;
   std::cout << " @two   LDI 2" << std::endl;
   std::cout << "        LD  0" << std::endl;
   std::cout << " @rv    LD  0" << std::endl;
   std::cout << " @rvl   LDI " << (context.Functions().find(context.m_currentFunction)->second.size() * 2 + 4) << std::endl;
   std::cout << " @bpa   LDI 4" << std::endl;
   std::cout << " @pbp   LD  bpa" << std::endl;
   std::cout << "        ADD pbp, rvl" << std::endl;
   std::cout << "        ST  rv, 0" << std::endl;
   std::cout << " @bp    ADD pbp, two" << std::endl;
   std::cout << "        ST  bp, two" << std::endl;
   std::cout << " @nbp   LD  pbp" << std::endl;
   std::cout << "        ST  nbp, bpa" << std::endl;
   std::cout << " @ra    ADD pbp, two" << std::endl;
   std::cout << "        LDI " << (context.Functions().find(context.m_currentFunction)->second.size() * 2 + 2) << std::endl;
   std::cout << "        ADD ra, 0" << std::endl;
   std::cout << "        ST  0, two" << std::endl;
   std::cout << "        LD  ra" << std::endl;
   std::cout << "        RET 0" << std::endl;
 }

 // TODO : leave each arg on the stack and copy it down so that "tailcall x + 2 ; x + 3" works correctly.
void TailCallStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; TailCall Arguments " << lineNo << std::endl;
   int loc = context.Functions().find(context.m_currentFunction)->second.size() * 2 + 2;
   for (const auto& arg : args)
    {
      arg->emit(context, data);
      std::cout << " @rvl   LDI " << loc << std::endl;
      std::cout << " @bpa   LDI 4" << std::endl;
      std::cout << " @pbp   LD  bpa" << std::endl;
      std::cout << " @ldr   ADD pbp, rvl" << std::endl;
      VS_pop();
      std::cout << "        LD  sp" << std::endl;
      std::cout << "        ST  0, ldr" << std::endl;
      loc -= 2;
    }
   std::cout << "    ; TailCall " << lineNo << std::endl;
   std::cout << "        LRA function_" << context.m_currentFunction << std::endl;
   std::cout << "        RET 0" << std::endl;
 }

void CallStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; Call " << lineNo << std::endl;
   fun->emit(context, data);
   VS_pop();
 }

void AsmStatement::emit(const CallingContext&, GlobalData&) const
 {
   std::cout << "    ; asm " << lineNo << std::endl;
   std::cout << line << std::endl;
 }
