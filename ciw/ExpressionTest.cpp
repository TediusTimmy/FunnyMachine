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
   throw std::exception();
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

int main (void)
 {
   std::map<std::string, std::map<std::string, int> > allGlobals;
   std::map<std::string, int> constants;
   std::map<std::string, std::vector<std::string> > functions;
   std::map<std::string, std::map<std::string, std::vector<size_t> > > funLocals;
   std::map<std::string, std::shared_ptr<StatementSeq> > funDefs;

   CallingContext NullContext (allGlobals, constants, functions, funLocals, funDefs);

   std::string input;
   getline(std::cin, input);
   while ("exit" != input)
    {
      try
       {
         Lexer lex (input);
         Parser parse (lex);
         std::shared_ptr<Expression> expression = parse.ParseExpression(NullContext);
         expression->emit(NullContext);
         std::cout << expression->evaluate(NullContext) << std::endl;
       }
      catch (...)
       {
         std::cerr << "Caught a something." << std::endl;
       }

      getline(std::cin, input);
    }

   return 0;
 }
