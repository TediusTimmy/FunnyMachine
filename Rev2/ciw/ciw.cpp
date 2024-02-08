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

void DB_panic (const std::string &, size_t) __attribute__ ((__noreturn__));

void DB_panic (const std::string & msg, size_t lineNo)
 {
   std::cerr << msg << std::endl;
   std::cerr << "    At line " << lineNo << std::endl;
   std::exit(1);
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
   TheContext.m_constantData = &data.constantData;

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

   std::cout << "    ; Assembler initialization : set up SDOs " << std::endl;
   std::cout << ".def SDO0 0, 2" << std::endl;
   std::cout << ".def SDO1 1, 0" << std::endl;
   std::cout << ".def SDO2 2, 1" << std::endl;
   std::cout << ".def SDO3 2, 2" << std::endl;
   std::cout << ".def SDO4 1, 0" << std::endl;
   std::cout << ".def SDO5 0, 1" << std::endl;
   std::cout << "    ; Program initialization : set SP and BP to 9FFE " << std::endl;
   std::cout << "        LDI $FFE" << std::endl;
   std::cout << "        LHN 0, 9" << std::endl;
   std::cout << "        STS 0, 0" << std::endl;
   std::cout << "        STS 0, 1" << std::endl;
   std::cout << "    ; Program initialization : identity map RAM and ROM banks " << std::endl;
   std::cout << "        LHN 15, $B" << std::endl;
   std::cout << "        LDI $7" << std::endl;
   std::cout << "        OR  1, 0" << std::endl;
   std::cout << "        INC 15" << std::endl;
   std::cout << "        STB 0, 1 ; B007 <- 1" << std::endl;
   std::cout << "        INC 3" << std::endl;
   std::cout << "        STB 1, 0 ; B001 <- 1" << std::endl;
   std::cout << "        INC 0" << std::endl;
   std::cout << "        INC 2" << std::endl;
   std::cout << "        STB 0, 1 ; B002 <- 2" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        STB 0, 1 ; B003 <- 3" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        INC 1" << std::endl;
   std::cout << "        STB 0, 1 ; B004 <- 4" << std::endl;
   programCall.emit(TheContext, data);
   std::cout << "    ; Program exit : halt machine " << std::endl;
   std::cout << "        HALT" << std::endl;

   for (auto function : funDefs)
    {
      TheContext.m_currentFunction = function.first;
      // Caller establishes half of the stack.
      // Callee clears the stack on return.
      std::cout << "function_" << function.first << ":" << std::endl;
      // Normal function prologue
      std::cout << "        LDS 1" << std::endl;
      std::cout << "        SDO4 0" << std::endl; // Push BP
      std::cout << "        LDS 0" << std::endl;
      std::cout << "        STS 0, 1" << std::endl; // BP <- SP
      if (0U != TheContext.getNumLocals(function.first, 0U))
       {
         std::cout << "        LDI " << TheContext.getNumLocals(function.first, 0U) << std::endl; // Assume it will fit.
         std::cout << "        SUB 1, 0" << std::endl;
         std::cout << "        STS 0, 0" << std::endl; // Allocate locals
       }
      std::cout << "tailcall_" << function.first << ":" << std::endl;
      function.second->emit(TheContext, data);
    }

   std::cout << "    ; Set the PC on reset " << std::endl;
   std::cout << ".assert * <= $1FFE" << std::endl;
   std::cout << "   .org $1FFE" << std::endl;
   std::cout << "   .dw  $C000" << std::endl;

   std::cout << "    ; Program constants " << std::endl;
   for (const auto& item : data.constants)
    {
      std::cout << item << std::endl;
    }

   std::cout << "    ; Define SDOs " << std::endl;
   std::cout << ".assert * <= $3F00" << std::endl;
   std::cout << "   .org $3F00" << std::endl;
   std::cout << "pop_and_a_half:" << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << "        LDS 0" << std::endl;
   std::cout << "        ADD 0, 1" << std::endl;
   std::cout << "        STS 0, 0" << std::endl;
   std::cout << "        LD 1" << std::endl;
   std::cout << "        LD 1" << std::endl;
   std::cout << "        ENDOP 0, 1" << std::endl;
   std::cout << "not_push:" << std::endl;
   std::cout << "        LDS 0" << std::endl;
   std::cout << "        ST 1, 0" << std::endl;
   std::cout << "        ENDOP 15, 15" << std::endl;
   std::cout << "mul_op:" << std::endl;
   std::cout << "divmod_op:" << std::endl;
   std::cout << "        HALT" << std::endl;
   std::cout << "real_push:" << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << "        LDS 0" << std::endl;
   std::cout << "        SUB 0, 1" << std::endl;
   std::cout << "        STS 0, 0" << std::endl;
   std::cout << "        ST  3, 0" << std::endl;
   std::cout << "        ENDOP 15, 15" << std::endl;
   std::cout << "real_pop:" << std::endl;
   std::cout << "        LDI 2" << std::endl;
   std::cout << "        LDS 0" << std::endl;
   std::cout << "        ADD 0, 1" << std::endl;
   std::cout << "        STS 0, 0" << std::endl;
   std::cout << "        LD  1" << std::endl;
   std::cout << "        ENDOP 0, 15" << std::endl;
   std::cout << "haltme:" << std::endl;
   std::cout << "        HALT" << std::endl;

   std::cout << "    ; SDO and Interrupt table " << std::endl;
   std::cout << ".assert * <= $3FC0" << std::endl;
   std::cout << "   .org $3FC0" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `real_pop       ; SDO5" << std::endl;
   std::cout << "   .dw  $E000 + `real_push      ; SDO4" << std::endl;
   std::cout << "   .dw  $E000 + `divmod_op      ; SDO3" << std::endl;
   std::cout << "   .dw  $E000 + `mul_op         ; SDO2" << std::endl;
   std::cout << "   .dw  $E000 + `not_push       ; SDO1" << std::endl;
   std::cout << "   .dw  $E000 + `pop_and_a_half ; SDO0" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $E000 + `haltme" << std::endl;
   std::cout << "   .dw  $C000" << std::endl;

   return 0;
 }
