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

extern "C"
 {
   #include "mio.h"
 }

#ifdef DO_TRACE
 #include <cstdio>
#endif

void KeyBoard::reset()
 {
   if (false == running)
    {
      running = true;
      thread = std::make_unique<std::thread>(&KeyBoard::operator(), this);
      thread->detach();
    }
   consumed = true;
 }

bool KeyBoard::doRead(word addr, word& OUT)
 {
#ifdef DO_TRACE
   std::printf("In keyboard read: %03x\n", addr);
#endif
   // First address : return if there is new input from the keyboard.
   if (0xFFC == addr)
    {
      if (true == consumed)
       {
         OUT = 0;
       }
      else
       {
         OUT = 1;
       }
      return true;
    }
   // Second address : return the keyboard character.
   else if (0xFFD == addr)
    {
      OUT = latest;
      consumed = true;
      return true;
    }
   return false;
 }

bool KeyBoard::doWrite(word, word)
 {
   return false;
 }

void KeyBoard::operator()()
 {
   while (true == running)
    {
      latest = GETC();
      consumed = false;
#ifdef DO_TRACE
   std::printf("Got Character: %02x\n", latest);
#endif
    }
 }

KeyBoard::~KeyBoard()
 {
   running = false;
 }
