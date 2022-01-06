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
   int base   = LDI(0xA00);
   int write  = SHL(getBeltLocation(base), getBeltLocation(four));
   int charH  = LDI('H' | (7 << 8));
   int chare  = LDI('e' | (7 << 8));
   int charCo = LDI(',' | (7 << 8));
   int charSp = LDI(' ' | (7 << 8));
   int charW  = LDI('W' | (7 << 8));
   int charo  = LDI('o' | (7 << 8));
   int charr  = LDI('r' | (7 << 8));
   int charl  = LDI('l' | (7 << 8));
   int chard  = LDI('d' | (7 << 8));
   int charEx = LDI('!' | (7 << 8));
   int two    = LDI(2);
                ST(getBeltLocation(charH), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(chare), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charl), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charl), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charo), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charCo), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charSp), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charW), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charo), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charr), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charl), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(chard), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                ST(getBeltLocation(charEx), getBeltLocation(write));
       write  = ADD(getBeltLocation(write), getBeltLocation(two));
                LDI(' ' | (7 << 8));
       write  = ADD(1, 15);

   int stloop = getCodeLoc();

                ST(1, 0);

   if (0 != getBeltLocation(write)) std::cerr << "Assertion failed: you don't know where the write pointer is: " << getBeltLocation(write) << std::endl;

                INC(0);
                INC(0);
                LDI(4);
                LDI(0xAFA);
                SHL(0, 1);
                SUB(0, 3);
                LDI(' ' | (7 << 8));
                ADD(5, 15);
                BR(2, convertLocationImm8(getRelativeLocation(stloop)));

   int loop   = getCodeLoc();

       four   = LDI(4);
       base   = LDI(0xBFF);
   int temp   = SHL(getBeltLocation(base), getBeltLocation(four));
   int bottom = LDI(0xC);
       write  = OR(getBeltLocation(temp), getBeltLocation(bottom));

   int chars  = LDB(getBeltLocation(write));
                BRZ(getBeltLocation(chars), convertLocationImm8(getRelativeLocation(loop)));

       write  = INC(getBeltLocation(write));
                LDB(getBeltLocation(write));

                LDI(10);
                SUB(1, 0);
                BR(0, convertLocationImm8(getRelativeLocation(loop)));

       four   = LDI(4);
       base   = LDI(0xBFF);
       temp   = SHL(getBeltLocation(base), getBeltLocation(four));
       bottom = LDI(0xF);
       write  = OR(getBeltLocation(temp), getBeltLocation(bottom));
                STB(getBeltLocation(write), getBeltLocation(write));
 }
