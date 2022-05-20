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
#ifndef FM_SCREEN_H
#define FM_SCREEN_H

#include "../Base/Base.h"

class Screen : public ClockDevice, public IODevice
 {
private:
   int ticks;
   byte frame;
   byte sec;
   MemoryController* M; // Our view into memory

public:
   void attach(MemoryController*); // DO THIS FIRST!
   void reset() override;
   void doOneOp() override;

   bool doRead(word addr, word& OUT) override;
   bool doWrite(word addr, word val) override;

   void * engine;
 };

#endif /* FM_SCREEN_H */
