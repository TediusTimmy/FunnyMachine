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
#include <stdio.h>

unsigned short bcd_add(unsigned short one, unsigned short two)
 {
   unsigned short res = one + two;
   if (((one & 15) + (two & 15)) > 9)
    {
      res += 6;
      two += 16;
    }
   one >>= 4;
   two >>= 4;
   if (((one & 15) + (two & 15)) > 9)
    {
      res += 0x60;
      two += 16;
    }
   one >>= 4;
   two >>= 4;
   if (((one & 0xF) + (two & 0xF)) > 0x9)
    {
      res += 0x600;
      two += 16;
    }
   one >>= 4;
   two >>= 4;
   if ((one + two) > 0x9) res += 0x6000;
   return res;
 }

int main (void)
 {
   printf("First, addition. Also test if the last digit is A because of a carry in.\n");
   printf("%04x %04x %04x\n", 0x1234, 0x1234, bcd_add(0x1234, 0x1234));
   printf("%04x %04x %04x\n", 0x1234, 0x1239, bcd_add(0x1234, 0x1239));
   printf("%04x %04x %04x\n", 0x1234, 0x123A, bcd_add(0x1234, 0x123A));
   printf("%04x %04x %04x\n", 0x9999, 0x0001, bcd_add(0x9999, 0x0001));
   printf("%04x %04x %04x\n", 0x9999, 0x0009, bcd_add(0x9999, 0x0009));
   printf("%04x %04x %04x\n", 0x9999, 0x000A, bcd_add(0x9999, 0x000A));
   printf("%04x %04x %04x\n", 0x0003, 0x0004, bcd_add(0x0003, 0x0004));
   printf("%04x %04x %04x\n", 0x0006, 0x0004, bcd_add(0x0006, 0x0004));
   printf("\nNext, subtraction. Note that because we add 1 for subtraction, subtraction with borrow just doesn't add one.\nIn effect, the least significant bit of the instruction is the carry in bit given to the ALU.\n");
   printf("%04x %04x %04x\n", 0x1234, 0x1234, bcd_add(0x1234, 0x9999 - 0x1234 + 1));
   printf("%04x %04x %04x\n", 0x1235, 0x1234, bcd_add(0x1235, 0x9999 - 0x1234 + 1));
   printf("%04x %04x %04x\n", 0x0010, 0x0005, bcd_add(0x0010, 0x9999 - 0x0005 + 1));
   printf("%04x %04x %04x\n", 0x1234, 0x1235, bcd_add(0x1234, 0x9999 - 0x1235 + 1));
   printf("%04x %04x %04x\n", 0x9999, 0x0000, bcd_add(0x9999, 0x9999 - 0x0000 + 1));
   return 0;
 }
