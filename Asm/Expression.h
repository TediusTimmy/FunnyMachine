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
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "SymbolTable.h"
#include <string>
#include <memory>
#include <vector>

class Expression
 {
   public:
      size_t lineNo;

      virtual int evaluate (const SymbolTable&) const = 0;
      virtual bool canEvaluate (const SymbolTable&) const = 0;
      virtual ~Expression() { }
 };

class Constant : public Expression
 {
   public:
      int value;

      int evaluate (const SymbolTable&) const { return value; }
      bool canEvaluate (const SymbolTable&) const { return true; }
 };

class Label : public Expression
 {
   public:
      std::string referent;

      int evaluate (const SymbolTable& context) const { return context.getLabel(referent) - context.getUseLocation(); }
      bool canEvaluate (const SymbolTable& context) const { return context.hasLabel(referent); }
 };

class BinaryOperation : public Expression
 {
   public:
      std::shared_ptr<Expression> lhs, rhs;

      int evaluate (const SymbolTable&) const = 0;
      bool canEvaluate (const SymbolTable& context) const { return lhs->canEvaluate(context) && rhs->canEvaluate(context); };
      BinaryOperation() { }
      ~BinaryOperation() { }
 };

class Multiply : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Divide : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Mod : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class ShiftLeft : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class ShiftRight : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class UnsignedShiftRight : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Plus : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Minus : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class BitAnd : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class BitOr : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class BitXor : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Equals : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class NotEquals : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Greater : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Less : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class GEQ : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class LEQ : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class ShortOr : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class ShortAnd : public BinaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };



class UnaryOperation : public Expression
 {
   public:
      std::shared_ptr<Expression> arg;
      int evaluate (const SymbolTable&) const = 0;
      bool canEvaluate (const SymbolTable& context) const { return arg->canEvaluate(context); };
      UnaryOperation() { }
      ~UnaryOperation() { }
 };

class ValueOf : public UnaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Abs : public UnaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Negate : public UnaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class BitNot : public UnaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class HighByte : public UnaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class LowByte : public UnaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

class Not : public UnaryOperation
 {
   public:
      int evaluate (const SymbolTable&) const;
 };

#endif /* EXPRESSION_H */
