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
#include "SpriteEngine.h"

void Screen::attach(MemoryController* mc)
 {
   M = mc;
   sprite_engine = nullptr; // This is not the right place for this, but, hopefully, it won't be an issue.
 }

void Screen::reset() // Thankfully, this is idempotent.
 {
   ticks = 0;
   frame = 0;
   sec = 0;
 }

void Screen::doOneOp()
 {
   ++ticks;
   // At 1 MHz and 30 fps, we redraw the screen every 33333 clock cycles.
   // Our "video card" will read the 512K bytes of screen in 3277 cycles (160 bytes per instruction).
   if (ticks < (33333 - 3277))
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

   ++frame;
   if (30 == frame)
   {
      ++sec;
      frame = 0;
   }

   if (nullptr == sprite_engine)
    {
      sprite_engine = new SpriteEngine(reinterpret_cast<olc::PixelGameEngine*>(pixel_engine), M->vram());
    }
   // Redraw the screen.
   reinterpret_cast<SpriteEngine*>(sprite_engine)->updateScreen();
 }

bool Screen::doRead(word addr, word& OUT_) // windows.h #defines OUT as ... something.
 {
   if (addr == 0x100)
    {
      OUT_ = frame;
      return true;
    }
   else if (addr == 0x101)
    {
      OUT_ = sec;
      return true;
    }
   return false;
 }

bool Screen::doWrite(word, word)
 {
   return false;
 }
