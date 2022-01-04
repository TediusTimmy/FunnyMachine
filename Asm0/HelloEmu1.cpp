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
   int base   = rom->LDI(0xBFF);
   int temp   = rom->SHL(rom->getBeltLocation(base), rom->getBeltLocation(four));
   int bottom = rom->LDI(0xE);
   int write  = rom->OR(rom->getBeltLocation(temp), rom->getBeltLocation(bottom));
   int charH  = rom->LDI('H');
   int chare  = rom->LDI('e');
   int charl  = rom->LDI('l');
   int charo  = rom->LDI('o');
   int charCo = rom->LDI(',');
   int charSp = rom->LDI(' ');
   int charW  = rom->LDI('W');
   int charr  = rom->LDI('r');
   int chard  = rom->LDI('d');
   int charEx = rom->LDI('!');
   int charNL = rom->LDI('\n');
                rom->STB(rom->getBeltLocation(charH), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(chare), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charl), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charl), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charo), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charCo), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charSp), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charW), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charo), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charr), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charl), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(chard), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charEx), rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(charNL), rom->getBeltLocation(write));
   int halt   = rom->INC(rom->getBeltLocation(write));
                rom->STB(rom->getBeltLocation(halt), rom->getBeltLocation(halt));

   std::ofstream file ("rom.rom");
   file.write(reinterpret_cast<const char*>(rom->getROM()), rom->getCodeLoc());

   return 0;
 }
