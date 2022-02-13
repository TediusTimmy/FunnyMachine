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
#ifndef LEXEMES_H
#define LEXEMES_H

enum Lexeme
 {
   BLANK,
   INVALID,
   END_OF_FILE,
   END_OF_LINE,

   PERIOD,                    // .
   COMMA,                     // ,
   COLON,                     // :
   POUND,                     // #
   AT,                        // @

   IDENTIFIER,
   NUMBER,
   STRING,

   OPEN_PARENS,               // (
   CLOSE_PARENS,              // )

   PLUS,                      // +
   MINUS,                     // -
   NOT,                       // ~
   GREATER_THAN,              // >
   LESS_THAN,                 // <
   MULTIPLY,                  // *
   DIVIDE,                    // /
   MOD,                       // %
   SHIFT_LEFT,                // <<
   SHIFT_RIGHT,               // >>
   UNSIGNED_SHIFT_RIGHT,      // >>>
   AND,                       // &
   OR,                        // |
   XOR,                       // ^
   EQUALITY,                  // =
   INEQUALITY,                // <>
   GREATER_THAN_OR_EQUAL_TO,  // >=
   LESS_THAN_OR_EQUAL_TO,     // <=   
   SHORT_OR,                  // ||
   SHORT_AND,                 // &&
   BOOL_NOT                   // !
 };

#endif /* LEXEMES_H */
