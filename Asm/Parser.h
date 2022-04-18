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
#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <list>

#include "Lexer.h"

class SymbolTable;
class Expression;
class RefContainer;

class Parser /* Syntax Analyzer */
 {
   private:

      void expect (const Lexeme &);

      std::vector<unsigned short> assembly ();

      unsigned short instruction (const SymbolTable& context, std::list<RefContainer>& references);

      std::shared_ptr<Expression> expression (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> disjunction (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> conjunction (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> relation (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> bitwise (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> simple (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> term (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> unary (const SymbolTable& context, bool isResult);
      std::shared_ptr<Expression> primary (const SymbolTable& context, bool isResult);

      Lexer & src;

      Token nextToken;

      void GNT (void);

      static std::map<std::string, int> args;
      static std::map<std::string, int> opcodes;
      static std::map<std::string, int> results;

   public:

      Parser (Lexer & input) : src(input), nextToken(0) { GNT(); }
      ~Parser() { }

      std::vector<unsigned short> Parse () { return assembly(); }
 };

#endif /* PARSER_H */
