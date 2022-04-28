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
#ifndef LEXEME_HPP
#define LEXEME_HPP

enum Lexeme
 {
   INVALID,
   COMMENT,
   END_OF_FILE,

   CONST,
   DIM,
   STATIC,
   FUNCTION,
   END_FUNCTION, //       If END is followed by FUNCTION, change both to one END_FUNCTION.
   DECLARE,

   IDENTIFIER,
   NUMBER,
   STRING,

   LEFT_BRACKET,
   RIGHT_BRACKET,
   OPEN_PARENS,
   CLOSE_PARENS,
   LEFT_BRACE,
   RIGHT_BRACE,

   SEMICOLON,
   COLON,

   NEW_LINE,

   END,        //         A naked END is a lexical error.
   IF,
   THEN,
   ELSE,
   FI,         //         If END is followed by IF, change both to one FI.
   DO,
   WHILE,
   UNTIL,
   LOOP,
   BREAK,
   CONTINUE,
   RETURN,
   TAILCALL,
   CALL,

   AT,         // @

   EQUAL_SIGN, // =       There is no good way to disambiguate EQUAL_SIGN.
   ASSIGNMENT, // <- :=
   EQUALITY,   // ==
   INEQUALITY, // <>, #, !=
   AND,        // and     There should never be a naked AND. If followed by CASE, change both to one CHOICE.
   AND_OP,     // & /\    If CHOICE is followed by ELSE, change both to one DEFAULT. If followed by THEN,
   SHORT_AND,  // &&      change both to one SHORT_AND. If none of these, mutate to AND_OP.
   OR,         // or      There should never be a naked OR. If followed by ELSE, change both to one SHORT_OR.
   OR_OP,      // | \/    Else, change to OR_OP.
   SHORT_OR,   // ||
   XOR_OP,     // ? xor
   GREATER_THAN,
   LESS_THAN,
   GREATER_THAN_OR_EQUAL_TO,
   LESS_THAN_OR_EQUAL_TO,
   PLUS,
   MINUS,
   SHIFT_LEFT,
   SHIFT_RIGHT,
   UNSIGNED_SHIFT_RIGHT,
   ROTATE_LEFT,
   ROTATE_RIGHT,
   MULTIPLY,
   DIVIDE,
   REMAINDER,
   NOT
 };

#endif /* LEXEME_HPP */
