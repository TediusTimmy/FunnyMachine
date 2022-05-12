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
#include <vector>
#include <iostream>

#include "Lexer.hpp"

void Lexer::Error (const std::string & errorMessage)
 {
   std::cerr << "Error: " << errorMessage << std::endl;
 }

int main (void)
 {
   std::vector<std::pair<std::string, Lexeme> > tests;

   tests.push_back(std::make_pair("12", NUMBER));
   tests.push_back(std::make_pair("\"Hello\"", STRING));
   tests.push_back(std::make_pair("\"Hello There\"", STRING));
   tests.push_back(std::make_pair("'Hello'", STRING));
   tests.push_back(std::make_pair("'Hello There'", STRING));
   tests.push_back(std::make_pair("program", IDENTIFIER));
   tests.push_back(std::make_pair("const", CONST));
   tests.push_back(std::make_pair("dim", DIM));
   tests.push_back(std::make_pair("static", STATIC));
   tests.push_back(std::make_pair("function", FUNCTION));
   tests.push_back(std::make_pair("end function", END_FUNCTION));
   tests.push_back(std::make_pair("declare", DECLARE));
   tests.push_back(std::make_pair("if", IF));
   tests.push_back(std::make_pair("then", THEN));
   tests.push_back(std::make_pair("else", ELSE));
   tests.push_back(std::make_pair("fi", FI));
   tests.push_back(std::make_pair("end if", FI));
   tests.push_back(std::make_pair("do", DO));
   tests.push_back(std::make_pair("while", WHILE));
   tests.push_back(std::make_pair("until", UNTIL));
   tests.push_back(std::make_pair("loop", LOOP));
   tests.push_back(std::make_pair("break", BREAK));
   tests.push_back(std::make_pair("continue", CONTINUE));
   tests.push_back(std::make_pair("return", RETURN));
   tests.push_back(std::make_pair("tailcall", TAILCALL));
   tests.push_back(std::make_pair("call", CALL));
   tests.push_back(std::make_pair("asm", ASM));
   tests.push_back(std::make_pair("=", EQUAL_SIGN));
   tests.push_back(std::make_pair(":=", ASSIGNMENT));
   tests.push_back(std::make_pair("<-", ASSIGNMENT));
   tests.push_back(std::make_pair("==", EQUALITY));
   tests.push_back(std::make_pair("!=", INEQUALITY));
   tests.push_back(std::make_pair("<>", INEQUALITY));
   tests.push_back(std::make_pair("#", INEQUALITY));
   tests.push_back(std::make_pair("&", AND_OP));
   tests.push_back(std::make_pair("/\\", AND_OP));
   tests.push_back(std::make_pair("and", AND_OP));
   tests.push_back(std::make_pair("&&", SHORT_AND));
   tests.push_back(std::make_pair("|", OR_OP));
   tests.push_back(std::make_pair("or", OR_OP));
   tests.push_back(std::make_pair("\\/", OR_OP));
   tests.push_back(std::make_pair("||", SHORT_OR));
   tests.push_back(std::make_pair("?", XOR_OP));
   tests.push_back(std::make_pair("xor", XOR_OP));
   tests.push_back(std::make_pair(">", GREATER_THAN));
   tests.push_back(std::make_pair("<", LESS_THAN));
   tests.push_back(std::make_pair(">=", GREATER_THAN_OR_EQUAL_TO));
   tests.push_back(std::make_pair("<=", LESS_THAN_OR_EQUAL_TO));
   tests.push_back(std::make_pair("+", PLUS));
   tests.push_back(std::make_pair("-", MINUS));
   tests.push_back(std::make_pair("*", MULTIPLY));
   tests.push_back(std::make_pair("/", DIVIDE));
   tests.push_back(std::make_pair("<<", SHIFT_LEFT));
   tests.push_back(std::make_pair(">>", SHIFT_RIGHT));
   tests.push_back(std::make_pair(">>>", UNSIGNED_SHIFT_RIGHT));
   tests.push_back(std::make_pair(">><", ROTATE_RIGHT));
   tests.push_back(std::make_pair("<<>", ROTATE_LEFT));
   tests.push_back(std::make_pair("!", NOT));
   tests.push_back(std::make_pair("not", NOT));
   tests.push_back(std::make_pair(":", COLON));
   tests.push_back(std::make_pair(";", SEMICOLON));
   tests.push_back(std::make_pair("(", OPEN_PARENS));
   tests.push_back(std::make_pair(")", CLOSE_PARENS));
   tests.push_back(std::make_pair("[", LEFT_BRACKET));
   tests.push_back(std::make_pair("]", RIGHT_BRACKET));
   tests.push_back(std::make_pair("{", LEFT_BRACE));
   tests.push_back(std::make_pair("}", RIGHT_BRACE));
   tests.push_back(std::make_pair("@", AT));
   tests.push_back(std::make_pair("\n", NEW_LINE));
   tests.push_back(std::make_pair("//   Hello There!\n", NEW_LINE));
   tests.push_back(std::make_pair("(* Hi there *) hello", IDENTIFIER));
   tests.push_back(std::make_pair("\\\n   12", NUMBER));

   for (std::vector<std::pair<std::string, Lexeme> >::iterator iter = tests.begin();
      iter != tests.end(); ++iter)
    {
      Lexer lexi(iter->first);
      Token test = lexi.GetNextToken();
      if (test.lexeme == iter->second)
         std::cout << "\t\tPassed: " << iter->first << " : " << test.text << std::endl;
      else
         std::cout << "Failed: " << iter->first << " : " << test.text << std::endl;
    }

   for (std::vector<std::pair<std::string, Lexeme> >::iterator iter = tests.begin();
      iter != tests.end(); ++iter)
    {
      Lexer lexi("   " + iter->first + "   ");
      Token test = lexi.GetNextToken();
      if (test.lexeme == iter->second)
         std::cout << "\t\tPassed: " << iter->first << " : " << test.text << std::endl;
      else
         std::cout << "Failed: " << iter->first << " : " << test.text << std::endl;
    }

   for (std::vector<std::pair<std::string, Lexeme> >::iterator iter = tests.begin();
      iter != tests.end(); ++iter)
    {
      Lexer lexi("\t\t\t" + iter->first + "\t\t\t");
      Token test = lexi.GetNextToken();
      if (test.lexeme == iter->second)
         std::cout << "\t\tPassed: " << iter->first << " : " << test.text << std::endl;
      else
         std::cout << "Failed: " << iter->first << " : " << test.text << std::endl;
    }

   return 0;
 }
