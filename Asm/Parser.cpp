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
      return val[0] | ((val.size() > 1) ? ((int)((unsigned char)val[1])) << 8 : 0);
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
       {
          std::shared_ptr<ValueOf> temp1 = std::make_shared<ValueOf>();
          temp1->location = context.getCurrentLocation();
          temp = temp1;
       }
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

std::shared_ptr<Expression> Parser::primary (const SymbolTable& context, bool isResult) // TODO FIXME FROM HERE DOWN
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
            op->value = context.getResult(nextToken.text);

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
         std::shared_ptr<Label> op = std::make_shared<Label>();
         op->referent = nextToken.text;

         op->lineNo = nextToken.lineNumber;
         GNT();

         ret = op;
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

static std::string TO_UPPER(std::string str)
 {
   std::transform(str.begin(), str.end(), str.begin(), ::toupper);
   return str;
 }

std::vector<unsigned short> Parser::assembly()
 {
   SymbolTable context;
   std::vector<unsigned short> result;

   while (END_OF_FILE != nextToken.lexeme)
    {
      while (END_OF_LINE == nextToken.lexeme)
       {
         GNT();
       }

      switch(nextToken.lexeme)
       {
      case IDENTIFIER:
         if (opcodes.end() != opcodes.find(TO_UPPER(nextToken.text)))
          {
            result.push_back(instruction(context));
          }
         break;
      default: break;
       }
    }

   return result;
 }

unsigned short Parser::instruction (const SymbolTable& context)
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

            if (0xf35 != ret) // Normal opcode
             {
               ret |= (arg & 15) << 8;
             }
            else // NEG
             {
               ret |= (arg & 15) << 12;
             }
          }
         catch (const ParserException& ex)
          {
            std::cerr << ex.what() << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) || (END_OF_FILE != nextToken.lexeme))
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

            ret |= ((lhs & 15) << 8) | ((rhs & 15) << 12);
          }
         catch (const ParserException& ex)
          {
            std::cerr << ex.what() << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) || (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         break;
      case 3: // 12-bit immediate
         try
          {
            ret = opcodes[operation];

            int arg = expression(context, false)->evaluate(context); // TODO - Forward references

            if (3 == ret) // LDI
               ret |= (arg & 0xfff) << 4;
            else // LRA
               ret |= ((arg >> 1) & 0xfff) << 4;
          }
         catch (const ParserException& ex)
          {
            std::cerr << ex.what() << std::endl;
            while ((END_OF_LINE != nextToken.lexeme) || (END_OF_FILE != nextToken.lexeme))
             {
               GNT();
             }
          }
         break;
      case 4: // 1 result and an 8-bit immediate
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
