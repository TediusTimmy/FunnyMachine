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

#include <cstdio>
#include <fstream>
#include <vector>

#ifdef MATCH_BAD_OUTPUT
 #define RESULT "%d"
 #define IMM12  "%d"
 #define IMM8   "%d"
#else
 #define RESULT "$%x"
 #define IMM12  "$%03x"
 #define IMM8   "$%02x"
#endif

// Yes, this is a copy-paste of Core::printDecompiled.
void disassemble (unsigned short I)
 {
   unsigned short A1 = (I >> 8) & 15;
   unsigned short A2 = (I >> 12) & 15;
   unsigned short A3 = (I >> 4) & 15;
   unsigned short imm12 = (I >> 4);
   unsigned short imm8 = (I >> 8);

   switch (I & 15)
    {
   case  0: // NOP
      std::printf("NOP\n");
      break;
   case  1: // LD
      if (I & 16)
         std::printf("LD " RESULT "\n", A1);
      else
         std::printf("LDB " RESULT "\n", A1);
      break;
   case  2: // ST
      if (I & 16)
         std::printf("ST " RESULT ", " RESULT "\n", A1, A2);
      else
         std::printf("STB " RESULT ", " RESULT "\n", A1, A2);
      break;
   case  3: // LDI
      std::printf("LDI " IMM12 "\n", imm12);
      break;
   case  4: // LRA
      std::printf("LRA " IMM12 "\n", imm12
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case  5: // ALU
      switch ((I >> 4) & 15)
       {
      case  0:
         std::printf("ADD " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  1:
#ifndef MATCH_BAD_OUTPUT
         if (A2 == 15)
            std::printf("INC " RESULT "\n", A1);
         else
#endif
            std::printf("ADC " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  2:
#ifndef MATCH_BAD_OUTPUT
         if (A2 == 15)
            std::printf("DEC " RESULT "\n", A1);
         else
#endif
            std::printf("SBB " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  3:
#ifndef MATCH_BAD_OUTPUT
         if (A1 == 15)
            std::printf("NEG " RESULT "\n", A2);
         else
#endif
            std::printf("SUB " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  4:
         std::printf("SHL " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  5:
         std::printf("SHL " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  6:
         std::printf("SHR " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  7:
         std::printf("ASR " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  8:
         std::printf("AND " RESULT ", " RESULT "\n", A1, A2);
         break;
      case  9:
         std::printf("OR " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 10:
         std::printf("XOR " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 11:
         std::printf("ROL " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 12:
         std::printf("NAND " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 13:
#ifndef MATCH_BAD_OUTPUT
         if (A2 == 15)
            std::printf("NOT " RESULT "\n", A1);
         else
#endif
            std::printf("NOR " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 14:
         std::printf("XNOR " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 15:
         std::printf("ROR " RESULT ", " RESULT "\n", A1, A2);
         break;
       }
      break;
   case  6: // BCD
      if (~I & 64)
       {
         switch ((I >> 4) & 3)
          {
         case  0:
            std::printf("BADD " RESULT ", " RESULT "\n", A1, A2);
            break;
         case  1:
            std::printf("BADC " RESULT ", " RESULT "\n", A1, A2);
            break;
         case  2:
            std::printf("BSBB " RESULT ", " RESULT "\n", A1, A2);
            break;
         case  3:
            std::printf("BSUB " RESULT ", " RESULT "\n", A1, A2);
            break;
          }
       }
      else
       {
         switch ((I >> 4) & 3)
          {
         case  0:
            std::printf("MUL " RESULT ", " RESULT "\n", A1, A2);
            break;
         case  1:
            std::printf("UMUL " RESULT ", " RESULT "\n", A1, A2);
            break;
         case  2:
            std::printf("DIV " RESULT ", " RESULT "\n", A1, A2);
            break;
         case  3:
            std::printf("UDIV " RESULT ", " RESULT "\n", A1, A2);
            break;
          }
       }
      break;
   case  7: // RET
      switch ((I >> 4) & 7)
       {
      case 0:
#ifndef MATCH_BAD_OUTPUT
         if (A1 == 15)
            std::printf("RET " RESULT "\n", A2);
         else
#endif
            std::printf("RETZ " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 1:
         std::printf("RETN " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 2:
         std::printf("RETO " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 3:
         std::printf("RETP " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 4:
         std::printf("RETC " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 5:
         std::printf("RETZP " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 6:
         std::printf("RETE " RESULT ", " RESULT "\n", A1, A2);
         break;
      case 7:
         std::printf("RETZN " RESULT ", " RESULT "\n", A1, A2);
         break;
       }
      break;
   case  8:
#ifndef MATCH_BAD_OUTPUT
      if (A3 == 15)
         std::printf("BRA " IMM8 "\n", imm8 << 1);
      else
#endif
         std::printf("BRZ " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case  9:
      std::printf("BRN " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case 10:
      std::printf("BRO " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case 11:
      std::printf("BRP " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case 12:
      std::printf("BR " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case 13:
      std::printf("BRZP " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case 14:
      std::printf("BRE " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
   case 15:
      std::printf("BRZN " RESULT ", " IMM8 "\n", A3, imm8
#ifndef MATCH_BAD_OUTPUT
         << 1
#endif
         );
      break;
    }

   return;
 }

int main (int argc, char ** argv)
 {
   std::vector<unsigned short> data;

   if (argc < 2)
    {
      std::printf("usage: dis rom.rom\n");
      return 1;
    }

   std::ifstream rom (argv[1]);
   if (rom.fail())
    {
      std::printf("Cannot open %s\n", argv[1]);
      return 1;
    }
   rom.seekg(0, rom.end);
   long len = rom.tellg();
   // If the ROM is over 2 MB then reject it
   if (len > 2097152)
    {
      std::printf("Rom too big: %s\n", argv[1]);
      return 1;
    }
   if (0 != (len & 1))
    {
      std::printf("Invalid rom: %s\n", argv[1]);
      return 1;
    }
   rom.seekg(0, rom.beg);
   data.resize(len / 2);
   rom.read(reinterpret_cast<char*>(reinterpret_cast<void*>(&data[0])), len);

   for (auto i : data)
    {
      disassemble(i);
    }

   return 0;
 }
