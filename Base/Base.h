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
#ifndef FM_BASE_H
#define FM_BASE_H

/*
   The fake architecture is for a 40 pin DIP processor. It has 15 address pins and 16 data pins.
   There are two Read/Write pins to indicate whether the low byte, high byte, or both are requested.
      The high byte pin can be thought of as the least significant address bit for a single byte read.
   The processor is attached to 4, 512K x 8 bit SRAM chips, for 2 MB of RAM.
   The chips are interleaved: one chip supplies the low 8 bits and the other the high 8 bits to fill the 16 line data bus.
      None of the hardware tries to correctly support a 2-byte read on an unaligned address.
*/

#include <string>
#include <list>

enum RequestBytes
 {
   LOW_BYTE,
   HIGH_BYTE,
   BOTH_BYTES
 };

typedef unsigned char  byte;
typedef unsigned short word;

// All devices need to be reset-able.
// Here come the diamond-of-death inheritance trees.
class Device
 {
   public:
      virtual void reset() = 0;
      virtual ~Device() { }
 };

// An IO Device is a device for which some read or write into the System memory block has meaning.
// As a device can only service a request in B000-BFFF, it will always get the truncated address in 000-FFF.
// Also, the memory controller will break word read/writes into its component byte read/writes.
//    While it is passed and/or returns a word, only a byte will ever be present.
class IODevice : virtual public Device
 {
   public:
      // The boolean is whether or not this device serviced the request.
      //    Some people consider this (pass a reference and return a boolean) to be an anti-pattern.
      virtual bool doRead(word addr, word& OUT) = 0;
      virtual bool doWrite(word addr, word val) = 0;
 };

// A Clock Device is a device for which a pulse from the system clock has meaning.
class ClockDevice : virtual public Device
 {
   public:
      virtual void doOneOp() = 0;
 };

// The DMA channels are a ClockDevice and an IODevice, and they are too intimate with the MemoryController to separate them.
class MemoryController : public ClockDevice, public IODevice
 {
   private:
      // The first 5 selectors select from the RAM bus.
      // The next selector selects from VRAM for the low half, and RAM bank FF for the high half (if the read is not otherwise delegated).
      // The last two select from the ROM bus.
      byte B [8]; // Bank registers
      bool w; // Top 1MB of ROM bus is writable.
      byte  RAM [2097152];
      byte  ROM [2097152];
      byte VRAM [1048576];
      bool gpuRead;
      byte DMA [16]; // 16 control bytes of the 4 DMA controllers.
      word loc [4]; // Location in a block. DMA transfers 32 bytes per instruction.

      std::list<IODevice*> devices;

      bool doRead(word addr, word& OUT) override;
      bool doWrite(word addr, word val) override;

   public:
      void reset() override;
      void doOneOp() override;
      bool readROMFile(const std::string& fileName);

      word doRead(word addr, RequestBytes);
      void doWrite(word addr, word val, RequestBytes);

      const byte* vram() const { return VRAM; }
      void lockVRAM() { gpuRead = true; }
      void unlockVRAM() { gpuRead = false; }

      void attach(IODevice*);
      void detach(IODevice*);
      void clear();
 };

class Core : public ClockDevice
 {
   private:
      word PC;
      word B [16]; // The belt
      byte P; // The location in the belt's ring buffer of the next write
      MemoryController* M; // Our view into memory

      void printDecompiled(word, word) const;

   public:
      void attach(MemoryController*); // DO THIS FIRST!
      void reset() override; // Read the PC from FFFE-FFFF
      void doOneOp() override;

      static word bcd_add(word, word);
 };

class ClockController : public ClockDevice
{
   private:
      std::list<ClockDevice*> devices;

   public:
      void reset() override;
      void doOneOp() override;

      void attach(ClockDevice*);
      void detach(ClockDevice*);
      void clear();
};

#endif /* FM_BASE_H */
