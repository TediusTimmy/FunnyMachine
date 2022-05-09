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

void VS_pushAddr (short addr);
void VS_pushVal (short val);
void VS_LDR (short addr);
void VS_popAddr (short addr);
void VS_pop (void);

class CallingContext;

class Expression
 {
public:
   size_t lineNo;
   virtual void emit(const CallingContext&) const = 0;
   virtual int evaluate(const CallingContext&) const = 0;
   virtual ~Expression() { }
 };

class Constant : public Expression
 {
public:
   int value;
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override { return value; }
 };

class Variable : public Expression
 {
public:
   std::string referent;
   int location;
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };



class BinaryOperation : public Expression
 {
public:
   std::shared_ptr<Expression> lhs, rhs;
 };

class OrOp : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class AndOp : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class XorOp : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShortOr : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShortAnd : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Equals : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class NotEquals : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Greater : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Less : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class GEQ : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class LEQ : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShiftLeft : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class ShiftRight : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class UnsignedShiftRight : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class RotateLeft : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class RotateRight : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Plus : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Minus : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Multiply : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Divide : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Remainder : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class DerefVar : public BinaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };



class UnaryOperation : public Expression
 {
public:
   std::shared_ptr<Expression> arg;
 };

class Abs : public UnaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Negate : public UnaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

class Not : public UnaryOperation
 {
public:
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };



class FunctionCall : public Expression
 {
public:
   std::vector<std::shared_ptr<Expression> > args;
   std::string name;
   void emit(const CallingContext&) const override;
   int evaluate(const CallingContext&) const override;
 };

#endif /* EXPRESSION_HPP */
