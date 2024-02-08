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
#ifndef FM_SPEED_H
#define FM_SPEED_H

#include <chrono>

#include "../Base/Base.h"

class Speed : public ClockDevice
 {
   private:
      int ticks;
      std::chrono::system_clock::time_point last;

   public:
      void reset() override;
      void doOneOp() override;
 };

#endif /* FM_SPEED_H */
