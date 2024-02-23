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
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <memory>

void beltVal(short val);

class CallingContext;
class GlobalData;

class Expression
 {
public:
   size_t lineNo;
    // Emit either stack or belt code, but always push the result.
   void emitPush(const CallingContext&, GlobalData&) const;
    // Emit belt code, but check for a constant.
   std::string emitNext(const CallingContext&, GlobalData&) const;
    // Emit either stack or belt code, but always return a result.
    // This should only be called by Statements.
   std::string emitResult(const CallingContext&, GlobalData&) const;
    // Emit only stack code
   virtual void emitStack(const CallingContext&, GlobalData&) const = 0;
    // Emit only belt code
   virtual std::string emitBelt(const CallingContext&, GlobalData&) const = 0;
    // Count the number of results needed to generate this result, assuming all belt code
   static int valueResults(int value) { if (((value > 0x7FF) || (value < -0x800)) && (0 != (value & 0xFFF))) return 2; return 1; }
   virtual int beltResults(const CallingContext&) const = 0;
    // arg1->canBelt() && arg2->canBelt() && (arg2->beltResults() < 14)
    // only false on function call
   virtual bool canBelt (const CallingContext&) const = 0;
   virtual int evaluate(const CallingContext&) const = 0;
   virtual bool canEvaluate (const CallingContext&) const = 0;
   virtual ~Expression() { }
 };

class Constant : public Expression
 {
public:
   int value;
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override { return valueResults(value); }
   bool canBelt (const CallingContext&) const override { return true; }
   int evaluate(const CallingContext&) const override { return value; }
   bool canEvaluate (const CallingContext&) const override { return true; }
 };

class Variable : public Expression
 {
public:
   std::string referent;
   int location;
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   bool canBelt (const CallingContext&) const override { return true; }
   int evaluate(const CallingContext&) const override;
   bool canEvaluate (const CallingContext&) const override;
 };



class BinaryOperation : public Expression
 {
public:
   std::shared_ptr<Expression> lhs, rhs;
   int beltResults(const CallingContext& context) const override
    {
      if (true == canEvaluate(context)) return valueResults(evaluate(context));
      return lhs->beltResults(context) + rhs->beltResults(context) + 1U;
    }
   bool canBelt (const CallingContext& context) const override { return lhs->canBelt(context) && rhs->canBelt(context) && (rhs->beltResults(context) < 14); }
   bool canEvaluate (const CallingContext& context) const override { return lhs->canEvaluate(context) && rhs->canEvaluate(context); };
 };

class OrOp : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class AndOp : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class XorOp : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShortOr : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShortAnd : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Equals : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class NotEquals : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Greater : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Less : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class GEQ : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class LEQ : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShiftLeft : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShiftRight : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class UnsignedShiftRight : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class RotateLeft : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class RotateRight : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Plus : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Minus : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Multiply : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Divide : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext& context) const override
    {
      if (true == canEvaluate(context)) return valueResults(evaluate(context));
      return lhs->beltResults(context) + rhs->beltResults(context) + 2;
    }
   int evaluate(const CallingContext&) const override;
 };

class Remainder : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class DerefVar : public BinaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext& context) const override
    {
      if (true == canEvaluate(context)) return valueResults(evaluate(context));
      return lhs->beltResults(context) + rhs->beltResults(context) + 3;
    }
    // One extra because we shift RHS first
   bool canBelt (const CallingContext& context) const override { return lhs->canBelt(context) && rhs->canBelt(context) && (rhs->beltResults(context) < 13); }
   int evaluate(const CallingContext&) const override;
   bool canEvaluate (const CallingContext&) const override;
 };



class UnaryOperation : public Expression
 {
public:
   std::shared_ptr<Expression> arg;
   int beltResults(const CallingContext& context) const override
    {
      if (true == canEvaluate(context)) return valueResults(evaluate(context));
      return arg->beltResults(context) + 1;
    }
   bool canBelt (const CallingContext& context) const override { return arg->canBelt(context); }
   bool canEvaluate (const CallingContext& context) const { return arg->canEvaluate(context); };
 };

class Abs : public UnaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int beltResults(const CallingContext& context) const override
    {
      if (true == canEvaluate(context)) return valueResults(evaluate(context));
      return arg->beltResults(context) + 4;
    }
   int evaluate(const CallingContext&) const override;
 };

class Negate : public UnaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Not : public UnaryOperation
 {
public:
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };



class FunctionCall : public Expression
 {
public:
   std::vector<std::shared_ptr<Expression> > args;
   std::string name;
   void emitStack(const CallingContext&, GlobalData&) const override;
   std::string emitBelt(const CallingContext&, GlobalData&) const override; // Die if called.
   int beltResults(const CallingContext&) const override; // Die if called.
   bool canBelt (const CallingContext&) const override { return false; }
   int evaluate(const CallingContext&) const override; // Die if called.
   bool canEvaluate (const CallingContext&) const override { return false; }
 };

#endif /* EXPRESSION_HPP */
