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
#include "Parser.h"

#include <fstream>
#include <string>
#include <cstdio>

int main (int argc, char ** argv)
 {
   if (argc < 3)
    {
      printf("usage: asm asmfile.asm result.rom\n");
      return 1;
    }

   std::ifstream file (argv[1]);
   if (true == file.fail())
    {
      printf("error opening file: %s\n", argv[1]);
      return 1;
    }

   std::ofstream out (argv[2], std::ios_base::binary);
   if (true == out.fail())
    {
      printf("error opening file: %s\n", argv[2]);
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
   Parser parser (lexer);

   std::vector<unsigned short> res = parser.Parse();

   out.write(reinterpret_cast<char*>(reinterpret_cast<void*>(&res[0])), res.size() * sizeof(unsigned short));

   return 0;
 }
