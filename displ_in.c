/*
Overgod
Copyright (C) 2005 Linley Henzell

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public Licence as published by
    the Free Software Foundation; either version 2 of the Licence, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public Licence for more details.

    You should have received a copy of the GNU General Public Licence
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    The GPL version 2 is included in this distribution in a file called
    LICENCE.TXT. Use any text editor or the TYPE command to read it.

    You should be able to reach me by sending an email to
    l_henzell@yahoo.com.au.
    
File: displ_in.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - bitmap loading and preparation
The bitmaps aren't organised into a datafile because I find datafiles a bit
unwieldy.

*/

#include "config.h"

#include "allegro.h"

#include <math.h>
#include "globvars.h"

//#include "stuff.h"

#include "display.h"

#include "palette.h"
#include "tile.h"

//#define FIX_FONT

extern int debug_info;


extern BITMAP *player1;
extern BITMAP *player2;

extern BITMAP *ship_bitmaps [NO_SHIP_TYPES] [17];

extern BITMAP *gb_ship_bitmaps [GB_SHIP_TYPES] [17];


/*
White (lines only)
Blue
Green
Amber
Red
*/
extern RLE_SPRITE *large_ships [16] [5];

extern BITMAP *enemy1_bmp [ENEMY1_BMPS];

extern RLE_SPRITE *enemy1_rle [ENEMY1_RLES];
extern RLE_SPRITE *enemy2_rle [ENEMY2_RLES];
extern RLE_SPRITE *enemy3_rle [ENEMY3_RLES];

extern BITMAP *enemy_bmps [ENEMY_BMPS];

extern RLE_SPRITE *enemy4_rle [ENEMY4_RLES];
extern RLE_SPRITE *enemy5_rle [ENEMY5_RLES];
extern RLE_SPRITE *enemy6_rle [ENEMY6_RLES];
extern RLE_SPRITE *enemy7_rle [ENEMY7_RLES];
extern RLE_SPRITE *enemy8_rle [ENEMY8_RLES];
extern RLE_SPRITE *enemy9_rle [ENEMY9_RLES];

extern RLE_SPRITE *small1_rle [SMALL1_RLES];
extern RLE_SPRITE *small3_rle [SMALL3_RLES];
extern BITMAP *small4_bmp [SMALL4_BMPS];
extern BITMAP *small2_bmp [SMALL2_BMPS];
extern BITMAP *superjelly_bmp [2];
extern BITMAP *pretile_bmp [NO_PRETILE_BMPS];
extern BITMAP *pretile_m_bmp [NO_MAZES] [NO_PRETILE_M_BMPS];
extern BITMAP *redbang_bmp [50];
extern RLE_SPRITE *light_rle [100];
extern BITMAP *shield_bmp [SHIELD_BMPS];
extern RLE_SPRITE *tile_background;

void bitmap_error(const char errtxt []);

void make_rle_enemy1(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy2(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy3(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy4(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy5(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_small1(BITMAP *source_bmp, int which_small, int width, int height);
void make_rle_small3(BITMAP *source_bmp, int which_small, int width, int height);
void make_bmp_small2(BITMAP *source_bmp, int which_small, int width, int height);
void make_bmp_small4(BITMAP *source_bmp, int which_small, int width, int height);
void make_rle_enemy6(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy7(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy8(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_enemy9(BITMAP *source_bmp, int which_enemy, int width, int height);
void make_rle_large_ship(BITMAP *source_bmp, int which_ship);
void make_bmp_enemy(BITMAP *source_bmp, int which_enemy, int width, int height);
//void make_superjelly_bmps(void);
void make_bang_bmps(void);
void make_light_bmps(void);
void make_bmp_tiles(void);

void make_actor_sprites(int at);
void progress_update(const char progtxt []);

extern FONT *font2;
extern FONT *small_font;
extern BITMAP *level_bmp;
extern BITMAP *distortion_mask;
extern BITMAP *crystal_bmp;
extern RGB light_palet [256];
extern RLE_SPRITE *waver1_circle;
extern RLE_SPRITE *waver2_circle;


void init_display(void)
{
 debug_info = 0;
 
 player1 = NULL;
 player2 = NULL;

 level_bmp = NULL;
 RGB temp_palette2 [1024]; // these seem to serve as a buffer against strange random corruptions of temp_palette. I don't understand this at all.
 RGB temp_palette [256];
 RGB temp_palette3 [1024];

 DATAFILE *datf = load_datafile("gfx//data.dat");
 if (datf == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Error: Couldn't find data.dat! \n");
  allegro_message("\n\r");
  exit(1);
 }

//progress_update("Data.dat loaded");

 font = (FONT *)datf[1].dat;
 font2 = (FONT *)datf[0].dat;
 small_font = (FONT *)datf[2].dat;

// text_mode(-1);
progress_update("Data.dat");
 textprintf_centre_ex(screen, font2, 300, 100, COLOUR_YELLOW8, -1, "{__loading__");
 textprintf_centre_ex(screen, font, 300, 100, -1, -1, "{__loading__");
 rectfill(screen, 390, 117, 450, 118, COLOUR_YELLOW8);

//rest(1000);

 BITMAP *temp_bitmap = load_bitmap("gfx//gb_si.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (ships)");
 }

 BITMAP *temp_bitmap2 = create_bitmap(24, 22);
 if (temp_bitmap2 == NULL)
 {
  bitmap_error("temp_bitmap2 (ships)");
 }

// blit(temp_bitmap, temp_bitmap2, 1, 1, 0, 0, 11, 11);
// blit(temp_bitmap, temp_bitmap2, 15, 1, 0, 0, 13, 13);

 int i;
 int j;
 int k;
 int l;
 int m;


 for (i = 0; i < 528; i ++)
 {
  for (j = 0; j < 249; j ++)
  {
   if (getpixel(temp_bitmap, i, j) == 17)
//       || getpixel(temp_bitmap, i, j) == 17)
        continue;
   m = 0;
   for (k = -1; k < 2; k ++)
   {
    for (l = -1; l < 2; l ++)
    {
     if (k == 0 && l == 0)
      continue;
     if (getpixel(temp_bitmap, i + k, j + l) != 0 
      && getpixel(temp_bitmap, i + k, j + l) != 17
      && getpixel(temp_bitmap, i + k, j + l) != COLOUR_GREY2)
      m ++;
    }
   }
   if (getpixel(temp_bitmap, i, j) == 0)
   {
    if (m > 0)
     putpixel(temp_bitmap, i, j, COLOUR_GREY2);
   }
    else
    { 
     if (getpixel(temp_bitmap, i, j) == 2)
      putpixel(temp_bitmap, i, j, COLOUR_GREY6);
       else
      putpixel(temp_bitmap, i, j, COLOUR_GREY5);
    }
      
/*     else
     {
      if (getpixel(temp_bitmap, i, j) == 2)
       putpixel(temp_bitmap, i, j, COLOUR_GREY6);
        else
         putpixel(temp_bitmap, i, j, COLOUR_GREY5);
     } */
  }
 }

 progress_update("Vehicles");

/*

 for (i = 0; i < 528; i ++)
 {
  for (j = 0; j < 249; j ++)
  {
   if (getpixel(temp_bitmap, i, j) == 0
       || getpixel(temp_bitmap, i, j) == 17)
        continue;
   m = 0;
   for (k = -1; k < 2; k ++)
   {
    for (l = -1; l < 2; l ++)
    {
     if (k == 0 && l == 0)
      continue;
     if (getpixel(temp_bitmap, i + k, j + l) == 0)
      m ++;
    }
   }
   if (m > 1)
    putpixel(temp_bitmap, i, j, COLOUR_GREY3);
     else
     {
      if (getpixel(temp_bitmap, i, j) == 2)
       putpixel(temp_bitmap, i, j, COLOUR_GREY6);
        else
         putpixel(temp_bitmap, i, j, COLOUR_GREY5);
     } 
  }
 }

*/
 for (i = 0; i < GB_SHIP_TYPES; i ++)
 {
/*  blit(temp_bitmap, temp_bitmap2, (i * 14) + 1, 1, 0, 0, 13, 13);
  for (j = 0; j < 17; j ++)
  {
   ship_bitmaps [i] [j] = create_bitmap(13, 13);
   if (ship_bitmaps [i] [j] == NULL)
   {
    bitmap_error("Ship bitmaps 0");
   }
   clear_bitmap(ship_bitmaps [i] [j]);
   blit(temp_bitmap, ship_bitmaps [i] [j], (j * 14) + 1, (i * 14) + 1, 0,0,13,13);*/
//   if (FALSE)//i != 11)// && i != 1 && i != 3 && i != 4 && FALSE)
//   if (i != 12 && i != 13 && i != 14 && i != 15)
   {
    for (j = 0; j < 17; j ++)
    {
     gb_ship_bitmaps [i] [j] = create_bitmap(24, 22);
     if (gb_ship_bitmaps [i] [j] == NULL)
     {
      bitmap_error("Ship bitmaps 0");
     }
     clear_bitmap(gb_ship_bitmaps [i] [j]);
     blit(temp_bitmap, gb_ship_bitmaps [i] [j], (j * 31) + 1, (i * 31) + 1, 0,0,24,22);
    }
   }
//    else
//      make_actor_sprites(i);
//   rotate_sprite(ship_bitmaps [i] [j], temp_bitmap2, 0, 0, itofix(j * 4));
 }

// save_bitmap("outsh.bmp",
 
/* blit(temp_bitmap, temp_bitmap2, 15, 1, 0, 0, 13, 13);

 for (i = 0; i < 17; i ++)
 {
  ship_bitmaps [1] [i] = create_bitmap(13, 13);
  if (ship_bitmaps [1] [i] == NULL)
  {
   bitmap_error("Ship bitmaps 0");
  }
  clear_bitmap(ship_bitmaps [1] [i]);
  rotate_sprite(ship_bitmaps [1] [i], temp_bitmap2, 0, 0, itofix(i * 4));
 }
 
 blit(temp_bitmap, temp_bitmap2, 10, 1, 0, 0, 8, 8);
*/
 destroy_bitmap(temp_bitmap);
 destroy_bitmap(temp_bitmap2);
 progress_update("More Vehicles");

#ifdef FIX_FONT

 RGB font_palette [256];
// BITMAP *fbmp = load_bitmap("gfx//font_cel.bmp", font_palette);
 BITMAP *fbmp = load_bitmap("gfx//font_oc.bmp", font_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (font_cel)");
 }
 
// int k;
 
 for (i = 0; i < 1000; i ++)
 {
  for (j = 0; j < 1000; j ++)
  {
   k = getpixel(fbmp, i, j);
//   if (k >= GC_BLACK && k <= GC_BLUE8)
//    k += 48;
//   if (k >= COLOUR_GREY2 && k <= COLOUR_WHITE)
//    k = COLOUR_GREY4; 
   if (k == 111)
    k = COLOUR_GREY4;
   if (k == 110)
    k = COLOUR_GREY1; 
   putpixel(fbmp, i, j, k); 
/*   switch(getpixel(fbmp, i, j))
   {
    case 110: putpixel(fbmp, i, j, GC_GREY1); break;
    case 111: putpixel(fbmp, i, j, GC_GREY6); break;
    case 28: putpixel(fbmp, i, j, GC_GREY6); break;
    case 26: putpixel(fbmp, i, j, GC_GREY6); break;
    case 24: putpixel(fbmp, i, j, GC_GREY6); break;
   }*/
  }
 }
/*
 for (i = 0; i < 1000; i ++)
 {
  for (j = 0; j < 1000; j ++)
  {
   if (getpixel(temp_bitmap, i, j) == 255)
    continue;
   for (k = -1; k < 2; k ++)
   {
    for (l = -1; l < 2; l ++)
    {
     if (k == 0 && l == 0)
      continue;
     if (getpixel(temp_bitmap, i + k, j + l) != 0 
      && getpixel(temp_bitmap, i + k, j + l) != 255
      && getpixel(temp_bitmap, i + k, j + l) != COLOUR_GREY2)
      m ++;
    }
   }
   if (getpixel(temp_bitmap, i, j) == 0)
   {
    if (m > 0)
     putpixel(temp_bitmap, i, j, COLOUR_GREY2);
   }
    else
    { 
     if (getpixel(temp_bitmap, i, j) == 2)
      putpixel(temp_bitmap, i, j, COLOUR_GREY6);
       else
      putpixel(temp_bitmap, i, j, COLOUR_GREY5);
    }

*/
 save_bitmap("font_oc.bmp", fbmp, font_palette);


#endif

#ifdef GENERATE_SHIPS


 temp_bitmap = load_bitmap("gfx//gb_ship.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("gb_ship");
 }

 temp_bitmap2 = create_bitmap(24, 22);
 if (temp_bitmap2 == NULL)
 {
  bitmap_error("temp_bitmap2 (gb_ships)");
 }

 blit(temp_bitmap, temp_bitmap2, 1, 1, 0, 0, 24, 24);
// blit(temp_bitmap, temp_bitmap2, 15, 1, 0, 0, 13, 13);

// int i;
// int j;

progress_update("Vehicles");

 for (i = 0; i < GB_SHIP_TYPES; i ++)
 {
  blit(temp_bitmap, temp_bitmap2, (i * 31) + 1, 1, 0, 0, 24, 24);
  for (j = 0; j < 17; j ++)
  {
   gb_ship_bitmaps [i] [j] = create_bitmap(24, 24);
   if (gb_ship_bitmaps [i] [j] == NULL)
   {
    bitmap_error("Gb_ship bitmaps");
   }
   clear_bitmap(gb_ship_bitmaps [i] [j]);
   rotate_sprite(gb_ship_bitmaps [i] [j], temp_bitmap2, 0, 0, itofix(j * 4.15));
  } 
 }
 destroy_bitmap(temp_bitmap);
 destroy_bitmap(temp_bitmap2);





progress_update("More Vehicles");

// Let's make the output ship bitmap:

 BITMAP *outp = create_bitmap((17 * 31) + 1, (17 * 31) + 1);
 if (outp == NULL)
 {
  bitmap_error("Output");
 }
 clear_bitmap(outp);

 for (i = 0; i < GB_SHIP_TYPES; i ++)
 {
  for (j = 0; j < 17; j ++)
  {
   hline(outp, 0, i * 31, 18 * 32 + 1, COLOUR_PURPLE8);
   vline(outp, j * 31, 0, GB_SHIP_TYPES * 31 + 1, COLOUR_PURPLE8);
   if (ship_bitmaps [i] [j] != NULL)
    draw_sprite(outp, gb_ship_bitmaps [i] [j], j * 31 + 1, i * 31 + 1);
  }
 }

 blit(outp, screen, 0,0,5,5, (16 * 31) + 1, (16 * 31) + 1);
 save_bitmap("gb_so.bmp", outp, temp_palette);

 while (key [KEY_K] == 0)
 {
 };

#endif

// destroy_bitmap(outp);


// Load in enemy bitmaps:

 temp_bitmap = load_bitmap("gfx//gb_nme1.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_nme1.bmp not loaded correctly?)");
 }
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_GUARDIAN1, 59, 49);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_BRACKET1, 51, 45);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_WORMER1, 61, 50);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_BRACKET2, 55, 38);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_BRACKET3, 55, 49);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_BRACKET4, 69, 49);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_GUARDIAN2, 51, 70);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_GUARDIAN3, 59, 61);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_WORMER2, 61, 50);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_WORMER3, 65, 65);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_FORKER1, 53, 53);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_FORKER2, 53, 53);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_MINER1, 39, 37);
 make_rle_enemy4(temp_bitmap, RLE_ENEMY4_MINER2, 49, 49);
 destroy_bitmap(temp_bitmap);

 progress_update("Enemies 1");


 temp_bitmap = load_bitmap("gfx//gb_nme2.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_nme2.bmp not loaded correctly?)");
 }
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_MINER3, 49, 67);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_CIRCLER1, 69, 45);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_MULTI1, 69, 69);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_MULTI2, 67, 67);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_MULTI3, 65, 65);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_GUARDIAN4, 69, 65);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_GUARDIAN5, 69, 59);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_BRACKET5, 63, 61);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_CURVE1, 69, 53);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_CURVE2, 69, 53);
 make_rle_enemy6(temp_bitmap, RLE_ENEMY6_CURVE3, 69, 53);
 destroy_bitmap(temp_bitmap);

 progress_update("Enemies 2");

//extern BITMAP *enemy_bmps [ENEMY_BMPS];


 temp_bitmap = load_bitmap("gfx//gb_nmebm.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_nmebm.bmp not loaded correctly?)");
 }
 make_bmp_enemy(temp_bitmap, BMP_ENEMY_PULSER1_V, 23, 28);
 make_bmp_enemy(temp_bitmap, BMP_ENEMY_PULSER1_H, 28, 23);
 make_bmp_enemy(temp_bitmap, BMP_ENEMY_PULSER2_V, 25, 28);
 make_bmp_enemy(temp_bitmap, BMP_ENEMY_PULSER2_H, 28, 25);

 destroy_bitmap(temp_bitmap);

 progress_update("Enemies 3");

 temp_bitmap = load_bitmap("gfx//gb_big1.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_big1.bmp not loaded correctly?)");
 }
 make_rle_enemy5(temp_bitmap, RLE_ENEMY5_OVERTRIANGLER, 105, 105);
 make_rle_enemy5(temp_bitmap, RLE_ENEMY5_DEFENDER1, 131, 67);
 make_rle_enemy5(temp_bitmap, RLE_ENEMY5_DEFENDER2, 67, 121);
 make_rle_enemy5(temp_bitmap, RLE_ENEMY5_DEFENDER3, 131, 67);
 make_rle_enemy5(temp_bitmap, RLE_ENEMY5_OVERDISRUPTER, 41, 139);
// make_rle_enemy5(temp_bitmap, RLE_ENEMY5_BOSS1_1, 139, 101);
 destroy_bitmap(temp_bitmap);

 progress_update("Enemies 4");

 temp_bitmap = load_bitmap("gfx//gb_big2.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_big2.bmp not loaded correctly?)");
 }
 make_rle_enemy7(temp_bitmap, RLE_ENEMY7_BOSS1_1, 189, 224);
 make_rle_enemy7(temp_bitmap, RLE_ENEMY7_BOSS1_2, 229, 216);
 make_rle_enemy7(temp_bitmap, RLE_ENEMY7_BOSS1_3, 229, 193);
 make_rle_enemy7(temp_bitmap, RLE_ENEMY7_BOSS2_2, 247, 213);
 make_rle_enemy7(temp_bitmap, RLE_ENEMY7_BOSS3_2, 231, 193);
 make_rle_enemy7(temp_bitmap, RLE_ENEMY7_BEAMER1, 185, 139);
 make_rle_enemy7(temp_bitmap, RLE_ENEMY7_BEAMER2, 185, 139);
 destroy_bitmap(temp_bitmap);

 progress_update("Enemies 5");

 temp_bitmap = load_bitmap("gfx//gb_big3.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_big3.bmp not loaded correctly?)");
 }

 make_rle_enemy8(temp_bitmap, RLE_ENEMY8_BOSS3, 239, 239);
 make_rle_enemy8(temp_bitmap, RLE_ENEMY8_BOSS2, 249, 249);
 make_rle_enemy8(temp_bitmap, RLE_ENEMY8_BOSS2_3, 265, 241);
 make_rle_enemy8(temp_bitmap, RLE_ENEMY8_BOSS3_3, 281, 223);

 destroy_bitmap(temp_bitmap);

 progress_update("Enemies 6");


 temp_bitmap = load_bitmap("gfx//gb_med1.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_med1.bmp not loaded correctly?)");
 }

 make_rle_enemy9(temp_bitmap, RLE_ENEMY9_DISRUPTER1, 89, 25);
 make_rle_enemy9(temp_bitmap, RLE_ENEMY9_DISRUPTER2, 29, 93);
 make_rle_enemy9(temp_bitmap, RLE_ENEMY9_DISRUPTER3, 89, 33);
 make_rle_enemy9(temp_bitmap, RLE_ENEMY9_TRIANGLER1, 73, 73);
 make_rle_enemy9(temp_bitmap, RLE_ENEMY9_TRIANGLER2, 71, 71);
 make_rle_enemy9(temp_bitmap, RLE_ENEMY9_TRIANGLER3, 71, 71);
 make_rle_enemy9(temp_bitmap, RLE_ENEMY9_WORMER4, 83, 83);
 destroy_bitmap(temp_bitmap);

 progress_update("Enemies 7");

/* temp_bitmap = load_bitmap("gfx//enemy3.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (enemy3.bmp not loaded correctly?)");
 }

 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_FIREBASE, 133, 133);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_FIREBASE2, 39, 69);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_FIREBASE3, 91, 93);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_BOSS1, 175, 159);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_BOSS3, 155, 147);
 make_rle_enemy3(temp_bitmap, RLE_ENEMY3_BOSS4, 223, 138);

// blit(temp_bitmap, temp_bitmap2, 1, 1, 0, 0, 35, 26);

// enemy1_rle [RLE_ENEMY1_STINGER] = get_rle_sprite(temp_bitmap2);

// destroy_bitmap(temp_bitmap2);

 destroy_bitmap(temp_bitmap);
// destroy_bitmap(temp_bitmap2);*/

/* progress_update("Enemy3 Loaded");

 temp_bitmap = load_bitmap("gfx//small1.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (small1.bmp not loaded correctly?)");
 }

 for (i = RLE_SMALL1_GREEN_BLOB_L; i < SMALL1_RLES; i ++)
 {
  make_rle_small1(temp_bitmap, i, 11, 11);
 }
 
 
 progress_update("Small1 Loaded");
*/
 temp_bitmap = load_bitmap("gfx//gb_small.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gm_small.bmp not loaded correctly?)");
 }

 make_rle_small3(temp_bitmap, RLE_SMALL3_GREEN_BLOB_1, 8, 8);
 make_rle_small3(temp_bitmap, RLE_SMALL3_GREEN_BLOB_2, 8, 8);
 make_rle_small3(temp_bitmap, RLE_SMALL3_GREEN_BLOB_3, 7, 7);
 make_rle_small3(temp_bitmap, RLE_SMALL3_GREEN_BLOB_4, 6, 6);
 make_rle_small3(temp_bitmap, RLE_SMALL3_GREEN_BLOB_5, 5, 5);
 make_rle_small3(temp_bitmap, RLE_SMALL3_GREEN_BLOB_6, 4, 4);
 make_rle_small3(temp_bitmap, RLE_SMALL3_GREEN_BLOB_7, 3, 3);
 make_rle_small3(temp_bitmap, RLE_SMALL3_ORANGE_BLOB_1, 8, 8);
 make_rle_small3(temp_bitmap, RLE_SMALL3_ORANGE_BLOB_2, 8, 8);
 make_rle_small3(temp_bitmap, RLE_SMALL3_ORANGE_BLOB_3, 7, 7);
 make_rle_small3(temp_bitmap, RLE_SMALL3_ORANGE_BLOB_4, 6, 6);
 make_rle_small3(temp_bitmap, RLE_SMALL3_ORANGE_BLOB_5, 5, 5);
 make_rle_small3(temp_bitmap, RLE_SMALL3_ORANGE_BLOB_6, 4, 4);
 make_rle_small3(temp_bitmap, RLE_SMALL3_ORANGE_BLOB_7, 3, 3);
 make_rle_small3(temp_bitmap, RLE_SMALL3_BLUE_BLOB_1, 8, 8);
 make_rle_small3(temp_bitmap, RLE_SMALL3_BLUE_BLOB_2, 7, 7);
 make_rle_small3(temp_bitmap, RLE_SMALL3_BLUE_BLOB_3, 6, 6);
 make_rle_small3(temp_bitmap, RLE_SMALL3_BLUE_BLOB_4, 6, 6);
 make_rle_small3(temp_bitmap, RLE_SMALL3_BLUE_BLOB_5, 5, 5);
 make_rle_small3(temp_bitmap, RLE_SMALL3_BLUE_BLOB_6, 5, 5);
 make_rle_small3(temp_bitmap, RLE_SMALL3_BLUE_BLOB_7, 3, 3);
 make_rle_small3(temp_bitmap, RLE_SMALL3_YELLOW_BLOB_1, 8, 8);
 make_rle_small3(temp_bitmap, RLE_SMALL3_YELLOW_BLOB_2, 8, 8);
 make_rle_small3(temp_bitmap, RLE_SMALL3_YELLOW_BLOB_3, 7, 7);
 make_rle_small3(temp_bitmap, RLE_SMALL3_YELLOW_BLOB_4, 6, 6);
 make_rle_small3(temp_bitmap, RLE_SMALL3_YELLOW_BLOB_5, 5, 5);
 make_rle_small3(temp_bitmap, RLE_SMALL3_YELLOW_BLOB_6, 4, 4);
 make_rle_small3(temp_bitmap, RLE_SMALL3_YELLOW_BLOB_7, 3, 3); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE1, 12, 12); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE2, 12, 12); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE3, 12, 12); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE4, 12, 12); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE2_1, 12, 12); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE2_2, 12, 12); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE2_3, 12, 12); 
 make_rle_small3(temp_bitmap, RLE_SMALL3_PULSE2_4, 12, 12); 
 destroy_bitmap(temp_bitmap);


 progress_update("Small Things");

 temp_bitmap = load_bitmap("gfx//gb_small2.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gm_small2.bmp not loaded correctly?)");
 }

 for (i = 0; i < SMALL4_BMPS; i ++)
 {
  make_bmp_small4(temp_bitmap, i, 10, 10);
 } 
/* 
 make_bmp_small4(temp_bitmap, BMP_SMALL4_EYE_1, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_EYE_2, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_EYE_3, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_EYE_4, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_EYE_5, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_EYE_6, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_EYE_7, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_BEYE_1, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_BEYE_2, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_BEYE_3, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_BEYE_4, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_BEYE_5, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_BEYE_6, 10, 10);
 make_bmp_small4(temp_bitmap, BMP_SMALL4_BEYE_7, 10, 10);*/
 destroy_bitmap(temp_bitmap);

 for (i = 0; i < 8; i ++)
 {
  shield_bmp [i] = create_bitmap(10, 10);
  if (shield_bmp [i] == NULL)
   bitmap_error("shield_bmp (out of memory?)");
  clear_bitmap(shield_bmp [i]); 
  shield_bmp [i + 8] = create_bitmap(10, 10);
  if (shield_bmp [i + 8] == NULL)
   bitmap_error("shield_bmp (out of memory?)");
  clear_bitmap(shield_bmp [i + 8]); 
  shield_bmp [i + 16] = create_bitmap(10, 10);
  if (shield_bmp [i + 16] == NULL)
   bitmap_error("shield_bmp (out of memory?)");
  clear_bitmap(shield_bmp [i + 16]); 
  
  draw_sprite_h_flip(shield_bmp [i], small4_bmp [BMP_SMALL4_SHIELD_1 + i], 0, 0);
  draw_sprite_vh_flip(shield_bmp [i + 8], small4_bmp [BMP_SMALL4_SHIELD_1 + i], 0, 0);
  draw_sprite_v_flip(shield_bmp [i + 16], small4_bmp [BMP_SMALL4_SHIELD_1 + i], 0, 0);
 }

 progress_update("More Small Things");


/*
 temp_bitmap = load_bitmap("gfx//small2.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (small2.bmp not loaded correctly?)");
 }

 for (i = BMP_SMALL2_MISSILE_1; i < BMP_SMALL2_SIDE_BOMB + 1; i ++)
 {
  make_bmp_small2(temp_bitmap, i, 7, 7);
 }

 destroy_bitmap(temp_bitmap);

 progress_update("Small3 Loaded");

 temp_bitmap = load_bitmap("gfx//enemy2.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (enemy2.bmp not loaded correctly?)");
 }

 for (i = 0; i < 9; i ++)
 {
  make_rle_enemy2(temp_bitmap, i, 27, 27);
 }

 destroy_bitmap(temp_bitmap);
 
 progress_update("Enemy2 Loaded");
*/
 temp_bitmap = load_bitmap("gfx//gb_lsh.bmp", temp_palette);
 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_lsh.bmp not loaded correctly?)");
 }

// int b;
 
 for (i = 0; i < 16; i ++)
 {
  make_rle_large_ship(temp_bitmap, i);
 }
 
 destroy_bitmap(temp_bitmap);
 
 progress_update("More Vehicles");

 tile_background = NULL;
 make_bmp_tiles();

 init_tiles(); // just clears the rle array, setting all to NULL, and prepares the menu tile

 level_bmp = create_bitmap(500, 500);

 if (!level_bmp)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Bitmap creation error (level_bmp) \n%s\n", allegro_error);
  exit(1);
 }
 
 progress_update("Tiles Loaded and Processed");

// level_bmp = create_bitmap(5, 5);

 make_bang_bmps();

 make_light_bmps();
 
 progress_update("Light Sourcing");

// init_messages();

// progress_update("Messages Initialised");
 
 distortion_mask = create_bitmap(201, 201);

 if (distortion_mask == NULL)
 {
  bitmap_error("distortion_mask (out of memory?)");
 }

 crystal_bmp = create_bitmap(20, 20);

 if (crystal_bmp == NULL)
 {
  bitmap_error("crystal_bmp (out of memory?)");
 }

 BITMAP *temp_waver_bmp = create_bitmap(41, 41);
  
 if (temp_waver_bmp == NULL)
  bitmap_error("temp_waver_bmp (out of memory?)");
 clear_bitmap(temp_waver_bmp);
 circlefill(temp_waver_bmp, 20, 20, 19, GC_RED5);
 circle(temp_waver_bmp, 20, 20, 19, GC_RED1);
 circlefill(temp_waver_bmp, 20, 20, 12, 0);
 circle(temp_waver_bmp, 20, 20, 12, GC_RED1);
 waver1_circle = get_rle_sprite(temp_waver_bmp);
 destroy_bitmap(temp_waver_bmp);

 temp_waver_bmp = create_bitmap(41, 41);
 if (temp_waver_bmp == NULL)
  bitmap_error("temp_waver_bmp (out of memory?)");
 clear_bitmap(temp_waver_bmp);
 circlefill(temp_waver_bmp, 20, 20, 19, GC_GREEN5);
 circle(temp_waver_bmp, 20, 20, 19, GC_GREEN1);
 circlefill(temp_waver_bmp, 20, 20, 15, 0);
 circle(temp_waver_bmp, 20, 20, 15, GC_GREEN1);
 waver2_circle = get_rle_sprite(temp_waver_bmp);
 destroy_bitmap(temp_waver_bmp);

 progress_update("Effects");

// set_palette(light_palet);
 set_light_palette();
 progress_update("Palette");
 
}

void make_rle_enemy1(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy1)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 51 + 1, 1, 0, 0, width, height);

 enemy1_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);
}

void make_rle_enemy2(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);

 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy2)");
 }

 clear_bitmap(temp_bmp);

 blit(source_bmp, temp_bmp, which_enemy * 41 + 1, 1, 0, 0, width, height);
 enemy2_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);


}

void make_rle_enemy3(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy3)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 301 + 1, 1, 0, 0, width, height);

 enemy3_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_enemy4(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy4)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 71 + 1, 1, 0, 0, width, height);

 enemy4_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_enemy5(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy5)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 141 + 1, 1, 0, 0, width, height);

 enemy5_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_enemy6(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy6)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 71 + 1, 1, 0, 0, width, height);

 enemy6_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_enemy7(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy7)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 249 + 1, 1, 0, 0, width, height);

 enemy7_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_enemy8(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy8)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 282 + 1, 1, 0, 0, width, height);

 enemy8_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_enemy9(BITMAP *source_bmp, int which_enemy, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_enemy9)");
 }

 blit(source_bmp, temp_bmp, which_enemy * 109 + 1, 1, 0, 0, width, height);

 enemy9_rle [which_enemy] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_small1(BITMAP *source_bmp, int which_small, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_small1)");
 }

 blit(source_bmp, temp_bmp, which_small * 21 + 1, 1, 0, 0, width, height);

 small1_rle [which_small] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_rle_small3(BITMAP *source_bmp, int which_small, int width, int height)
{
 BITMAP *temp_bmp = create_bitmap(width, height);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_small3)");
 }

 blit(source_bmp, temp_bmp, which_small * 15 + 1, 1, 0, 0, width, height);

 small3_rle [which_small] = get_rle_sprite(temp_bmp);

 destroy_bitmap(temp_bmp);

}

void make_bmp_small2(BITMAP *source_bmp, int which_small, int width, int height)
{

 small2_bmp [which_small] = create_bitmap(width, height);
 
 if (small2_bmp [which_small] == NULL)
 {
  bitmap_error("temp_bmp (make_bmp_small2)");
 }

 blit(source_bmp, small2_bmp [which_small], which_small * 21 + 1, 1, 0, 0, width, height);

}

void make_bmp_enemy(BITMAP *source_bmp, int which_enemy, int width, int height)
{

 enemy_bmps [which_enemy] = create_bitmap(width, height);
 
 if (enemy_bmps [which_enemy] == NULL)
 {
  bitmap_error("temp_bmp (make_bmp_enemy)");
 }

 blit(source_bmp, enemy_bmps [which_enemy], which_enemy * 71 + 1, 1, 0, 0, width, height);

}


void make_bmp_small4(BITMAP *source_bmp, int which_small, int width, int height)
{

 small4_bmp [which_small] = create_bitmap(width, height);
 
 if (small4_bmp [which_small] == NULL)
 {
  bitmap_error("temp_bmp (make_bmp_small4)");
 }

 blit(source_bmp, small4_bmp [which_small], which_small * 15 + 1, 1, 0, 0, width, height);

}

void make_rle_large_ship(BITMAP *source_bmp, int which_ship)
{
 BITMAP *temp_bmp = create_bitmap(49, 49);
 if (temp_bmp == NULL)
 {
  bitmap_error("temp_bmp (make_rle_large_ship)");
 }

// if (which_ship > 11)
// {
//  blit(source_bmp, temp_bmp, 51 + 1, 1, 0, 0, 49, 49);
// }
//  else
   blit(source_bmp, temp_bmp, which_ship * 51 + 1, 1, 0, 0, 49, 49);

 int i, x, y, px, base_col;

 for (i = 0; i < 5; i ++)
 {
  switch(i)
  {
   case 0:
    base_col = COLOUR_BLUE1; break;
   case 1:
    base_col = COLOUR_GREEN1; break;
   case 2:
    base_col = COLOUR_YELLOW1; break;
   case 3:
    base_col = COLOUR_RED1; break;
  }
  for (x = 0; x < 50; x ++)
  {
   for (y = 0; y < 50; y ++)
   {
    px = getpixel(temp_bmp, x, y);
    if (i == 4)
    {
//     if (px % 8 != 5 && px != 0)
//     if ((y % 4 == 0 || x % 4 == 0) && px != 0)
     if (px % 8 < 4 && px != 0)
      putpixel(temp_bmp, x, y, COLOUR_WHITE);
       else
        putpixel(temp_bmp, x, y, 0);
    }
     else
     {
      if (px != 0)
       putpixel(temp_bmp, x, y, base_col + (px % 8) - 1);
     }
   }
  }
  large_ships [which_ship] [i] = get_rle_sprite(temp_bmp);
 }
 
 destroy_bitmap(temp_bmp);

}

void make_bmp_tiles(void)
{
// RGB temp_palette2 [1024];
 RGB temp_palette [256];
// RGB temp_palette3 [1024];
 
 BITMAP *temp_bitmap = load_bitmap("gfx//gb_tiles.bmp", temp_palette);

 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_tiles.bmp not loaded correctly?)");
 }

// clear_bitmap(temp_bmp2);

 int i, j;
 
 for (i = 0; i < NO_PRETILE_BMPS; i ++)
 {

  pretile_bmp [i] = create_bitmap(50, 50);
 
  if (pretile_bmp [i] == NULL)
  {
   bitmap_error("pretile (make_bmp_tiles)");
  }

  blit(temp_bitmap, pretile_bmp [i], i * 51 + 1, 1, 0, 0, 50, 50);
 }

 destroy_bitmap(temp_bitmap);

 temp_bitmap = load_bitmap("gfx//gb_maze.bmp", temp_palette);

 if (temp_bitmap == NULL)
 {
  bitmap_error("temp_bitmap (gb_maze.bmp not loaded correctly?)");
 }



 for (j = 0; j < NO_MAZES; j ++)
 {
  for (i = 0; i < NO_PRETILE_M_BMPS; i ++)
  {

   pretile_m_bmp [j] [i] = create_bitmap(50, 50);
// not [i] [j]...
   if (pretile_m_bmp [j] [i] == NULL)
   {
    bitmap_error("pretile_m_bmp (make_bmp_tiles)");
   }
   blit(temp_bitmap, pretile_m_bmp [j] [i], i * 51 + 1, j * 51 + 1, 0, 0, 50, 50);
  }
 }

 destroy_bitmap(temp_bitmap);

}

/*
void make_superjelly_bmps(void)
{
 superjelly_bmp [0] = create_bitmap(21, 21);
 if (superjelly_bmp [0] == NULL)
 {
  bitmap_error("temp_bmp (make_superjelly_bmps - superjelly_bmp [0])");
 }
 clear_bitmap(superjelly_bmp [0]);

 superjelly_bmp [1] = create_bitmap(21, 21);
 if (superjelly_bmp [1] == NULL)
 {
  bitmap_error("temp_bmp (make_superjelly_bmps - superjelly_bmp [1])");
 }
 clear_bitmap(superjelly_bmp [1]);

 draw_rle_sprite(superjelly_bmp [0], enemy1_rle [RLE_ENEMY1_SUPERJELLY], 0, 0);

 rotate_sprite(superjelly_bmp [1], superjelly_bmp [0], 0, 0, itofix(64));

}
*/

void make_light_bmps(void)
{

 int i;
 int c1 = 3, c2 = 2;
 BITMAP *temp_bmp;
   
 for (i = 0; i < 100; i ++)
 {
  if (i == 0)
   temp_bmp = create_bitmap(1, 1);
    else
     temp_bmp = create_bitmap(i * 2 + 2, i * 2 + 2);
  if (temp_bmp == NULL)
   bitmap_error("light_bmp");
  clear_bitmap(temp_bmp);
  c1 = i;
  c2 = i - (i / 6);
//  c3 = i - (i / 4);
  circlefill(temp_bmp, i + 1, i + 1, c1, LIGHT_1);
  circlefill(temp_bmp, i + 1, i + 1, c2, LIGHT_2);
  light_rle [i] = get_rle_sprite(temp_bmp);
  if (light_rle [i] == NULL)
   bitmap_error("light_rle");
  destroy_bitmap(temp_bmp);
 }

}

void make_bang_bmps(void)
{
 return;
/*
 int i;
 int c1 = 3, c2 = 2, c3 = 1;

 c1 = 0;
 c2 = -250;
 c3 = -500; 
 for (i = 0; i < 50; i ++)
 {
  redbang_bmp [i] = create_bitmap(i * 2, i * 2);
  if (redbang_bmp [i] == NULL)
   bitmap_error("redbang_bmp");
  clear_bitmap(redbang_bmp [i]);
  c1 += 10;
//  c2 = i - (i / 7);
//  c3 = i - (i / 4);
  c2 += 15;
  c3 += 20;
  circlefill(redbang_bmp [i], i, i, c1 / 10, TRANS_DRED);
  if (c2 > 0)
   circlefill(redbang_bmp [i], i, i, c2 / 10, TRANS_LRED);
  if (c3 > 0) 
   circlefill(redbang_bmp [i], i, i, c3 / 10, TRANS_YELLOW);
 }
*/
}

/*
void draw_sprite_v_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);

void draw_sprite_h_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);

void draw_sprite_vh_flip(BITMAP *bmp, BITMAP *sprite, int x, int y);
*/
void bitmap_error(const char errtxt [])
{
 set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
 allegro_message("Bitmap creation error  \n%s\n\n\r%s", allegro_error, errtxt);
// allegro_message("\n\r");
// allegro_message(errtxt);
 exit(1);

}

void progress_update(const char progtxt [])
{
 static int ypos = 170;
 rectfill(screen, 400, 117, 450, 118, COLOUR_YELLOW8);
 rectfill(screen, 449, 117, 450, ypos + 5, COLOUR_YELLOW8);
 hline(screen, 449, ypos + 5, 433, COLOUR_YELLOW8);
 textprintf_right_ex(screen, small_font, 430, ypos, COLOUR_GREEN8, -1, progtxt);
 ypos += 11; 
// do {rest(30);} while (!key [KEY_X]);
// rest(200);
}

void prepare_display(void)
{
// player bmps are set to NULL in init_display
 if (game.users == 1)
 {
  if (player1 != NULL)
   destroy_bitmap(player1);
  player1 = create_bitmap(scr_x, scr_y);
  clear_bitmap(player1);
 }
  else
  {
   if (player1 != NULL)
    destroy_bitmap(player1);
   if (player2 != NULL)
    destroy_bitmap(player2);
   player1 = create_bitmap(tp_window_width, scr_y);
   clear_bitmap(player1);
   player2 = create_bitmap(tp_window_width, scr_y);
   clear_bitmap(player2);
  }

}

#ifdef DONT_COMPILE

void make_actor_sprites_old_small(int at)
{

 int x,y,x1, x2, x3, y1, y2, y3;
 int i;
 float angle;
 int a1, a2, a3;

 int points [16];

 BITMAP *tbmp = create_bitmap(21, 21);
 
 switch(at)
 {
  case 1:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI + PI / 3) * 6;
    y2 = 100 + sin(angle + PI + PI / 3) * 6;
    x3 = 100 + cos(angle + PI - PI / 3) * 6;
    y3 = 100 + sin(angle + PI - PI / 3) * 6;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 3) * 5;
    y2 = 100 + sin(angle + PI + PI / 3) * 5;
    x3 = 100 + cos(angle + PI - PI / 3) * 5;
    y3 = 100 + sin(angle + PI - PI / 3) * 5;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
   }
  break;
  case 2:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x2 = 100 + cos(angle + PI + PI / 3) * 5;
    y2 = 100 + sin(angle + PI + PI / 3) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 3) * (6 - (angle == 16));
    y2 = 100 + sin(angle + PI - PI / 3) * (6 - (angle == 16));
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x = 10;
    y = 10;
    circlefill(ship_bitmaps [at] [i], x, y, 5, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x, y, 4, COLOUR_GREY5);
//    x1 = 10;// + cos(angle) * 2;
//    y1 = 10;// + sin(angle) * 2;
//    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 19);
    a2 = 64 - (i * 4 - 19);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 3, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 3, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 3, COLOUR_WHITE);
    }
   }
   break;
  case 3:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    circlefill(ship_bitmaps [at] [i], 10, 10, 2, COLOUR_GREY4);
    x2 = 100 + cos(angle + PI + PI / 4) * 5;
    y2 = 100 + sin(angle + PI + PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI + PI / 4) * 2;
    y2 = 100 + sin(angle + PI + PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 5;
    y2 = 100 + sin(angle + PI - PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 2;
    y2 = 100 + sin(angle + PI - PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 5;
    y2 = 100 + sin(angle + PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 2;
    y2 = 100 + sin(angle + PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 5;
    y2 = 100 + sin(angle - PI / 4) * 5;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x2, y2, 1, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 2;
    y2 = 100 + sin(angle - PI / 4) * 2;
    circlefill(ship_bitmaps [at] [i], x2, y2, 2, COLOUR_GREY5);
    x = 10;
    y = 10;
    circlefill(ship_bitmaps [at] [i], x, y, 1, COLOUR_GREY5);
//    x1 = 10;// + cos(angle) * 2;
//    y1 = 10;// + sin(angle) * 2;
//    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    }
   }
   break;
  case 4:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp);
    circlefill(tbmp, 10, 10, 7, COLOUR_GREY4);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 7, 0);
    blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);
    clear_bitmap(tbmp);
    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 8, 0);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY4);
    masked_blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 2, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 5:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp);
    circlefill(tbmp, 10, 10, 7, COLOUR_GREY4);
    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
/*    x = 10 - cos(angle) * 4;
    y = 10 - sin(angle) * 4;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 3;
    y = 10 - sin(angle) * 3;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 2;
    y = 10 - sin(angle) * 2;
    circlefill(tbmp, x, y, 3, 0);*/
    x = 10;// ;;+ cos(angle) * 1;
    y = 10;// + sin(angle) * 1;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 2;
    y = 100 + sin(angle) * 2;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 3;
    y = 100 + sin(angle) * 3;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 4;
    y = 100 + sin(angle) * 4;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 5;
    y = 100 + sin(angle) * 5;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 3, 0);
    x = 100 + cos(angle) * 7;
    y = 100 + sin(angle) * 7;
    circlefill(tbmp, x, y, 3, 0);
    blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);
    clear_bitmap(tbmp);
/*    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
    x = 100 + cos(angle) * 6;
    y = 100 + sin(angle) * 6;
    circlefill(tbmp, x, y, 8, 0);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 4, COLOUR_GREY4);
    masked_blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);*/
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 6:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle + PI) * 5;
    y1 = 100 + sin(angle + PI) * 5;
    x2 = 100 + cos(angle + PI / 2) * 9;
    y2 = 100 + sin(angle + PI / 2) * 9;
    x3 = 100 + cos(angle - PI / 2) * 9;
    y3 = 100 + sin(angle - PI / 2) * 9;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI + PI / 3) * 4;
    y2 = 100 + sin(angle + PI + PI / 3) * 4;
    x3 = 100 + cos(angle + PI - PI / 3) * 4;
    y3 = 100 + sin(angle + PI - PI / 3) * 4;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 3) * 3;
    y2 = 100 + sin(angle + PI + PI / 3) * 3;
    x3 = 100 + cos(angle + PI - PI / 3) * 3;
    y3 = 100 + sin(angle + PI - PI / 3) * 3;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI) * 4;
    y1 = 100 + sin(angle + PI) * 4;
    x2 = 100 + cos(angle + PI / 2) * 8;
    y2 = 100 + sin(angle + PI / 2) * 8;
    x3 = 100 + cos(angle - PI / 2) * 8;
    y3 = 100 + sin(angle - PI / 2) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);



    
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
   }
  break;
  case 7:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    x = 100 + cos(angle) * 3;
    y = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 2, COLOUR_GREY4);
    x1 = x + cos(angle + PI / 3) * 6;
    y1 = y + sin(angle + PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle - PI / 3) * 6;
    y1 = y + sin(angle - PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI + PI / 2) * 6;
    y1 = y + sin(angle + PI + PI / 2) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI - PI / 2) * 7;
    y1 = y + sin(angle + PI - PI / 2) * 7;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 3) * 6;
    y1 = 100 + sin(angle + PI + PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 3) * 6;
    y1 = 100 + sin(angle + PI - PI / 3) * 6;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 6) * 7;
    y1 = 100 + sin(angle + PI + PI / 6) * 7;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 5) * 7;
    y1 = 100 + sin(angle + PI - PI / 5) * 7;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x = 100 + cos(angle) * 3;
    y = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY5);
    
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 8:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI / 2) * 4;
    y2 = 100 + sin(angle + PI / 2) * 4;
    x3 = 100 + cos(angle - PI / 2) * 4;
    y3 = 100 + sin(angle - PI / 2) * 4;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI / 2) * 3;
    y2 = 100 + sin(angle + PI / 2) * 3;
    x3 = 100 + cos(angle - PI / 2) * 3;
    y3 = 100 + sin(angle - PI / 2) * 3;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI - PI / 2) * 8;
    y2 = 100 + sin(angle + PI - PI / 2) * 8;
    x3 = 100 + cos(angle + PI - PI / 4) * 8;
    y3 = 100 + sin(angle + PI - PI / 4) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 2) * 8;
    y2 = 100 + sin(angle + PI + PI / 2) * 8;
    x3 = 100 + cos(angle + PI + PI / 4) * 8;
    y3 = 100 + sin(angle + PI + PI / 4) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI - PI / 2) * 7;
    y2 = 100 + sin(angle + PI - PI / 2) * 7;
    x3 = 100 + cos(angle + PI - PI / 4) * 7;
    y3 = 100 + sin(angle + PI - PI / 4) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI + PI / 2) * 7;
    y2 = 100 + sin(angle + PI + PI / 2) * 7;
    x3 = 100 + cos(angle + PI + PI / 4) * 7;
    y3 = 100 + sin(angle + PI + PI / 4) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
   }
  break;
  case 10:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 7;
    y1 = 100 + sin(angle) * 7;
    x2 = 100 + cos(angle + PI / 2) * 4;
    y2 = 100 + sin(angle + PI / 2) * 4;
    x3 = 100 + cos(angle - PI / 2) * 4;
    y3 = 100 + sin(angle - PI / 2) * 4;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI / 2) * 3;
    y2 = 100 + sin(angle + PI / 2) * 3;
    x3 = 100 + cos(angle - PI / 2) * 3;
    y3 = 100 + sin(angle - PI / 2) * 3;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI - PI / 4) * 8;
    y2 = 100 + sin(angle + PI - PI / 4) * 8;
    x3 = 100 + cos(angle + PI - PI / 8) * 8;
    y3 = 100 + sin(angle + PI - PI / 8) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 6;
    y1 = 100 + sin(angle) * 6;
    x2 = 100 + cos(angle + PI + PI / 4) * 8;
    y2 = 100 + sin(angle + PI + PI / 4) * 8;
    x3 = 100 + cos(angle + PI + PI / 8) * 8;
    y3 = 100 + sin(angle + PI + PI / 8) * 8;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI - PI / 4) * 7;
    y2 = 100 + sin(angle + PI - PI / 4) * 7;
    x3 = 100 + cos(angle + PI - PI / 8) * 7;
    y3 = 100 + sin(angle + PI - PI / 8) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 5;
    y1 = 100 + sin(angle) * 5;
    x2 = 100 + cos(angle + PI + PI / 4) * 7;
    y2 = 100 + sin(angle + PI + PI / 4) * 7;
    x3 = 100 + cos(angle + PI + PI / 8) * 7;
    y3 = 100 + sin(angle + PI + PI / 8) * 7;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 3;
    y1 = 100 + sin(angle) * 3;
    circlefill(ship_bitmaps [at] [i], x1, y1, 1, COLOUR_WHITE);
   }
  break;
//  case 11:
   for (i = 0; i < 17; i ++)
   {
    tbmp = create_bitmap(210, 210);
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
/*
    points [0] = 100 + cos(angle + PI / 2) * 30;
    points [1] = 100 + sin(angle + PI / 2) * 30;
    points [2] = 100 + cos(angle + PI / 10) * 80;
    points [3] = 100 + sin(angle + PI / 10) * 80;
    points [4] = 100 + cos(angle + PI / 7) * 90;
    points [5] = 100 + sin(angle + PI / 7) * 90;
    points [6] = 100 + cos(angle + PI - PI / 3) * 50;
    points [7] = 100 + sin(angle + PI - PI / 3) * 50;
    points [8] = 100 + cos(angle + PI + PI / 3) * 50;
    points [9] = 100 + sin(angle + PI + PI / 3) * 50;
    points [10] = 100 + cos(angle - PI / 7) * 90;
    points [11] = 100 + sin(angle - PI / 7) * 90;
    points [12] = 100 + cos(angle - PI / 10) * 80;
    points [13] = 100 + sin(angle - PI / 10) * 80;
    points [14] = 100 + cos(angle - PI / 2) * 30;
    points [15] = 100 + sin(angle - PI / 2) * 30;
    polygon(ship_bitmaps [at] [i], 8, points, COLOUR_GREY4);
    
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 70;
    y2 = 100 + sin(angle + PI / 2) * 70;
    x3 = 100 + cos(angle - PI / 2) * 70;
    y3 = 100 + sin(angle - PI / 2) * 70;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI) * 30;
    y1 = 100 + sin(angle + PI) * 30;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle - PI / 2) * 60;
    y3 = 100 + sin(angle - PI / 2) * 60;
    triangle(ship_bitmaps [at] [i], x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    
    points [0] = 100 + cos(angle + PI / 2) * 20;
    points [1] = 100 + sin(angle + PI / 2) * 20;
    points [2] = 100 + cos(angle + PI / 8) * 70;
    points [3] = 100 + sin(angle + PI / 8) * 70;
    points [4] = 100 + cos(angle + PI / 5) * 80;
    points [5] = 100 + sin(angle + PI / 5) * 80;
    points [6] = 100 + cos(angle + PI - PI / 3) * 40;
    points [7] = 100 + sin(angle + PI - PI / 3) * 40;
    points [8] = 100 + cos(angle + PI + PI / 3) * 40;
    points [9] = 100 + sin(angle + PI + PI / 3) * 40;
    points [10] = 100 + cos(angle - PI / 5) * 80;
    points [11] = 100 + sin(angle - PI / 5) * 80;
    points [12] = 100 + cos(angle - PI / 8) * 70;
    points [13] = 100 + sin(angle - PI / 8) * 70;
    points [14] = 100 + cos(angle - PI / 2) * 20;
    points [15] = 100 + sin(angle - PI / 2) * 20;
    polygon(ship_bitmaps [at] [i], 8, points, COLOUR_GREY5);



    x1 = 10;// - cos(angle) * 2;
    y1 = 10;// - sin(angle) * 2;
    circlefill(ship_bitmaps [at] [i], x1, y1, 10, COLOUR_WHITE);*/
    points [0] = 100 + cos(angle + PI / 2) * 30;
    points [1] = 100 + sin(angle + PI / 2) * 30;
    points [2] = 100 + cos(angle + PI / 10) * 80;
    points [3] = 100 + sin(angle + PI / 10) * 80;
    points [4] = 100 + cos(angle + PI / 7) * 90;
    points [5] = 100 + sin(angle + PI / 7) * 90;
    points [6] = 100 + cos(angle + PI - PI / 3) * 50;
    points [7] = 100 + sin(angle + PI - PI / 3) * 50;
    points [8] = 100 + cos(angle + PI + PI / 3) * 50;
    points [9] = 100 + sin(angle + PI + PI / 3) * 50;
    points [10] = 100 + cos(angle - PI / 7) * 90;
    points [11] = 100 + sin(angle - PI / 7) * 90;
    points [12] = 100 + cos(angle - PI / 10) * 80;
    points [13] = 100 + sin(angle - PI / 10) * 80;
    points [14] = 100 + cos(angle - PI / 2) * 30;
    points [15] = 100 + sin(angle - PI / 2) * 30;
    polygon(tbmp, 8, points, COLOUR_GREY4);
    
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 70;
    y2 = 100 + sin(angle + PI / 2) * 70;
    x3 = 100 + cos(angle - PI / 2) * 70;
    y3 = 100 + sin(angle - PI / 2) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI) * 30;
    y1 = 100 + sin(angle + PI) * 30;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle - PI / 2) * 60;
    y3 = 100 + sin(angle - PI / 2) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    
    points [0] = 100 + cos(angle + PI / 2) * 20;
    points [1] = 100 + sin(angle + PI / 2) * 20;
    points [2] = 100 + cos(angle + PI / 8) * 70;
    points [3] = 100 + sin(angle + PI / 8) * 70;
    points [4] = 100 + cos(angle + PI / 5) * 80;
    points [5] = 100 + sin(angle + PI / 5) * 80;
    points [6] = 100 + cos(angle + PI - PI / 3) * 40;
    points [7] = 100 + sin(angle + PI - PI / 3) * 40;
    points [8] = 100 + cos(angle + PI + PI / 3) * 40;
    points [9] = 100 + sin(angle + PI + PI / 3) * 40;
    points [10] = 100 + cos(angle - PI / 5) * 80;
    points [11] = 100 + sin(angle - PI / 5) * 80;
    points [12] = 100 + cos(angle - PI / 8) * 70;
    points [13] = 100 + sin(angle - PI / 8) * 70;
    points [14] = 100 + cos(angle - PI / 2) * 20;
    points [15] = 100 + sin(angle - PI / 2) * 20;
    polygon(tbmp, 8, points, COLOUR_GREY5);



    x1 = 100;// - cos(angle) * 2;
    y1 = 100;// - sin(angle) * 2;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }

   
  break;
//  case 11:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    points [0] = 100 + cos(angle) * 2;
    points [1] = 100 + sin(angle) * 2;
    points [2] = 100 + cos(angle + PI / 2) * 9;
    points [3] = 100 + sin(angle + PI / 2) * 9;
    points [4] = 100 + cos(angle + PI) * 2;
    points [5] = 100 + sin(angle + PI) * 2;
    points [6] = 100 + cos(angle - PI / 2) * 7;
    points [7] = 100 + sin(angle - PI / 2) * 7;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY4);
    circle(ship_bitmaps [at] [i], 10, 10, 7, COLOUR_GREY5);
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
//    circle(ship_bitmaps [at] [i], x, y, 7, COLOUR_GREY4);
    circle(ship_bitmaps [at] [i], x, y, 8, COLOUR_GREY5);
    x = 100 + cos(angle) * 12;
    y = 100 + sin(angle) * 12;
    circlefill(ship_bitmaps [at] [i], x, y, 9, 0);
    x = 10 - cos(angle) * 12;
    y = 10 - sin(angle) * 12;
    circlefill(ship_bitmaps [at] [i], x, y, 8, 0);
    circlefill(ship_bitmaps [at] [i], 10, 10, 3, COLOUR_GREY4);
    
    points [0] = 100 + cos(angle) * 1;
    points [1] = 100 + sin(angle) * 1;
    points [2] = 100 + cos(angle + PI / 2) * 8;
    points [3] = 100 + sin(angle + PI / 2) * 8;
    points [4] = 100 + cos(angle + PI) * 1;
    points [5] = 100 + sin(angle + PI) * 1;
    points [6] = 100 + cos(angle - PI / 2) * 6;
    points [7] = 100 + sin(angle - PI / 2) * 6;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY5);
    
    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
//    circlefill(ship_bitmaps [at] [i], x, y, 3, COLOUR_GREY4);
    circlefill(ship_bitmaps [at] [i], x, y, 2, COLOUR_GREY5);

//    circlefill(ship_bitmaps [at] [i], 10, 10, 1, COLOUR_GREY6);

    x = 10;// - cos(angle) * 3;
    y = 10;// - sin(angle) * 3;
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
      }
       else
        arc(ship_bitmaps [at] [i], x, y, itofix(a1), itofix(a2), 1, COLOUR_WHITE);
     }
   }
   break;
  case 11:
   for (i = 0; i < 17; i ++)
   {
    tbmp = create_bitmap(210, 210);
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 4) * 80;
    y2 = 100 + sin(angle + PI - PI / 4) * 80;
    x3 = 100 + cos(angle + PI - PI / 8) * 80;
    y3 = 100 + sin(angle + PI - PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 4) * 80;
    y2 = 100 + sin(angle + PI + PI / 4) * 80;
    x3 = 100 + cos(angle + PI + PI / 8) * 80;
    y3 = 100 + sin(angle + PI + PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 4) * 70;
    y2 = 100 + sin(angle + PI - PI / 4) * 70;
    x3 = 100 + cos(angle + PI - PI / 8) * 70;
    y3 = 100 + sin(angle + PI - PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 4) * 70;
    y2 = 100 + sin(angle + PI + PI / 4) * 70;
    x3 = 100 + cos(angle + PI + PI / 8) * 70;
    y3 = 100 + sin(angle + PI + PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  
 }

 destroy_bitmap(tbmp);
}



void make_actor_sprites(int at)
{

 int x,y,x1, x2, x3, y1, y2, y3;
 int i;
 float angle;
 int a1, a2;//, a3;

 int points [16];

 BITMAP *tbmp = create_bitmap(210, 210);
 BITMAP *tbmp2;
 
 switch(at)
 {
  case 1:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI + PI / 3) * 60;
    y2 = 100 + sin(angle + PI + PI / 3) * 60;
    x3 = 100 + cos(angle + PI - PI / 3) * 60;
    y3 = 100 + sin(angle + PI - PI / 3) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 3) * 50;
    y2 = 100 + sin(angle + PI + PI / 3) * 50;
    x3 = 100 + cos(angle + PI - PI / 3) * 50;
    y3 = 100 + sin(angle + PI - PI / 3) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 2:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x2 = 100 + cos(angle + PI + PI / 3) * 50;
    y2 = 100 + sin(angle + PI + PI / 3) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 3) * (6 - (angle == 16));
    y2 = 100 + sin(angle + PI - PI / 3) * (6 - (angle == 16));
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x = 100;
    y = 100;
    circlefill(tbmp, x, y, 50, COLOUR_GREY4);
    circlefill(tbmp, x, y, 40, COLOUR_GREY5);
//    x1 = 100;// + cos(angle) * 20;
//    y1 = 100;// + sin(angle) * 20;
//    circlefill(tbmp, x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 19);
    a2 = 64 - (i * 4 - 19);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 30, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 30, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 30, COLOUR_WHITE);
    }
    
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   
   break;
  case 3:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    circlefill(tbmp, 100, 100, 20, COLOUR_GREY4);
    x2 = 100 + cos(angle + PI + PI / 4) * 50;
    y2 = 100 + sin(angle + PI + PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI + PI / 4) * 20;
    y2 = 100 + sin(angle + PI + PI / 4) * 20;
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 50;
    y2 = 100 + sin(angle + PI - PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI - PI / 4) * 20;
    y2 = 100 + sin(angle + PI - PI / 4) * 20;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 50;
    y2 = 100 + sin(angle + PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle + PI / 4) * 20;
    y2 = 100 + sin(angle + PI / 4) * 20;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 50;
    y2 = 100 + sin(angle - PI / 4) * 50;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY4);
    circlefill(tbmp, x2, y2, 10, COLOUR_GREY5);
    x2 = 100 + cos(angle - PI / 4) * 20;
    y2 = 100 + sin(angle - PI / 4) * 20;
    circlefill(tbmp, x2, y2, 20, COLOUR_GREY5);
    x = 100;
    y = 100;
    circlefill(tbmp, x, y, 10, COLOUR_GREY5);
//    x1 = 100;// + cos(angle) * 20;
//    y1 = 100;// + sin(angle) * 20;
//    circlefill(tbmp, x1, y1, 1, COLOUR_WHITE);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
//     a3 = a1;
//     a1 = a2;
//     a2 = a3;
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   break;
  case 4:
  tbmp2 = create_bitmap(210, 210);
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    clear_bitmap(tbmp2);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp);
    circlefill(tbmp2, 100, 100, 70, COLOUR_GREY4);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp2, x, y, 70, 0);
    blit(tbmp2, tbmp, 0,0,0,0,210,210);
    clear_bitmap(tbmp);
    circlefill(tbmp2, 100, 100, 60, COLOUR_GREY5);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp2, x, y, 80, 0);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp2, x, y, 30, COLOUR_GREY4);
    masked_blit(tbmp2, tbmp, 0,0,0,0,210,210);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp, x, y, 20, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   destroy_bitmap(tbmp2);
   break;
  case 5:
   tbmp2 = create_bitmap(210, 210);
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp2);
    circlefill(tbmp2, 100, 100, 70, COLOUR_GREY4);
    circlefill(tbmp2, 100, 100, 60, COLOUR_GREY5);
/*    x = 10 - cos(angle) * 40;
    y = 10 - sin(angle) * 40;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 30;
    y = 10 - sin(angle) * 30;
    circlefill(tbmp, x, y, 3, 0);
    x = 10 - cos(angle) * 20;
    y = 10 - sin(angle) * 20;
    circlefill(tbmp, x, y, 3, 0);*/
    x = 100;// ;;+ cos(angle) * 1;
    y = 100;// + sin(angle) * 1;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 20;
    y = 100 + sin(angle) * 20;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 30;
    y = 100 + sin(angle) * 30;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 40;
    y = 100 + sin(angle) * 40;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 50;
    y = 100 + sin(angle) * 50;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 + cos(angle) * 70;
    y = 100 + sin(angle) * 70;
    circlefill(tbmp2, x, y, 30, 0);
    blit(tbmp2, tbmp, 0,0,0,0,210,210);
    clear_bitmap(tbmp2);
/*    circlefill(tbmp, 10, 10, 6, COLOUR_GREY5);
    x = 100 + cos(angle) * 60;
    y = 100 + sin(angle) * 60;
    circlefill(tbmp, x, y, 8, 0);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp, x, y, 4, COLOUR_GREY4);
    masked_blit(tbmp, ship_bitmaps [at] [i], 0,0,0,0,21,21);*/
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    circlefill(tbmp, x, y, 30, COLOUR_GREY5);
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   destroy_bitmap(tbmp2);
   break;
  case 6:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle + PI) * 50;
    y1 = 100 + sin(angle + PI) * 50;
    x2 = 100 + cos(angle + PI / 2) * 90;
    y2 = 100 + sin(angle + PI / 2) * 90;
    x3 = 100 + cos(angle - PI / 2) * 90;
    y3 = 100 + sin(angle - PI / 2) * 90;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI + PI / 3) * 40;
    y2 = 100 + sin(angle + PI + PI / 3) * 40;
    x3 = 100 + cos(angle + PI - PI / 3) * 40;
    y3 = 100 + sin(angle + PI - PI / 3) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 3) * 30;
    y2 = 100 + sin(angle + PI + PI / 3) * 30;
    x3 = 100 + cos(angle + PI - PI / 3) * 30;
    y3 = 100 + sin(angle + PI - PI / 3) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 80;
    y2 = 100 + sin(angle + PI / 2) * 80;
    x3 = 100 + cos(angle - PI / 2) * 80;
    y3 = 100 + sin(angle - PI / 2) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);



    
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
//  line(ship_bitmaps [at] [i], x1, y1, x2, y2, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x2, y2, x3, y3, COLOUR_GREY4);
//  line(ship_bitmaps [at] [i], x1, y1, x3, y3, COLOUR_GREY4);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 7:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    x = 100 + cos(angle) * 30;
    y = 100 + sin(angle) * 30;
    circlefill(tbmp, x, y, 20, COLOUR_GREY4);
    x1 = x + cos(angle + PI / 3) * 60;
    y1 = y + sin(angle + PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle - PI / 3) * 60;
    y1 = y + sin(angle - PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI + PI / 2) * 60;
    y1 = y + sin(angle + PI + PI / 2) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = x + cos(angle + PI - PI / 2) * 70;
    y1 = y + sin(angle + PI - PI / 2) * 70;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 3) * 60;
    y1 = 100 + sin(angle + PI + PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 3) * 60;
    y1 = 100 + sin(angle + PI - PI / 3) * 60;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI + PI / 6) * 70;
    y1 = 100 + sin(angle + PI + PI / 6) * 70;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x1 = 100 + cos(angle + PI - PI / 5) * 70;
    y1 = 100 + sin(angle + PI - PI / 5) * 70;
    line(ship_bitmaps [at] [i], x, y, x1, y1, COLOUR_GREY4);
    putpixel(ship_bitmaps [at] [i], x1, y1, COLOUR_GREY6);
    x = 100 + cos(angle) * 30;
    y = 100 + sin(angle) * 30;
    circlefill(tbmp, x, y, 30, COLOUR_GREY5);
    
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   break;
  case 8:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 50;
    y2 = 100 + sin(angle + PI / 2) * 50;
    x3 = 100 + cos(angle - PI / 2) * 50;
    y3 = 100 + sin(angle - PI / 2) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 2) * 80;
    y2 = 100 + sin(angle + PI - PI / 2) * 80;
    x3 = 100 + cos(angle + PI - PI / 4) * 80;
    y3 = 100 + sin(angle + PI - PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 2) * 80;
    y2 = 100 + sin(angle + PI + PI / 2) * 80;
    x3 = 100 + cos(angle + PI + PI / 4) * 80;
    y3 = 100 + sin(angle + PI + PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 2) * 70;
    y2 = 100 + sin(angle + PI - PI / 2) * 70;
    x3 = 100 + cos(angle + PI - PI / 4) * 70;
    y3 = 100 + sin(angle + PI - PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 2) * 70;
    y2 = 100 + sin(angle + PI + PI / 2) * 70;
    x3 = 100 + cos(angle + PI + PI / 4) * 70;
    y3 = 100 + sin(angle + PI + PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
/*    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 2) * 80;
    y2 = 100 + sin(angle + PI - PI / 2) * 80;
    x3 = 100 + cos(angle + PI - PI / 4) * 80;
    y3 = 100 + sin(angle + PI - PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 2) * 80;
    y2 = 100 + sin(angle + PI + PI / 2) * 80;
    x3 = 100 + cos(angle + PI + PI / 4) * 80;
    y3 = 100 + sin(angle + PI + PI / 4) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 2) * 70;
    y2 = 100 + sin(angle + PI - PI / 2) * 70;
    x3 = 100 + cos(angle + PI - PI / 4) * 70;
    y3 = 100 + sin(angle + PI - PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 2) * 70;
    y2 = 100 + sin(angle + PI + PI / 2) * 70;
    x3 = 100 + cos(angle + PI + PI / 4) * 70;
    y3 = 100 + sin(angle + PI + PI / 4) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);*/
   }
  break;
  case 9:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 20;
    y2 = 100 + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2) * 20;
    y3 = 100 + sin(angle - PI / 2) * 20;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 4) * 80;
    y2 = 100 + sin(angle + PI - PI / 4) * 80;
    x3 = 100 + cos(angle + PI - PI / 8) * 80;
    y3 = 100 + sin(angle + PI - PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 4) * 80;
    y2 = 100 + sin(angle + PI + PI / 4) * 80;
    x3 = 100 + cos(angle + PI + PI / 8) * 80;
    y3 = 100 + sin(angle + PI + PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 4) * 70;
    y2 = 100 + sin(angle + PI - PI / 4) * 70;
    x3 = 100 + cos(angle + PI - PI / 8) * 70;
    y3 = 100 + sin(angle + PI - PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 4) * 70;
    y2 = 100 + sin(angle + PI + PI / 4) * 70;
    x3 = 100 + cos(angle + PI + PI / 8) * 70;
    y3 = 100 + sin(angle + PI + PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 11:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
//    tbmp = create_bitmap(210, 210);
//    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    points [0] = 100 + cos(angle + PI / 2) * 30;
    points [1] = 100 + sin(angle + PI / 2) * 30;
    points [2] = 100 + cos(angle + PI / 10) * 80;
    points [3] = 100 + sin(angle + PI / 10) * 80;
    points [4] = 100 + cos(angle + PI / 7) * 90;
    points [5] = 100 + sin(angle + PI / 7) * 90;
    points [6] = 100 + cos(angle + PI - PI / 3) * 50;
    points [7] = 100 + sin(angle + PI - PI / 3) * 50;
    points [8] = 100 + cos(angle + PI + PI / 3) * 50;
    points [9] = 100 + sin(angle + PI + PI / 3) * 50;
    points [10] = 100 + cos(angle - PI / 7) * 90;
    points [11] = 100 + sin(angle - PI / 7) * 90;
    points [12] = 100 + cos(angle - PI / 10) * 80;
    points [13] = 100 + sin(angle - PI / 10) * 80;
    points [14] = 100 + cos(angle - PI / 2) * 30;
    points [15] = 100 + sin(angle - PI / 2) * 30;
    polygon(tbmp, 8, points, COLOUR_GREY4);
    
    x1 = 100 + cos(angle + PI) * 40;
    y1 = 100 + sin(angle + PI) * 40;
    x2 = 100 + cos(angle + PI / 2) * 70;
    y2 = 100 + sin(angle + PI / 2) * 70;
    x3 = 100 + cos(angle - PI / 2) * 70;
    y3 = 100 + sin(angle - PI / 2) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI) * 30;
    y1 = 100 + sin(angle + PI) * 30;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle - PI / 2) * 60;
    y3 = 100 + sin(angle - PI / 2) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    
    points [0] = 100 + cos(angle + PI / 2) * 20;
    points [1] = 100 + sin(angle + PI / 2) * 20;
    points [2] = 100 + cos(angle + PI / 8) * 70;
    points [3] = 100 + sin(angle + PI / 8) * 70;
    points [4] = 100 + cos(angle + PI / 5) * 80;
    points [5] = 100 + sin(angle + PI / 5) * 80;
    points [6] = 100 + cos(angle + PI - PI / 3) * 40;
    points [7] = 100 + sin(angle + PI - PI / 3) * 40;
    points [8] = 100 + cos(angle + PI + PI / 3) * 40;
    points [9] = 100 + sin(angle + PI + PI / 3) * 40;
    points [10] = 100 + cos(angle - PI / 5) * 80;
    points [11] = 100 + sin(angle - PI / 5) * 80;
    points [12] = 100 + cos(angle - PI / 8) * 70;
    points [13] = 100 + sin(angle - PI / 8) * 70;
    points [14] = 100 + cos(angle - PI / 2) * 20;
    points [15] = 100 + sin(angle - PI / 2) * 20;
    polygon(tbmp, 8, points, COLOUR_GREY5);



    x1 = 100;// - cos(angle) * 20;
    y1 = 100;// - sin(angle) * 20;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }

   
  break;
/*  case 12:
   for (i = 0; i < 17; i ++)
   {
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    points [0] = 100 + cos(angle) * 20;
    points [1] = 100 + sin(angle) * 20;
    points [2] = 100 + cos(angle + PI / 2) * 90;
    points [3] = 100 + sin(angle + PI / 2) * 90;
    points [4] = 100 + cos(angle + PI) * 20;
    points [5] = 100 + sin(angle + PI) * 20;
    points [6] = 100 + cos(angle - PI / 2) * 70;
    points [7] = 100 + sin(angle - PI / 2) * 70;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY4);
    circle(tbmp, 100, 100, 70, COLOUR_GREY5);
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
//    circle(tbmp, x, y, 7, COLOUR_GREY4);
    circle(tbmp, x, y, 80, COLOUR_GREY5);
    x = 100 + cos(angle) * 12;
    y = 100 + sin(angle) * 12;
    circlefill(tbmp, x, y, 90, 0);
    x = 10 - cos(angle) * 12;
    y = 10 - sin(angle) * 12;
    circlefill(tbmp, x, y, 80, 0);
    circlefill(tbmp, 100, 100, 30, COLOUR_GREY4);

    points [0] = 100 + cos(angle) * 1;
    points [1] = 100 + sin(angle) * 1;
    points [2] = 100 + cos(angle + PI / 2) * 80;
    points [3] = 100 + sin(angle + PI / 2) * 80;
    points [4] = 100 + cos(angle + PI) * 1;
    points [5] = 100 + sin(angle + PI) * 1;
    points [6] = 100 + cos(angle - PI / 2) * 60;
    points [7] = 100 + sin(angle - PI / 2) * 60;
    polygon(ship_bitmaps [at] [i], 4, points, COLOUR_GREY5);
    
    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
//    circlefill(tbmp, x, y, 3, COLOUR_GREY4);
    circlefill(tbmp, x, y, 20, COLOUR_GREY5);

//    circlefill(tbmp, 10, 10, 1, COLOUR_GREY6);

    x = 100;// - cos(angle) * 30;
    y = 100;// - sin(angle) * 30;
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
      }
       else
        arc(tbmp, x, y, itofix(a1), itofix(a2), 10, COLOUR_WHITE);
     }
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   break;*/
/*  case 13:
   for (i = 0; i < 17; i ++)
   {
//.    tbmp = create_bitmap(210, 210);
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI - PI / 4) * 80;
    y2 = 100 + sin(angle + PI - PI / 4) * 80;
    x3 = 100 + cos(angle + PI - PI / 8) * 80;
    y3 = 100 + sin(angle + PI - PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 60;
    y1 = 100 + sin(angle) * 60;
    x2 = 100 + cos(angle + PI + PI / 4) * 80;
    y2 = 100 + sin(angle + PI + PI / 4) * 80;
    x3 = 100 + cos(angle + PI + PI / 8) * 80;
    y3 = 100 + sin(angle + PI + PI / 8) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI - PI / 4) * 70;
    y2 = 100 + sin(angle + PI - PI / 4) * 70;
    x3 = 100 + cos(angle + PI - PI / 8) * 70;
    y3 = 100 + sin(angle + PI - PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 50;
    y1 = 100 + sin(angle) * 50;
    x2 = 100 + cos(angle + PI + PI / 4) * 70;
    y2 = 100 + sin(angle + PI + PI / 4) * 70;
    x3 = 100 + cos(angle + PI + PI / 8) * 70;
    y3 = 100 + sin(angle + PI + PI / 8) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 30;
    y1 = 100 + sin(angle) * 30;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;*/
  case 12:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 90;
    y1 = 100 + sin(angle) * 90;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI - PI / 3) * 90;
    y1 = 100 + sin(angle + PI - PI / 3) * 90;
    x2 = 100 + cos(angle + PI / 4) * 40;
    y2 = 100 + sin(angle + PI / 4) * 40;
    x3 = 100 + cos(angle + PI) * 20;
    y3 = 100 + sin(angle + PI) * 20;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI + PI / 3) * 90;
    y1 = 100 + sin(angle + PI + PI / 3) * 90;
    x2 = 100 + cos(angle - PI / 4) * 40;
    y2 = 100 + sin(angle - PI / 4) * 40;
    x3 = 100 + cos(angle + PI) * 20;
    y3 = 100 + sin(angle + PI) * 20;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100;
    y1 = 100;
    circlefill(tbmp, x1, y1, 30, COLOUR_GREY4);

    x1 = 100 + cos(angle) * 80;
    y1 = 100 + sin(angle) * 80;
    x2 = 100 + cos(angle + PI / 2) * 30;
    y2 = 100 + sin(angle + PI / 2) * 30;
    x3 = 100 + cos(angle - PI / 2) * 30;
    y3 = 100 + sin(angle - PI / 2) * 30;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI - PI / 3) * 80;
    y1 = 100 + sin(angle + PI - PI / 3) * 80;
    x2 = 100 + cos(angle + PI / 4) * 30;
    y2 = 100 + sin(angle + PI / 4) * 30;
    x3 = 100 + cos(angle + PI) * 10;
    y3 = 100 + sin(angle + PI) * 10;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI + PI / 3) * 80;
    y1 = 100 + sin(angle + PI + PI / 3) * 80;
    x2 = 100 + cos(angle - PI / 4) * 30;
    y2 = 100 + sin(angle - PI / 4) * 30;
    x3 = 100 + cos(angle + PI) * 10;
    y3 = 100 + sin(angle + PI) * 10;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100;
    y1 = 100;
    circlefill(tbmp, x1, y1, 20, COLOUR_GREY5);
/*    x1 = 100 + cos(angle + PI / 2) * 80;
    y1 = 100 + sin(angle + PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 80;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle - PI / 2) * 80;
    y1 = 100 + sin(angle - PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 80;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 35;
    y2 = 100 + sin(angle + PI / 2) * 35;
    x3 = 100 + cos(angle - PI / 2) * 35;
    y3 = 100 + sin(angle - PI / 2) * 35;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI / 2) * 70;
    y1 = 100 + sin(angle + PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 70;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle - PI / 2) * 70;
    y1 = 100 + sin(angle - PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 70;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);*/
    x1 = 100 + cos(angle) * 40;
    y1 = 100 + sin(angle) * 40;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 13:
   tbmp2 = create_bitmap(210, 210);
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    if (i == 0)
     angle = 1 / (16 / (PI / 2)) - PI / 2;;
    clear_bitmap(tbmp2);
    x1 = 100 - cos(angle) * 20;
    y1 = 100 - sin(angle) * 20;
    circlefill(tbmp2, x1, y1, 80, COLOUR_GREY4);
    circlefill(tbmp2, x1, y1, 70, COLOUR_GREY5);
//    x = 100;// ;;+ cos(angle) * 1;
//    y = 100;// + sin(angle) * 1;
//    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 70;
    y = 100 - sin(angle) * 70;
    circlefill(tbmp2, x, y, 75, 0);
/*    x = 100 - cos(angle) * 30;
    y = 100 - sin(angle) * 30;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 40;
    y = 100 - sin(angle) * 40;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 50;
    y = 100 - sin(angle) * 50;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 60;
    y = 100 - sin(angle) * 60;
    circlefill(tbmp2, x, y, 30, 0);
    x = 100 - cos(angle) * 70;
    y = 100 - sin(angle) * 70;
    circlefill(tbmp2, x, y, 30, 0);*/
    blit(tbmp2, tbmp, 0,0,0,0,210,210);
    clear_bitmap(tbmp2);
    x = x1 + cos(angle + PI / 4 + PI / 6) * 40;
    y = y1 + sin(angle + PI / 4 + PI / 6) * 40;
    circlefill(tbmp, x, y, 20, COLOUR_GREY4);
    circlefill(tbmp, x, y, 10, COLOUR_GREY5);
    x = x1 + cos(angle - PI / 4 - PI / 6) * 40;
    y = y1 + sin(angle - PI / 4 - PI / 6) * 40;
    circlefill(tbmp, x, y, 20, COLOUR_GREY4);
    circlefill(tbmp, x, y, 10, COLOUR_GREY5);
    x = 100 - cos(angle) * 20;
    y = 100 - sin(angle) * 20;
    a1 = 64 - (i * 4 + 12);
    a2 = 64 - (i * 4 - 12);
    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
    }
       arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 45, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 44, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 43, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 42, COLOUR_WHITE);
/*    if (i == 12)
    {
       a1 = 0 + 256;//64 - (i * 4 + 19);
       a2 = 35 + 256; //64 - (i * 4 - 19);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
    } else
     {
      if (i > 11)
      {
       arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
       arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
      }
       else
       {
        arc(tbmp, x, y, itofix(a1), itofix(a2), 50, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 49, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 48, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 47, COLOUR_WHITE);
        arc(tbmp, x, y, itofix(a1), itofix(a2), 46, COLOUR_WHITE);
       }
     }*/
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
   destroy_bitmap(tbmp2);
   break;
  case 14:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle) * 80;
    y1 = 100 + sin(angle) * 80;
    x2 = 100 + cos(angle + PI / 2) * 40;
    y2 = 100 + sin(angle + PI / 2) * 40;
    x3 = 100 + cos(angle - PI / 2) * 40;
    y3 = 100 + sin(angle - PI / 2) * 40;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI / 2) * 80;
    y1 = 100 + sin(angle + PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 80;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle - PI / 2) * 80;
    y1 = 100 + sin(angle - PI / 2) * 80;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 80;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 80;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle) * 70;
    y1 = 100 + sin(angle) * 70;
    x2 = 100 + cos(angle + PI / 2) * 35;
    y2 = 100 + sin(angle + PI / 2) * 35;
    x3 = 100 + cos(angle - PI / 2) * 35;
    y3 = 100 + sin(angle - PI / 2) * 35;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle + PI / 2) * 70;
    y1 = 100 + sin(angle + PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle + PI / 2 + PI / 5) * 70;
    y3 = 100 + sin(angle + PI / 2 + PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle - PI / 2) * 70;
    y1 = 100 + sin(angle - PI / 2) * 70;
    x2 = 100;// + cos(angle + PI / 2) * 20;
    y2 = 100;// + sin(angle + PI / 2) * 20;
    x3 = 100 + cos(angle - PI / 2 - PI / 5) * 70;
    y3 = 100 + sin(angle - PI / 2 - PI / 5) * 70;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle) * 40;
    y1 = 100 + sin(angle) * 40;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;
  case 15:
   for (i = 0; i < 17; i ++)
   {
    clear_bitmap(tbmp);
    ship_bitmaps [at] [i] = create_bitmap(21, 21);
    clear_bitmap(ship_bitmaps [at] [i]);
    angle = i / (16 / (PI / 2)) - PI / 2;
    if (i == 16)
     angle = 0;
    x1 = 100 + cos(angle + PI / 26) * 90;
    y1 = 100 + sin(angle + PI / 26) * 90;
    x2 = 100 + cos(angle + PI / 2) * 60;
    y2 = 100 + sin(angle + PI / 2) * 60;
    x3 = 100 + cos(angle + PI - PI / 26) * 60;
    y3 = 100 + sin(angle + PI - PI / 26) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
    x1 = 100 + cos(angle - PI / 26) * 90;
    y1 = 100 + sin(angle - PI / 26) * 90;
    x2 = 100 + cos(angle - PI / 2) * 60;
    y2 = 100 + sin(angle - PI / 2) * 60;
    x3 = 100 + cos(angle + PI + PI / 26) * 60;
    y3 = 100 + sin(angle + PI + PI / 26) * 60;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY4);
/*    x1 = 100 + cos(angle + PI - PI / 4) * 50;
    y1 = 100 + sin(angle + PI - PI / 4) * 50;
    circlefill(tbmp, x1, y1, 35, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI + PI / 4) * 50;
    y1 = 100 + sin(angle + PI + PI / 4) * 50;
    circlefill(tbmp, x1, y1, 35, COLOUR_GREY4);*/
    x1 = 100 - cos(angle) * 30;
    y1 = 100 - sin(angle) * 30;
    circlefill(tbmp, x1, y1, 40, COLOUR_GREY4);
    x1 = 100 + cos(angle + PI / 26) * 80;
    y1 = 100 + sin(angle + PI / 26) * 80;
    x2 = 100 + cos(angle + PI / 2) * 50;
    y2 = 100 + sin(angle + PI / 2) * 50;
    x3 = 100 + cos(angle + PI - PI / 26) * 50;
    y3 = 100 + sin(angle + PI - PI / 26) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 + cos(angle - PI / 26) * 80;
    y1 = 100 + sin(angle - PI / 26) * 80;
    x2 = 100 + cos(angle - PI / 2) * 50;
    y2 = 100 + sin(angle - PI / 2) * 50;
    x3 = 100 + cos(angle + PI + PI / 26) * 50;
    y3 = 100 + sin(angle + PI + PI / 26) * 50;
    triangle(tbmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
    x1 = 100 - cos(angle) * 30;
    y1 = 100 - sin(angle) * 30;
    circlefill(tbmp, x1, y1, 30, COLOUR_GREY5);
    x1 = 100 - cos(angle) * 20;
    y1 = 100 - sin(angle) * 20;
    circlefill(tbmp, x1, y1, 10, COLOUR_WHITE);
    stretch_sprite(ship_bitmaps [at] [i], tbmp, 0, 0, 21, 21);
   }
  break;


  
 }

 destroy_bitmap(tbmp);
}

#endif
