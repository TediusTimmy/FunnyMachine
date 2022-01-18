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
#include "Base.h"

void Core::attach(MemoryController* mc)
 {
   M = mc;
 }

void Core::reset()
 {
   PC = M->doRead(0xFFFE, BOTH_BYTES) & 0xFFFE;
   B[ 0] = 0;
   B[ 1] = 0;
   B[ 2] = 0;
   B[ 3] = 0;
   B[ 4] = 0;
   B[ 5] = 0;
   B[ 6] = 0;
   B[ 7] = 0;
   B[ 8] = 0;
   B[ 9] = 0;
   B[10] = 0;
   B[11] = 0;
   B[12] = 0;
   B[13] = 0;
   B[14] = 0;
   B[15] = 0;
   P = 0;
 }

void Core::doOneOp()
 {
   word I = M->doRead(PC, BOTH_BYTES);
#ifdef DO_TRACE
   printDecompiled(I, PC);
#endif
   PC += 2;

   word t;
   t = (I >> 8) & 15;
   word A1 = (t == 15) ? 0 : B[(P + t) & 15];
   t = (I >> 12) & 15;
   word A2 = (t == 15) ? 0 : B[(P + t) & 15];
   t = (I >> 4) & 15;
   word A3 = (t == 15) ? 0 : B[(P + t) & 15];
   word imm12 = (I >> 4) | ((I & 0x8000) ? 0xF000 : 0);
   word imm8 = ((I >> 8) | ((I & 0x8000) ? 0xFF00 : 0)) << 1;

   switch (I & 15)
    {
   case  0: // NOP
      break;
   case  1: // LD
      P = (P - 1) & 15;
      B[P] = M->doRead(A1 & 0xFFFE, ((I & 16) ? BOTH_BYTES : ((A1 & 1) ? HIGH_BYTE : LOW_BYTE)));
      if (~I & 16) // If we aren't doing a word read...
       {
         if (A1 & 1)
            B[P] >>= 8; // If we only asked for the high byte, give me the high byte.
         else
            B[P] &= 0xFF; // Else, give me only the low byte.
       }
      break;
   case  2: // ST
      if ((~I & 16) && (A2 & 1)) A1 <<= 8; // If we are only writing the high byte, write it in that position.
      M->doWrite(A2 & 0xFFFE, A1, ((I & 16) ? BOTH_BYTES : ((A2 & 1) ? HIGH_BYTE : LOW_BYTE)));
      break;
   case  3: // LDI
      P = (P - 1) & 15;
      B[P] = imm12;
      break;
   case  4: // LRA
      P = (P - 1) & 15;
      B[P] = PC + (imm12 << 1);
      break;
   case  5: // ALU
      P = (P - 1) & 15;
      switch ((I >> 4) & 15)
       {
      case  0:
         t = A1 + A2;
         break;
      case  1:
         t = A1 + A2 + 1;
         break;
      case  2:
         t = A1 - A2 - 1;
         break;
      case  3:
         t = A1 - A2;
         break;
      case  4:
         t = A1 << (A2 & 15);
         break;
      case  5:
         t = A1 << (A2 & 15);
         break;
      case  6:
         t = A1 >> (A2 & 15);
         break;
      case  7:
         t = (word)(((signed short)A1) >> (A2 & 15));
         break;
      case  8:
         t = A1 & A2;
         break;
      case  9:
         t = A1 | A2;
         break;
      case 10:
         t = A1 ^ A2;
         break;
      case 11:
         t = (A1 << (A2 & 15)) | (A1 >> (-A2 & 15));
         break;
      case 12:
         t = ~(A1 & A2);
         break;
      case 13:
         t = ~(A1 | A2);
         break;
      case 14:
         t = ~(A1 ^ A2);
         break;
      case 15:
         t = (A1 >> (A2 & 15)) | (A1 << (-A2 & 15));
         break;
       }
      B[P] = t;
      break;
   case  6: // BCD
      if (~I & 64) // Actual BCD instructions have one result
       {
         P = (P - 1) & 15;
         switch ((I >> 4) & 3)
          {
         case  0:
            t = bcd_add(A1, A2);
            break;
         case  1:
            t = bcd_add(A1, A2 + 1);
            break;
         case  2:
            t = bcd_add(A1, 0x9999 - A2);
            break;
         case  3:
            t = bcd_add(A1, 0x9999 - A2 + 1);
            break;
          }
         B[P] = t;
       }
      else // MUL/DIV instructions give two results
       {
         P = (P - 1) & 15;
         t = P;
         P = (P - 1) & 15;
         switch ((I >> 4) & 3)
          {
         case  0:
          {
            int T = ((int)((signed short)A1)) * ((int)((signed short)A2));
            B[P] = T >> 16;
            B[t] = T & 0xFFFF;
          }
            break;
         case  1:
          {
            int T = ((int)A1) * ((int)A2);
            B[P] = T >> 16;
            B[t] = T & 0xFFFF;
          }
            break;
         case  2:
            if (0 != A2)
             {
               B[P] = ((signed short)A1) % ((signed short)A2);
               B[t] = ((signed short)A1) / ((signed short)A2);
             }
            else
             {
               B[P] = A1;
               B[t] = 0;
             }
            break;
         case  3:
            if (0 != A2)
             {
               B[P] = A1 % A2;
               B[t] = A1 / A2;
             }
            else
             {
               B[P] = A1;
               B[t] = 0;
             }
            break;
          }
       }
      break;
   case  7: // RET
      switch ((I >> 4) & 7)
       {
      case 0:
         if (A1 == 0) PC = A2 & 0xFFFE;
         break;
      case 1:
         if (A1 & 0x8000) PC = A2 & 0xFFFE;
         break;
      case 2:
         if (A1 & 1) PC = A2 & 0xFFFE;
         break;
      case 3:
         if (!((A1 == 0) || (A1 & 0x8000))) PC = A2 & 0xFFFE;
         break;
      case 4:
         if (A1 != 0) PC = A2 & 0xFFFE;
         break;
      case 5:
         if (~A1 & 0x8000) PC = A2 & 0xFFFE;
         break;
      case 6:
         if (~A1 & 1) PC = A2 & 0xFFFE;
         break;
      case 7:
         if ((A1 == 0) || (A1 & 0x8000)) PC = A2 & 0xFFFE;
         break;
       }
      break;
   case  8:
      if (A3 == 0) PC = PC + imm8;
      break;
   case  9:
      if (A3 & 0x8000) PC = PC + imm8;
      break;
   case 10:
      if (A3 & 1) PC = PC + imm8;
      break;
   case 11:
      if (!((A3 == 0) || (A3 & 0x8000))) PC = PC + imm8;
      break;
   case 12:
      if (A3 != 0) PC = PC + imm8;
      break;
   case 13:
      if (~A3 & 0x8000) PC = PC + imm8;
      break;
   case 14:
      if (~A3 & 1) PC = PC + imm8;
      break;
   case 15:
      if ((A3 == 0) || (A3 & 0x8000)) PC = PC + imm8;
      break;
    }

   return;
 }

word Core::bcd_add(word one, word two)
 {
   word res = one + two;
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

#ifndef DO_TRACE
void Core::printDecompiled(word, word) const
 {
 }
#endif
