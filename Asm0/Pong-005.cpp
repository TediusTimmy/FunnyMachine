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
   // 0008      : Timer
   // 0009      : Random Counter
   // 0010      : Y location of the left paddle
   // 0011      : Y location of the right paddle
   // 0012      : X location of the ball
   // 0013      : Y location of the ball
   // 0014      : X velocity of the ball
   // 0015      : Y velocity of the ball
   // 0016      : Most recent input read in for the left paddle.
   // 0017      : Most recent input read in for the right paddle.

   int start = getCodeLoc();

                        LRA(695);     // A print later will tell you where this needs to jump to.
                        RET(14, 0);


// Clear Screen : (This just BARELY fits between frames...)
   int cls = getCodeLoc();

                        LDI(4);
                        LDI(0xA00);
                        SHL(0, 1);
                        LDI(' ' | (7 << 8));
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
                        LDI(' ' | (7 << 8));
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
                        LDI(' ' | (7 << 8));
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
                        LDI(' ' | (7 << 8));
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
                        LDI(' ' | (7 << 8));
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
                        LDI(' ' | (7 << 8));
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
                        LDI('#' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);               // We can CUT from here to LDI if need be.
                        LDI('#' | (7 << 8));
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
                        LDI('#' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
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
                        LDI(' ' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI(' ' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI(' ' | (7 << 8));
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
                        LDI('#' | (7 << 8));
                        ADD(1, 15);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        ST(1, 0);
                        INC(0);
                        INC(0);
                        ST(3, 0);
                        LDI(0x9E);
                        LDI('#' | (7 << 8));
                        ADD(2, 1);

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Check for paddle up/down. (This only flags whether the user is pressing a key for left/right paddle up/down.)
   int checkInput = getCodeLoc();

                        LDI(4);
                        LDI(0xBFF);
                        SHL(0, 1);
                        LDI(0xC);
                        ADD(1, 0); // BFFC is the address of the character present IO port
                        LDB(0);

                        LDI(2);
                        LD(0);
                        RETZ(2, 0); // If it is zero, there is nothing more to do.

                        INC(3); // BFFD is the character.
                        LDB(0);
                        LDI(32);
                        OR(0, 1); // OR it with 32 to make it a lower-case letter.

                        LDI('w'); // 'w' -> Left Paddle Up
                        SUB(0, 1);
                        ADD(2, 15);
                        BR(1, 4);
                        LDI(2);
                        LDI(0x16);
                        STB(1, 0);
                        ADD(2, 15);

                        LDI('s'); // 's' -> Left Paddle Down
                        SUB(0, 1);
                        ADD(2, 15);
                        BR(1, 4);
                        LDI(1);
                        LDI(0x16);
                        STB(1, 0);
                        ADD(2, 15);

                        LDI('p'); // 'p' -> Right Paddle Up
                        SUB(0, 1);
                        ADD(2, 15);
                        BR(1, 4);
                        LDI(2);
                        LDI(0x17);
                        STB(1, 0);
                        ADD(2, 15);

                        LDI('l'); // 'l' -> Right Paddle Down
                        SUB(0, 1);
                        ADD(2, 15);
                        BR(1, 4);
                        LDI(1);
                        LDI(0x17);
                        STB(1, 0);
                        ADD(2, 15);

                        LDI('6'); // '6' -> Reset
                        SUB(0, 1);
                        ADD(2, 15);
                        BR(1, 4);
                        LDI(4);
                        LDI(0xC00);
                        SHL(0, 1);
                        RET(14, 0);

                        LDI(2);
                        LD(0);
                        RET(14, 0);

// Update left paddle.
   int updateLeft = getCodeLoc();

                        LDI(0x10);
                        LDB(0);

                        LDI(0x16);
                        LDB(0);
                        LDI(1);
                        SUB(0 ,1);
                        ADD(4, 15);
                        BR(1, 6);
                        INC(0);
                        LDI(20);
                        SUB(0, 1);
                        ADD(2, 15);
                        BRZP(1, 1);
                        LDI(20);

                        LDI(0x16);
                        LDB(0);
                        LDI(2);
                        SUB(0, 1);
                        ADD(4, 15);
                        BR(1, 3);
                        DEC(0);
                        BRZP(0, 1);
                        LDI(0);

                        LDI(0x10);
                        STB(1, 0);
                        LDI(0x16);
                        STB(15, 0);

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Update right paddle.
   int updateRight = getCodeLoc();

                        LDI(0x11);
                        LDB(0);

                        LDI(0x17);
                        LDB(0);
                        LDI(1);
                        SUB(0 ,1);
                        ADD(4, 15);
                        BR(1, 6);
                        INC(0);
                        LDI(20);
                        SUB(0, 1);
                        ADD(2, 15);
                        BRZP(1, 1);
                        LDI(20);

                        LDI(0x17);
                        LDB(0);
                        LDI(2);
                        SUB(0, 1);
                        ADD(4, 15);
                        BR(1, 3);
                        DEC(0);
                        BRZP(0, 1);
                        LDI(0);

                        LDI(0x11);
                        STB(1, 0);
                        LDI(0x17);
                        STB(15, 0);

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Update the ball.
   int updateBall = getCodeLoc();

                        LDI(0xA);
                        LDB(0);
                        DEC(0);
                        BRZ(0, 4);
                        STB(0, 2);
                        LDI(2);
                        LD(0);
                        RET(14, 0);

                        LDI(4);
                        STB(0, 3);

                        LDI(0x13); // Change Y
                        LDI(0x15);
                        LDB(1);
                        LDB(1);
                        ADD(0, 1);
                        STB(0, 4);

                        LDI(0x13);
                        LDB(0);
                        BRP(0, 4);
                        LDI(1);
                        LDI(0x15);
                        STB(1, 0);
                        BRA(5);
                        LDI(23);
                        SUB(0, 1);
                        BRP(0, 2);
                        LDI(0x15);
                        STB(14, 0);

                        LDI(0x12); // Change X
                        LDI(0x14);
                        LDB(1);
                        LDB(1);
                        ADD(0, 1);
                        STB(0, 4);

                        LDI(0x12);
                        LDB(0);
                        BRP(0, 4);
                        LDI(1);
                        LDI(0x14);
                        STB(1, 0);
                        BRA(5);
                        LDI(78);
                        SUB(0, 1);
                        BRP(0, 2);
                        LDI(0x14);
                        STB(14, 0);

                        // TODO : Detect collision with paddle.

                        LDI(2);
                        LD(0);
                        RET(14, 0);


// Primary initialization.
   std::cout << "Jump location: " << ((-(getRelativeLocation(start) + 4)) >> 1) << " : " << getCodeLoc() << std::endl;

                        LDI(0x10); // Left Paddle
                        LDI(0x11); // Right Paddle
                        LDI(10);   // Put in the center
                        STB(0, 2);
                        STB(0, 1);

                        LDI(0xA);  // Frame limiter counter.
                        LDI(4);
                        STB(0, 1);

                        LDI(151);  // A prime
                        LDI(9);    // Random number
                        LDB(0);
                        ADD(0, 2);
                        STB(0, 2);

                        LDI(4);
                        SHR(1, 0);
                        LDI(15);
                        AND(0, 1);
                        ADD(0, 3);
                        LDI(0x13); // Y location : random
                        STB(1, 0);
                        LDI(0x12); // X location of ball : put in center
                        LDI(39);
                        STB(0, 1);

                        LDI(9);    // X direction : random
                        LDB(0);
                        LDI(4);
                        SHR(1, 0);
                        LDI(0x14);
                        BRO(1, 3);
                        LDI(1);
                        STB(0, 1);
                        BRA(1);
                        STB(14, 0);

                        LDI(9);    // Y direction : random
                        LDB(0);
                        LDI(5);
                        SHR(1, 0);
                        LDI(0x15);
                        BRO(1, 3);
                        LDI(1);
                        STB(0, 1);
                        BRA(1);
                        STB(14, 0);

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


// Main loop : spin and do updates on frame 0, 10, and 20. (Nope! Every Frame!)
   int mainLoop = getCodeLoc();

                        LDI(4);
                        LDI(0xB10);
                        SHL(0, 1);
                        LDB(0);
                        INC(0);
                        LDI(8);    // Store the timer in 8
                        STB(1, 0);

   int busyWait = getCodeLoc(); // This loop waits until the next frame has started to do any more processing. If we are lagging, this will CREATE stutter.

                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(checkInput)));
                        RET(14, 0);

                        LDI(4);
                        LDI(0xB10);
                        SHL(0, 1);
                        LDB(0);
                        INC(0);
                        LDI(8);    // Previous timer in 8
                        LDB(0);
                        SUB(0, 2);
                        LRA(convertLocationImm12(getRelativeLocation(busyWait)));
                        RETZ(1, 0);

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
                        LRA(convertLocationImm12(getRelativeLocation(clearBall)));
                        RET(14, 0);

                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(updateLeft)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(updateRight)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(updateBall)));
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
                        LRA(convertLocationImm12(getRelativeLocation(drawCourt)));
                        RET(14, 0);
                        LDI(2);
                        LRA(3);
                        ST(0, 1);
                        LRA(convertLocationImm12(getRelativeLocation(drawBall)));
                        RET(14, 0);

                        LRA(convertLocationImm12(getRelativeLocation(mainLoop)));
                        RET(14, 0);

                        LRA(0);
                        RET(14, 0);
 }
