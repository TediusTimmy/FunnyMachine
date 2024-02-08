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

// Yes, this is a copy-paste of Core::printDecompiled.
void disassemble (unsigned short I)
 {
   unsigned short A1 = (I >> 8) & 15;
   unsigned short A2 = (I >> 12) & 15;
   unsigned short A3 = (I >> 4) & 15;
   unsigned short imm12 = (I >> 4);

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
         std::printf("ENDOP $%x, $%x", A1, A2);
         break;
      default:
         std::printf("NOP");
         break;
       }
      break;
   case  1: // LD
      if (I & 32)
         std::printf("LDS $%x\n", A1, R[A1]);
      else if (I & 16)
         std::printf("LD $%x\n", A1);
      else
         std::printf("LDB $%x\n", A1);
      break;
   case  2: // ST
      if (I & 32)
         std::printf("STS $%x, $%x", A1, A2);
      else if (I & 16)
         std::printf("ST $%x, $%x\n", A1, A2);
      else
         std::printf("STB $%x, $%x\n", A1, A2);
      break;
   case  3: // LDI
      std::printf("LDI $%03x\n", imm12);
      break;
   case  4: // LRA
      std::printf("LRA $%03x\n", imm12 << 1);
      break;
   case  5: // ALU
      switch ((I >> 4) & 15)
       {
      case  0:
         std::printf("ADD $%x, $%x\n", A1, A2);
         break;
      case  1:
         if (A2 == 15)
            std::printf("INC $%x\n", A1);
         else
            std::printf("ADC $%x, $%x\n", A1, A2);
         break;
      case  2:
         if (A2 == 15)
            std::printf("DEC $%x\n", A1);
         else
            std::printf("SBB $%x, $%x\n", A1, A2);
         break;
      case  3:
         if (A1 == 15)
            std::printf("NEG $%x\n", A2);
         else
            std::printf("SUB $%x, $%x\n", A1, A2);
         break;
      case  4:
         std::printf("SHL $%x, $%x\n", A1, A2);
         break;
      case  5:
         std::printf("SHL $%x, $%x\n", A1, A2);
         break;
      case  6:
         std::printf("SHR $%x, $%x\n", A1, A2);
         break;
      case  7:
         std::printf("ASR $%x, $%x\n", A1, A2);
         break;
      case  8:
         std::printf("AND $%x, $%x\n", A1, A2);
         break;
      case  9:
         std::printf("OR $%x, $%x\n", A1, A2);
         break;
      case 10:
         std::printf("XOR $%x, $%x\n", A1, A2);
         break;
      case 11:
         std::printf("LHN $%x, $%x\n", A1, A2);
         break;
      case 12:
         std::printf("NAND $%x, $%x\n", A1, A2);
         break;
      case 13:
         if (A2 == 15)
            std::printf("NOT $%x\n", A1);
         else
            std::printf("NOR $%x, $%x\n", A1, A2);
         break;
      case 14:
         std::printf("XNOR $%x, $%x\n", A1, A2);
         break;
      case 15:
         std::printf("ROL $%x, $%x\n", A1, A2);
         break;
       }
      break;
   case  6: // SDOx
      std::printf("SDO%c $%x, $%x", ((A3 < 10) ? (A3 + '0') : (A3 - 10 + 'A')), A1, A2);
      break;
   case  7: // RET
      switch ((I >> 4) & 7)
       {
      case 0:
         if (A1 == 15)
            std::printf("RET $%x\n", A2);
         else
            std::printf("RETZ $%x, $%x\n", A1, A2);
         break;
      case 1:
         std::printf("RETN $%x, $%x\n", A1, A2);
         break;
      case 2:
         std::printf("RETO $%x, $%x\n", A1, A2);
         break;
      case 3:
         std::printf("RETP $%x, $%x\n", A1, A2);
         break;
      case 4:
         std::printf("RETC $%x, $%x\n", A1, A2);
         break;
      case 5:
         std::printf("RETZP $%x, $%x\n", A1, A2);
         break;
      case 6:
         std::printf("RETE $%x, $%x\n", A1, A2);
         break;
      case 7:
         std::printf("RETZN $%x, $%x\n", A1, A2);
         break;
       }
      break;
   case  8:
      std::printf("PICKZ $%x, $%x, $%x\n", A3, A1, A2);
      break;
   case  9:
      std::printf("PICKN $%x, $%x, $%x\n", A3, A1, A2);
      break;
   case 10:
      std::printf("PICKO $%x, $%x, $%x\n", A3, A1, A2);
      break;
   case 11:
      std::printf("PICKP $%x, $%x, $%x\n", A3, A1, A2);
      break;
   case 12:
      std::printf("PICK $%x, $%x, $%x\n", A3, A1, A2);
      break;
   case 13:
      std::printf("PICKZP $%x, $%x, $%x\n", A3, A1, A2);
      break;
   case 14:
      std::printf("PICKE $%x, $%x, $%x\n", A3, A1, A2);
      break;
   case 15:
      std::printf("PICKZN $%x, $%x, $%x\n", A3, A1, A2);
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
