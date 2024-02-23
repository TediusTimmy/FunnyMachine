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

#include <cstdio>

void Core::printDecompiled(word I, word pc) const
 {
   word NEXT = pc + 2;

   word A1 = (I >> 8) & 15;
   word V1 = (A1 == 15) ? 0 : getBelt(A1);
   word A2 = (I >> 12) & 15;
   word V2 = (A2 == 15) ? 0 : getBelt(A2);
   word A3 = (I >> 4) & 15;
   word V3 = (A3 == 15) ? 0 : getBelt(A3);
   word imm12 = (I >> 4) | ((I & 0x8000) ? 0xF000 : 0);

   word l = I & 0xFF;
   word h = I >> 8;
   if ((l < 32) || (l > 126)) l = '.';
   if ((h < 32) || (h > 126)) h = '.';
   std::printf("%04x: %02x%02x (%c%c) : ", pc, I & 0xFF, I >> 8, l, h);
   // Yes, this is a copy-paste of doOneOp.
   switch (I & 15)
    {
   case  0: // NOP
      switch (A3)
       {
      case 2:
         std::printf("INT $%x", A1);
         break;
      case 3:
         std::printf("RTI");
         break;
      case 4:
         std::printf("CLI");
         break;
      case 5:
         std::printf("ENI");
         break;
      case 6:
         std::printf("WAIT");
         break;
      case 7:
         std::printf("HALT");
         break;
      case 8:
         std::printf("INTER $%x, $%x", A1, A2);
         break;
      case 15:
         std::printf("ENDOP $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      default:
         std::printf("NOP");
         break;
       }
      break;
   case  1: // LD
      if (I & 32)
         std::printf("LDS $%x(%04x)", A1, R[A1]); // This function has too much LDS.
      else if (I & 16)
         std::printf("LD $%x(%04x)", A1, V1);
      else
         std::printf("LDB $%x(%04x)", A1, V1);
      break;
   case  2: // ST
      if (I & 32)
         std::printf("STS $%x(%04x), $%x", A1, V1, A2);
      else if (I & 16)
         std::printf("ST $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
      else
         std::printf("STB $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
      break;
   case  3: // LDI
      std::printf("LDI $%03x", imm12 & 0xFFF);
      break;
   case  4: // LRA
      std::printf("LRA $%03x : (%04x)", imm12 & 0xFFF, (NEXT + (imm12 << 1)) & 0xFFFF);
      break;
   case  5: // ALU
      switch (A3)
       {
      case  0:
         std::printf("ADD $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  1:
         if (A2 == 15)
            std::printf("INC $%x(%04x)", A1, V1);
         else
            std::printf("ADC $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  2:
         if (A2 == 15)
            std::printf("DEC $%x(%04x)", A1, V1);
         else
            std::printf("SBB $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  3:
         if (A1 == 15)
            std::printf("NEG $%x(%04x)", A2, V2);
         else
            std::printf("SUB $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  4:
         std::printf("SHR $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  5:
         std::printf("ASR $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  6:
         std::printf("SHL $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  7:
         std::printf("LHN $%x(%04x), $%x", A1, V1, A2);
         break;
      case  8:
         std::printf("AND $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case  9:
         std::printf("OR $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 10:
         std::printf("XOR $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 11:
         std::printf("ROR $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 12:
         std::printf("NAND $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 13:
         if (A2 == 15)
            std::printf("NOT $%x(%04x)", A1, V1);
         else
            std::printf("NOR $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 14:
         std::printf("XNOR $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 15:
         std::printf("ROL $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
       }
      break;
   case  6: // SDOx
      std::printf("SDO%c $%x(%04x), $%x(%04x)", ((A3 < 10) ? (A3 + '0') : (A3 - 10 + 'A')), A1, V1, A2, V2);
      break;
   case  7: // RET
      switch (A3 & 7)
       {
      case 0:
         if (A1 == 15)
            std::printf("RET $%x(%04x)", A2, V2);
         else
            std::printf("RETZ $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 1:
         std::printf("RETN $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 2:
         std::printf("RETO $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 3:
         std::printf("RETP $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 4:
         std::printf("RETC $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 5:
         std::printf("RETZP $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 6:
         std::printf("RETE $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
      case 7:
         std::printf("RETZN $%x(%04x), $%x(%04x)", A1, V1, A2, V2);
         break;
       }
      break;
   case  8:
      std::printf("PICKZ $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
   case  9:
      std::printf("PICKN $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
   case 10:
      std::printf("PICKO $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
   case 11:
      std::printf("PICKP $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
   case 12:
      std::printf("PICK $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
   case 13:
      std::printf("PICKZP $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
   case 14:
      std::printf("PICKE $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
   case 15:
      std::printf("PICKZN $%x(%04x) ? $%x(%04x) : $%x(%04x)", A3, V3, A1, V1, A2, V2);
      break;
    }
   std::printf(" [%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x]\n",
      getBelt(0), getBelt(1), getBelt(2), getBelt(3), getBelt(4), getBelt(5), getBelt(6), getBelt(7),
      getBelt(8), getBelt(9), getBelt(10), getBelt(11), getBelt(12), getBelt(13), getBelt(14), getBelt(15));

   return;
 }
