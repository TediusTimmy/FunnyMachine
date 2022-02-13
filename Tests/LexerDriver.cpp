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
#include "Lexer.h"

#include <fstream>
#include <string>
#include <cstdio>

int main (int argc, char ** argv)
 {
   if (argc < 2)
    {
      printf("usage: LexerDriver asmfile.asm\n");
      return 1;
    }

   std::ifstream file (argv[1]);
   if (true == file.fail())
    {
      printf("error opening file: %s\n", argv[1]);
      return 1;
    }

   std::string line, input;
   line = "";
   std::getline(file, line);
   while (false == file.eof())
    {
      input += line;
      input += '\n';
      line = "";
      std::getline(file, line);
    }
   input += line;

   Lexer lexer (input);
   Token token = lexer.GetNextToken();
   while (Lexeme::END_OF_FILE != token.lexeme)
    {
      std::printf("%s ", token.text.c_str());
      token = lexer.GetNextToken();
    }

   return 0;
 }