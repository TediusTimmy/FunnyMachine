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
#include "Speed.h"

#include <thread>

void Speed::reset()
 {
   ticks = 0;
   last = std::chrono::system_clock::now();
 }

void Speed::doOneOp()
 {
   ++ticks;
   // At 1 MHz and 30 fps, we redraw the screen every 33333 clock cycles.
   // We will achieve this by sleeping for the remaining time, every time we hit this event.
   if (ticks < 33333)
    {
      return;
    }
   ticks = 0;

   last += std::chrono::microseconds(33333);
   std::this_thread::sleep_until(last);
 }
