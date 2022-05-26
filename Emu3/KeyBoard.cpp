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

static const std::map<word, olc::Key> keyMap =
 {
    {'a', olc::Key::A},
    {'b', olc::Key::B},
    {'c', olc::Key::C},
    {'d', olc::Key::D},
    {'e', olc::Key::E},
    {'f', olc::Key::F},
    {'g', olc::Key::G},
    {'h', olc::Key::H},
    {'i', olc::Key::I},
    {'j', olc::Key::J},
    {'k', olc::Key::K},
    {'l', olc::Key::L},
    {'m', olc::Key::M},
    {'n', olc::Key::N},
    {'o', olc::Key::O},
    {'p', olc::Key::P},
    {'q', olc::Key::Q},
    {'r', olc::Key::R},
    {'s', olc::Key::S},
    {'t', olc::Key::T},
    {'u', olc::Key::U},
    {'v', olc::Key::V},
    {'w', olc::Key::W},
    {'x', olc::Key::X},
    {'y', olc::Key::Y},
    {'z', olc::Key::Z},
    {'1', olc::Key::K1},
    {'2', olc::Key::K2},
    {'3', olc::Key::K3},
    {'4', olc::Key::K4},
    {'5', olc::Key::K5},
    {'6', olc::Key::K6},
    {'7', olc::Key::K7},
    {'8', olc::Key::K8},
    {'9', olc::Key::K9},
    {'0', olc::Key::K0},
    {'`', olc::Key::OEM_3},
    {'-', olc::Key::MINUS},
    {'=', olc::Key::EQUALS},
    {'[', olc::Key::OEM_4},
    {']', olc::Key::OEM_6},
    {';', olc::Key::OEM_1},
    {'\'', olc::Key::OEM_7},
    {'\\', olc::Key::OEM_5},
    {',', olc::Key::COMMA},
    {'.', olc::Key::PERIOD},
    {'/', olc::Key::OEM_2},
    {' ', olc::Key::SPACE},
    {27, olc::Key::ESCAPE},
    {8, olc::Key::BACK},
    {9, olc::Key::TAB},
    {13, olc::Key::ENTER},
    {127, olc::Key::DEL},
    {128, olc::Key::F1},
    {129, olc::Key::F2},
    {130, olc::Key::F3},
    {131, olc::Key::F4},
    {132, olc::Key::F5},
    {133, olc::Key::F6},
    {134, olc::Key::F7},
    {135, olc::Key::F8},
    {136, olc::Key::F9},
    {137, olc::Key::F10},
    {138, olc::Key::F11},
    {139, olc::Key::F12},
    {140, olc::Key::CAPS_LOCK},
    {141, olc::Key::SHIFT},
    {142, olc::Key::CTRL},
    {143, olc::Key::SCROLL},
    {144, olc::Key::PAUSE},
    {145, olc::Key::INS},
    {146, olc::Key::HOME},
    {147, olc::Key::PGUP},
    {148, olc::Key::END},
    {149, olc::Key::PGDN},
    {150, olc::Key::UP},
    {151, olc::Key::LEFT},
    {152, olc::Key::DOWN},
    {153, olc::Key::RIGHT},
    {154, olc::Key::NP_DIV},
    {155, olc::Key::NP_MUL},
    {156, olc::Key::NP_SUB},
    {157, olc::Key::NP_ADD},
    {158, olc::Key::NP_DECIMAL},
    {159, olc::Key::NP0},
    {160, olc::Key::NP1},
    {161, olc::Key::NP2},
    {162, olc::Key::NP3},
    {163, olc::Key::NP4},
    {164, olc::Key::NP5},
    {165, olc::Key::NP6},
    {166, olc::Key::NP7},
    {167, olc::Key::NP8},
    {168, olc::Key::NP9}
 };

bool KeyBoard::doWrite(word addr, word val)
 {
   // First address : this is a write-only address.
   if (0xFFC == addr)
    {
      auto needle = keyMap.find(val);
      if (keyMap.end() != needle)
       {
         if (true == reinterpret_cast<olc::PixelGameEngine*>(engine)->GetKey(needle->second).bHeld)
          {
            latest = 1;
          }
         else
          {
            latest = 0;
          }
       }
      else
       {
         latest = 2;
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
