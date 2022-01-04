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

#include <memory>
#include <fstream>

#include "Assembler.h"

int main (void)
 {
   std::unique_ptr<ROM> rom = std::make_unique<ROM>();

   int four   = rom->LDI(4);
   int base   = rom->LDI(0xA00);
   int write  = rom->SHL(rom->getBeltLocation(base), rom->getBeltLocation(four));
   int charH  = rom->LDI('H' | (7 << 8));
   int chare  = rom->LDI('e' | (7 << 8));
   int charCo = rom->LDI(',' | (7 << 8));
   int charSp = rom->LDI(' ' | (7 << 8));
   int charW  = rom->LDI('W' | (7 << 8));
   int charo  = rom->LDI('o' | (7 << 8));
   int charr  = rom->LDI('r' | (7 << 8));
   int charl  = rom->LDI('l' | (7 << 8));
   int chard  = rom->LDI('d' | (7 << 8));
   int charEx = rom->LDI('!' | (7 << 8));
   int two    = rom->LDI(2);
                rom->ST(rom->getBeltLocation(charH), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(chare), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charl), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charl), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charo), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charCo), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charSp), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charW), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charo), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charr), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charl), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(chard), rom->getBeltLocation(write));
       write  = rom->ADD(rom->getBeltLocation(write), rom->getBeltLocation(two));
                rom->ST(rom->getBeltLocation(charEx), rom->getBeltLocation(write));

   int loop   = rom->getCodeLoc();

       four   = rom->LDI(4);
       base   = rom->LDI(0xBFF);
   int temp   = rom->SHL(rom->getBeltLocation(base), rom->getBeltLocation(four));
   int bottom = rom->LDI(0xC);
       write  = rom->OR(rom->getBeltLocation(temp), rom->getBeltLocation(bottom));

   int chars  = rom->LDB(rom->getBeltLocation(write));
                rom->BRZ(rom->getBeltLocation(chars), rom->convertLocationImm8(rom->getRelativeLocation(loop)));

       write  = rom->INC(rom->getBeltLocation(write));
                rom->LDB(rom->getBeltLocation(write));

                rom->LDI(10);
                rom->SUB(1, 0);
                rom->BR(0, rom->convertLocationImm8(rom->getRelativeLocation(loop)));

       four   = rom->LDI(4);
       base   = rom->LDI(0xBFF);
       temp   = rom->SHL(rom->getBeltLocation(base), rom->getBeltLocation(four));
       bottom = rom->LDI(0xF);
       write  = rom->OR(rom->getBeltLocation(temp), rom->getBeltLocation(bottom));
                rom->STB(rom->getBeltLocation(write), rom->getBeltLocation(write));

   std::ofstream file ("rom.rom");
   file.write(reinterpret_cast<const char*>(rom->getROM()), rom->getCodeLoc());

   return 0;
 }
