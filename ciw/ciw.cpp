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
#include <iostream>
#include <fstream>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "SymbolTable.hpp"

void DB_panic (const std::string & msg) __attribute__ ((__noreturn__));

void DB_panic (const std::string & msg)
 {
   std::cerr << msg << std::endl;
   std::exit(1);
 }

void DB_panic (const std::string & msg, const CallingContext& stack, size_t lineNo)
 {
   std::cerr << msg << std::endl;

   std::cerr << "At line " << lineNo << " in \"" << stack.Name << "\"" << std::endl;

   const CallingContext * next = &stack;
   while (nullptr != next && (nullptr != next->Parent) && ("BaseContext" != next->Parent->Name))
    {
      std::cerr << "\tfrom line " << next->ParentLine << " in \"";
      next = next->Parent;
      std::cerr << next->Name << "\"" << std::endl;
    }

   throw std::exception();
 }

void Lexer::Error (const std::string & errorMessage)
 {
   DB_panic(errorMessage);
 }

int main (int argc, char ** argv)
 {
   if (argc < 2)
      DB_panic("Usage: ciw source_file");

   std::map<std::string, std::map<std::string, int> > allGlobals;
   std::map<std::string, int> constants;
   std::map<std::string, int> allLocals;
   std::map<std::string, std::vector<std::string> > functions;
   std::map<std::string, std::map<std::string, int> > funLocals;
   std::map<std::string, std::shared_ptr<StatementSeq> > funDefs;

   CallingContext TheContext (allGlobals, constants, allLocals, functions, funLocals, funDefs);
   GlobalData data;

   std::ifstream file (argv[1]);
   std::string input;

   if (false == file.fail())
    {
      input.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

   if (true == file.fail())
      DB_panic(std::string("Error opening file \"") + argv[1] + "\".");

   Lexer lex (input);
   Parser parse (lex);

   parse.Parse(TheContext, data);

   FunctionCall programCall;
   programCall.name = "program";
   programCall.lineNo = 0U;

   if (0 < functions["program"].size())
      DB_panic("Too many arguments for function \"program\".");

   std::cout << "    ; Program initialization : set SP and BP to 9FFE " << std::endl;
   std::cout << "        LDI $9FF" << std::endl;
   std::cout << " @bpa   LDI 4" << std::endl;
   std::cout << "        SHL 1, 0" << std::endl;
   std::cout << "        LDI $E" << std::endl;
   std::cout << " @isp   OR  1, 0" << std::endl;
   std::cout << " @spa   LDI 2" << std::endl;
   std::cout << "        ST  isp, bpa" << std::endl;
   std::cout << "        ST  isp, spa" << std::endl;
   std::cout << "    ; Program initialization : identity map RAM and ROM banks " << std::endl;
   std::cout << "        LDI $B00" << std::endl;
   std::cout << "        LDI 4" << std::endl;
   std::cout << "        SHL 1, 0" << std::endl;
   std::cout << "        LDI $8" << std::endl;
   std::cout << "        OR  1, 0" << std::endl;
   std::cout << "        INC 15" << std::endl;
   std::cout << "        STB 0, 1 ; B008 <- 1" << std::endl;
   std::cout << "        SUB 1, 2" << std::endl;
   std::cout << "        INC 0" << std::endl;
   std::cout << "        STB 2, 0 ; B001 <- 1" << std::endl;
   std::cout << "        INC 0" << std::endl;
   std::cout << "        INC 3" << std::endl;
   std::cout << "        STB 0, 1 ; B002 <- 2" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        STB 0, 1 ; B003 <- 3" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        STB 0, 1 ; B004 <- 4" << std::endl;
   programCall.emit(TheContext, data);
   std::cout << "    ; Program exit : halt machine " << std::endl;
   std::cout << "        LDI $BFF" << std::endl;
   std::cout << "        LDI 4" << std::endl;
   std::cout << "        SHL 1, 0" << std::endl;
   std::cout << "        LDI $F" << std::endl;
   std::cout << "        OR  1, 0" << std::endl;
   std::cout << "        ST  0, 0" << std::endl;

   for (auto function : funDefs)
    {
      TheContext.m_currentFunction = function.first;
      // Caller establishes the stack.
      // Callee clears the stack on return.
      std::cout << "function_" << function.first << ":" << std::endl;
      function.second->emit(TheContext, data);
    }

   return 0;
 }
