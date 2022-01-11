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

/*
   So, how does this work?
   First, you compile and run this program, that will produce a file called "rom.rom".
   That file, you run in the emulator.

   It's a two-stage thing.
   This way, all of the power of the C++ compiler is utilized in managing the assmebly. (And, I really should use more of that power....)
*/

void ROM::generateAssembly()
 {
   // Memory map : 
   // 0002-0003 : Return pointer - where to return from subroutine.
   // 0010      : Y location of the left paddle
   // 0011      : Y location of the right paddle
   // 0012      : X location of the ball
   // 0013      : Y location of the ball
   // 0014      : X velocity of the ball
   // 0015      : Y velocity of the ball

   int start = getCodeLoc();

                        LRA(530);     // A print later will tell you where this needs to jump to.
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


// Clear the left side of the court
   int clearLeft = getCodeLoc();

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        LDI('%' | (7 << 8));
                        ADD(1, 15);
    {
   int stloop = getCodeLoc();

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        ADD(1, 0);
                        LDI(4);
                        LDI(0xAFA);
                        SHL(0, 1);
                        SUB(0, 3);
                        LDI('%' | (7 << 8));
                        ADD(5, 15);
                        BRP(2, convertLocationImm8(getRelativeLocation(stloop)));
    }

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Clear the right side of the court
   int clearRight = getCodeLoc();

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        LDI(0x9C);
                        ADD(1, 0);
                        LDI('&' | (7 << 8));
                        ADD(1, 15);
    {
   int stloop = getCodeLoc();

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        ADD(1, 0);
                        LDI(4);
                        LDI(0xAFA);
                        SHL(0, 1);
                        SUB(0, 3);
                        LDI('&' | (7 << 8));
                        ADD(5, 15);
                        BRP(2, convertLocationImm8(getRelativeLocation(stloop)));
    }

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Draw the left paddle
   int drawLeft = getCodeLoc();

                        LDI(0x10);
                        LDB(0); // Here's the location of the paddle, but I need its screen memory position: 160 * y
                        LDI(1); // Current Bit
                        LDI(160); // Current Addend
                        LDI(0); // Current Sum

// While it would be easy to use the multiplication instruction, I am going to limit myself to not do it.
// I'm going to do this algorithmically with shifts and adds.
// The following block of code we run five times, in order to multiply a number by up to 31.

                        AND(2, 3);  // Test if the current bit is set.
                        BRZ(0, 5);  // If the current bit is not set, jump down.
                        ADD(4, 15); // The current bit is set, so recreate the belt with the modified sum. Copy the paddle position.
                        ADD(4, 15); // Copy the current bit
                        ADD(4, 15); // Copy the current addend.
                        ADD(4, 0);  // Add the current addend to the sum to make the new sum.
                        LDI(0);     // This is the result of the AND, needed to keep belt coherency.
                        LDI(1);
                        SHL(4, 0);  // Multiply the current bit by 2
                        SHL(4, 1);  // Multiply the current addend by two.
                        ADD(7, 15); // Copy the paddle position.
                        ADD(2, 15); // Copy the new current bit.
                        ADD(2, 15); // Copy the new addend.
                        ADD(7, 15); // Copy the current sum.

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);        // I could CUT everything from here down to the next portion.
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

// We now have 160 * the y position of the paddle (which should be less than 31).
// Now, lets assume that a paddle is 5 characters tall and unroll that loop, too:

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        ADD(0, 3);
                        LDI('L' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('L' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('L' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('L' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('L' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);               // We can CUT from here to LDI if need be.
                        LDI('L' | (7 << 8));
                        ADD(2, 1);

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Draw the right paddle
   int drawRight = getCodeLoc();

                        LDI(0x11);
                        LDB(0); // Here's the location of the paddle, but I need its screen memory position: 160 * y
                        LDI(1); // Current Bit
                        LDI(160); // Current Addend
                        LDI(0); // Current Sum

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        ADD(0, 3);
                        LDI(0x9C);
                        ADD(1, 0);
                        LDI('R' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('R' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('R' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('R' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('R' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('R' | (7 << 8));
                        ADD(2, 1);

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Clear the ball. (I really need to make multiplication a subroutine!)
   int clearBall = getCodeLoc();

                        LDI(0x13);
                        LDB(0);   // Y location
                        LDI(1);   // Current Bit
                        LDI(160); // Current Addend
                        LDI(0);   // Current Sum

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        ADD(0, 3);
                        LDI(0x12); // X Location
                        LDB(0);
                        ADD(0, 0); // Multiply by 2.
                        ADD(3, 0);
                        LDI('C' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('C' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('C' | (7 << 8));
                        ADD(2, 1);

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Draw the ball.
   int drawBall = getCodeLoc();

                        LDI(0x13);
                        LDB(0);   // Y location
                        LDI(1);   // Current Bit
                        LDI(160); // Current Addend
                        LDI(0);   // Current Sum

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        AND(2, 3);
                        BRZ(0, 5);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 15);
                        ADD(4, 0);
                        LDI(0);
                        LDI(1);
                        SHL(4, 0);
                        SHL(4, 1);
                        ADD(7, 15);
                        ADD(2, 15);
                        ADD(2, 15);
                        ADD(7, 15);

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        ADD(0, 3);
                        LDI(0x12); // X Location
                        LDB(0);
                        ADD(0, 0); // Multiply by 2.
                        ADD(3, 0);
                        LDI('B' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('B' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('B' | (7 << 8));
                        ADD(2, 1);

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Update the ball.
// Check for paddle up/down. (This only flags whether the user is pressing a key for left/right paddle up/down.)
// Update left paddle.
// Update right paddle.
// Main loop : spin and do updates on frame 0, 10, and 20.


   std::cout << "Jump location: " << ((-(getRelativeLocation(start) + 4)) >> 1) << " : " << getCodeLoc() << std::endl;

                        LDI(0x10);
                        LDI(17);
                        STB(0, 1);
                        LDI(0x11);
                        LDI(15);
                        STB(0, 1);
                        LDI(0x12);
                        LDI(0x13);
                        LDI(5);
                        STB(0, 2);
                        STB(0, 1);

                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(cls)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(drawCourt)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(clearLeft)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(clearRight)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(drawLeft)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(drawRight)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(clearBall)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(drawBall)));
                        RET(14, 0);

                        LRA(0);
                        RET(14, 0);
 }
