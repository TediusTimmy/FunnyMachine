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
#include "KeyBoard.h"
#include "external/olcPixelGameEngine.h"

void KeyBoard::reset()
 {
   latest = 0;
 }

bool KeyBoard::doRead(word addr, word& OUT_)
 {
   // First address : write-only address.
   if (0xFFC == addr)
    {
      OUT_ = 0;
      return true;
    }
   // Second address : return the state of the keyboard key.
   else if (0xFFD == addr)
    {
      OUT_ = latest;
      return true;
    }
   return false;
 }

bool KeyBoard::doWrite(word addr, word val)
 {
   // First address : this is a write-only address.
   if (0xFFC == addr)
    {
      if (true == reinterpret_cast<olc::PixelGameEngine*>(engine)->GetKey((olc::Key)val).bHeld)
       {
         latest = 1;
       }
      else
       {
         latest = 0;
       }
      return true;
    }
   // Second address : read-only address.
   else if (0xFFD == addr)
    {
      return true;
    }
   return false;
 }

KeyBoard::~KeyBoard()
 {
 }
