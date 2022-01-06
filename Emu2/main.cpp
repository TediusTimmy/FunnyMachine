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
#include "Screen.h"
#include "KeyBoard.h"
#include "Speed.h"

extern "C"
 {
   #include "mio.h"
 }

#include <cstdio>
#include <memory>

bool doRun = true;

class Halter : public IODevice
 {
   public:
      void reset() override
       {
         doRun = true;
       }
      bool doRead(word, word&) override
       {
         return false;
       }
      bool doWrite(word addr, word) override
       {
         if (addr == 0xFFF)
          {
            doRun = false;
            return true;
          }
         return false;
       }
 };

int main (int argc, char** argv)
 {
   INIS();
   if (argc < 2)
    {
      DINI();
      std::printf("usage: emu rom_file_name\n");
      return 1;
    }

   // The memory controller is on the heap because it's large.
   std::unique_ptr<MemoryController> bus = std::make_unique<MemoryController>();
   ClockController devs;
   Core core;

   // Other useful devices...
   Halter halter;
   bus->attach(&halter);

   KeyBoard keyboard;
   bus->attach(&keyboard);

   Screen screen;
   screen.attach(bus.get());
   devs.attach(&screen);

   Speed speed;
   devs.attach(&speed);

   // Initialize everything and GO!
   bus->reset();
   if (false == bus->readROMFile(argv[1]))
    {
      DINI();
      std::printf("Error reading rom file. Check that file exists and is less than two megabytes.\n");
      return 1;
    }

   core.attach(bus.get());
   devs.attach(&core);
   devs.reset();

   // DMA happens last. We have to reset the bus before we read the ROM,
   // and we have to read the ROM before we can reset the core.
   devs.attach(bus.get());

   while (true == doRun)
    {
      devs.doOneOp();
    }

   DINI();
   return 0;
 }
