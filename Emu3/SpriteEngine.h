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
#ifndef FM_SPRITE_ENGINE_H
#define FM_SPRITE_ENGINE_H

#include "../Base/Base.h"
#include "external/olcPixelGameEngine.h"

class SpriteEngine
 {
private:
   olc::PixelGameEngine* engine;
   const byte* VRAM;
   std::vector<std::shared_ptr<olc::Decal> > decals;

public:
   SpriteEngine(olc::PixelGameEngine*, const byte*);

   void updateScreen();
 };

#endif /* FM_SPRITE_ENGINE_H */
