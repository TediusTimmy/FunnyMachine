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
   // Memory map : 
   // 0002-0003 : Return pointer - where to return from subroutine.

   int start = getCodeLoc();

                        LRA(42);     // TODO : Figure out where the main loop is and make this point there.
                        RET(14, 0);


// Clear Screen : (This just BARELY fits between frames...)
   int cls = getCodeLoc();

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        LDI('.' | (7 << 8));
                        ADD(1, 15);
    {
   int stloop = getCodeLoc();

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        LDI(4);
                        LDI(0xAFA);
                        SHL(0, 1);
                        SUB(0, 3);
                        LDI('.' | (7 << 8));
                        ADD(5, 15);
                        BR(2, convertLocationImm8(getRelativeLocation(stloop)));
    }

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Draw the dividing line in the court
   int drawCourt = getCodeLoc();

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        LDI(0x4E);
                        ADD(1, 0);
                        LDI('#' | (7 << 8));
                        ADD(1, 15);
    {
   int stloop = getCodeLoc();

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x13E);
                        ADD(1, 0);
                        LDI(4);
                        LDI(0xAFA);
                        SHL(0, 1);
                        SUB(0, 3);
                        LDI('#' | (7 << 8));
                        ADD(5, 15);
                        BRP(2, convertLocationImm8(getRelativeLocation(stloop)));
    }

                        LDI(2);
                        LD(0);
                        RET(14, 0);


   std::cout << "Jump location: " << (-getRelativeLocation(start) >> 1) << std::endl;

                        LDI(2);
                        LRA(2);
                        ST(0, 1);
                        BRA(convertLocationImm8(getRelativeLocation(cls)));
                        LDI(2);
                        LRA(2);
                        ST(0, 1);
                        BRA(convertLocationImm8(getRelativeLocation(drawCourt)));

                        LRA(0);
                        RET(14, 0);
 }
