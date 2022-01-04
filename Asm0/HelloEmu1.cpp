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
#include "Assembler.h"

void ROM::generateAssembly()
 {
   int four   = LDI(4);
   int base   = LDI(0xBFF);
   int temp   = SHL(getBeltLocation(base), getBeltLocation(four));
   int bottom = LDI(0xE);
   int write  = OR(getBeltLocation(temp), getBeltLocation(bottom));
   int charH  = LDI('H');
   int chare  = LDI('e');
   int charl  = LDI('l');
   int charo  = LDI('o');
   int charCo = LDI(',');
   int charSp = LDI(' ');
   int charW  = LDI('W');
   int charr  = LDI('r');
   int chard  = LDI('d');
   int charEx = LDI('!');
   int charNL = LDI('\n');
                STB(getBeltLocation(charH), getBeltLocation(write));
                STB(getBeltLocation(chare), getBeltLocation(write));
                STB(getBeltLocation(charl), getBeltLocation(write));
                STB(getBeltLocation(charl), getBeltLocation(write));
                STB(getBeltLocation(charo), getBeltLocation(write));
                STB(getBeltLocation(charCo), getBeltLocation(write));
                STB(getBeltLocation(charSp), getBeltLocation(write));
                STB(getBeltLocation(charW), getBeltLocation(write));
                STB(getBeltLocation(charo), getBeltLocation(write));
                STB(getBeltLocation(charr), getBeltLocation(write));
                STB(getBeltLocation(charl), getBeltLocation(write));
                STB(getBeltLocation(chard), getBeltLocation(write));
                STB(getBeltLocation(charEx), getBeltLocation(write));
                STB(getBeltLocation(charNL), getBeltLocation(write));
   int halt   = INC(getBeltLocation(write));
                STB(getBeltLocation(halt), getBeltLocation(halt));
 }
