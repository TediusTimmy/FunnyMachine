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
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "Lexemes.h"

class Token
 {
   private:
      Token();

   public:
      Lexeme lexeme;
      std::string text;
      size_t lineNumber;

      Token(size_t lineNo) : lineNumber(lineNo) { }
      Token(Lexeme lexeme, std::string text, size_t lineNo) : lexeme(lexeme), text(text), lineNumber(lineNo) { }

      Token& operator= (const Token & src) = default;
 };

#endif /* TOKEN_H */
