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
   bg_sprites.reserve(512); // This ought to handle simple games well.
   bg_decals.reserve(512);

   usr_sprites.reserve(256);
   usr_decals.reserve(256);
   for (int i = 0; i < 256; ++i)
    {
      // Initialize these to the maximum sprite size so we don't have to do further allocations.
      usr_sprites.emplace_back(std::make_unique<olc::Sprite>(64, 64));
      usr_decals.emplace_back(std::make_unique<olc::Decal>(usr_sprites[i].get()));
    }
   spr_3.reserve(256);
   spr_2.reserve(256);
   spr_1.reserve(256);
   spr_0.reserve(256);
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

void SpriteEngine::drawBackground(int bg_base, int offsets)
 {
   int x_offset = ((int)VRAM[255 * 4096 + offsets]) | ((int)VRAM[255 * 4096 + offsets + 1]) << 8;
   int y_offset = ((int)VRAM[255 * 4096 + offsets + 2]) | ((int)VRAM[255 * 4096 + offsets + 3]) << 8;
   for (int y = 0; y < 60; ++y)
    {
      for (int x = 0; x < 80; ++x)
       {
         int tile = ((int)VRAM[bg_base + (y * 80 + x) * 2]) | ((int)VRAM[bg_base + (y * 80 + x) * 2 + 1]) << 8;
         int loc = bg_base + 3 * 4096 + (y * 80 + x) / 4;
         int shl = 6 - ((y * 80 + x) & 3) * 2;
         float vMirror = (VRAM[loc] & (2 << shl)) ? -1.0f : 1.0f;
         float hMirror = (VRAM[loc] & (1 << shl)) ? -1.0f : 1.0f;
         engine->DrawDecal({x * 16.0f + (hMirror < 0.0f ? 16.0f : 0.0f) - x_offset, y * 16.0f + (vMirror < 0.0f ? 16.0f : 0.0f) - y_offset},
            bg_decals[bg_tiles[tile]].get(), {hMirror, vMirror});
       }
    }
 }

void SpriteEngine::drawSprites(const std::vector<int>& list)
 {
   for (int thing : list)
    {
      int base = 252 * 4096 + thing * 8;
      int x = ((int)VRAM[base + 0]) | ((int)VRAM[base + 1]) << 8;
      int y = ((int)VRAM[base + 2]) | ((int)VRAM[base + 3]) << 8;
      float vMirror = 1.0f;
      float hMirror = 1.0f;
      if (VRAM[base + 7] & 2)
       {
         vMirror = -1.0f;
         y += usr_sprites[thing]->height;
       }
      if (VRAM[base + 7] & 1)
       {
         hMirror = -1.0f;
         x += usr_sprites[thing]->width;
       }
      usr_decals[thing]->Update();
      engine->DrawDecal({static_cast<float>(x), static_cast<float>(y)}, usr_decals[thing].get(), {hMirror, vMirror});
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
   // Now sprites
   spr_3.clear();
   spr_2.clear();
   spr_1.clear();
   spr_0.clear();
   for (int i = 255; i > -1; --i) // Backwards so that our priority lists are in order.
    {
      int base = 252 * 4096 + i * 8;
      int palette = VRAM[base + 6] & 0x3F;
      int priority = VRAM[base + 6] >> 6;
      switch (priority)
       {
         case 3: spr_3.push_back(i); break;
         case 2: spr_2.push_back(i); break;
         case 1: spr_1.push_back(i); break;
         case 0: spr_0.push_back(i); break;
       }
      // WHAT AM I DOING? This seems so bad.
      usr_sprites[i]->height = 1 << (3 + ((VRAM[base + 5] >> 4) & 3));
      usr_sprites[i]->width = 1 << (3 + ((VRAM[base + 5] >> 6) & 3));
      int tile = ((int)VRAM[base + 4]) | ((int)VRAM[base + 5] & 15) << 8;
      for (int y = 0; y < usr_sprites[i]->height; ++y)
       {
         for (int x = 0; x < usr_sprites[i]->width; ++x)
          {
            usr_sprites[i]->GetData()[y * usr_sprites[i]->width + x] = palettes[palette][VRAM[128 * 4096 + 64 * tile + y * usr_sprites[i]->width + x]];
          }
       }
    }

   // Step 2: Display
   for (auto& thing : bg_decals)
    {
      thing->Update();
    }
   drawSprites(spr_0);
   drawBackground(248 * 4096, 8); // Bank 248 : BG3
   drawSprites(spr_1);
   drawBackground(244 * 4096, 4); // Bank 244 : BG2
   drawSprites(spr_2);
   drawBackground(240 * 4096, 0); // Bank 240 : BG1
   drawSprites(spr_3);
 }
