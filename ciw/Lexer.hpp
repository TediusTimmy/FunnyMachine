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
#ifndef LEXER_HPP
#define LEXER_HPP

#include "Lexemes.hpp"
#include "Token.hpp"
#include <string>
#include <map>

class Lexer /* Lex Me Up, Scotty */
 {
   private:
      std::string input; // All of input, passed in to constructor.
      size_t inputPosition; // Position in input string.
      size_t lineNumber; // Current line number in the input string.

      Token nextToken; // The next token that will be returned.

      std::map<std::string, Lexeme> keyWords; // The map of keywords to Lexemes.

       /*
         Internal functions for operating on buffered input.
       */
      void AfterNewline (void);
      char GetNextNonWhite (void);
      char GetNextChar (void);
      void UnGetNextChar (void);

      void Get_NextToken (void); // Updates nextToken.

      Lexer();

      // Print a message and die.
      void Error (const std::string & errorMessage) __attribute__ ((__noreturn__));

   public:

      Token GetNextToken (void); // Returns the next token.
      size_t LineNo (void) const { return lineNumber; } // Return the current line number.

      Lexer (const std::string & input);
 };

#endif /* LEXER_HPP */
