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

void Core::clearBelt(int n)
 {
   B [n][ 0] = 0;
   B [n][ 1] = 0;
   B [n][ 2] = 0;
   B [n][ 3] = 0;
   B [n][ 4] = 0;
   B [n][ 5] = 0;
   B [n][ 6] = 0;
   B [n][ 7] = 0;
   B [n][ 8] = 0;
   B [n][ 9] = 0;
   B [n][10] = 0;
   B [n][11] = 0;
   B [n][12] = 0;
   B [n][13] = 0;
   B [n][14] = 0;
   B [n][15] = 0;
   P [n] = 0;
 }

void Core::reset()
 {
   PC[0] = M->doRead(0xFFFE, BOTH_BYTES) & 0xFFFE;
   F = 0;
   L = 0;
   clearBelt(0);
 }

void Core::dropResult(word r)
 {
   P[L] = (P[L] - 1) & 15;
   B[L][P[L]] = r;
 }

word Core::getBelt(word l) const
 {
   return B[L][(P[L] + l) & 15];
 }

bool Core::halted() const
 {
   return !!(F & 0x40);
 }

void Core::doOneOp()
 {
   word I = M->doRead(PC[L], BOTH_BYTES);
#ifdef DO_TRACE
   printDecompiled(I, PC[L]);
#endif
   PC[L] += 2;

   word I1 = (I >> 8) & 15;
   word A1 = (I1 == 15) ? 0 : getBelt(I1);
   word I2 = (I >> 12) & 15;
   word A2 = (I2 == 15) ? 0 : getBelt(I2);
   word I3 = (I >> 4) & 15;
   word A3 = (I3 == 15) ? 0 : getBelt(I3);
   word imm12 = (I >> 4) | ((I & 0x8000) ? 0xF000 : 0);

   word t;
   switch (I & 15)
    {
   case  0: // NOP
      switch (I3) // NOP was ripe for abuse...
       {
      case 0: // REAL NOP
         break;
      case 2: // INT
         if (I1 < 8) // Regular interrupt
          {
            if (L <= 2) // An NMI cannot call a regular interrupt
             {
               F |= 2;
               L = 2;
               PC[2] = M->doRead(0xFFFE - ((I1 ^ 8) << 1), BOTH_BYTES) & 0xFFFE;
               clearBelt(2);
             }
          }
         else // NMI : there isn't a real distinction here
          {
            if (I1 != 8) // NMI 0 is __SPECIAL__
             {
               F |= 4;
               L = 3;
               PC[3] = M->doRead(0xFFFE - ((I1 ^ 8) << 1), BOTH_BYTES) & 0xFFFE;
               clearBelt(3);
             }
            else
             {
               reset(); // Notice that the read logic for NMI 0 is the reset vector.
             }
          }
         break;
      case 3: // RTI
            // The four modes are layered, but not stacked:
            // you can't return from a lower mode to a higher mode, and
            // you can't invoke a lower mode from a higher mode.
            // This preserves the integrity of the processor state.
         if (L == 3)
          {
            F &= ~4;
            if (F & 2)
               L = 2;
            else if (F & 1)
               L = 1;
            else
               L = 0;
          }
         else if (L == 2)
          {
            F &= ~2;
            if (F & 1)
               L = 1;
            else
               L = 0;
          }
         break;
      case 4: // CLI : Nothing uses this flag.
         F &= ~0x80;
         break;
      case 5: // ENI : Nothing uses this flag.
         F |= 0x80;
         break;
      case 6: // WAIT for interrupt. As there are no interrupts, this will just be HALT.
         F |= 0x40;
         break;
      case 7: // HALT and catch fire.
         F |= 0x40;
         break;
      case 8: // INTER interrogate state, can only be performed from an interrupt
         if (L > 1)
          {
            if (I1 < 4)
             {
               dropResult(B[I1][(P[I1] + I2) & 15]);
             }
            else if (I1 < 8)
             {
               dropResult(PC[I1 & 3]);
             }
            else
             {
               dropResult(0xFFFF);
             }
          }
         break;
      case 15: // ENDOP
         if (L == 1) // We must be in a SDO to end it. This OUGHT to be an error if we are in an interrupt.
          {
            F &= ~1; // We should check this flag. L should never == 1 when F & 1 is zero (or F & 6 is non-zero).
            L = 0; // This changes PC and the belt.
            if (15 != I2)
               dropResult(A2);
            if (15 != I1)
               dropResult(A1);
          }
       }
      break;
   case  1: // LD
      if (~I3 & 2) // If this is a read from memory
       {
         t = M->doRead(A1 & 0xFFFE, ((I3 & 1) ? BOTH_BYTES : ((A1 & 1) ? HIGH_BYTE : LOW_BYTE)));
         if (~I3 & 1) // If we aren't doing a word read...
          {
            if (A1 & 1)
               t >>= 8; // If we only asked for the high byte, give me the high byte.
            else
               t &= 0xFF; // Else, give me only the low byte.

            if (t & 0x80)
               t |= 0xFF00; // Sign-extend a byte read.
          }
       }
      else // This is a scratchpad read.
       {
         t = R[I1];
       }
      dropResult(t);
      break;
   case  2: // ST
      if (~I3 & 2) // If this is a memory write.
       {
         if ((~I3 & 1) && (A2 & 1)) A1 <<= 8; // If we are only writing the high byte, write it in that position.
         M->doWrite(A2 & 0xFFFE, A1, ((I3 & 1) ? BOTH_BYTES : ((A2 & 1) ? HIGH_BYTE : LOW_BYTE)));
       }
      else // This is a scratchpad write.
       {
         R[I2] = A1;
       }
      break;
   case  3: // LDI
      dropResult(imm12);
      break;
   case  4: // LRA
      dropResult(PC[L] + (imm12 << 1));
      break;
   case  5: // ALU
      switch (I3)
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
         t = (A1 & 0xFFF) | (I2 << 12);
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
         t = (A1 << (A2 & 15)) | (A1 >> (-A2 & 15));
         break;
       }
      dropResult(t);
      break;
   case  6: // SDO0 - SDOF - Software-defined operation
      if (L <= 1) // An interrupt cannot call an SDO, see RTI for why.
       {
         F |= 1;
         L = 1;
         PC[1] = M->doRead(0xFFDE - (I3 << 1), BOTH_BYTES) & 0xFFFE;
         clearBelt(1);
         if (15 != I2)
            dropResult(A2);
         if (15 != I1)
            dropResult(A1);
       }
      break;
   case  7: // RET
      switch (I3 & 7)
       {
      case 0:
         if (A1 == 0) PC[L] = A2 & 0xFFFE;
         break;
      case 1:
         if (A1 & 0x8000) PC[L] = A2 & 0xFFFE;
         break;
      case 2:
         if (A1 & 1) PC[L] = A2 & 0xFFFE;
         break;
      case 3:
         if (!((A1 == 0) || (A1 & 0x8000))) PC[L] = A2 & 0xFFFE;
         break;
      case 4:
         if (A1 != 0) PC[L] = A2 & 0xFFFE;
         break;
      case 5:
         if (~A1 & 0x8000) PC[L] = A2 & 0xFFFE;
         break;
      case 6:
         if (~A1 & 1) PC[L] = A2 & 0xFFFE;
         break;
      case 7:
         if ((A1 == 0) || (A1 & 0x8000)) PC[L] = A2 & 0xFFFE;
         break;
       }
      break;
   case  8:
      if (A3 == 0)
         dropResult(A1);
      else
         dropResult(A2);
      break;
   case  9:
      if (A3 & 0x8000)
         dropResult(A1);
      else
         dropResult(A2);
      break;
   case 10:
      if (A3 & 1)
         dropResult(A1);
      else
         dropResult(A2);
      break;
   case 11:
      if (!((A3 == 0) || (A3 & 0x8000)))
         dropResult(A1);
      else
         dropResult(A2);
      break;
   case 12:
      if (A3 != 0)
         dropResult(A1);
      else
         dropResult(A2);
      break;
   case 13:
      if (~A3 & 0x8000)
         dropResult(A1);
      else
         dropResult(A2);
      break;
   case 14:
      if (~A3 & 1)
         dropResult(A1);
      else
         dropResult(A2);
      break;
   case 15:
      if ((A3 == 0) || (A3 & 0x8000))
         dropResult(A1);
      else
         dropResult(A2);
      break;
    }

   return;
 }

#ifndef DO_TRACE
void Core::printDecompiled(word, word) const
 {
 }
#endif
