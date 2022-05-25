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

#include <cstring>
#include <fstream>
#include <algorithm>

void MemoryController::reset()
 {
   B[0] = 0;
   B[1] = 0;
   B[2] = 0;
   B[3] = 0;
   B[4] = 0;
   B[5] = 0;
   B[6] = 0;
   B[7] = 0;
   w = false;
   gpuRead = false;
   std::memset( RAM, '\0', 2097152);
   std::memset( ROM, '\0', 2097152);
   std::memset(VRAM, '\0', 1048576);
   if (devices.end() == std::find(devices.begin(), devices.end(), this))
    {
      devices.push_front(this); // The Memory controller's IO has priority.
    }
   for (auto& device : devices)
    {
      if (device != this)
       {
         device->reset();
       }
    }
 }

bool MemoryController::readROMFile(const std::string& fileName)
 {
   std::ifstream rom (fileName, std::ios_base::binary);
   if (rom.fail())
    {
      return false;
    }
   rom.seekg(0, rom.end);
   // We don't actually care if this gets truncated due to overflow.
   // If the user tries to load a, specially crafted, > 4GB ROM file, then I don't have any sympathy for the failure.
   long len = rom.tellg();
   // If the ROM is over 2 MB then reject it : it won't fit.
   if (len > 2097152)
    {
      return false;
    }
   // If the ROM is less than 1MB, then the other two slots are assumed to be extra RAM.
   if (len < 1048576)
    {
      w = true;
    }
   rom.seekg(0, rom.beg);
   rom.read(reinterpret_cast<char*>(ROM), len);
   // If the ROM is less than 8K, then we have a problem: the reset vector will be 0000, and that isn't good.
   // Just set the reset vector to C000, as that is EFFECTIVELY what it is.
   // As memory is initialized to zeros, which are NOPs, the PC will get there eventually.
   // And I'm sure the demoscene or golfing people will appreciate being able to make the ROM smaller than 8K.
   if (len < 8192)
    {
      ROM[8191] = 0xC0;
    }
   if (rom.fail())
    {
      return false;
    }
   return true;
 }

word MemoryController::doRead(word addr, RequestBytes which)
 {
   word bank = addr >> 13;
   word res = 0;
   if (bank < 5)
    {
      res = (((word)RAM[(((int)B[bank]) << 13) | (addr & 0x1FFE) | 1]) << 8) | RAM[(((int)B[bank]) << 13) | (addr & 0x1FFE)];
    }
   else if (bank > 5)
    {
      res = (((word)ROM[(((int)B[bank]) << 13) | (addr & 0x1FFE) | 1]) << 8) | ROM[(((int)B[bank]) << 13) | (addr & 0x1FFE)];
    }
   else // bank 5
    {
      if (~addr & 0x1000) // Low 4K -> VRAM
       {
         if (false == gpuRead)
          {
            res = (((word)VRAM[(((int)B[bank]) << 12) | (addr & 0xFFE) | 1]) << 8) | VRAM[(((int)B[bank]) << 12) | (addr & 0xFFE)];
          }
         else
          {
            res = ~0;
          }
       }
      else // System memory
       {
         word low = 0;
         word high = 0;
         // Tame the complexity of this by treating ourself as an IO device.
         if ((which == LOW_BYTE) || (which == BOTH_BYTES))
          {
            bool serviced = false;
            for (auto& device : devices)
             {
               if (device->doRead(addr & 0xFFE, low))
                {
                  serviced = true;
                  break;
                }
             }
            if (false == serviced)
             {
               low = RAM[0x1FF000 | (addr & 0xFFE)];
             }
          }
         if ((which == HIGH_BYTE) || (which == BOTH_BYTES))
          {
            bool serviced = false;
            for (auto& device : devices)
             {
               if (device->doRead((addr & 0xFFE) | 1, high))
                {
                  serviced = true;
                  break;
                }
             }
            if (false == serviced)
             {
               high = RAM[0x1FF001 | (addr & 0xFFE)];
             }
          }
         res = (high << 8) | (low & 0xFF);
       }
    }
   return res;
 }

void MemoryController::doWrite(word addr, word val, RequestBytes which)
 {
   word bank = addr >> 13;
   if (bank < 5)
    {
      if ((which == LOW_BYTE) || (which == BOTH_BYTES))
       {
         RAM[(((int)B[bank]) << 13) | (addr & 0x1FFE)] = val;
       }
      if ((which == HIGH_BYTE) || (which == BOTH_BYTES))
       {
         RAM[(((int)B[bank]) << 13) | (addr & 0x1FFE) | 1] = val >> 8;
       }
    }
   else if (bank > 5)
    {
      int dest = (((int)B[bank]) << 13) | (addr & 0x1FFE);
      if ((true == w) && (dest & 0x100000)) // The top 1 MB may be RAM, depending on the ROM read in.
       {
         if ((which == LOW_BYTE) || (which == BOTH_BYTES))
          {
            ROM[dest] = val;
          }
         if ((which == HIGH_BYTE) || (which == BOTH_BYTES))
          {
            ROM[dest | 1] = val >> 8;
          }
       }
    }
   else // bank 5
    {
      if (~addr & 0x1000) // Low 4K -> VRAM
       {
         if (false == gpuRead)
          {
            if ((which == LOW_BYTE) || (which == BOTH_BYTES))
             {
               VRAM[(((int)B[bank]) << 12) | (addr & 0xFFE)] = val;
             }
            if ((which == HIGH_BYTE) || (which == BOTH_BYTES))
             {
               VRAM[(((int)B[bank]) << 12) | (addr & 0xFFE) | 1] = val >> 8;
             }
          }
       }
      else // System memory
       {
         // Tame the complexity of this by treating ourself as an IO device.
         if ((which == LOW_BYTE) || (which == BOTH_BYTES))
          {
            bool serviced = false;
            for (auto& device : devices)
             {
               if (device->doWrite(addr & 0xFFE, val))
                {
                  serviced = true;
                  break;
                }
             }
            if (false == serviced)
             {
               RAM[0x1FF000 | (addr & 0xFFE)] = val;
             }
          }
         if ((which == HIGH_BYTE) || (which == BOTH_BYTES))
          {
            bool serviced = false;
            for (auto& device : devices)
             {
               if (device->doWrite((addr & 0xFFE) | 1, val >> 8))
                {
                  serviced = true;
                  break;
                }
             }
            if (false == serviced)
             {
               RAM[0x1FF001 | (addr & 0xFFE)] = val >> 8;
             }
          }
       }
    }
 }

bool MemoryController::doRead(word addr, word& OUT)
 {
   if (addr < 8)
    {
      OUT = B[addr];
      return true;
    }
   if (addr == 9)
    {
      if (true == gpuRead)
         OUT = 1;
      else
         OUT = 0;
      return true;
    }
   if ((addr > 15) && (addr < 32) && (0 == (addr & 3))) // Only the control registers are readable.
    {
      OUT = DMA[addr - 16];
      return true;
    }
   return false;
 }

bool MemoryController::doWrite(word addr, word val)
 {
   if (addr < 8)
    {
      B[addr] = val;
      return true;
    }
   if ((addr > 15) && (addr < 32))
    {
      if (0 == DMA[(addr - 16) & 12]) // Mask out control register : if the DMA is busy, don't write.
       {
         DMA[addr - 16] = val;
       }
      return true;
    }
   return false;
 }

void MemoryController::doOneOp()
 {
   const int DMA1_R = 0;
   const int DMA1_D = 1;
   const int DMA1_S = 2;
   const int DMA1_C = 3;
   const int DMA2_R = 4;
   const int DMA2_D = 5;
   const int DMA2_S = 6;
   const int DMA2_C = 7;
   const int DMA3_R = 8;
   const int DMA3_D = 9;
   const int DMA3_S = 10;
   const int DMA3_C = 11;
   const int DMA4_R = 12;
   const int DMA4_D = 13;
   const int DMA4_S = 14;
   const int DMA4_C = 15;
   const int DMA1_L = 0;
   const int DMA2_L = 1;
   const int DMA3_L = 2;
   const int DMA4_L = 3;


   if (0 != DMA[DMA1_R]) // DMA 1 : ROM to RAM
    {
      if (2 == DMA[DMA1_R]) // Initiate Transfer
       {
         DMA[DMA1_R] = 1;
         loc[DMA1_L] = 0;
         if (0 == DMA[DMA1_C]) // Transfer zero blocks?
          {
            DMA[DMA1_R] = 0;
          }
       }
      else if (1 != DMA[DMA1_R]) // Didn't Understand That
       {
         DMA[DMA1_R] = 0;
       }
      if (1 == DMA[DMA1_R]) // Are we running a transfer?
       {
         std::memcpy(RAM + ((word)DMA[DMA1_D] << 13) + loc[DMA1_L], ROM + ((word)DMA[DMA1_S] << 13) + loc[DMA1_L], 32);
         loc[DMA1_L] += 32;
         if (loc[DMA1_L] == 8192) // Have we transferred a full bank?
          {
            ++DMA[DMA1_D];
            ++DMA[DMA1_S];
            --DMA[DMA1_C];
            loc[DMA1_L] = 0;
            if (0 == DMA[DMA1_C])
             {
               DMA[DMA1_R] = 0;
             }
          }
       }
    }

   if (0 != DMA[DMA2_R]) // DMA 2 : ROM to VRAM
    {
      if (2 == DMA[DMA2_R]) // Initiate Transfer
       {
         DMA[DMA2_R] = 1;
         loc[DMA2_L] = 0;
         if (0 == DMA[DMA2_C]) // Transfer zero blocks?
          {
            DMA[DMA2_R] = 0;
          }
       }
      else if (1 != DMA[DMA2_R]) // Didn't Understand That
       {
         DMA[DMA2_R] = 0;
       }
      if ((1 == DMA[DMA2_R]) && (false == gpuRead)) // Are we running a transfer?
       {
         std::memcpy(VRAM + ((word)DMA[DMA2_D] << 12) + (loc[DMA2_L] & 0xFFF), ROM + ((word)DMA[DMA2_S] << 13) + loc[DMA2_L], 32);
         loc[DMA2_L] += 32;
         if (0 != (loc[DMA2_L] & 0x3000)) // Have we transferred a full bank of VRAM?
          {
            ++DMA[DMA2_D];
            --DMA[DMA2_C];
            if (0 == DMA[DMA2_C])
             {
               DMA[DMA2_R] = 0;
             }
          }
         if (loc[DMA2_L] == 8192) // Have we transferred a full bank of ROM?
          {
            ++DMA[DMA2_S];
            loc[DMA2_L] = 0;
          }
       }
    }

   if (0 != DMA[DMA3_R]) // DMA 3 : RAM to VRAM
    {
      if (2 == DMA[DMA3_R]) // Initiate Transfer
       {
         DMA[DMA3_R] = 1;
         loc[DMA3_L] = 0;
         if (0 == DMA[DMA3_C]) // Transfer zero blocks?
          {
            DMA[DMA3_R] = 0;
          }
       }
      else if (1 != DMA[DMA3_R]) // Didn't Understand That
       {
         DMA[DMA3_R] = 0;
       }
      if ((1 == DMA[DMA3_R]) && (false == gpuRead)) // Are we running a transfer?
       {
         std::memcpy(VRAM + ((word)DMA[DMA3_D] << 12) + (loc[DMA3_L] & 0xFFF), RAM + ((word)DMA[DMA3_S] << 13) + loc[DMA3_L], 32);
         loc[DMA3_L] += 32;
         if (0 != (loc[DMA3_L] & 0x3000)) // Have we transferred a full bank of VRAM?
          {
            ++DMA[DMA3_D];
            --DMA[DMA3_C];
            if (0 == DMA[DMA3_C])
             {
               DMA[DMA3_R] = 0;
             }
          }
         if (loc[DMA3_L] == 8192) // Have we transferred a full bank of RAM?
          {
            ++DMA[DMA3_S];
            loc[DMA3_L] = 0;
          }
       }
    }

   if (0 != DMA[DMA4_R]) // DMA 4 : VRAM to RAM
    {
      if (2 == DMA[DMA4_R]) // Initiate Transfer
       {
         DMA[DMA4_R] = 1;
         loc[DMA4_L] = 0;
         if (0 == DMA[DMA4_C]) // Transfer zero blocks?
          {
            DMA[DMA4_R] = 0;
          }
       }
      else if (1 != DMA[DMA4_R]) // Didn't Understand That
       {
         DMA[DMA4_R] = 0;
       }
      if ((1 == DMA[DMA4_R]) && (false == gpuRead)) // Are we running a transfer?
       {
         std::memcpy(RAM + ((word)DMA[DMA4_D] << 13) + loc[DMA4_L], VRAM + ((word)DMA[DMA4_S] << 12) + (loc[DMA4_L] & 0xFFF), 32);
         loc[DMA4_L] += 32;
         if (0 != (loc[DMA4_L] & 0x3000)) // Have we transferred a full bank of VRAM?
          {
            ++DMA[DMA4_S];
            --DMA[DMA4_C];
            if (0 == DMA[DMA4_C])
             {
               DMA[DMA4_R] = 0;
             }
          }
         if (loc[DMA4_L] == 8192) // Have we transferred a full bank of RAM?
          {
            ++DMA[DMA4_D];
            loc[DMA4_L] = 0;
          }
       }
    }
 }

void MemoryController::attach(IODevice* device)
 {
   devices.push_back(device);
 }

void MemoryController::detach(IODevice* device)
 {
   devices.remove(device);
 }

void MemoryController::clear()
 {
   devices.clear();
 }
