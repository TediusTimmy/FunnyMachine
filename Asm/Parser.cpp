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
#include "Parser.h"

#include "SymbolTable.h"
#include "Expression.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <algorithm>

class ParserException : public std::exception
 {
private:
   std::string message;

public:
   ParserException(const std::string& message) : message(message) { }
   ~ParserException() noexcept { }
   const char * what() const noexcept { return message.c_str(); }
 };

static int InterpretNumber(const std::string& val)
 {
   if ('%' == val[0])
    {
      return std::stoi(val.substr(1, std::string::npos), nullptr, 2);
    }
   else if ('$' == val[0])
    {
      return std::stoi(val.substr(1, std::string::npos), nullptr, 16);
    }
   else if ('\'' == val[0])
    {
      return val[1] | ((val.size() > 2) ? ((int)((unsigned char)val[2])) << 8 : 0);
    }
   return std::stod(val);
 }

void Parser::GNT (void)
 {
   nextToken = src.GetNextToken();
 }

void Parser::expect (const Lexeme & tok)
 {
   if (tok != nextToken.lexeme)
    {
      std::stringstream str;
      str << "Token \"" << nextToken.text << "\" was not expected on " << nextToken.lineNumber << ".";
      throw ParserException(str.str());
    }
   GNT();
 }

std::shared_ptr<Expression> Parser::expression (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> ret;

   if (BOOL_NOT == nextToken.lexeme)
    {
      std::shared_ptr<Not> temp = std::make_shared<Not>();
      temp->lineNo = nextToken.lineNumber;

      GNT();

      temp->arg = expression(context, isResult);
      ret = temp;
    }
   else
    {
      ret = disjunction(context, isResult);
    }

   return ret;
 }

std::shared_ptr<Expression> Parser::disjunction (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> lhs (conjunction(context, isResult));

   while (SHORT_OR == nextToken.lexeme)
    {
      std::shared_ptr<ShortOr> temp = std::make_shared<ShortOr>();

      temp->lineNo = nextToken.lineNumber;
      temp->lhs = lhs;
      GNT();

      temp->rhs = conjunction(context, isResult);
      lhs = temp;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::conjunction (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> lhs (relation(context, isResult));

   while (SHORT_AND == nextToken.lexeme)
    {
      std::shared_ptr<ShortAnd> temp = std::make_shared<ShortAnd>();

      temp->lineNo = nextToken.lineNumber;
      temp->lhs = lhs;
      GNT();

      temp->rhs = relation(context, isResult);
      lhs = temp;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::relation (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> lhs (bitwise(context, isResult));

   while ((EQUALITY == nextToken.lexeme) || (INEQUALITY == nextToken.lexeme) || 
          (GREATER_THAN == nextToken.lexeme) || (LESS_THAN_OR_EQUAL_TO == nextToken.lexeme) ||
          (GREATER_THAN_OR_EQUAL_TO == nextToken.lexeme) || (LESS_THAN == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> temp;
      switch(nextToken.lexeme)
       {
      case EQUALITY:
         temp = std::make_shared<Equals>();
         break;
      case INEQUALITY:
         temp = std::make_shared<NotEquals>();
         break;
      case GREATER_THAN:
         temp = std::make_shared<Greater>();
         break;
      case LESS_THAN:
         temp = std::make_shared<Less>();
         break;
      case GREATER_THAN_OR_EQUAL_TO:
         temp = std::make_shared<GEQ>();
         break;
      case LESS_THAN_OR_EQUAL_TO:
         temp = std::make_shared<LEQ>();
         break;
      default: break;
       }

      temp->lhs = lhs;
      temp->lineNo = nextToken.lineNumber;
      GNT();

      temp->rhs = bitwise(context, isResult);
      lhs = temp;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::bitwise (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> lhs (simple(context, isResult));

   while ((AND == nextToken.lexeme) || (OR == nextToken.lexeme) || (XOR == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> temp;
      switch(nextToken.lexeme)
       {
      case AND:
         temp = std::make_shared<BitAnd>();
         break;
      case OR:
         temp = std::make_shared<BitOr>();
         break;
      case XOR:
         temp = std::make_shared<BitXor>();
         break;
      default: break;
       }

      temp->lhs = lhs;
      temp->lineNo = nextToken.lineNumber;
      GNT();

      temp->rhs = simple(context, isResult);
      lhs = temp;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::simple (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> lhs (term(context, isResult));

   while ((PLUS == nextToken.lexeme) || (MINUS == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> temp;
      switch(nextToken.lexeme)
       {
      case PLUS:
         temp = std::make_shared<Plus>();
         break;
      case MINUS:
         temp = std::make_shared<Minus>();
         break;
      default: break;
       }

      temp->lhs = lhs;
      temp->lineNo = nextToken.lineNumber;
      GNT();

      temp->rhs = term(context, isResult);
      lhs = temp;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::term (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> lhs (unary(context, isResult));

   while ((MULTIPLY == nextToken.lexeme) || (DIVIDE == nextToken.lexeme) || (MOD == nextToken.lexeme) ||
      (SHIFT_LEFT == nextToken.lexeme) || (SHIFT_RIGHT == nextToken.lexeme) || (UNSIGNED_SHIFT_RIGHT == nextToken.lexeme))
    {
      std::shared_ptr<BinaryOperation> temp;
      switch(nextToken.lexeme)
       {
      case MULTIPLY:
         temp = std::make_shared<Multiply>();
         break;
      case DIVIDE:
         temp = std::make_shared<Divide>();
         break;
      case MOD:
         temp = std::make_shared<Mod>();
         break;
      case SHIFT_LEFT:
         temp = std::make_shared<ShiftLeft>();
         break;
      case SHIFT_RIGHT:
         temp = std::make_shared<ShiftRight>();
         break;
      case UNSIGNED_SHIFT_RIGHT:
         temp = std::make_shared<UnsignedShiftRight>();
         break;
      default: break;
       }

      temp->lhs = lhs;
      temp->lineNo = nextToken.lineNumber;
      GNT();

      temp->rhs = unary(context, isResult);
      lhs = temp;
    }

   return lhs;
 }

std::shared_ptr<Expression> Parser::unary (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> ret;

   if ((PLUS == nextToken.lexeme) || (MINUS == nextToken.lexeme) || (NOT == nextToken.lexeme) ||
      (GREATER_THAN == nextToken.lexeme) || (LESS_THAN == nextToken.lexeme) || (LOCAL_REF == nextToken.lexeme))
    {
      std::shared_ptr<UnaryOperation> temp;
      switch(nextToken.lexeme)
       {
      case PLUS:
          temp = std::make_shared<Abs>();
         break;
      case MINUS:
          temp = std::make_shared<Negate>();
         break;
      case NOT:
          temp = std::make_shared<BitNot>();
         break;
      case GREATER_THAN:
          temp = std::make_shared<HighByte>();
         break;
      case LESS_THAN:
          temp = std::make_shared<LowByte>();
         break;
      case LOCAL_REF:
          temp = std::make_shared<ValueOf>();
         break;
      default: break;
       }

      temp->lineNo = nextToken.lineNumber;
      GNT();

      temp->arg = unary(context, isResult);
      ret = temp;
    }
   else
    {
      ret = primary(context, isResult);
    }

   return ret;
 }

std::shared_ptr<Expression> Parser::primary (const SymbolTable& context, bool isResult)
 {
   std::shared_ptr<Expression> ret;

   switch(nextToken.lexeme)
    {
   case IDENTIFIER:
      if (true == isResult)
       {
         if (true == context.hasResult(nextToken.text))
          {
            std::shared_ptr<Constant> op = std::make_shared<Constant>();
            op->value = context.getCurrentResult() - context.getResult(nextToken.text);
            if (op->value > 14)
             {
               std::cerr << "Use of result \"" << nextToken.text << "\" that has left the belt on " << nextToken.lineNumber << "." << std::endl;
             }

            op->lineNo = nextToken.lineNumber;
            GNT();

            ret = op;
          }
         else if (true == context.hasSymbol(nextToken.text))
          {
            std::shared_ptr<Constant> op = std::make_shared<Constant>();
            op->value = context.getSymbol(nextToken.text);

            op->lineNo = nextToken.lineNumber;
            GNT();

            ret = op;
          }
         else // Can't reference results that haven't happened.
          {
            std::stringstream str;
            str << "Undefined result \"" << nextToken.text << "\" used in expression on " << nextToken.lineNumber << ".";
            throw ParserException(str.str());
          }
       }
      else
       {
         if (true == context.hasSymbol(nextToken.text))
          {
            std::shared_ptr<Constant> op = std::make_shared<Constant>();
            op->value = context.getSymbol(nextToken.text);

            op->lineNo = nextToken.lineNumber;
            GNT();

            ret = op;
          }
         else
          {
            std::shared_ptr<Label> op = std::make_shared<Label>();
            op->referent = nextToken.text;

            op->lineNo = nextToken.lineNumber;
            GNT();

            ret = op;
          }
       }
      break;
   case NUMBER:
    {
      std::shared_ptr<Constant> op = std::make_shared<Constant>();
      op->value = InterpretNumber(nextToken.text);

      op->lineNo = nextToken.lineNumber;
      GNT();

      ret = op;
    }
      break;
   case MULTIPLY:
    {
      std::shared_ptr<Constant> op = std::make_shared<Constant>();
      op->value = context.getUseLocation() - 2;

      op->lineNo = nextToken.lineNumber;
      GNT();

      ret = op;
    }
      break;
   case OPEN_PARENS:
      GNT();
      ret = expression(context, isResult);
      expect(CLOSE_PARENS);
      break;
   case STRING: // Is not valid in this context.
   default:
    {
      std::stringstream str;
      str << "Expected primary expression, found \"" << nextToken.text << "\" on " << nextToken.lineNumber << ".";
      throw ParserException(str.str());
    }
    }

   return ret;
 }

std::map<std::string, int> Parser::args = {
   { "NOP",   0 }, // 0 : Zero arguments
   { "LD",    1 }, // 1 : One RESULT argument
   { "LDB",   1 },
   { "ST",    2 }, // 2 : Two RESULT arguments
   { "STB",   2 },
   { "LDI",   3 }, // 3 : One 12-bit immediate argument
   { "LRA",   3 }, // As there are only two of these, we'll deal with the shift there
   { "ADD",   2 },
   { "ADC",   2 },
   { "SUB",   2 },
   { "SBB",   2 },
   { "SHL",   2 },
   { "SHR",   2 },
   { "ASR",   2 },
   { "AND",   2 },
   { "OR",    2 },
   { "XOR",   2 },
   { "NAND",  2 },
   { "NOR",   2 },
   { "XNOR",  2 },
   { "ROL",   2 },
   { "ROR",   2 },
   { "BADD",  2 },
   { "BADC",  2 },
   { "BSUB",  2 },
   { "BSBB",  2 },
   { "MUL",   2 },
   { "UMUL",  2 },
   { "DIV",   2 },
   { "UDIV",  2 },
   { "RETZ",  2 },
   { "RETN",  2 },
   { "RETO",  2 },
   { "RETP",  2 },
   { "RETC",  2 },
   { "RETZP", 2 },
   { "RETE",  2 },
   { "RETZN", 2 },
   { "BRZ",   4 }, // 4 : One RESULT argument, one 8-bit immediate argument
   { "BRN",   4 },
   { "BRO",   4 },
   { "BRP",   4 },
   { "BR",    4 },
   { "BRZP",  4 },
   { "BRE",   4 },
   { "BRZN",  4 },
   { "DEC",   1 },
   { "INC",   1 },
   { "NEG",   1 },
   { "NOT",   1 },
   { "BRA",   4 }, // As there is only this one, we'll deal with the implicit argument there.
   { "RET",   1 }
 };

std::map<std::string, int> Parser::opcodes = {
   { "NOP",   0 },
   { "LD",    0x11 },
   { "LDB",   0x01 },
   { "ST",    0x12 },
   { "STB",   0x02 },
   { "LDI",   3 },
   { "LRA",   4 },
   { "ADD",   0x05 },
   { "ADC",   0x15 },
   { "SBB",   0x25 },
   { "SUB",   0x35 },
   { "SHL",   0x45 },
   { "SHR",   0x65 },
   { "ASR",   0x75 },
   { "AND",   0x85 },
   { "OR",    0x95 },
   { "XOR",   0xa5 },
   { "NAND",  0xc5 },
   { "NOR",   0xd5 },
   { "XNOR",  0xe5 },
   { "ROL",   0xb5 },
   { "ROR",   0xf5 },
   { "BADD",  0x06 },
   { "BADC",  0x16 },
   { "BSBB",  0x26 },
   { "BSUB",  0x36 },
   { "MUL",   0x46 },
   { "UMUL",  0x56 },
   { "DIV",   0x66 },
   { "UDIV",  0x76 },
   { "RETZ",  0x07 },
   { "RETN",  0x17 },
   { "RETO",  0x27 },
   { "RETP",  0x37 },
   { "RETC",  0x47 },
   { "RETZP", 0x57 },
   { "RETE",  0x67 },
   { "RETZN", 0x77 },
   { "BRZ",   8 },
   { "BRN",   9 },
   { "BRO",   10 },
   { "BRP",   11 },
   { "BR",    12 },
   { "BRZP",  13 },
   { "BRE",   14 },
   { "BRZN",  15 },
   { "INC",   0xf015 },
   { "DEC",   0xf025 },
   { "NEG",   0xf35 },
   { "NOT",   0xf0d5 },
   { "BRA",   0xf8 },
   { "RET",   0xf07 }
 };

std::map<std::string, int> Parser::results = {
   { "NOP",   0 },
   { "LD",    1 },
   { "LDB",   1 },
   { "ST",    0 },
   { "STB",   0 },
   { "LDI",   1 },
   { "LRA",   1 },
   { "ADD",   1 },
   { "ADC",   1 },
   { "SUB",   1 },
   { "SBB",   1 },
   { "SHL",   1 },
   { "SHR",   1 },
   { "ASR",   1 },
   { "AND",   1 },
   { "OR",    1 },
   { "XOR",   1 },
   { "NAND",  1 },
   { "NOR",   1 },
   { "XNOR",  1 },
   { "ROL",   1 },
   { "ROR",   1 },
   { "BADD",  1 },
   { "BADC",  1 },
   { "BSUB",  1 },
   { "BSBB",  1 },
   { "MUL",   2 },
   { "UMUL",  2 },
   { "DIV",   2 },
   { "UDIV",  2 },
   { "RETZ",  0 },
   { "RETN",  0 },
   { "RETO",  0 },
   { "RETP",  0 },
   { "RETC",  0 },
   { "RETZP", 0 },
   { "RETE",  0 },
   { "RETZN", 0 },
   { "BRZ",   0 },
   { "BRN",   0 },
   { "BRO",   0 },
   { "BRP",   0 },
   { "BR",    0 },
   { "BRZP",  0 },
   { "BRE",   0 },
   { "BRZN",  0 },
   { "DEC",   1 },
   { "INC",   1 },
   { "NEG",   1 },
   { "NOT",   1 },
   { "BRA",   0 },
   { "RET",   0 }
 };

static std::string TO_UPPER(std::string str)
 {
   std::transform(str.begin(), str.end(), str.begin(), ::toupper);
   return str;
 }

class RefContainer
 {
public:
   int codeLocation;
   int type; // Should be an enum.
   std::shared_ptr<Expression> expr;
   bool done;

   RefContainer() = default;
   RefContainer(const RefContainer&) = default;
   RefContainer(RefContainer&&) = default;
   RefContainer(int codeLocation, int type, std::shared_ptr<Expression> expr) : codeLocation(codeLocation), type(type), expr(expr), done(false) { }
   RefContainer& operator= (const RefContainer&) = default;
   RefContainer& operator= (RefContainer&&) = default;
 };

static void processReferences(SymbolTable& context, std::vector<unsigned short>& result, std::list<RefContainer>& references)
 {
   for (auto& ref : references)
    {
      if (true == ref.expr->canEvaluate(context))
       {
         switch (ref.type)
          {
         case 1: // LDI 12-bit immediate
          {
            context.setUseLocation(ref.codeLocation);
            int arg = ref.expr->evaluate(context);
            if ((arg > 4095) || (arg < -2048))
             {
               std::cerr << "Immediate offset exceeds field on " << ref.expr->lineNo << " : " << arg << std::endl;
             }
            result[ref.codeLocation / 2 - 1] |= (arg & 0xfff) << 4;
          }
            break;
         case 2: // LRA 12-bit immediate code location
          {
            context.setUseLocation(ref.codeLocation);
            int arg = ref.expr->evaluate(context);
            if ((arg > 4094) || (arg < -4096))
             {
               std::cerr << "Relative offset exceeds field on " << ref.expr->lineNo << " : " << arg << std::endl;
             }
            result[ref.codeLocation / 2 - 1] |= ((arg >> 1) & 0xfff) << 4;
          }
            break;
         case 3: // BR 8-bit immediate
          {
            context.setUseLocation(ref.codeLocation);
            int arg = ref.expr->evaluate(context);
            if ((arg > 254) || (arg < -256))
             {
               std::cerr << "BRANCH offset exceeds field on " << ref.expr->lineNo << " : " << arg << std::endl;
             }
            result[ref.codeLocation / 2 - 1] |= ((arg >> 1) & 0xff) << 8;
          }
            break;
          }
         ref.done = true;
       }
    }
   references.remove_if([](const RefContainer& a){ return a.done; });
 }

void addByte(int& temp, int& count, unsigned char add, std::vector<unsigned short>& result, SymbolTable& context)
 {
   temp |= (add << (8 * count));
   ++count;
   if (2 == count)
    {
      context.addLocation();
      if (static_cast<int>(result.size()) < (context.getCurrentLocation() / 2))
       {
         result.resize(context.getCurrentLocation() / 2);
       }
      result[context.getCurrentLocation() / 2 - 1] = temp;
      temp = 0;
      count = 0;
    }
 }

std::vector<unsigned short> Parser::assembly()
 {
   SymbolTable context;
   std::vector<unsigned short> result;
   std::list<RefContainer> references;

   while (END_OF_FILE != nextToken.lexeme)
    {
      // Cache the results to update so that we can use the prior state of results in the current opcode.
      std::map<std::string, int> resultsToUpdate;

      while (END_OF_LINE == nextToken.lexeme)
       {
         GNT();
       }

      if ((AT != nextToken.lexeme) && (IDENTIFIER != nextToken.lexeme) && (PERIOD != nextToken.lexeme) &&
          (POUND != nextToken.lexeme) && (MOD != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
       {
         std::cerr << "Expected assembly statement, found \"" << nextToken.text << "\" on " << nextToken.lineNumber << "." << std::endl;
         while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
          {
            GNT();
          }
       }

         // In general, a line of assembly has one of these two formats:
         //    [label]  [result] opcode
         //    assembler directive
         // So, we'll do this in three passes
      switch(nextToken.lexeme)
       {
      case AT: // Either a local label or a result
       {
         GNT();
         if (IDENTIFIER != nextToken.lexeme) // If this isn't an identifier, then error.
          {
            std::cerr << "Expected identifier, found \"" << nextToken.text << "\" on " << nextToken.lineNumber << "." << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         else
          {
            std::string name = nextToken.text;
            GNT();
            if (COLON == nextToken.lexeme) // This is a local label.
             {
               if (true == context.hasLabel(name))
                {
                  std::cerr << "Redefinition of label \"" << name << "\" on " << nextToken.lineNumber << "." << std::endl;
                }
               if (true == context.hasSymbol(name))
                {
                  std::cerr << "Definition of label \"" << name << "\" on " << nextToken.lineNumber << " will mask symbol of the same name." << std::endl;
                }
               context.setLabel(name, context.getCurrentLocation());
               context.addLocalLabel(name);
               expect(COLON);
               processReferences(context, result, references);
             }
            else // Maybe there is garbage, but assume this is a result.
             {
               if (true == context.hasSymbol(name))
                {
                  std::cerr << "Definition of result \"" << name << "\" on " << nextToken.lineNumber << " will mask symbol of the same name." << std::endl;
                }
               resultsToUpdate.insert(std::make_pair(name, context.getCurrentResult() + 1)); // Assign this name to the next result, clobbering any prior use.
             }
          }
       }
         break;
      case IDENTIFIER: // Either a label or a symbol
         // If this IS an opcode, defer to pass three
         if (opcodes.end() == opcodes.find(TO_UPPER(nextToken.text)))
          {
            std::string name = nextToken.text;
            GNT();
            if (COLON == nextToken.lexeme)
             {
               if (true == context.hasLabel(name))
                {
                  std::cerr << "Redefinition of label \"" << name << "\" on " << nextToken.lineNumber << "." << std::endl;
                }
               if (true == context.hasSymbol(name))
                {
                  std::cerr << "Definition of label \"" << name << "\" on " << nextToken.lineNumber << " will mask symbol of the same name." << std::endl;
                }
               context.setLabel(name, context.getCurrentLocation());
               expect(COLON);
               processReferences(context, result, references);
               context.flushLocalLabels();
             }
            else if (EQUALITY == nextToken.lexeme)
             {
               if ((true == context.hasLabel(name)) || (true == context.hasResult(name)))
                {
                  std::cerr << "Predefined label or result will mask symbol \"" << name << "\" on " << nextToken.lineNumber << "." << std::endl;
                }
               if (true == context.hasSymbol(name))
                {
                  std::cerr << "Redefinition of symbol \"" << name << "\" on " << nextToken.lineNumber << "." << std::endl;
                }
               expect(EQUALITY); // What commie wrote this line of code?
               try
                {
                  // Symbols should be able to reference labels, but that is a WHOLE other ball of wax.
                  int val = expression(context, true)->evaluate(context);
                  context.setSymbol(name, val);
                }
               catch (const ParserException& ex)
                {
                  std::cerr << ex.what() << std::endl;
                  while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
                   {
                     GNT();
                   }
                }
             }
            else
             {
               std::cerr << "Expected a label declaration ':' or symbol declaration '=' but found \"" << nextToken.text << "\" on " << nextToken.lineNumber << "." << std::endl;
               while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
                {
                  GNT();
                }
             }
          }
         break;
      case PERIOD: // Assembler directive
         GNT();
         if ("dw" == nextToken.text)
          {
            do
             {
               GNT();
               try
                {
                  context.addLocation();
                  context.setUseLocation(context.getCurrentLocation());
                  int value = expression(context, false)->evaluate(context);
                  if (static_cast<int>(result.size()) < (context.getCurrentLocation() / 2))
                   {
                     result.resize(context.getCurrentLocation() / 2);
                   }
                  result[context.getCurrentLocation() / 2 - 1] = value;
                  if ((value > 65535) || (value < -32768))
                   {
                     std::cerr << "Word value out of range on " << nextToken.lineNumber << "." << std::endl;
                   }
                }
               catch (const ParserException& ex)
                {
                  std::cerr << ex.what() << std::endl;
                  while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
                   {
                     GNT();
                   }
                }
             }
            while (COMMA == nextToken.lexeme);
          }
         else if ("assert" == nextToken.text)
          {
            GNT();
            try
             {
               int assertion = expression(context, false)->evaluate(context);

               if (0 == assertion)
                {
                  std::cerr << "Assertion failed on line " << nextToken.lineNumber << "." << std::endl;
                }
             }
            catch (const ParserException& ex)
             {
               std::cerr << ex.what() << std::endl;
               while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
                {
                  GNT();
                }
             }
          }
         else if ("db" == nextToken.text)
          {
            int temp = 0, count = 0;
            do
             {
               GNT();
               if (STRING == nextToken.lexeme)
                {
                  for (size_t i = 0U; i < nextToken.text.length(); ++i)
                   {
                     addByte(temp, count, static_cast<unsigned char>(nextToken.text[i]), result, context);
                   }
                  GNT();
                }
               else if (NUMBER == nextToken.lexeme)
                {
                  int value = InterpretNumber(nextToken.text);
                  GNT();
                  if ((value > 255) || (value < -128))
                   {
                     std::cerr << "Word value out of range on " << nextToken.lineNumber << "." << std::endl;
                   }
                  addByte(temp, count, static_cast<unsigned char>(value), result, context);
                }
               else
                {
                  std::cerr << "Expected string or number on line " << nextToken.lineNumber << "." << std::endl;
                  while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
                   {
                     GNT();
                   }
                }
             }
            while (COMMA == nextToken.lexeme);
            if (0 != count)
             {
               addByte(temp, count, 0U, result, context);
             }
          }
         else if ("org" == nextToken.text)
          {
            GNT();
            try
             {
               int location = expression(context, false)->evaluate(context);

               if (location > 2097152)
                {
                  std::cerr << "Org directive creates a rom that is too large at " << nextToken.lineNumber << "." << std::endl;
                }
               if (location & 1)
                {
                  location &= ~1;
                  std::cerr << "Org directive to odd address at " << nextToken.lineNumber << "." << std::endl;
                }

               context.setCurrentLocation(location);
             }
            catch (const ParserException& ex)
             {
               std::cerr << ex.what() << std::endl;
               while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
                {
                  GNT();
                }
             }
          }
         else
          {
            std::cerr << "Expected an assembler directive but found \"" << nextToken.text << "\" on " << nextToken.lineNumber << "." << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         break;
      default: break;
       }

      switch(nextToken.lexeme)
       {
      case AT: // Must be a result
         GNT();
         if (IDENTIFIER != nextToken.lexeme) // If this isn't an identifier, then error.
          {
            std::cerr << "Expected identifier, found \"" << nextToken.text << "\" on " << nextToken.lineNumber << "." << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         else
          {
            // Don't check for symbol existence in the next two cases, because both of them imply that a result by that name exists.
            if (true == context.hasSymbol(nextToken.text))
             {
               std::cerr << "Definition of result \"" << nextToken.text << "\" on " << nextToken.lineNumber << " will mask symbol of the same name." << std::endl;
             }
            resultsToUpdate.insert(std::make_pair(nextToken.text, context.getCurrentResult() + 1)); // Assign this name to the next result, clobbering any prior use.
            GNT();
          }
         break;
      case POUND:
         GNT();
         if (IDENTIFIER != nextToken.lexeme) // If this isn't an identifier, then error.
          {
            std::cerr << "Expected identifier, found \"" << nextToken.text << "\" on " << nextToken.lineNumber << "." << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         else
          {
            if ((false == context.hasResult(nextToken.text)) ||
               (context.getCurrentResult() + 1 - context.getResult(nextToken.text) < 15))
             {
               std::cerr << "Use of result name \"" << nextToken.text << "\" on " << nextToken.lineNumber << " while previous result by that name is still on the belt." << std::endl;
             }
            resultsToUpdate.insert(std::make_pair(nextToken.text, context.getCurrentResult() + 1));
            GNT();
          }
         break;
      case MOD:
         GNT();
         if (IDENTIFIER != nextToken.lexeme) // If this isn't an identifier, then error.
          {
            std::cerr << "Expected identifier, found \"" << nextToken.text << "\" on " << nextToken.lineNumber << "." << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         else
          {
            if ((false == context.hasResult(nextToken.text)) ||
               (context.getCurrentResult() + 1 - context.getResult(nextToken.text) > 14))
             {
               std::cerr << "Use of result name \"" << nextToken.text << "\" on " << nextToken.lineNumber << " while previous result by that name has left the belt." << std::endl;
             }
            resultsToUpdate.insert(std::make_pair(nextToken.text, context.getCurrentResult() + 1));
            GNT();
          }
         break;
      default: break;
       }

      switch(nextToken.lexeme)
       {
      case IDENTIFIER:
         if (opcodes.end() != opcodes.find(TO_UPPER(nextToken.text)))
          {
            int resultCount = results[TO_UPPER(nextToken.text)];
            context.addLocation();
            context.setUseLocation(context.getCurrentLocation());
            if (static_cast<int>(result.size()) < (context.getCurrentLocation() / 2))
             {
               result.resize(context.getCurrentLocation() / 2);
             }
            result[context.getCurrentLocation() / 2 - 1] = instruction(context, references);
            if (0 != resultCount)
             {
               context.addResult(resultCount - 1);
             }
          }
         break;
      default: break;
       }

      for (auto pair : resultsToUpdate)
       {
         context.setResult(pair.first, pair.second);
       }
    }

   for (auto& ref : references)
    {
      std::set<std::string> refd;
      ref.expr->cantEvaluate(context, refd);
      std::cerr << "ERROR: Missing " << ((refd.size() < 2) ? "reference" : "references") << " on line " << ref.expr->lineNo << " to ";
      bool first = true;
      for (const auto& str : refd)
       {
         if (true == first)
          {
            first = false;
          }
         else
          {
            std::cerr << ", ";
          }
         std::cerr << str;
       }
      std::cerr << std::endl;
    }

   return result;
 }

unsigned short Parser::instruction (const SymbolTable& context, std::list<RefContainer>& references)
 {
   unsigned int ret = 0U; // NOP

   switch(nextToken.lexeme)
    {
   case IDENTIFIER:
    {
      std::string operation = TO_UPPER(nextToken.text);
      size_t lineNo = nextToken.lineNumber;
      GNT();

      std::map<std::string, int>::const_iterator iter = args.find(operation);
      if (iter == args.end())
       {
         std::stringstream str;
         str << "We shouldn't have got here, on " << lineNo << ".";
         throw ParserException(str.str());
       }

      switch (iter->second)
       {
      case 0:
         ret = opcodes[operation];
         break;
      case 1: // One result
         try
          {
            ret = opcodes[operation];

            int arg = expression(context, true)->evaluate(context);

            if (arg > 15)
             {
               std::cerr << "Specified result " << arg << " that has left the belt on " << nextToken.lineNumber << "." << std::endl;
             }

            if (0 == (ret & 0xf00)) // Normal opcode
             {
               ret |= (arg & 15) << 8;
             }
            else // NEG or RET
             {
               ret |= (arg & 15) << 12;
             }
          }
         catch (const ParserException& ex)
          {
            std::cerr << ex.what() << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         break;
      case 2: // Two results
         try
          {
            ret = opcodes[operation];

            int lhs = expression(context, true)->evaluate(context);
            expect(COMMA);
            int rhs = expression(context, true)->evaluate(context);

            if (lhs > 15)
             {
               std::cerr << "Specified result " << lhs << " that has left the belt on " << nextToken.lineNumber << "." << std::endl;
             }
            if (rhs > 15)
             {
               std::cerr << "Specified result " << rhs << " that has left the belt on " << nextToken.lineNumber << "." << std::endl;
             }

            ret |= ((lhs & 15) << 8) | ((rhs & 15) << 12);
          }
         catch (const ParserException& ex)
          {
            std::cerr << ex.what() << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         break;
      case 3: // 12-bit immediate
         try
          {
            ret = opcodes[operation];

            std::shared_ptr<Expression> expr = expression(context, false);
            int arg = 0;
            if (true == expr->canEvaluate(context))
             {
               arg = expr->evaluate(context);
               if (3 == ret)
                {
                  if ((arg > 4095) || (arg < -2048))
                   {
                     std::cerr << "Immediate offset exceeds field on " << expr->lineNo << " : " << arg << std::endl;
                   }
                }
               else
                {
                  if ((arg > 4094) || (arg < -4096))
                   {
                     std::cerr << "Relative offset exceeds field on " << expr->lineNo << " : " << arg << std::endl;
                   }
                }
             }
            else
             {
               references.push_back(RefContainer(context.getCurrentLocation(), ((3 == ret) ? 1 : 2), expr));
             }

            if (3 == ret) // LDI
               ret |= (arg & 0xfff) << 4;
            else // LRA
               ret |= ((arg >> 1) & 0xfff) << 4;
          }
         catch (const ParserException& ex)
          {
            std::cerr << ex.what() << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         break;
      case 4: // 1 result and an 8-bit immediate
         try
          {
            ret = opcodes[operation];

            int lhs = 0;
            if (0 == (ret & 0xf0)) // Is this not BRA?
             {
               lhs = expression(context, true)->evaluate(context);
               expect(COMMA);
             }

            if (lhs > 15)
             {
               std::cerr << "Specified result " << lhs << " that has left the belt on " << nextToken.lineNumber << "." << std::endl;
             }

            std::shared_ptr<Expression> expr = expression(context, false);
            int rhs = 0;
            if (true == expr->canEvaluate(context))
             {
               rhs = expr->evaluate(context);
               if ((rhs > 254) || (rhs < -256))
                {
                  std::cerr << "BRANCH offset exceeds field on " << expr->lineNo << " : " << rhs << std::endl;
                }
             }
            else
             {
               references.push_back(RefContainer(context.getCurrentLocation(), 3, expr));
             }

            ret |= ((lhs & 15) << 4) | (((rhs >> 1) & 0xFF) << 8);
          }
         catch (const ParserException& ex)
          {
            std::cerr << ex.what() << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) && (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         break;
       }
    }

      break;
   default:
    {
      std::stringstream str;
      str << "Expected instruction, found \"" << nextToken.text << "\" on " << nextToken.lineNumber << ".";
      throw ParserException(str.str());
    }
    }

   return ret;
 }
