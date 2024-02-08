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

void ClockController::ClockController::reset()
{
   for (auto& device : devices)
   {
      device->reset();
   }
}

void ClockController::doOneOp()
{
   for (auto& device : devices)
   {
      device->doOneOp();
   }
}

void ClockController::attach(ClockDevice* device)
 {
   devices.push_back(device);
 }

void ClockController::detach(ClockDevice* device)
 {
   devices.remove(device);
 }

void ClockController::clear()
 {
   devices.clear();
 }
