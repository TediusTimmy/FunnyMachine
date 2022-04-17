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

      @four    LDI 4                         ; @ Declares the name of a result. If there is already a result by this name, clobber it.
      @base    LDI $A00
      @write   SHL base, four
      @charH   LDI 'H' | (7 << 8)            ; Look, ma! We can do some complex math to generate a constant.
      @chare   LDI 'e' | (7 << 8)
      @charCo  LDI ',' | (7 << 8)
      @charSp  LDI ' ' | (7 << 8)
      @charW   LDI 'W' | (7 << 8)
      @charo   LDI 'o' | (7 << 8)
      @charr   LDI 'r' | (7 << 8)
      @charl   LDI 'l' | (7 << 8)
      @chard   LDI 'd' | (7 << 8)
      @charEx  LDI '!' | (7 << 8)
      @two     LDI 2
               ST  charH, write
      %write   ADD write, two                ; % Tells the assembler to generate an error if the result by this name is not still alive.
               ST  chare, write              ; We are insisting that we are overwriting the name of a previous result.
      %write   ADD write, two
               ST  charl, write
      %write   ADD write, two
               ST  charl, write
      %write   ADD write, two
               ST  charo, write
      %write   ADD write, two
               ST  charCo, write
      %write   ADD write, two
               ST  charSp, write
      %write   ADD write, two
               ST  charW, write
      %write   ADD write, two
               ST  charo, write
      %write   ADD write, two
               ST  charr, write
      %write   ADD write, two
               ST  charl, write
      %write   ADD write, two
               ST  chard, write
      %write   ADD write, two
               ST  charEx, write

loop: #four    LDI 4                         ; # Tells the assembler to generate an error if the result by this name is not dead.
      #base    LDI $BFF                      ; We are insisting that we are recycling this name, not overwriting it.
      @temp    SHL base, four
      @bottom  LDI $C
      %write   OR  temp, bottom

      @chars   LDB write
               BRZ chars, loop

      %write   INC write
               LDB write

               LDI 10
               SUB 1, 0
               BR  0, loop

      @four    LDI 4                         ; And, sometimes, we don't care. Both % and # are tools for the programmer to check assertions.
      @base    LDI $BFF
      @temp    SHL base, four
      @bottom  LDI $F
      @write   OR  temp, bottom
               STB write, write
