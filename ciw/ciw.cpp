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
   std::map<std::string, std::vector<std::string> > functions;
   std::map<std::string, std::map<std::string, int> > funLocals;
   std::map<std::string, std::shared_ptr<StatementSeq> > funDefs;

   CallingContext TheContext (allGlobals, constants, functions, funLocals, funDefs);

   std::ifstream file (argv[1]);
   std::string input;

   if (true == file.good())
    {
      input.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

   if (true == file.bad())
      DB_panic(std::string("Error opening file \"") + argv[1] + "\".");

   Lexer lex (input);
   Parser parse (lex);

   parse.Parse(TheContext);

   FunctionCall programCall;
   programCall.name = "program";
   programCall.lineNo = 0U;

   if (0 < functions["program"].size())
      DB_panic("Too many arguments for function \"program\".");

   // Perform initialization!
   programCall.emit(TheContext);
   std::cout << "        LDI $BFF" << std::endl;
   std::cout << "        LDI 4" << std::endl;
   std::cout << "        SHL 1, 0" << std::endl;
   std::cout << "        LDI $F" << std::endl;
   std::cout << "        OR 1, 0" << std::endl;
   std::cout << "        ST 0, 0" << std::endl;

   for (auto function : funDefs)
    {
      // Caller establishes the stack.
      // Callee clears the stack on return.
      std::cout << "function_" << function.first << ":" << std::endl;
      function.second->emit(TheContext);
    }

   return 0;
 }
