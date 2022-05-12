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
#include "Parser.hpp"

#include "SymbolTable.hpp"
#include "Expression.hpp"
#include "Statement.hpp"

void DB_panic (const std::string &) __attribute__ ((__noreturn__));

void Parser::GNT (void)
 {
   nextToken = src.GetNextToken();
 }

std::string Parser::LN (void)
 {
   return std::to_string(nextToken.lineNumber);
 }

void Parser::expect (const Lexeme & tok)
 {
   if (tok != nextToken.lexeme)
    {
      DB_panic("Token \"" + nextToken.text + "\" was not expected on " + LN() + ".");
    }
   GNT();
 }





std::shared_ptr<Expression> Parser::expression (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = boolean(context);

   while ((AND_OP == nextToken.lexeme) || (OR_OP == nextToken.lexeme) || (XOR_OP == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> rhs;
      switch(nextToken.lexeme)
       {
         case AND_OP:
            rhs = std::make_shared<AndOp>();
            break;
         case OR_OP:
            rhs = std::make_shared<OrOp>();
            break;
         case XOR_OP:
            rhs = std::make_shared<XorOp>();
            break;
         default: break;
       }

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = boolean(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::boolean (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = clause(context);

   while (SHORT_OR == nextToken.lexeme)
    {
      std::shared_ptr<BinaryOperation> rhs = std::make_shared<ShortOr>();

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = clause(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::clause (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = predicate(context);

   while (SHORT_AND == nextToken.lexeme)
    {
      std::shared_ptr<BinaryOperation> rhs = std::make_shared<ShortAnd>();

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = predicate(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::predicate (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = relation(context);

   if ((EQUALITY == nextToken.lexeme) || (INEQUALITY == nextToken.lexeme) || (EQUAL_SIGN == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> rhs;
      switch(nextToken.lexeme)
       {
         case EQUAL_SIGN:
         case EQUALITY:
            rhs = std::make_shared<Equals>();
            break;
         case INEQUALITY:
            rhs = std::make_shared<NotEquals>();
            break;
         default: break;
       }

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = relation(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::relation (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = shift(context);

   if ((GREATER_THAN == nextToken.lexeme) || (LESS_THAN_OR_EQUAL_TO == nextToken.lexeme) ||
       (GREATER_THAN_OR_EQUAL_TO == nextToken.lexeme) || (LESS_THAN == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> rhs;
      switch(nextToken.lexeme)
       {
         case GREATER_THAN:
            rhs = std::make_shared<Greater>();
            break;
         case LESS_THAN:
            rhs = std::make_shared<Less>();
            break;
         case GREATER_THAN_OR_EQUAL_TO:
            rhs = std::make_shared<GEQ>();
            break;
         case LESS_THAN_OR_EQUAL_TO:
            rhs = std::make_shared<LEQ>();
            break;
         default: break;
       }

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = shift(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::shift (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = simple(context);

   while ((SHIFT_LEFT == nextToken.lexeme) || (SHIFT_RIGHT == nextToken.lexeme) ||
          (UNSIGNED_SHIFT_RIGHT == nextToken.lexeme) || (ROTATE_LEFT == nextToken.lexeme) ||
          (ROTATE_RIGHT == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> rhs;
      switch(nextToken.lexeme)
       {
         case SHIFT_LEFT:
            rhs = std::make_shared<ShiftLeft>();
            break;
         case SHIFT_RIGHT:
            rhs = std::make_shared<ShiftRight>();
            break;
         case UNSIGNED_SHIFT_RIGHT:
            rhs = std::make_shared<UnsignedShiftRight>();
            break;
         case ROTATE_LEFT:
            rhs = std::make_shared<RotateLeft>();
            break;
         case ROTATE_RIGHT:
            rhs = std::make_shared<RotateRight>();
            break;
         default: break;
       }

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = simple(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::simple (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = term(context);

   while ((PLUS == nextToken.lexeme) || (MINUS == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> rhs;
      switch(nextToken.lexeme)
       {
         case PLUS:
            rhs = std::make_shared<Plus>();
            break;
         case MINUS:
            rhs = std::make_shared<Minus>();
            break;
         default: break;
       }

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = term(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::term (const CallingContext& context)
 {
   std::shared_ptr<Expression> lhs = primary(context);

   while ((MULTIPLY == nextToken.lexeme) || (DIVIDE == nextToken.lexeme) || (REMAINDER == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> rhs;
      switch(nextToken.lexeme)
       {
         case MULTIPLY:
            rhs = std::make_shared<Multiply>();
            break;
         case DIVIDE:
            rhs = std::make_shared<Divide>();
            break;
         case REMAINDER:
            rhs = std::make_shared<Remainder>();
            break;
         default: break;
       }

      rhs->lhs = lhs;
      rhs->lineNo = nextToken.lineNumber;
      GNT();

      rhs->rhs = primary(context);
      lhs = rhs;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::primary (const CallingContext& context)
 {
   std::shared_ptr<Expression> ret;

   switch(nextToken.lexeme)
    {
      case IDENTIFIER:
         switch(context.lookup(nextToken.text))
          {
            case CallingContext::VARIABLE:
                {
                  std::shared_ptr<Variable> op = std::make_shared<Variable>();
                  ret = op;

                  op->referent = nextToken.text;
                  op->location = context.getValue(op->referent, op->lineNo);

                  ret->lineNo = nextToken.lineNumber;
                  GNT();
                }
               break;
            case CallingContext::CONSTANT:
                {
                  std::shared_ptr<Constant> op = std::make_shared<Constant>();
                  ret = op;

                  op->value = context.getValue(nextToken.text, nextToken.lineNumber);

                  ret->lineNo = nextToken.lineNumber;
                  GNT();
                }
               break;
            case CallingContext::FUNCTION:
                {
                  std::shared_ptr<FunctionCall> op = std::make_shared<FunctionCall>();
                  ret = op;

                  op->name = nextToken.text;

                  ret->lineNo = nextToken.lineNumber;
                  GNT();

                  expect(OPEN_PARENS);

                  if (CLOSE_PARENS != nextToken.lexeme)
                   {
                     op->args.push_back(expression(context));

                     while (SEMICOLON == nextToken.lexeme)
                      {
                        GNT();
                        op->args.push_back(expression(context));
                      }
                   }

                  expect(CLOSE_PARENS);

                  if (op->args.size() != context.Functions().find(op->name)->second.size())
                     DB_panic("Call to \"" + op->name + "\" with bad number of arguments on " + LN() + ".");
                }
               break;
            case CallingContext::LABEL:
               DB_panic("Label \"" + nextToken.text + "\" used in expression on " + LN() + ".");
            case CallingContext::UNDEFINED:
               DB_panic("Undefined identifier \"" + nextToken.text + "\" used in expression on " + LN() + ".");
          }
         break;
      case PLUS:
          {
            std::shared_ptr<Abs> op = std::make_shared<Abs>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->arg = primary(context);
          }
         break;
      case MINUS:
          {
            std::shared_ptr<Negate> op = std::make_shared<Negate>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->arg = primary(context);
          }
         break;
      case NOT:
          {
            std::shared_ptr<Not> op = std::make_shared<Not>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->arg = primary(context);
          }
         break;
      case NUMBER:
          {
            std::shared_ptr<Constant> op = std::make_shared<Constant>();
            ret = op;

            op->value = std::stod(nextToken.text);

            ret->lineNo = nextToken.lineNumber;
            GNT();
          }
         break;
      case OPEN_PARENS:
         GNT();
         ret = expression(context);
         expect(CLOSE_PARENS);
         break;
      default:
         DB_panic("Expected primary expression, found \"" + nextToken.text + "\" on " + LN() + ".");
    }

   // Handle this here, despite NUMEROUS instances where this DOES NOT MAKE SENSE.
   //    Chewbacca statement: "Hello, World!"[5]
   while (LEFT_BRACKET == nextToken.lexeme)
    {
      std::shared_ptr<DerefVar> op = std::make_shared<DerefVar>();

      op->lhs = ret;
      ret = op;

      ret->lineNo = nextToken.lineNumber;
      GNT();

      op->rhs = expression(context);

      expect(RIGHT_BRACKET);
    }

   return ret;
 }





void Parser::program (CallingContext & context)
 {
   constants(context);
   globals(context);
   functions(context);
   expect(END_OF_FILE);
 }

void Parser::constants (CallingContext & context)
 {
   while ((CONST == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if (CONST == nextToken.lexeme)
       {
         GNT();

         std::string name = nextToken.text;
         expect(IDENTIFIER);

         if (CallingContext::UNDEFINED != context.lookup(name))
            DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

         if (EQUAL_SIGN == nextToken.lexeme)
            GNT();
         else
            expect(ASSIGNMENT);

         std::shared_ptr<Expression> temp = expression(context);

         int val = temp->evaluate(context);

         context.Constants().insert(std::make_pair(name, val));

         expect(NEW_LINE);
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }

void Parser::globals (CallingContext & context)
 {
   while ((DIM == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if (DIM == nextToken.lexeme)
       {
         do
          {
            GNT();

            std::string name = nextToken.text;
            expect(IDENTIFIER);

            if (CallingContext::UNDEFINED != context.lookup(name))
               DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

            int val = 1;

            if (LEFT_BRACKET == nextToken.lexeme)
             {
               GNT();

               std::shared_ptr<Expression> temp = expression(context);

               int length = temp->evaluate(context);

               expect(RIGHT_BRACKET);

               val = context.createArray(length);
             }

            context.Globals().insert(std::make_pair(name, val));
          }
         while (SEMICOLON == nextToken.lexeme);

         expect(NEW_LINE);
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }

void Parser::variables (CallingContext & context)
 {
   while ((DIM == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if (DIM == nextToken.lexeme)
       {
         do
          {
            GNT();

            bool isStatic = false;
            if (STATIC == nextToken.lexeme)
             {
               GNT();
               isStatic = true;
             }

            std::string name = nextToken.text;
            expect(IDENTIFIER);

            if (CallingContext::UNDEFINED != context.lookup(name))
               DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

            int length = 0;
            if (LEFT_BRACKET == nextToken.lexeme)
             {
                  GNT();

                  std::shared_ptr<Expression> temp = expression(context);

                  length = temp->evaluate(context);

                  expect(RIGHT_BRACKET);
             }

            if (true == isStatic)
             {
               int val = context.createArray(length);
               context.Statics().insert(std::make_pair(name, val));
             }
            else
             {
               context.FunLocals()[context.Name].insert(std::make_pair(name, length));
             }
          }
         while (SEMICOLON == nextToken.lexeme);

         expect(NEW_LINE);
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }

void Parser::functions (CallingContext & context)
 {
   while ((FUNCTION == nextToken.lexeme) || (DECLARE == nextToken.lexeme) || (NEW_LINE == nextToken.lexeme))
    {
      if ((DECLARE == nextToken.lexeme) || (FUNCTION == nextToken.lexeme))
       {
         bool isDeclare = (DECLARE == nextToken.lexeme);
         GNT();

         std::string name = nextToken.text;
         expect(IDENTIFIER);

         bool wasDeclared = false;
         if (CallingContext::FUNCTION == context.lookup(name))
          {
            wasDeclared = true;

            // If we have a definition and this is not a declare, then we have a problem.
            if ((context.FunDefs().end() != context.FunDefs().find(name)) && (false == isDeclare))
               DB_panic("Redefinition of function \"" + name + "\" on " + LN() + ".");
          }
         else if (CallingContext::UNDEFINED != context.lookup(name))
            DB_panic("Reuse of identifier \"" + name + "\" on " + LN() + ".");

         std::vector<std::string> args;

         expect(OPEN_PARENS);
         if (IDENTIFIER == nextToken.lexeme)
          {
            std::string temp = nextToken.text;

            GNT();

            args.push_back(temp);

            if (CallingContext::UNDEFINED != context.lookup(temp))
               DB_panic("Reuse of identifier \"" + temp + "\" on " + LN() + ".");

            while (SEMICOLON == nextToken.lexeme)
             {
               GNT();

               temp = nextToken.text;
               expect(IDENTIFIER);

               args.push_back(temp);

               if (CallingContext::UNDEFINED != context.lookup(temp))
                  DB_panic("Reuse of identifier \"" + temp + "\" on " + LN() + ".");
             }
          }
         expect(CLOSE_PARENS);

         if (true == wasDeclared)
          {
            // Check the previous definition/declaration and make sure
            // it matches the current declaration/definition.
            std::vector<std::string> & check = context.Functions().find(name)->second;

            if (check.size() != args.size())
               DB_panic("Declaration or definition of function \"" + name + "\" on " + LN() +
                  "\ndoes not match previous declaration(s) or definition(s).");

            for (std::vector<std::string>::iterator cIter = check.begin(), aIter = args.begin();
               cIter != check.end(); ++cIter, ++aIter)
             {
               if (*cIter != *aIter)
                  DB_panic("Declaration or definition of function \"" + name + "\" on " + LN() +
                     "\ndoes not match previous declaration(s) or definition(s).");
             }
          }
         else
          {
            context.Functions().insert(std::make_pair(name, args));
          }
         expect(NEW_LINE);

         // This is a define, so define the function.
         if (false == isDeclare)
          {
            // Build a new calling context, inserting dummy locals.
            CallingContext newFunction (context, name, nextToken.lineNumber);

            int locloc = -static_cast<int>(args.size()) * 2 - 2;
            for (std::vector<std::string>::iterator iter = args.begin();
               args.end() != iter; ++iter)
             {
               newFunction.Locals().insert(std::make_pair(*iter, locloc));
               locloc += 2;
             }

            variables(newFunction);

            locloc = 2;
            for (std::map<std::string, int>::iterator iter = newFunction.FunLocals()[name].begin();
               iter != newFunction.FunLocals()[name].end(); ++iter)
             {
               newFunction.Locals().insert(std::make_pair(iter->first, locloc));
               locloc += 2;
               if (iter->second > 1)
                {
                  locloc += 2 * (iter->second - 1);
                }
             }

            std::shared_ptr<StatementSeq> seq = std::make_shared<StatementSeq>();

            while (END_FUNCTION != nextToken.lexeme)
             {
               while (NEW_LINE == nextToken.lexeme)
                  GNT();

               std::shared_ptr<Statement> statmnt = statement(newFunction);

               if (nullptr != statmnt)
                  seq->statements.push_back(statmnt);

               expect(NEW_LINE);
             }
            GNT();

            context.FunDefs().insert(std::make_pair(name, seq));
          }
       }
      else // NEW_LINE
       {
         GNT();
       }
    }
 }





std::shared_ptr<Statement> Parser::statement (CallingContext& context)
 {
   std::shared_ptr<Statement> ret;

   std::string label;
   if (COLON == nextToken.lexeme)
    {
      GNT();

      if (CallingContext::UNDEFINED != context.lookup(nextToken.text))
         DB_panic("Identifier \"" + nextToken.text + "\" redefined as label on " + LN() + ".");

      label = nextToken.text;

      expect(IDENTIFIER);
      expect(COLON);
    }

   switch (nextToken.lexeme)
    {
      case IDENTIFIER: // Assignment Statement
          {
            std::shared_ptr<Assignment> op = std::make_shared<Assignment>();
            ret = op;

            if (CallingContext::VARIABLE != context.lookup(nextToken.text))
               DB_panic("L-value \"" + nextToken.text + "\" not a variable on " + LN() + ".");

            op->lhs = nextToken.text;
            op->location = context.getValue(op->lhs, op->lineNo);
            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (LEFT_BRACKET == nextToken.lexeme)
             {
               GNT();

               op->index = std::make_shared<RecAssignState>();
               op->index->index = expression(context);

               expect(RIGHT_BRACKET);
             }

            if (EQUAL_SIGN == nextToken.lexeme)
               GNT();
            else
               expect(ASSIGNMENT);

            op->rhs = expression(context);
          }
         break;

      case IF:
          {
            std::shared_ptr<IfStatement> op = std::make_shared<IfStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->condition = expression(context);

            expect(THEN);

            if (NEW_LINE == nextToken.lexeme)
             {
               GNT();

               std::shared_ptr<StatementSeq> thenSeq = std::make_shared<StatementSeq>();

               while ((ELSE != nextToken.lexeme) && (FI != nextToken.lexeme))
                {
                  std::shared_ptr<Statement> statmnt = statement(context);

                  if (nullptr != statmnt)
                     thenSeq->statements.push_back(statmnt);

                  expect(NEW_LINE);
                }

               if (0 < thenSeq->statements.size())
                  op->thenSeq = thenSeq;

               if (ELSE == nextToken.lexeme)
                {
                  GNT();

                  std::shared_ptr<Statement>* cur = &(op->elseSeq);

                  while (IF == nextToken.lexeme)
                   {
                     GNT();

                     std::shared_ptr<IfStatement> temp = std::make_shared<IfStatement>();
                     *cur = temp;
                     cur = &(temp->elseSeq);

                     temp->condition = expression(context);

                     expect(THEN);
                     expect(NEW_LINE);

                     std::shared_ptr<StatementSeq> tempThenSeq = std::make_shared<StatementSeq>();
                     while ((ELSE != nextToken.lexeme) && (FI != nextToken.lexeme))
                      {
                        std::shared_ptr<Statement> statmnt = statement(context);

                        if (nullptr != statmnt)
                           tempThenSeq->statements.push_back(statmnt);

                        expect(NEW_LINE);
                      }

                     if (0 < tempThenSeq->statements.size())
                        temp->thenSeq = tempThenSeq;

                     if (ELSE == nextToken.lexeme)
                        GNT();
                   }

                   // At this point, we are either at FI
                   // Or a NEW_LINE. NEW_LINE implies ELSE.
                  if (NEW_LINE == nextToken.lexeme)
                   {
                     GNT();

                     std::shared_ptr<StatementSeq> elseSeq = std::make_shared<StatementSeq>();

                     while (FI != nextToken.lexeme)
                      {
                        std::shared_ptr<Statement> statmnt = statement(context);

                        if (nullptr != statmnt)
                           elseSeq->statements.push_back(statmnt);

                        expect(NEW_LINE);
                      }

                     if (0 < elseSeq->statements.size())
                        *cur = elseSeq;
                   }
                }

               expect(FI);
             }
            else
             {
               op->thenSeq = statement(context);

               if (ELSE == nextToken.lexeme)
                {
                  GNT();
                  op->elseSeq = statement(context);
                }
             }
          }
         break;

      case DO:
          {
            std::shared_ptr<DoStatement> op = std::make_shared<DoStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            // Push a new label to the stack so the symbol table can remember these.
            context.m_labels.push_back(label);
            op->label = label;
            label = "";

            if (WHILE == nextToken.lexeme)
             {
               GNT();

               op->preCondition = expression(context);
             }
            else if (UNTIL == nextToken.lexeme)
             {
               GNT();

               std::shared_ptr<Not> neg = std::make_shared<Not>();
               neg->arg = expression(context);
               op->preCondition = neg;
             }

            expect(NEW_LINE);

            op->seq = std::make_shared<StatementSeq>();

            while (LOOP != nextToken.lexeme)
             {
               std::shared_ptr<Statement> statmnt = statement(context);

               if (nullptr != statmnt)
                  op->seq->statements.push_back(statmnt);

               expect(NEW_LINE);
             }
            GNT();

            if (WHILE == nextToken.lexeme)
             {
               GNT();

               op->postCondition = expression(context);
             }
            else if (UNTIL == nextToken.lexeme)
             {
               GNT();

               std::shared_ptr<Not> neg = std::make_shared<Not>();
               neg->arg = expression(context);
               op->postCondition = neg;
             }

            // Pop the label stack.
            context.m_labels.pop_back();
          }
         break;

      case BREAK:
          {
            std::shared_ptr<BreakStatement> op = std::make_shared<BreakStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }
          }
         break;

      case CONTINUE:
          {
            std::shared_ptr<BreakStatement> op = std::make_shared<BreakStatement>();
            op->toContinue = true;
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }
          }
         break;

      case WHILE:
          {
            std::shared_ptr<BreakStatement> op = std::make_shared<BreakStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }

            op->condition = expression(context);
          }
         break;

      case UNTIL:
          {
            std::shared_ptr<BreakStatement> op = std::make_shared<BreakStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (0 == context.m_labels.size())
               DB_panic("No enclosing loop on " + LN() + ".");

            if (COLON == nextToken.lexeme)
             {
               expect(COLON);
               op->label = nextToken.text;
               expect(IDENTIFIER);

               if (CallingContext::LABEL != context.lookup(op->label))
                  DB_panic("Use of undefined label \"" + op->label + "\" on " + LN() + ".");

               expect(COLON);
             }

            std::shared_ptr<Not> neg = std::make_shared<Not>();
            neg->arg = expression(context);
            op->condition = neg;
          }
         break;

      case RETURN:
          {
            std::shared_ptr<ReturnStatement> op = std::make_shared<ReturnStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (NEW_LINE != nextToken.lexeme)
             {
               op->value = expression(context);
             }
            else
             {
               std::shared_ptr<Constant> val = std::make_shared<Constant>();
               val->value = 0;
               op->value = val;
               val->lineNo = op->lineNo;
             }
          }
         break;

      case CALL:
          {
            std::shared_ptr<CallStatement> op = std::make_shared<CallStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (IDENTIFIER == nextToken.lexeme)
             {
               if (CallingContext::FUNCTION == context.lookup(nextToken.text))
                {
                  op->fun = primary(context);
                }
             }

            if (nullptr == op->fun)
               DB_panic("Invalid call to \"" + nextToken.text + "\" on " + LN() + ".");
          }
         break;

      case TAILCALL:
          {
            std::shared_ptr<TailCallStatement> op = std::make_shared<TailCallStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            if (NEW_LINE != nextToken.lexeme)
             {
               op->args.push_back(expression(context));

               while (SEMICOLON == nextToken.lexeme)
                {
                  GNT();

                  op->args.push_back(expression(context));
                }
             }

            if (context.Functions().find(context.Name)->second.size() != op->args.size())
               DB_panic("Wrong number of arguments in tailcall for \"" + context.Name + "\" on " + LN() + ".");
          }
         break;

      case ASM:
          {
            std::shared_ptr<AsmStatement> op = std::make_shared<AsmStatement>();
            ret = op;

            ret->lineNo = nextToken.lineNumber;
            GNT();

            op->line = nextToken.text;
            expect(STRING);
          }
         break;

      default: break;
    }

   if ("" != label)
      DB_panic("Invalid label on or before " + LN() + ".");

   return ret;
 }
