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

void beltVal(short val); // Result is 0
void beltVal(short val, const std::string& name); // Result is 0 and has name

class CallingContext;
class GlobalData;

class Expression
 {
public:
   size_t lineNo;
   virtual std::string emit(const CallingContext&, GlobalData&) const = 0;
   virtual int evaluate(const CallingContext&) const = 0;
   virtual bool canEvaluate (const CallingContext&) const = 0;
   virtual ~Expression() { }
 };

class Constant : public Expression
 {
public:
   int value;
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override { return value; }
   bool canEvaluate (const CallingContext&) const override { return true; }
 };

class Variable : public Expression
 {
public:
   std::string referent;
   int location;
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
   bool canEvaluate (const CallingContext&) const override;
 };



class BinaryOperation : public Expression
 {
public:
   std::shared_ptr<Expression> lhs, rhs;
   bool canEvaluate (const CallingContext& context) const override { return lhs->canEvaluate(context) && rhs->canEvaluate(context); };
 };

class OrOp : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class AndOp : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class XorOp : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShortOr : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShortAnd : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Equals : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class NotEquals : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Greater : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Less : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class GEQ : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class LEQ : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShiftLeft : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShiftRight : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class UnsignedShiftRight : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class RotateLeft : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class RotateRight : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Plus : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Minus : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Multiply : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Divide : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Remainder : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class DerefVar : public BinaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
   bool canEvaluate (const CallingContext&) const override;
 };



class UnaryOperation : public Expression
 {
public:
   std::shared_ptr<Expression> arg;
   bool canEvaluate (const CallingContext& context) const { return arg->canEvaluate(context); };
 };

class Abs : public UnaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Negate : public UnaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Not : public UnaryOperation
 {
public:
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
 };



class FunctionCall : public Expression
 {
public:
   std::vector<std::shared_ptr<Expression> > args;
   std::string name;
   std::string emit(const CallingContext&, GlobalData&) const override;
   int evaluate(const CallingContext&) const override;
   bool canEvaluate (const CallingContext&) const override { return false; }
 };

#endif /* EXPRESSION_HPP */
