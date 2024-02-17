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
   index->emitPush(context, data); // Thankfully, I'm not supporting a full "Recursive Assignment Statement".
 }

void Assignment::emit(const CallingContext& context, GlobalData& data) const
 {
   if (nullptr == index)
    {
      std::string RHS = rhs->emitResult(context, data);
      std::cout << "    ; Assignment to " << lhs << " " << lineNo << std::endl;
      if (location < 128)
       {
         beltVal(location & ~1);
         std::cout << "        LDS 1" << std::endl;
         std::cout << "        ADD 0, 1" << std::endl;
         std::cout << "        ST  " << RHS << ", 0" << std::endl;
       }
      else
       {
         beltVal(location & ~1);
         std::cout << "        ST  " << RHS << ", 0" << std::endl;
       }
    }
   else
    {
      if ((index->index->beltResults(context) + Expression::valueResults(location & ~1) + 3) > 14)
       {
         rhs->emitPush(context, data);
         index->emit(context, data);
         std::cout << "    ; Assignment [] to " << lhs << " " << lineNo << std::endl;
         if (location < 128)
          {
            beltVal(location & ~1);
            std::cout << "        LDS 1" << std::endl;
            std::cout << "        ADD 0, 1" << std::endl;
            if (0 == (location & 1)) // If an array, the name is the address.
             {
               std::cout << "        LD  0" << std::endl;
             }
          }
         else
          {
            if (0 == (location & 1))
             {
               beltVal(location & ~1);
               std::cout << "        LD  0" << std::endl;
             }
            else
             {
               beltVal(location & ~1);
             }
          }
         std::cout << "        SDO5" << std::endl; // belt -> index base
         std::cout << "        SDO5" << std::endl; // belt -> rhs index base
         std::cout << "        ADD 1, 1" << std::endl;
         std::cout << "        ADD 0, 3" << std::endl;
         std::cout << "        ST  2, 0" << std::endl;
       }
      else
       {
         std::string RHS = rhs->emitResult(context, data);
         std::string INDEX = index->index->emitResult(context, data);
         std::cout << "    ; Assignment [] to " << lhs << " " << lineNo << std::endl;
         if (location < 128)
          {
            beltVal(location & ~1);
            std::cout << "        LDS 1" << std::endl;
            std::cout << "        ADD 0, 1" << std::endl;
            if (0 == (location & 1)) // If an array, the name is the address.
             {
               std::cout << "        LD  0" << std::endl;
             }
          }
         else
          {
            if (0 == (location & 1))
             {
               beltVal(location & ~1);
               std::cout << "        LD  0" << std::endl;
             }
            else
             {
               beltVal(location & ~1);
             }
          }
         std::cout << "        ADD " << INDEX << ", " << INDEX << std::endl;
         std::cout << "        ADD 0, 1" << std::endl;
         std::cout << "        ST  " << RHS << ", 0" << std::endl;
       }
    }
 }

void IfStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   std::string COND = condition->emitResult(context, data);
   std::cout << "    ; If " << lineNo << std::endl;
   std::string toElse = data.getNextLabel() + "_else";
   std::cout << "        LRA " << toElse << std::endl;
   std::cout << "        RETZ " << COND << ", 0" << std::endl;
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
      std::string PRE = preCondition->emitResult(context, data);
      std::cout << "        LRA " << bottomLabel << std::endl;
      std::cout << "        RETZ " << PRE << ", 0" << std::endl;
    }
   data.labels.push_back(std::make_pair(label, std::make_pair(topLabel, bottomLabel)));
   seq->emit(context, data); // For DO, this is not null.
   data.labels.pop_back();
   if (nullptr != postCondition)
    {
      std::cout << topLabel << ":" << std::endl;
      std::string POST = postCondition->emitResult(context, data);
      std::cout << "        LRA " << bottomLabel << std::endl;
      std::cout << "        RETZ " << POST << ", 0" << std::endl;
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
      std::string COND = condition->emitResult(context, data);
      std::cout << "        LRA " << dest << std::endl;
      std::cout << "        RETZ " << COND << ", 0" << std::endl;
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
   std::string VAL = value->emitResult(context, data);
   std::cout << "    ; Return " << lineNo << std::endl;
   std::cout << "        LDS 1" << std::endl;
   std::cout << "        STS 0, 0 ; SP <- BP" << std::endl;
   std::cout << "        SDO5" << std::endl;
   std::cout << "        STS 0, 1 ; POP BP" << std::endl;
   std::cout << " @ra    SDO5" << std::endl;
   std::cout << "        LDS 0" << std::endl;
   if (0U != context.Functions().find(context.m_currentFunction)->second.size())
    {
      std::cout << "        LDI " << (context.Functions().find(context.m_currentFunction)->second.size() * 2) << std::endl;
      std::cout << "        ADD 1, 0" << std::endl;
      std::cout << "        STS 0, 0 ; Clear arguments from stack, but not return value" << std::endl;
    }
   std::cout << "        ST  " << VAL << ", 0" << std::endl;
   std::cout << "        RET ra" << std::endl;
 }

void TailCallStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; TailCall Arguments " << lineNo << std::endl;
   for (const auto& arg : args)
    {
      arg->emitPush(context, data);
    }
   int loc = 4;
   for (const auto& arg : args)
    {
      std::cout << " @rvl   LDI " << loc << std::endl;
      std::cout << " @bp    LDS 1" << std::endl;
      std::cout << " @ldr   ADD bp, rvl" << std::endl;
      std::cout << "        SDO5" << std::endl;
      std::cout << "        ST  0, ldr" << std::endl;
      loc += 2;
      (void) arg; // We are only using it to count.
    }
   std::cout << "    ; TailCall " << lineNo << std::endl;
   std::cout << "        LRA tailcall_" << context.m_currentFunction << std::endl;
   std::cout << "        RET 0" << std::endl;
 }

void CallStatement::emit(const CallingContext& context, GlobalData& data) const
 {
   std::cout << "    ; Call " << lineNo << std::endl;
   (void) fun->emitResult(context, data);
 }

void AsmStatement::emit(const CallingContext&, GlobalData&) const
 {
   std::cout << "    ; asm " << lineNo << std::endl;
   std::cout << line << std::endl;
 }
