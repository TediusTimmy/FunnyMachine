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

#include <iostream>
#include <fstream>

typedef unsigned char  byte;
typedef unsigned short word;

class ROM
 {
   private:
      byte M[2097152];
      int l;
      int r;

      void emit(word c)
       {
         if (l == 2097152)
          {
            std::cerr << "Too much code!" << std::endl;
            l = 0;
          }
         M[l] = c;
         M[l + 1] = c >> 8;
         l += 2;
       }
      void validateBeltAddr(word b) const
       {
         if (b > 15)
          {
            std::cerr << "Bad belt address." << std::endl;
          }
       }
      void validateImmediate(word imm, word max) const
       {
         if (imm > max)
          {
            std::cerr << "Bad immediate." << std::endl;
          }
       }

   public:
      ROM()
       {
         l = 0;
         r = 1;
       }

      int getCodeLoc() const
       {
         return l;
       }
      int getResultLoc() const
       {
         return r;
       }
      int getRelativeLocation(int loc) const
       {
         return loc - l - 2;
       }
      word convertLocationImm8(int loc) const
       {
         loc >>= 1;
         if (loc > 127)
          {
            std::cerr << "Jump offset greater than imm8." << std::endl;
            loc = 255;
          }
         else if (loc < -128)
          {
            std::cerr << "Jump offset greater than imm8." << std::endl;
            loc = 255;
          }
         return loc & 0xFF;
       }
      word convertLocationImm12(int loc) const
       {
         loc >>= 1;
         if (loc > 0x7FF)
          {
            std::cerr << "Jump offset greater than imm12." << std::endl;
            loc = 0xFFF;
          }
         else if (loc < -0x800)
          {
            std::cerr << "Jump offset greater than imm12." << std::endl;
            loc = 0xFFF;
          }
         return loc & 0xFFF;
       }
      int getBeltLocation(int in) const
       {
         int res = r - in - 1;
         if (res > 13)
          {
            std::cerr << "Request for result that has fallen off of the belt." << std::endl;
            res = 0;
          }
         return res;
       }
      const byte* getROM() const { return M; }

      void NOP()
       {
         emit(0);
       }
      int LD(word loc)
       {
         validateBeltAddr(loc);
         emit(17 | (loc << 8));
         std::cout << "LD " << loc << std::endl;
         return r++;
       }
      int LDB(word loc)
       {
         validateBeltAddr(loc);
         emit(1 | (loc << 8));
         std::cout << "LDB " << loc << std::endl;
         return r++;
       }
      void ST(word val, word addr)
       {
         validateBeltAddr(val);
         validateBeltAddr(addr);
         emit(18 | (val << 8) | (addr << 12));
         std::cout << "ST " << val << ", " << addr << std::endl;
       }
      void STB(word val, word addr)
       {
         validateBeltAddr(val);
         validateBeltAddr(addr);
         emit(2 | (val << 8) | (addr << 12));
         std::cout << "STB " << val << ", " << addr << std::endl;
       }
      int LDI(word imm)
       {
         validateImmediate(imm, 0xFFF);
         emit(3 | (imm << 4));
         std::cout << "LDI " << imm << std::endl;
         return r++;
       }
      int LRA(word imm)
       {
         validateImmediate(imm, 0xFFF);
         emit(4 | (imm << 4));
         std::cout << "LRA " << imm << std::endl;
         return r++;
       }
      int ADD(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (0 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "ADD " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int ADC(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (1 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "ADC " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int SBB(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (2 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "SBB " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int SUB(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (3 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "SUB " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int SHL(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (4 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "SHL " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int SHR(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (6 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "SHR " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int ASR(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (7 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "ASR " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int AND(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (8 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "AND " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int OR(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (9 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "OR " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int XOR(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (10 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "XOR " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int ROL(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (11 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "ROL " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int NAND(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (12 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "NAND " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int NOR(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (13 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "NOR " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int XNOR(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (14 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "XNOR " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int ROR(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(5 | (15 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "ROR " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int BADD(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (0 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "BADD " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int BADC(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (1 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "BADC " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int BSBB(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (2 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "BSBB " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int BSUB(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (3 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "BSUB " << lhs << ", " << rhs << std::endl;
         return r++;
       }
      int MUL(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (4 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "MUL " << lhs << ", " << rhs << std::endl;
         return (r += 2) - 2;
       }
      int UMUL(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (5 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "UMUL " << lhs << ", " << rhs << std::endl;
         return (r += 2) - 2;
       }
      int DIV(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (6 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "DIV " << lhs << ", " << rhs << std::endl;
         return (r += 2) - 2;
       }
      int UDIV(word lhs, word rhs)
       {
         validateBeltAddr(lhs);
         validateBeltAddr(rhs);
         emit(6 | (7 << 4) | (lhs << 8) | (rhs << 12));
         std::cout << "UDIV " << lhs << ", " << rhs << std::endl;
         return (r += 2) - 2;
       }
      void RETZ(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (0 << 4) | (code << 8) | (dest << 12));
         std::cout << "RETZ " << code << ", " << dest << std::endl;
       }
      void RETN(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (1 << 4) | (code << 8) | (dest << 12));
         std::cout << "RETN " << code << ", " << dest << std::endl;
       }
      void RETO(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (2 << 4) | (code << 8) | (dest << 12));
         std::cout << "RETO " << code << ", " << dest << std::endl;
       }
      void RETP(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (3 << 4) | (code << 8) | (dest << 12));
         std::cout << "RETP " << code << ", " << dest << std::endl;
       }
      void RETC(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (4 << 4) | (code << 8) | (dest << 12));
         std::cout << "RET " << code << ", " << dest << std::endl;
       }
      void RETZP(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (5 << 4) | (code << 8) | (dest << 12));
         std::cout << "RETZP " << code << ", " << dest << std::endl;
       }
      void RETE(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (6 << 4) | (code << 8) | (dest << 12));
         std::cout << "RETE " << code << ", " << dest << std::endl;
       }
      void RETZN(word code, word dest)
       {
         validateBeltAddr(code);
         validateBeltAddr(dest);
         emit(7 | (7 << 4) | (code << 8) | (dest << 12));
         std::cout << "RETZN " << code << ", " << dest << std::endl;
       }
      void BRZ(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(8 | (code << 4) | (imm << 8));
         std::cout << "BRZ " << code << ", " << imm << std::endl;
       }
      void BRN(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(9 | (code << 4) | (imm << 8));
         std::cout << "BRN " << code << ", " << imm << std::endl;
       }
      void BRO(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(10 | (code << 4) | (imm << 8));
         std::cout << "BRO " << code << ", " << imm << std::endl;
       }
      void BRP(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(11 | (code << 4) | (imm << 8));
         std::cout << "BRP " << code << ", " << imm << std::endl;
       }
      void BR(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(12 | (code << 4) | (imm << 8));
         std::cout << "BR " << code << ", " << imm << std::endl;
       }
      void BRZP(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(13 | (code << 4) | (imm << 8));
         std::cout << "BRZP " << code << ", " << imm << std::endl;
       }
      void BRE(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(14 | (code << 4) | (imm << 8));
         std::cout << "BRE " << code << ", " << imm << std::endl;
       }
      void BRZN(word code, word imm)
       {
         validateBeltAddr(code);
         validateImmediate(imm, 0xFF);
         emit(15 | (code << 4) | (imm << 8));
         std::cout << "BRZN " << code << ", " << imm << std::endl;
       }
      int DEC(word loc)
       {
         return ADD(loc, 14);
       }
      int INC(word loc)
       {
         return SUB(loc, 14);
       }
      int NEG(word loc)
       {
         return SUB(15, loc);
       }
      int NOT(word loc)
       {
         return XOR(loc, 14);
       }
      void BRA(word imm)
       {
         BRZ(15, imm);
       }
      void RET(word imm)
       {
         RETZ(15, imm);
       }

      void generateAssembly();
 };

ROM rom;

int main (void)
 {
   rom.generateAssembly();

   std::ofstream file ("rom.rom");
   file.write(reinterpret_cast<const char*>(rom.getROM()), rom.getCodeLoc());
 }
