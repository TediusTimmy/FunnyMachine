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
#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include <string>
#include <vector>
#include <set>
#include <memory>

class Expression;
class FunctionCall;
class CallingContext;

class Statement
 {
public:
   size_t lineNo;

   virtual void emit(const CallingContext&) const = 0;
   virtual ~Statement() { }
 };

class RecAssignState
 {
public:
   std::shared_ptr<Expression> index;

   void emit(const CallingContext&) const;
 };

class Assignment : public Statement
 {
public:
   std::string lhs;
   int location;
   std::shared_ptr<RecAssignState> index;
   std::shared_ptr<Expression> rhs;

   void emit(const CallingContext&) const override;
 };

class StatementSeq : public Statement
 {
public:
   std::vector<std::shared_ptr<Statement> > statements;

   void emit(const CallingContext& context) const override
    {
      for (std::vector<std::shared_ptr<Statement> >::const_iterator iter = statements.begin();
         iter != statements.end(); ++iter)
       {
         (*iter)->emit(context);
       }
    }
 };

class IfStatement : public Statement
 {
public:
   std::shared_ptr<Expression> condition;
   std::shared_ptr<Statement> thenSeq;
   std::shared_ptr<Statement> elseSeq;

   void emit(const CallingContext&) const override;
 };

class DoStatement : public Statement
 {
public:
   std::string label;

   std::shared_ptr<Expression> preCondition;
   std::shared_ptr<Expression> postCondition;
   std::shared_ptr<StatementSeq> seq;

   void emit(const CallingContext&) const override;
 };

class BreakStatement : public Statement
 {
public:
   std::shared_ptr<Expression> condition;
   std::string label;
   bool toContinue;

   void emit(const CallingContext&) const override;
 };

class ReturnStatement : public Statement
 {
public:
   std::shared_ptr<Expression> value;

   void emit(const CallingContext&) const override;
 };

class TailCallStatement : public Statement
 {
public:
   std::vector<std::shared_ptr<Expression> > args;

   void emit(const CallingContext&) const override;
 };

class CallStatement : public Statement
 {
public:
   std::shared_ptr<Expression> fun;

   void emit(const CallingContext&) const override;
 };

#endif /* STATEMENT_HPP */
