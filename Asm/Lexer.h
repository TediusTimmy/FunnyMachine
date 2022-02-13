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
#ifndef LEXER_H
#define LEXER_H

#include "Lexemes.h"
#include "Token.h"
#include <string>

class Lexer
 {
   private:
      std::string input;    // All of input, passed in to constructor.
      size_t inputPosition; // Position in input string.
      size_t lineNumber;    // Current line number in the input string.

      Token nextToken; // The next token that will be returned.

       /*
         Internal functions for operating on buffered input.
       */
      char GetNextNonWhite (void);
      char GetNextChar (void);
      char PeekNextChar (void);

      void Get_NextToken (void); // Updates nextToken.

      Lexer();

   public:

      Token GetNextToken (void); // Returns the next token.
      size_t LineNo (void) const { return lineNumber; } // Return the current line number.

      Lexer (const std::string & input);
 };

#endif /* LEXER_H */
