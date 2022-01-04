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
   word V1 = (A1 > 13) ? C[A1] : B[(P + A1) & 15];
   word A2 = (I >> 12) & 15;
   word V2 = (A2 > 13) ? C[A2] : B[(P + A2) & 15];
   word A3 = (I >> 4) & 15;
   word V3 = (A3 > 13) ? C[A3] : B[(P + A3) & 15];
   word imm12 = (I >> 4) | ((I & 0x8000) ? 0xF000 : 0);
   word imm8 = (I >> 8) | ((I & 0x8000) ? 0xFF00 : 0);

   word l = I & 0xFF;
   word h = I >> 8;
   if ((l < 32) || (l > 126)) l = '.';
   if ((h < 32) || (h > 126)) h = '.';
   std::printf("%04x: %02x%02x (%c%c) : ", pc, I & 0xFF, I >> 8, l, h);
   // Yes, this is a copy-paste of doOneOp.
   switch (I & 15)
    {
   case  0: // NOP
      std::printf("NOP\n");
      break;
   case  1: // LD
      if (I & 16)
         std::printf("LD $%x(%04x)\n", A1, V1);
      else
         std::printf("LDB $%x(%04x)\n", A1, V1);
      break;
   case  2: // ST
      if (I & 16)
         std::printf("ST $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
      else
         std::printf("STB $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
      break;
   case  3: // LDI
      std::printf("LDI $%03x\n", imm12 & 0xFFF);
      break;
   case  4: // LRA
      std::printf("LRA $%03x : (%04x)\n", imm12 & 0xFFF, NEXT + (imm12 << 1));
      break;
   case  5: // ALU
      switch ((I >> 4) & 15)
       {
      case  0:
         if (A2 == 14)
            std::printf("DEC $%x(%04x)\n", A1, V1);
         else
            std::printf("ADD $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  1:
         std::printf("ADC $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  2:
         std::printf("SBB $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  3:
         if (A2 == 14)
            std::printf("INC $%x(%04x)\n", A1, V1);
         else if (A1 == 15)
            std::printf("NEG $%x(%04x)\n", A2, V2);
         else
            std::printf("SUB $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  4:
         std::printf("SHL $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  5:
         std::printf("SHL $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  6:
         std::printf("SHR $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  7:
         std::printf("ASR $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  8:
         std::printf("AND $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case  9:
         std::printf("OR $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 10:
         if (A2 == 14)
            std::printf("NOT $%x(%04x)\n", A1, V1);
         else
            std::printf("XOR $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 11:
         std::printf("ROL $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 12:
         std::printf("NAND $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 13:
         std::printf("NOR $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 14:
         std::printf("XNOR $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 15:
         std::printf("ROR $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
       }
      break;
   case  6: // BCD
      if (~I & 64)
       {
         switch ((I >> 4) & 3)
          {
         case  0:
            std::printf("BADD $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
         case  1:
            std::printf("BADC $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
         case  2:
            std::printf("BSBB $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
         case  3:
            std::printf("BSUB $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
          }
       }
      else
       {
         switch ((I >> 4) & 3)
          {
         case  0:
            std::printf("MUL $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
         case  1:
            std::printf("UMUL $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
         case  2:
            std::printf("DIV $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
         case  3:
            std::printf("UDIV $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
            break;
          }
       }
      break;
   case  7: // RET
      switch ((I >> 4) & 7)
       {
      case 0:
         std::printf("RETZ $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 1:
         std::printf("RETN $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 2:
         std::printf("RETO $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 3:
         std::printf("RETP $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 4:
         std::printf("RET $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 5:
         std::printf("RETZP $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 6:
         std::printf("RETE $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
      case 7:
         std::printf("RETZN $%x(%04x), $%x(%04x)\n", A1, V1, A2, V2);
         break;
       }
      break;
   case  8:
      if (A3 == 15)
         std::printf("BRA $%02x ; (%04x)\n", imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      else
         std::printf("BRZ $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
   case  9:
      std::printf("BRN $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
   case 10:
      std::printf("BRO $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
   case 11:
      std::printf("BRP $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
   case 12:
      std::printf("BR $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
   case 13:
      std::printf("BRZP $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
   case 14:
      std::printf("BRE $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
   case 15:
      std::printf("BRZN $%x(%04x), $%02x ; (%04x)\n", A3, V3, imm8 & 0xFF, (NEXT + (imm8 << 1)) & 0xFFFF);
      break;
    }

   return;
 }
 