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

   @four    LDI 4
   @base    LDI $BFF
   @temp    SHL base, four
   @bottom  LDI $E
   @write   OR  temp, bottom
   @charH   LDI 'H'
   @chare   LDI 'e'
   @charl   LDI 'l'
   @charo   LDI 'o'
   @charCo  LDI ','
   @charSp  LDI ' '
   @charW   LDI 'W'
   @charr   LDI 'r'
   @chard   LDI 'd'
   @charEx  LDI '!'
   @charNL  LDI 10
            STB charH, write
            STB chare, write
            STB charl, write
            STB charl, write
            STB charo, write
            STB charCo, write
            STB charSp, write
            STB charW, write
            STB charo, write
            STB charr, write
            STB charl, write
            STB chard, write
            STB charEx, write
            STB charNL, write
   @halt    INC write
            STB halt, halt
