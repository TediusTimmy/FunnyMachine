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
#include "SpriteEngine.h"

SpriteEngine::SpriteEngine(olc::PixelGameEngine* engine, const byte* VRAM) : engine(engine), VRAM(VRAM)
 {
 }

void SpriteEngine::pullBGSpritesFrom(int bg_base)
 {
   for (int i = 0; i < (80 * 60 * 2); i += 2)
    {
      int tile = ((int)VRAM[bg_base + i]) | ((int)VRAM[bg_base + i + 1]) << 8;
      if (bg_tiles.end() == bg_tiles.find(tile))
       {
         bg_sprites.emplace_back(std::make_unique<olc::Sprite>(16, 16)); // All background tiles are 16x16
         bg_decals.emplace_back(std::make_unique<olc::Decal>(bg_sprites.back().get()));
         bg_tiles[tile] = bg_decals.size() - 1;
         int base = 192 * 4096 + 16 * 16 * 1 * (tile & 0x1FF); // Bank 192, 16 rows, 16 columns, 8 bit color.
         for (int y = 0; y < 16; ++y)
          {
            for (int x = 0; x < 16; ++x)
             {
               bg_sprites.back()->GetData()[y * 16 + x] = palettes[tile >> 9][VRAM[base + y * 16 + x]];
             }
          }
       }
    }
 }

void SpriteEngine::drawBackground(int bg_base)
 {
   for (int y = 0; y < 80; ++y)
    {
      for (int x = 0; x < 60; ++x)
       {
         int tile = ((int)VRAM[bg_base + (y * 60 + x) * 2]) | ((int)VRAM[bg_base + (y * 60 + x) * 2 + 1]) << 8;
         int loc = bg_base + 3 * 4096 + (y * 60 + x) / 4;
         int shl = 6 - ((y * 60 + x) & 3) * 2;
         float vMirror = (VRAM[loc] & (2 << shl)) ? -1.0f : 1.0f;
         float hMirror = (VRAM[loc] & (1 << shl)) ? -1.0f : 1.0f;
         engine->DrawDecal({x * 16.0f + (hMirror < 0.0f ? 16.0f : 0.0f), y * 16.0f + (vMirror < 0.0f ? 16.0f : 0.0f)},
            bg_decals[bg_tiles[tile]].get(), {hMirror, vMirror});
       }
    }
 }

void SpriteEngine::updateScreen()
 {
   // Step 0: Convert all of the Palettes.
   for (int p = 0; p < 64; ++p)
    {
      // Bank 224, 1K per palette, 4 bytes per color, 256 colors.
      std::memcpy(reinterpret_cast<void*>(&palettes[p][0]), reinterpret_cast<const void*>(&VRAM[224 * 4096 + 1024 * p]), 1024);
      palettes[p][0] = olc::Pixel(0, 0, 0, 0); // Transparent Black
      for (int c = 1; c < 256; ++c)
       {
         palettes[p][c].c.a = 255; // Set to opaque
       }
    }
   // Step 1: Convert all backgrounds and sprites to decals.
   // Do all of them, all of the time, so that we don't have magical slowdowns in complex programs.
   // Actually, the optimizations we use here are totally going to create magical slowdowns in complex programs.
   bg_sprites.clear();
   bg_decals.clear();
   bg_tiles.clear();
   // We handle background tiles in this inefficient way because they can have one of 64 palettes applied.
   pullBGSpritesFrom(240 * 4096); // Bank 240 : BG1
   pullBGSpritesFrom(244 * 4096); // Bank 244 : BG2
   pullBGSpritesFrom(248 * 4096); // Bank 248 : BG2

   // Display
   for (auto& thing : bg_decals)
    {
      thing->Update();
    }
   drawBackground(240 * 4096); // Bank 240 : BG1
   drawBackground(244 * 4096); // Bank 244 : BG2
   drawBackground(248 * 4096); // Bank 248 : BG2
 }
