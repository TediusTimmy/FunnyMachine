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
#include "../Base/Base.h"

#include <cstdio>
#include <memory>

class Printer : public IODevice
 {
   public:
      void reset() override
       {
       }
      bool doRead(word, word&) override
       {
         return false;
       }
      bool doWrite(word addr, word val) override
       {
         if (addr == 0xFFE)
          {
            std::putchar(val);
            return true;
          }
         return false;
       }
 };

int main (int argc, char** argv)
 {
   if (argc < 2)
    {
      std::printf("usage: emu rom_file_name\n");
      return 1;
    }

   // The memory controller is on the heap because it's large.
   std::unique_ptr<MemoryController> bus = std::make_unique<MemoryController>();
   ClockController devs;
   Core core;

   // Other useful devices...
   Printer printer;
   bus->attach(&printer);

   // Initialize everything and GO!
   bus->reset();
   if (false == bus->readROMFile(argv[1]))
    {
      std::printf("Error reading rom file. Check that file exists and is less than two megabytes.\n");
      return 1;
    }

   core.attach(bus.get());
   devs.attach(&core);
   devs.reset();

   // DMA happens last. We have to reset the bus before we read the ROM,
   // and we have to read the ROM before we can reset the core.
   devs.attach(bus.get());

   while (!core.halted())
    {
      devs.doOneOp();
    }

   return 0;
 }
