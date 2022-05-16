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
#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>

#include "Lexer.hpp"

class CallingContext;
class Statement;
class Expression;
class GlobalData;

class Parser /* Syntax Analyzer */
 {
   private:

      void expect (const Lexeme&);

      void program (CallingContext&, GlobalData&);
      void constants (CallingContext&, GlobalData&);
      void globals (CallingContext&, GlobalData&);
      void variables (CallingContext&, GlobalData&);
      void functions (CallingContext&, GlobalData&);

      std::shared_ptr<Statement> statement (CallingContext&);

      std::shared_ptr<Expression> expression (const CallingContext&);
      std::shared_ptr<Expression> boolean (const CallingContext&);
      std::shared_ptr<Expression> clause (const CallingContext&);
      std::shared_ptr<Expression> predicate (const CallingContext&);
      std::shared_ptr<Expression> relation (const CallingContext&);
      std::shared_ptr<Expression> shift (const CallingContext&);
      std::shared_ptr<Expression> simple (const CallingContext&);
      std::shared_ptr<Expression> term (const CallingContext&);
      std::shared_ptr<Expression> primary (const CallingContext&);

      Lexer & src;

      Token nextToken;

      void GNT (void);

      std::string LN (void);

   public:

      Parser (Lexer & input) : src(input), nextToken(0) { GNT(); }
      ~Parser() { }

      void Parse (CallingContext& context, GlobalData& data) { program(context, data); }
      std::shared_ptr<Expression> ParseExpression (const CallingContext& context) { return expression(context); }
 };

#endif /* PARSER_HPP */
