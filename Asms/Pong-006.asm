;   Copyright (C) 2022 Thomas DiModica <ricinwich@yahoo.com>
;
;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.
;
;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   ; Memory map : 
   ; 0002-0003 : Return pointer - where to return from subroutine.
   ; 0008      : Timer
   ; 0009      : Random Counter
   ; 0010      : Y location of the left paddle
   ; 0011      : Y location of the right paddle
   ; 0012      : X location of the ball
   ; 0013      : Y location of the ball
   ; 0014      : X velocity of the ball
   ; 0015      : Y velocity of the ball
   ; 0016      : Most recent input read in for the left paddle.
   ; 0017      : Most recent input read in for the right paddle.

                        LRA main       ; The only thing I've changed: this first jump.
                        RET 0


cls: ; Clear Screen : (This just BARELY fits between frames...)
                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        LDI ' ' | (7 << 8)
                        ADD 1, 15
@stloop:
                        ST  1, 0
                        INC 0
                        INC 0
                        LDI 4
                        LDI $AFA
                        SHL 0, 1
                        SUB 0, 3
                        LDI ' ' | (7 << 8)
                        ADD 5, 15
                        BR  2, stloop

                        LDI 2
                        LD  0
                        RET 0


drawCourt: ; Draw the dividing line in the court
                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        LDI $4E
                        ADD 1, 0
                        LDI '#' | (7 << 8)
                        ADD 1, 15
@stloop:
                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $13E
                        ADD 1, 0
                        LDI 4
                        LDI $AFA
                        SHL 0, 1
                        SUB 0, 3
                        LDI '#' | (7 << 8)
                        ADD 5, 15
                        BRP 2, stloop

                        LDI 2
                        LD  0
                        RET 0


clearLeft: ; Clear the left side of the court
                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        LDI ' ' | (7 << 8)
                        ADD 1, 15
@stloop:
                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        ADD 1, 0
                        LDI 4
                        LDI $AFA
                        SHL 0, 1
                        SUB 0, 3
                        LDI ' ' | (7 << 8)
                        ADD 5, 15
                        BRP 2, stloop

                        LDI 2
                        LD  0
                        RET 0


clearRight: ; Clear the right side of the court
                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        LDI $9C
                        ADD 1, 0
                        LDI ' ' | (7 << 8)
                        ADD 1, 15
@stloop:
                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        ADD 1, 0
                        LDI 4
                        LDI $AFA
                        SHL 0, 1
                        SUB 0, 3
                        LDI ' ' | (7 << 8)
                        ADD 5, 15
                        BRP 2, stloop

                        LDI 2
                        LD  0
                        RET 0


drawLeft: ; Draw the left paddle
                        LDI $10
                        LDB 0   ; Here's the location of the paddle, but I need its screen memory position: 160 * y
                        LDI 1   ; Current Bit
                        LDI 160 ; Current Addend
                        LDI 0   ; Current Sum

; While it would be easy to use the multiplication instruction, I am going to limit myself to not do it.
; I'm going to do this algorithmically with shifts and adds.
; The following block of code we run five times, in order to multiply a number by up to 31.

                        AND 2, 3  ; Test if the current bit is set.
                        BRZ 0, 10 ; If the current bit is not set, jump down.
                        ADD 4, 15 ; The current bit is set, so recreate the belt with the modified sum. Copy the paddle position.
                        ADD 4, 15 ; Copy the current bit
                        ADD 4, 15 ; Copy the current addend.
                        ADD 4, 0  ; Add the current addend to the sum to make the new sum.
                        LDI 0     ; This is the result of the AND, needed to keep belt coherency.
                        LDI 1
                        SHL 4, 0  ; Multiply the current bit by 2
                        SHL 4, 1  ; Multiply the current addend by two.
                        ADD 7, 15 ; Copy the paddle position.
                        ADD 2, 15 ; Copy the new current bit.
                        ADD 2, 15 ; Copy the new addend.
                        ADD 7, 15 ; Copy the current sum.

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0     ; I could CUT everything from here down to the next portion.
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

; We now have 160 * the y position of the paddle (which should be less than 31).
; Now, lets assume that a paddle is 5 characters tall and unroll that loop, too:

                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        ADD 0, 3
                        LDI '#' | (7 << 8)
                        ADD 1, 15

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E            ; We can CUT from here to LDI if need be.
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        LDI 2
                        LD  0
                        RET 0


drawRight: ; Draw the right paddle
                        LDI $11
                        LDB 0   ; Here's the location of the paddle, but I need its screen memory position: 160 * y
                        LDI 1   ; Current Bit
                        LDI 160 ; Current Addend
                        LDI 0   ; Current Sum

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        ADD 0, 3
                        LDI $9C
                        ADD 1, 0
                        LDI '#' | (7 << 8)
                        ADD 1, 15

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        LDI 2
                        LD  0
                        RET 0


clearBall: ; Clear the ball. (I really need to make multiplication a subroutine!)
                        LDI $13
                        LDB 0   ; Y location
                        LDI 1   ; Current Bit
                        LDI 160 ; Current Addend
                        LDI 0   ; Current Sum

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        ADD 0, 3
                        LDI $12  ; X Location
                        LDB 0
                        ADD 0, 0 ; Multiply by 2.
                        ADD 3, 0
                        LDI ' ' | (7 << 8)
                        ADD 1, 15

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI ' ' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI ' ' | (7 << 8)
                        ADD 2, 1

                        LDI 2
                        LD  0
                        RET 0


drawBall: ; Draw the ball.
                        LDI $13
                        LDB 0   ; Y location
                        LDI 1   ; Current Bit
                        LDI 160 ; Current Addend
                        LDI 0   ; Current Sum

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        AND 2, 3
                        BRZ 0, 10
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 15
                        ADD 4, 0
                        LDI 0
                        LDI 1
                        SHL 4, 0
                        SHL 4, 1
                        ADD 7, 15
                        ADD 2, 15
                        ADD 2, 15
                        ADD 7, 15

                        LDI 4
                        LDI $A00
                        SHL 0, 1
                        ADD 0, 3
                        LDI $12  ; X Location
                        LDB 0
                        ADD 0, 0 ; Multiply by 2.
                        ADD 3, 0
                        LDI '#' | (7 << 8)
                        ADD 1, 15

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        ST  1, 0
                        INC 0
                        INC 0
                        ST  3, 0
                        LDI $9E
                        LDI '#' | (7 << 8)
                        ADD 2, 1

                        LDI 2
                        LD  0
                        RET 0


checkInput: ; Check for paddle up/down. (This only flags whether the user is pressing a key for left/right paddle up/down.)
                        LDI 4
                        LDI $BFF
                        SHL 0, 1
                        LDI $C
                        ADD 1, 0 ; BFFC is the address of the character present IO port
                        LDB 0

                        LDI 2
                        LD  0
                        RETZ 2, 0 ; If it is zero, there is nothing more to do.

                        INC 3 ; BFFD is the character.
                        LDB 0
                        LDI 32
                        OR  0, 1 ; OR it with 32 to make it a lower-case letter.

                        LDI 'w' ; 'w' -> Left Paddle Up
                        SUB 0, 1
                        ADD 2, 15
                        BR  1, 8
                        LDI 2
                        LDI $16
                        STB 1, 0
                        ADD 2, 15

                        LDI 's' ; 's' -> Left Paddle Down
                        SUB 0, 1
                        ADD 2, 15
                        BR  1, 8
                        LDI 1
                        LDI $16
                        STB 1, 0
                        ADD 2, 15

                        LDI 'p' ; 'p' -> Right Paddle Up
                        SUB 0, 1
                        ADD 2, 15
                        BR  1, 8
                        LDI 2
                        LDI $17
                        STB 1, 0
                        ADD 2, 15

                        LDI 'l' ; 'l' -> Right Paddle Down
                        SUB 0, 1
                        ADD 2, 15
                        BR  1, 8
                        LDI 1
                        LDI $17
                        STB 1, 0
                        ADD 2, 15

                        LDI '6' ; '6' -> Reset
                        SUB 0, 1
                        ADD 2, 15
                        BR  1, 8
                        LDI 4
                        LDI $C00
                        SHL 0, 1
                        RET 0

                        LDI 2
                        LD  0
                        RET 0


updateLeft: ; Update left paddle.
                        LDI $10
                        LDB 0

                        LDI $16
                        LDB 0
                        LDI 1
                        SUB 0 ,1
                        ADD 4, 15
                        BR  1, 12
                        INC 0
                        LDI 20
                        SUB 0, 1
                        ADD 2, 15
                        BRZP 1, 2
                        LDI 20

                        LDI $16
                        LDB 0
                        LDI 2
                        SUB 0, 1
                        ADD 4, 15
                        BR  1, 6
                        DEC 0
                        BRZP 0, 2
                        LDI 0

                        LDI $10
                        STB 1, 0
                        LDI $16
                        STB 15, 0

                        LDI 2
                        LD  0
                        RET 0


updateRight: ; Update right paddle.
                        LDI $11
                        LDB 0

                        LDI $17
                        LDB 0
                        LDI 1
                        SUB 0 ,1
                        ADD 4, 15
                        BR  1, 12
                        INC 0
                        LDI 20
                        SUB 0, 1
                        ADD 2, 15
                        BRZP 1, 2
                        LDI 20

                        LDI $17
                        LDB 0
                        LDI 2
                        SUB 0, 1
                        ADD 4, 15
                        BR  1, 6
                        DEC 0
                        BRZP 0, 2
                        LDI 0

                        LDI $11
                        STB 1, 0
                        LDI $17
                        STB 15, 0

                        LDI 2
                        LD  0
                        RET 0


updateBall: ; Update the ball.

   ; 0010      : Y location of the left paddle
   ; 0011      : Y location of the right paddle
   ; 0012      : X location of the ball
   ; 0013      : Y location of the ball
   ; 0014      : X velocity of the ball
   ; 0015      : Y velocity of the ball

                        LDI $A    ; Load the frame counter: only update every 4 frames (7.5 frames / sec)
                        LDB 0
                        DEC 0     ; Decrement counter
                        BRZ 0, 8  ; If the counter is zero, update
                        STB 0, 2  ; Else, store the updated counter and return
                        LDI 2
                        LD  0
                        RET 0

                        LDI 4     ; Reset the counter
                        STB 0, 3

                                  ; Detect collision with paddle (do before the update)
                        LDI $12   ; X location of ball
                        LDB 0
                        LDI 2
                        SUB 0, 1
                        NOP
                        BR  0, 40 ; If we are at column 2, else goto $$$
                        LDI $13   ; Y location of the ball
                        LDI $10   ; Y location of left paddle
                        LDB 1     ;     ball
                        LDB 1     ;     paddle
                        DEC 0
                        SUB 0, 2  ; If paddle minus ball is positive, then ball is above paddle
                        BRP 0, 64 ; goto %%%
                        LDI 4
                        ADD 0, 3
                        SUB 5, 0  ; If ball minus paddle is positive, then ball is below paddle
                        BRP 0, 56 ; goto %%%
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        LDI $14
                        LDI 1
                        STB 0, 1
                        BRA 38    ; goto %%%
                        LDI 76    ; $$$
                        SUB 0, 3
                        BRP 0, 32
                        LDI $14
                        NOT 15
                        STB 0, 1
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP
                        NOP      ; %%% : after

                                 ; Actual update code:

                        LDI $13  ; Change Y
                        LDI $15
                        LDB 1
                        LDB 1
                        ADD 0, 1
                        STB 0, 4  ; Load the current Y and Y change, add them, and unconditionally store the new Y

                        LDI $13   ; Reload Y : because we don't sign extend the change, this drops the carry out when change is -1
                        LDB 0
                        BRP 0, 8  ; If Y is not zero, jump ahead to !!!
                        LDI 1     ; Y is zero, so change the direction of Y change
                        LDI $15
                        STB 1, 0
                        BRA 12
                        LDI 23    ; !!! : If Y is not zero, compare it with 23 (the bottom of the court)
                        SUB 0, 1
                        BRP 0, 6
                        LDI $15   ; If Y is 23 (!or more!) change the direction of Y change
                        NOT 15
                        STB 0, 1

                        LDI $12   ; Change X
                        LDI $14
                        LDB 1
                        LDB 1
                        ADD 0, 1
                        STB 0, 4  ; Load the current X and X change, add them, and unconditionally store the new X

                        LDI $12   ; Reload X : because we don't sign extend the change, this drops the carry out when change is -1
                        LDB 0
                        BRZ 0, 12 ; If zero, then we are on the far left, and the right player has scored: jump to reset
                        LDI 78
                        SUB 0, 1
                        BRZN 0, 6 ; If > 78, then we are on the far right, and the left player has scored: jump to reset

                        LDI 2
                        LD  0
                        RET 0


main: ; Primary initialization.
                        LDI $10  ; Left Paddle
                        LDI $11  ; Right Paddle
                        LDI 10   ; Put in the center
                        STB 0, 2
                        STB 0, 1

                        LDI $A   ; Frame limiter counter.
                        LDI 4
                        STB 0, 1

                        LDI 151  ; A prime
                        LDI 9    ; Random number
                        LDB 0
                        ADD 0, 2
                        STB 0, 2

                        LDI 4
                        SHR 1, 0
                        LDI 15
                        AND 0, 1
                        ADD 0, 3
                        LDI $13  ; Y location : random
                        STB 1, 0
                        LDI $12  ; X location of ball : put in center
                        LDI 39
                        STB 0, 1

                        LDI 9    ; X direction : random
                        LDB 0
                        LDI 4
                        SHR 1, 0
                        LDI $14
                        BRO 1, 6
                        LDI 1
                        STB 0, 1
                        BRA 4
                        NOT 15
                        STB 0, 1

                        LDI 9    ; Y direction : random
                        LDB 0
                        LDI 5
                        SHR 1, 0
                        LDI $15
                        BRO 1, 6
                        LDI 1
                        STB 0, 1
                        BRA 4
                        NOT 15
                        STB 0, 1

                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA cls
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA drawCourt
                        RET 0


mainLoop: ; Main loop : spin and do updates on frame 0, 10, and 20. (Nope! Every Frame!)

                        LDI 4
                        LDI $B10
                        SHL 0, 1
                        LDB 0
                        INC 0
                        LDI 8    ; Store the timer in 8
                        STB 1, 0

busyWait: ; This loop waits until the next frame has started to do any more processing. If we are lagging, this will CREATE stutter.
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA checkInput
                        RET 0

                        LDI 4
                        LDI $B10
                        SHL 0, 1
                        LDB 0
                        INC 0
                        LDI 8    ; Previous timer in 8
                        LDB 0
                        SUB 0, 2
                        LRA busyWait
                        RETZ 1, 0

                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA clearLeft
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA clearRight
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA clearBall
                        RET 0

                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA updateLeft
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA updateRight
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA updateBall
                        RET 0

                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA drawLeft
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA drawRight
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA drawCourt
                        RET 0
                        LDI 2
                        LRA 6
                        ST  0, 1
                        LRA drawBall
                        RET 0

                        LRA mainLoop
                        RET 0

                        LRA 0
                        RET 0
