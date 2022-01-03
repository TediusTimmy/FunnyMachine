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
#include "Screen.h"

extern "C"
 {
   #include "mio.h"
 }

void Screen::attach(MemoryController* mc)
 {
   M = mc;
 }

void Screen::reset()
 {
   ticks = 0;
 }

void Screen::doOneOp()
 {
   ++ticks;
   // At 1 MHz and 30 fps, we redraw the screen every 33333 clock cycles.
   // Our "video card" will read the 4000 bytes of screen in 16 cycles (DMA speed).
   if (ticks < 33317)
    {
      return;
    }
   M->lockVRAM();
   if (ticks < 33333)
    {
      return;
    }
   ticks = 0;
   M->unlockVRAM();

   const byte* VRAM = M->vram();
   // Redraw the screen.
   for (int y = 0; y < 25; ++y)
    {
      GOTO(0, y);
      for (int x = 0; x < 80; ++x)
       {
         COLOR(VRAM[y * 25 + x * 2 + 1]);
         PUTC(VRAM[y * 25 + x * 2]);
       }
    }
 }
