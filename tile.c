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
    
File: stuff.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - generation of the tiled background

*/

#include "config.h"

#include "allegro.h"

#include <math.h>
#include <string.h>

//#include <stdlib.h>

#include "globvars.h"
#include "palette.h"
#include "cloud.h"

#include "display.h"

#include "stuff.h"

#define ROTATE_90 64
#define ROTATE_180 128
#define ROTATE_270 192


#define CONNECT_U 1
#define CONNECT_D 2
#define CONNECT_L 4
#define CONNECT_R 8


enum 
{
MAZE_NOTHING,
MAZE_UD,
MAZE_UL,
MAZE_UR,
MAZE_DL,
MAZE_DR,
MAZE_LR,
MAZE_ULR,
MAZE_DLR,
MAZE_UDL,
MAZE_UDR,
MAZE_UDLR,
MAZE_U,
MAZE_D,
MAZE_L,
MAZE_R,
MAZE_OUTER_U,
MAZE_OUTER_D,
MAZE_OUTER_L,
MAZE_OUTER_R,
MAZE_OUTER_UL,
MAZE_OUTER_DL,
MAZE_OUTER_UR,
MAZE_OUTER_DR,
MAZE_OUTER2,
MAZE_REMOTE_NODE,
MAZE_EYE_CLOSED_NODE,
MAZE_EYE_OPEN_NODE,
MAZE_EYE_CLOSED_REMOTE_NODE,
MAZE_EYE_OPEN_REMOTE_NODE
}; 


extern FONT *small_font;
extern struct optionstruct options;

//BITMAP *make_bmp(int x, int y);
BITMAP *make_bmp(int x, int y, const char errtxt []);
void make_maze_map(int x, int y);
RLE_SPRITE *make_tile(int index, int index2, int colourise1, int colourise2, int rotation);
RLE_SPRITE *make_m_tile(int index, int tmaze, int index2, int colourise1, int colourise2, int rotation);
RLE_SPRITE *make_complex_tile(int index, int index2, int tmaze, int colourise1, int colourise2, int colourise2_1, int colourise2_2, int rotation);
void draw_maze_1(BITMAP *level_bmp, int x, int y);
void draw_maze_2(BITMAP *level_bmp, int x, int y);

void put_eyes_in_maze(BITMAP *level_bmp, int x, int y);
void find_place_for_eye(int x_min, int y_min, int x_max, int y_max, int eye);
int draw_eye_path(BITMAP *level_bmp, int x, int y, int ex, int ey, int xd, int yd, int draw);

// these eye functions are obsolete:
void place_eyes(BITMAP *level_bmp, int x, int y);
void put_eye(int x1, int y1, int x2, int y2, int k, int node_x [100], int node_y [100]);
void put_eye2(int x, int y);
int eye_okay(int i, int j);


void make_tile_background_rle(void);

void tile_progress_update(const char progtxt []);

extern BITMAP *level_bmp;

extern RLE_SPRITE *tile_background;
extern int scr_x; // so we know how big to make tile_background
extern int scr_y;

extern BITMAP *pretile_bmp [NO_PRETILE_BMPS];
extern BITMAP *pretile_m_bmp [NO_MAZES] [NO_PRETILE_M_BMPS];
extern RLE_SPRITE *tile_rle [NO_TILE_RLES];
extern RLE_SPRITE *menu_tile;

int next_eye;

void init_tiles(void)
{
 int i;
 for (i = 0; i < NO_TILE_RLES; i ++)
 {
  tile_rle [i] = NULL;
 }

 menu_tile = make_tile(0, -1, CONVERT_GREEN1 + grand(5) * 8, CONVERT_GREEN2 + grand(5) * 8, 0);

}

void make_tile_map(int x, int y)
{

 int i;

// Must remember to make sure that an empty tile_rle always is NULL! 
//  This should be done in init_tiles at first, then here during preparation for each level.
 for (i = 0; i < NO_TILE_RLES; i ++)
 {
  if (tile_rle [i] != NULL)
  {
   destroy_rle_sprite(tile_rle [i]);
   tile_rle [i] = NULL;
  } 
 }
 
// BITMAP *temp_bitmap = make_bmp(50, 50, "temp_bitmap (make_tile_map)");

 make_maze_map(x, y);

 make_tile_background_rle();

}



void make_maze_map(int x, int y)
{
 int col1 = CONVERT_GREEN1 + grand(5) * 8;
 int col2 = CONVERT_GREEN2 + grand(5) * 8;
 int bcol1 = CONVERT_GREEN1;
 int bcol2 = CONVERT_GREEN1;

 int hcola1, hcola2, hcolb1, hcolb2, adjusted_level;

  col1 = CONVERT_RED1;
  col2 = CONVERT_RED2;
  hcola1 = CONVERT_BLUE1;
  hcola2 = CONVERT_BLUE2;
  hcolb1 = CONVERT_BLUE1;
  hcolb2 = CONVERT_BLUE2;
  adjusted_level = arena.level - 10;


 if (arena.level <= 10)
 {
  col1 = CONVERT_YELLOW1;
  col2 = CONVERT_YELLOW2;
  hcola1 = CONVERT_ORANGE1;
  hcola2 = CONVERT_ORANGE2;
  hcolb1 = CONVERT_ORANGE1;
  hcolb2 = CONVERT_ORANGE2;
  adjusted_level = arena.level - 5;
 }

 if (arena.level <= 5)
 {
  col1 = CONVERT_GREEN1;
  col2 = CONVERT_GREEN2;
  hcola1 = CONVERT_YELLOW1;
  hcola2 = CONVERT_YELLOW2;
  hcolb1 = CONVERT_ORANGE1;
  hcolb2 = CONVERT_ORANGE2;
  adjusted_level = arena.level;
 }


  if (grand(5) + 1 < adjusted_level)
    col1 = hcola1;
   if (grand(5) + 1 < adjusted_level)
    col2 = hcola2;
   if (grand(10) + 2 < adjusted_level)
    col1 = hcolb1;
   if (grand(10) + 2 < adjusted_level)
    col2 = hcolb2;

    bcol1 = col1;
   if (grand(5) + 1 < adjusted_level)
    bcol1 = hcola1;
   if (grand(10) + 2 < adjusted_level)
    bcol1 = hcolb1;

    bcol2 = col2;
   if (grand(5) + 1 < adjusted_level)
    bcol2 = hcola2;
   if (grand(10) + 2 < adjusted_level)
    bcol2 = hcolb2;

  if (grand(3) == 0)
  {
   switch(col1)
   {
    case CONVERT_GREEN1: bcol1 = CONVERT_GREEN1; break;
    case CONVERT_YELLOW1: bcol1 = CONVERT_YELLOW1; break;
    case CONVERT_ORANGE1: bcol1 = CONVERT_ORANGE1; break;
    case CONVERT_RED1: bcol1 = CONVERT_RED1; break;
    case CONVERT_BLUE1: bcol1 = CONVERT_BLUE1; break;
   }
  }

 if (grand(50) == 0)
 {
  bcol1 = CONVERT_GREEN1 + grand(5) * 8;
  bcol2 = CONVERT_GREEN2 + grand(5) * 8;
  col1 = CONVERT_GREEN1 + grand(5) * 8;
  col2 = CONVERT_GREEN2 + grand(5) * 8;
 }

 if (arena.level == 1)
 {
  col1 = CONVERT_GREEN1;
  col2 = CONVERT_GREEN2;
  bcol1 = CONVERT_GREEN1;
  bcol2 = CONVERT_GREEN2;
 }

 
/*
 if (arena.level == 1)
 {
  col1 = CONVERT_ORANGE1;
  col2 = CONVERT_ORANGE2;
  bcol = CONVERT_ORANGE1;
 }

 if (arena.level == 2)
 {
  col1 = CONVERT_RED1;
  col2 = CONVERT_RED2;
  bcol = CONVERT_RED1;
 }
*/
// tile_progress_update("making");
// int base_index = PRETILE_STRAIGHT_UD;
// if (grand(3) == 0)
//  base_index = PRETILE_ZIG_UD;
// tile_rle [MAZE_NOTHING] = make_tile(0, col1, col2, 0);
// if (grand(3) == 0)
//  tile_rle [MAZE_NOTHING] = make_tile(PRETILE_BG_CHECK, -1, bcol, 0, 0);
//   else
//    tile_rle [MAZE_NOTHING] = make_tile(PRETILE_BG_DIAMOND + grand(5), -1, bcol, 0, 0);

 int back_tile = PRETILE_BG_CHECK;
 if (arena.level > 1)
 {
  switch(grand(5))
  {
   case 0: back_tile = PRETILE_BG_CHECK; break;
   case 1: back_tile = PRETILE_BG_DIAMOND; break;
   case 2: back_tile = PRETILE_BG_MESH1; break;
   case 3: back_tile = PRETILE_BG_MESH2; break;
   case 4: back_tile = PRETILE_BG_GRID; break;
  }
 }
 if (arena.level > 5 && grand(2) == 0)
 {
  switch(grand(8))
  {
   case 0: back_tile = PRETILE_BG_FLOWERS; break;
   case 1: back_tile = PRETILE_BG_SQUARES; break;
   case 2: back_tile = PRETILE_BG_DIAMOND2; break;
   case 3: back_tile = PRETILE_BG_CROSS; break;
   case 4: back_tile = PRETILE_BG_SQUARES2; break;
   case 5: back_tile = PRETILE_BG_CIRCLES; break;
   case 6: back_tile = PRETILE_BG_THING; break;
   case 7: back_tile = PRETILE_BG_PATTERN; break;
  }
 }
 
 tile_rle [MAZE_NOTHING] = make_tile(back_tile, -1, bcol1, bcol2, 0);

 int tmaze;

 if (arena.level <= 4)
 {
  switch(grand(4))
  {
   case 0: tmaze = MAZE_STRAIGHT; break;
   case 1: tmaze = MAZE_ZIGZAG; break;
   case 2: tmaze = MAZE_CIRCLES; break;
   case 3: tmaze = MAZE_TUBES; break;
  }
 }
  else
  {
   tmaze = grand(NO_MAZES);
  }


// screenshot
//tmaze = MAZE_RINGS;

 tile_rle [MAZE_UD] = make_m_tile(PRETILE_M_UD, tmaze, -1, col1, col2, 0);
 tile_rle [MAZE_UL] = make_m_tile(PRETILE_M_UR, tmaze, -1, col1, col2, ROTATE_270);
 tile_rle [MAZE_UR] = make_m_tile(PRETILE_M_UR, tmaze, -1, col1, col2, 0);
 tile_rle [MAZE_DL] = make_m_tile(PRETILE_M_UR, tmaze, -1, col1, col2, ROTATE_180);
 tile_rle [MAZE_DR] = make_m_tile(PRETILE_M_UR, tmaze, -1, col1, col2, ROTATE_90);
 tile_rle [MAZE_LR] = make_m_tile(PRETILE_M_LR, tmaze, -1, col1, col2, 0); // index + 1 is redundant. Too late...
 tile_rle [MAZE_ULR] = make_m_tile(PRETILE_M_ULR, tmaze, -1, col1, col2, 0);
 tile_rle [MAZE_DLR] = make_m_tile(PRETILE_M_ULR, tmaze, -1, col1, col2, ROTATE_180);
 tile_rle [MAZE_UDL] = make_m_tile(PRETILE_M_ULR, tmaze, -1, col1, col2, ROTATE_270);
 tile_rle [MAZE_UDR] = make_m_tile(PRETILE_M_ULR, tmaze, -1, col1, col2, ROTATE_90);
 tile_rle [MAZE_UDLR] = make_m_tile(PRETILE_M_UDLR, tmaze, -1, col1, col2, 0);
 tile_rle [MAZE_U] = make_m_tile(PRETILE_M_U, tmaze, -1, col1, col2, 0);
 tile_rle [MAZE_D] = make_m_tile(PRETILE_M_U, tmaze, -1, col1, col2, ROTATE_180);
 tile_rle [MAZE_L] = make_m_tile(PRETILE_M_U, tmaze, -1, col1, col2, ROTATE_270);
 tile_rle [MAZE_R] = make_m_tile(PRETILE_M_U, tmaze, -1, col1, col2, ROTATE_90);
 
int outer_base = PRETILE_OUTER1_D;
int base_choose = grand(3);
if (arena.level > 5)
 base_choose = grand(6);
 if (grand(2) == 0 && arena.level > 1)
 {
  switch(base_choose)
  {
   case 0: outer_base = PRETILE_OUTER1_D; break;
   case 1: outer_base = PRETILE_OUTER2_D; break;
   case 2: outer_base = PRETILE_OUTER3_D; break;
   case 3: outer_base = PRETILE_OUTER4_D; break;
   case 4: outer_base = PRETILE_OUTER5_D; break;
   case 5: outer_base = PRETILE_OUTER6_D; break;
  }
 }
//  col1 = CONVERT_RED1;
//  col2 = CONVERT_BLUE2;
// outer_base = PRETILE_OUTER5_D;
 tile_rle [MAZE_OUTER_U] = make_tile(outer_base, -1, col1, col2, ROTATE_180);
 tile_rle [MAZE_OUTER_D] = make_tile(outer_base, -1, col1, col2, 0);
 tile_rle [MAZE_OUTER_L] = make_tile(outer_base, -1, col1, col2, ROTATE_90);
 tile_rle [MAZE_OUTER_R] = make_tile(outer_base, -1, col1, col2, ROTATE_270);
 tile_rle [MAZE_OUTER_UR] = make_tile(outer_base + 1, -1, col1, col2, ROTATE_270);
 tile_rle [MAZE_OUTER_DR] = make_tile(outer_base + 1, -1, col1, col2, 0);
 tile_rle [MAZE_OUTER_UL] = make_tile(outer_base + 1, -1, col1, col2, ROTATE_180);
 tile_rle [MAZE_OUTER_DL] = make_tile(outer_base + 1, -1, col1, col2, ROTATE_90);
 tile_rle [MAZE_OUTER2] = make_tile(outer_base + 2, -1, col1, col2, 0);
 arena.colour3 = col1 + 1;

 tile_rle [MAZE_REMOTE_NODE] = make_m_tile(PRETILE_M_NODE, tmaze, -1, col1, col2, 0);
// col1 = CONVERT_GREEN1 + grand(5) * 8;
// col2 = CONVERT_GREEN2 + grand(5) * 8;
 int col3 = col1; //CONVERT_GREEN1 + grand(5) * 8;
 int col4 = CONVERT_GREEN2 + grand(5) * 8;

 int eye_base = PRETILE_EYE4_CLOSED;
// if (arena.level == 2)
//  eye_base = PRETILE_EYE3_CLOSED;

 switch(grand(4))
 {
  case 0: eye_base = PRETILE_EYE1_CLOSED; break;
  case 1: eye_base = PRETILE_EYE2_CLOSED; break;
  case 2: eye_base = PRETILE_EYE3_CLOSED; break;
  case 3: eye_base = PRETILE_EYE4_CLOSED; break;
 } 

// screenshot
//eye_base = PRETILE_EYE3_CLOSED;
  
 tile_rle [MAZE_EYE_CLOSED_NODE] = make_complex_tile(eye_base, PRETILE_M_EYE_UDLR, tmaze, col1, col2, col3, col4, 0);
 tile_rle [MAZE_EYE_OPEN_NODE] = make_complex_tile(eye_base + 1, PRETILE_M_EYE_UDLR, tmaze, col1, col2, col3, col4, 0);
 tile_rle [MAZE_EYE_CLOSED_REMOTE_NODE] = make_complex_tile(eye_base, PRETILE_M_NODE, tmaze, col1, col2, col3, col4, 0);
 tile_rle [MAZE_EYE_OPEN_REMOTE_NODE] = make_complex_tile(eye_base + 1, PRETILE_M_NODE, tmaze, col1, col2, col3, col4, 0);

 int i, j, k;
// tile_progress_update("wiping");
 
// First we wipe the whole map:
 for (i = 0; i < x; i ++)
 {
  for (j = 0; j < y; j ++)
  {
//   putpixel(level_bmp, i, j, i % 10); //grand(MAZE_UDLR));
//   putpixel(level_bmp, i, j, grand(MAZE_OUTER));
     putpixel(level_bmp, i, j, 0); //MAZE_NOTHING);
  } 
 }

// tile_progress_update("drawing");
 draw_maze_1(level_bmp, x, y);

 put_eyes_in_maze(level_bmp, x, y);

// tile_progress_update("fixing");
 for (i = 0; i < x; i ++)
 {
  for (j = 0; j < y; j ++)
  {
   if (i <= 1 || j <= 1 || i >= x - 2 || j >= y - 2)
    putpixel(level_bmp, i, j, MAZE_OUTER2);
  } 
 }

 int connections, putty;

// then we fix the map so that the paths join up properly:
 for (i = 0; i < x; i ++)
 {
  for (j = 0; j < y; j ++)
  {
   if (getpixel(level_bmp, i, j) == 0)
    continue;
   if (getpixel(level_bmp, i, j) == MAZE_OUTER2)
   {
    if (i == 0 || j == 0 || i == x - 1 || j == y - 1)
    {
     putpixel(level_bmp, i, j, MAZE_OUTER2);
     continue;
    } 
    if (i == 1)
     putpixel(level_bmp, i, j, MAZE_OUTER_L);
    if (j == 1)
     putpixel(level_bmp, i, j, MAZE_OUTER_U);
    if (i == x - 2)
     putpixel(level_bmp, i, j, MAZE_OUTER_R);
    if (j == y - 2)
     putpixel(level_bmp, i, j, MAZE_OUTER_D);
    if (i == 1 && j == 1)
     putpixel(level_bmp, i, j, MAZE_OUTER_UL);
    if (i == 1 && j == y - 2)
     putpixel(level_bmp, i, j, MAZE_OUTER_DL);
    if (i == x - 2 && j == y - 2)
     putpixel(level_bmp, i, j, MAZE_OUTER_DR);
    if (i == x - 2 && j == 1)
     putpixel(level_bmp, i, j, MAZE_OUTER_UR);
//     putpixel(level_bmp, i, j, MAZE_OUTER_U);
     continue;
   }
   
   connections = 0;
   putty = 0;
   if (getpixel(level_bmp, i - 1, j) > 0)// && getpixel(level_bmp, i - 1, j) != MAZE_OUTER2)
    connections += CONNECT_L;
   if (getpixel(level_bmp, i + 1, j) > 0)// && getpixel(level_bmp, i + 1, j) != MAZE_OUTER2)
    connections += CONNECT_R;
   if (getpixel(level_bmp, i, j - 1) > 0)// && getpixel(level_bmp, i, j - 1) != MAZE_OUTER2)
    connections += CONNECT_U;
   if (getpixel(level_bmp, i, j + 1) > 0)// && getpixel(level_bmp, i, j + 1) != MAZE_OUTER2)
    connections += CONNECT_D;
   switch(connections)
   {
    case 1: putty = MAZE_U; break;
    case 2: putty = MAZE_D; break;
    case 4: putty = MAZE_L; break;
    case 8: putty = MAZE_R; break;
    case 3: putty = MAZE_UD; break;
    case 12: putty = MAZE_LR; break;
    case 5: putty = MAZE_UL; break;
    case 6: putty = MAZE_DL; break;
    case 9: putty = MAZE_UR; break;
    case 10: putty = MAZE_DR; break;
    case 13: putty = MAZE_ULR; break;
    case 14: putty = MAZE_DLR; break;
    case 7: putty = MAZE_UDL; break;
    case 11: putty = MAZE_UDR; break;
    case 15: putty = MAZE_UDLR; 
     for (k = 0; k < arena.eyes_on_level; k ++)
     {
      // for now, arena.eye_x is in GRID_WIDTH, not GRAIN. It gets changed to GRAIN later.
      if (arena.eye_x [k] == i && arena.eye_y [k] == j)
      {
       putty = MAZE_EYE_CLOSED_NODE; // assumes all eyes are on udlr
       break;
      } 
     }
     break;
   }
   putpixel(level_bmp, i, j, putty);
  } 
 }

// place_eyes(level_bmp, x, y);

// tile_progress_update("fixed");

// now let's change arena.eye_x from GRID_WIDTH to GRAIN:
 for (k = 0; k < arena.eyes_on_level; k ++)
 {
  arena.eye_x [k] = (arena.eye_x [k] - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN;
  arena.eye_y [k] = (arena.eye_y [k] - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN;
 }
 
}

void place_eyes(BITMAP *level_bmp, int x, int y)
{
/*
 int i, j, k = 0;
 int node_x [100];
 int node_y [100];
 
 for (i = 0; i < x; i ++)
 {
  for (j = 0; j < y; j ++)
  {
   if (k >= 100)
    break;
   if (getpixel(level_bmp, i, j) == MAZE_UDLR)
   {
    node_x [k] = i;
    node_y [k] = j;
    k++;
//    putpixel(level_bmp, i, j, MAZE_EYE_CLOSED_NODE);
   }
  }
 }
 
// int no_eyes = 4;
 
 for (i = 0; i < MAX_EYES; i ++)
 {
  arena.eye_x [i] = 0;
  arena.eye_y [i] = 0;
 }
 arena.eyes_on_level = 0;
 
 next_eye = 0;
 
 put_eye(2, 2, x / 2, y / 2, k, node_x, node_y);
 put_eye(x - x / 2, 2, x - 2, y / 2, k, node_x, node_y);
 put_eye(x - x / 2, y - y / 2, x - 2, y - 2, k, node_x, node_y);
 put_eye(2, y - y / 2, x / 2, y - 2, k, node_x, node_y);
 

 arena.eye_colour1 = 0; // pupil - absolute black. won't be overwritten by transparencies

  arena.eye_colour2 = TRANS_YELLOW;
  arena.eye_colour3 = TRANS_LRED;
  if (grand(3) == 0)
  {
   arena.eye_colour2 = TRANS_ORANGE;
   arena.eye_colour3 = TRANS_DRED;
  } 
 
 if (arena.level > 5 && grand(3) != 0)
 {
  arena.eye_colour2 = TRANS_WHITE;
  arena.eye_colour3 = TRANS_LBLUE;
  if (grand(5) == 0)
   arena.eye_colour3 = TRANS_PURPLE;
  if (grand(5) == 0)
   arena.eye_colour3 = TRANS_DBLUE;
 }
 
 if (arena.level > 10 && grand(3) != 0)
 {
  arena.eye_colour2 = TRANS_YELLOW;
  arena.eye_colour3 = TRANS_LGREEN;
  if (grand(3) == 0)
  {
   arena.eye_colour2 = TRANS_LGREEN;
   arena.eye_colour3 = TRANS_DGREEN;
  } 
 }

// not the usual order of colours. 
*/ 
}

void put_eye(int x1, int y1, int x2, int y2, int k, int node_x [100], int node_y [100])
{

 int i, j;
 
 for (i = 0; i < k; i ++)
 {
  if (node_x [i] > x1 && node_x [i] < x2 && node_y [i] > y1 && node_y [i] < y2)
  {
   put_eye2(node_x [i], node_y [i]);
    return;
  }
 }
// okay, we couldn't find a node in that area. We'll have to make one:
 int timeout = 0;

 do
 {
   i = x1 + grand(x2 - x1 + 1);
   j = y1 + grand(y2 - y1 + 1);
   if (eye_okay(i, j))
   {
     put_eye2(i, j);
     return;
   } 
   timeout ++;
 } while(timeout < 10000);

// that didn't work, so let's find any suitable position:
 for (i = x1; i < x2; i ++)
 {
  for (j = y1; j < y2; j ++)
  {
    if (eye_okay(i, j)) 
    {
     put_eye2(i, j);
     return;
    } 
  } 
 }

}

void put_eye2(int x, int y)
{
 int placed = 0;
 if (getpixel(level_bmp, x, y) == MAZE_UDLR)
 {
  putpixel(level_bmp, x, y, MAZE_EYE_CLOSED_NODE);
  placed = 1;
 } 
 if (getpixel(level_bmp, x, y) == 0)
 {
  putpixel(level_bmp, x, y, MAZE_EYE_CLOSED_REMOTE_NODE);
  placed = 1;
 } 
 if (placed == 1)
 {
//  arena.eye_x [next_eye] = x * GRID_WIDTH * GRAIN;
//  arena.eye_y [next_eye] = y * GRID_WIDTH * GRAIN;
  arena.eye_x [next_eye] = (x - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN;
  arena.eye_y [next_eye] = (y - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN;
  arena.eyes_on_level ++;
  next_eye ++;
 } 
}

int eye_okay(int i, int j)
{
   if (getpixel(level_bmp, i, j) != 0)
    return 0;
   if (getpixel(level_bmp, i + 1, j) == 0
    && getpixel(level_bmp, i - 1, j) == 0
    && getpixel(level_bmp, i, j + 1) == 0
    && getpixel(level_bmp, i, j - 1) == 0)
     return 1;
    
    return 0; 
}

void open_eyes(void)
{
 int i,j, opened;
 int passing_colours [5];

 int x = arena.max_x / 50000 + 5;
 int y = arena.max_y / 50000 + 5;

 for (i = 0; i < x; i ++)
 {
  for (j = 0; j < y; j ++)
  {
   opened = 0;
   if (getpixel(level_bmp, i, j) == MAZE_EYE_CLOSED_NODE)
   {
    putpixel(level_bmp, i, j, MAZE_EYE_OPEN_NODE);
    opened = 1;
   } 
   if (getpixel(level_bmp, i, j) == MAZE_EYE_CLOSED_REMOTE_NODE)
   {
    putpixel(level_bmp, i, j, MAZE_EYE_OPEN_REMOTE_NODE);
    opened = 1;
   } 
   if (opened == 1)
   {
    passing_colours [0] = TRANS_DBLUE;
    passing_colours [1] = TRANS_LBLUE;
    passing_colours [2] = TRANS_WHITE;
    place_explosion((i - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN + grand(3001) - 1500, (j - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN + grand(3001) - 1500, 0, 0, 700 + grand(300), passing_colours);
    passing_colours [0] = TRANS_LBLUE;
    passing_colours [1] = COLOUR_BLUE8;
    place_line_burst((i - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN, (j - 2) * GRID_WIDTH * GRAIN + (GRID_WIDTH / 2.5) * GRAIN, 0, 0, 7 + grand(4), 15, 15, 2500, 300, passing_colours);
   }
  }
 }
}

void draw_maze_1(BITMAP *level_bmp, int x, int y)
{
// now we draw various paths onto the map with value 1:

 int m, n, i, j;
 int path_length = 5 + grand(30);
 int no_paths = 3 + grand(4);
  
 for (m = 0; m < no_paths; m ++)
 {
  i = grand(x - 2) + 2;
  j = grand(y - 2) + 2;
  for (n = 0; n < path_length; n ++)
  {
   putpixel(level_bmp, i, j, 1); 
   if (grand(2) == 0)
   {
    if (grand(2) == 0) i ++;
     else i --;
   }
    else
    {
     if (grand(2) == 0) j ++;
      else j --;
    }
    if (i < 2 || i > x - 2)
     i = grand(x - 2) + 2;
    if (j < 2 || j > y - 2)
     j = grand(y - 2) + 2;
  }
 }

}

void draw_maze_2(BITMAP *level_bmp, int x, int y)
{

// now we draw various paths onto the map with value 1:

 int m;//, n, i, j;
// int path_length = 5 + grand(30);
 int no_paths = 8 + grand(4);
 
 for (m = 0; m < no_paths; m ++)
 {
   if (grand(2) == 0)
    hline(level_bmp, grand(x - 2) + 2, grand(y - 2) + 2, grand(x - 2) + 2, 1);
     else
      vline(level_bmp, grand(x - 2) + 2, grand(y - 2) + 2, grand(y - 2) + 2, 1);
 }
 
}

void put_eyes_in_maze(BITMAP *level_bmp, int x, int y)
{
 int i;

 for (i = 0; i < MAX_EYES; i ++)
 {
  arena.eye_x [i] = 0;
  arena.eye_y [i] = 0;
 }
 arena.eyes_on_level = 4;
 
 arena.eye_colour1 = 0; // pupil - absolute black. won't be overwritten by transparencies

  arena.eye_colour2 = TRANS_YELLOW;
  arena.eye_colour3 = TRANS_LRED;
  if (grand(3) == 0)
  {
   arena.eye_colour2 = TRANS_ORANGE;
   arena.eye_colour3 = TRANS_DRED;
  } 
 
 if (arena.level > 5 && grand(3) != 0)
 {
  arena.eye_colour2 = TRANS_WHITE;
  arena.eye_colour3 = TRANS_LBLUE;
  if (grand(5) == 0)
   arena.eye_colour3 = TRANS_PURPLE;
  if (grand(5) == 0)
   arena.eye_colour3 = TRANS_DBLUE;
 }
 
 if (arena.level > 10 && grand(3) != 0)
 {
  arena.eye_colour2 = TRANS_YELLOW;
  arena.eye_colour3 = TRANS_LGREEN;
  if (grand(3) == 0)
  {
   arena.eye_colour2 = TRANS_LGREEN;
   arena.eye_colour3 = TRANS_DGREEN;
  } 
 }

// not the usual order of colours. 



 find_place_for_eye(3, 3, x / 2, y / 2, 0);
 find_place_for_eye(x / 2, y / 2, x - 3, y - 3, 1);
 find_place_for_eye(3, y / 2, x / 2, y - 3, 2);
 find_place_for_eye(x / 2, 3, x - 3, y / 2, 3);
 
 if (grand(2) == 0)
 {
  find_place_for_eye(3, 3, x / 2, y / 2, 3);
  find_place_for_eye(x / 2, y / 2, x - 3, y - 3, 2);
  find_place_for_eye(3, y / 2, x / 2, y - 3, 1);
  find_place_for_eye(x / 2, 3, x - 3, y / 2, 0);
 }

 for (i = 0; i < arena.eyes_on_level; i ++)
 {
  putpixel(level_bmp, arena.eye_x [i], arena.eye_y [i], 1);

/*   draw_eye_path(level_bmp, x, y, arena.eye_x [i] + 1, arena.eye_y [i], 1, 0, 1); // same but draw = 1
   draw_eye_path(level_bmp, x, y, arena.eye_x [i] - 1, arena.eye_y [i], -1, 0, 1); // same but draw = 1
   draw_eye_path(level_bmp, x, y, arena.eye_x [i], arena.eye_y [i] + 1, 0, 1, 1); // same but draw = 1
   draw_eye_path(level_bmp, x, y, arena.eye_x [i], arena.eye_y [i] - 1, 0, -1, 1); // same but draw = 1*/
     putpixel(level_bmp, arena.eye_x [i] + 1, arena.eye_y [i], 1);
     putpixel(level_bmp, arena.eye_x [i] - 1, arena.eye_y [i], 1);
     putpixel(level_bmp, arena.eye_x [i], arena.eye_y [i] + 1, 1);
     putpixel(level_bmp, arena.eye_x [i], arena.eye_y [i] - 1, 1);
  if (draw_eye_path(level_bmp, x, y, arena.eye_x [i] + 1, arena.eye_y [i], 1, 0, 0))
   draw_eye_path(level_bmp, x, y, arena.eye_x [i] + 1, arena.eye_y [i], 1, 0, 1); // same but draw = 1
    else
     putpixel(level_bmp, arena.eye_x [i] + 1, arena.eye_y [i], 1);

  if (draw_eye_path(level_bmp, x, y, arena.eye_x [i] - 1, arena.eye_y [i], -1, 0, 0))
   draw_eye_path(level_bmp, x, y, arena.eye_x [i] - 1, arena.eye_y [i], -1, 0, 1); // same but draw = 1
    else
     putpixel(level_bmp, arena.eye_x [i] - 1, arena.eye_y [i], 1);

  if (draw_eye_path(level_bmp, x, y, arena.eye_x [i], arena.eye_y [i] + 1, 0, 1, 0))
   draw_eye_path(level_bmp, x, y, arena.eye_x [i], arena.eye_y [i] + 1, 0, 1, 1); // same but draw = 1
    else
     putpixel(level_bmp, arena.eye_x [i], arena.eye_y [i] + 1, 1);

  if (draw_eye_path(level_bmp, x, y, arena.eye_x [i], arena.eye_y [i] - 1, 0, -1, 0))
   draw_eye_path(level_bmp, x, y, arena.eye_x [i], arena.eye_y [i] - 1, 0, -1, 1); // same but draw = 1
    else
     putpixel(level_bmp, arena.eye_x [i], arena.eye_y [i] - 1, 1);

 }

}

void find_place_for_eye(int x_min, int y_min, int x_max, int y_max, int eye)
{
// _min values should be at least 1

 arena.eye_x [eye] = x_min + grand(x_max - x_min);
 arena.eye_y [eye] = y_min + grand(y_max - y_min);
// arena.eye_x [eye] = x_min + 3 + grand(x_max - x_min - 6);
// arena.eye_y [eye] = y_min + 3 + grand(y_max - y_min - 6);
// arena.eye_x [eye] = x_max - 1;
// arena.eye_y [eye] = y_max - 1;

}

// if draw is zero, it doesn't change anything.
// returns zero if it hits the wall before it finds another path to join.
int draw_eye_path(BITMAP *level_bmp, int x, int y, int ex, int ey, int xd, int yd, int draw)
{

 int i, j;
 
 while (TRUE)
 {
 
  for (i = -1; i < 2; i ++)
  {
   for (j = -1; j < 2; j ++)
   {
    if (i == 0 && j == 0)
     continue; // no centre
    if (i != 0 && j != 0)
     continue; // no diagonals
    if (i == xd * -1 && j == yd * -1)
     continue; // so not the one we just drew
    if (getpixel(level_bmp, ex + i, ey + j) != 0)
    {
     if (draw)
      putpixel(level_bmp, ex, ey, 1);
     return 1; 
    }
   }
  }
  ex += xd;
  ey += yd;  
  if (ex <= 1 || ex >= x - 2 || ey <= 1 || ey >= y - 2)
   return 0; // hit the wall!
  if (draw)
   putpixel(level_bmp, ex, ey, 1);
 }
 

}


RLE_SPRITE *make_tile(int index, int index2, int colourise1, int colourise2, int rotation)
{

 BITMAP *temp_bmp = make_bmp(50, 50, "make_tile (temp_bmp)");
 
 clear_bitmap(temp_bmp);


   if (index2 != -1)
   {
    if (index2 >= NO_PRETILE_BMPS || pretile_bmp [index2] == NULL)
    {
     do
     {
      textprintf_centre_ex(screen, small_font, 200, 200, COLOUR_WHITE, COLOUR_GREY1, "Pretile error index2 %i: X to continue", index2);
     }
      while (key [KEY_X] == 0);
    }
     else
       rotate_sprite(temp_bmp, pretile_bmp [index2], 0, 0, itofix(rotation));
   }    

   if (index >= NO_PRETILE_BMPS || pretile_bmp [index] == NULL)
   {
    do
    {
     textprintf_centre_ex(screen, small_font, 200, 200, COLOUR_WHITE, COLOUR_GREY1, "Pretile error index %i: X to continue", index);
    }
     while (key [KEY_X] == 0);
   }
    else
      rotate_sprite(temp_bmp, pretile_bmp [index], 0, 0, itofix(rotation));

  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  if (colourise1 != 0)
   rectfill(temp_bmp, 0, 0, 50, 50, colourise1);
  if (colourise2 != 0)
   rectfill(temp_bmp, 0, 0, 50, 50, colourise2);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

 RLE_SPRITE *return_sprite = get_rle_sprite(temp_bmp);
 
 destroy_bitmap(temp_bmp);

 return return_sprite;



}

RLE_SPRITE *make_m_tile(int index, int tmaze, int index2, int colourise1, int colourise2, int rotation)
{

 BITMAP *temp_bmp = make_bmp(50, 50, "make_m_tile (temp_bmp)");
 
 clear_bitmap(temp_bmp);


   if (index2 != -1)
   {
    if (index2 >= NO_PRETILE_M_BMPS || pretile_m_bmp [tmaze] [index2] == NULL)
    {
     do
     {
      textprintf_centre_ex(screen, small_font, 200, 200, COLOUR_WHITE, COLOUR_GREY1, "Pretile m error index2 %i, %i: Space to continue", index2, tmaze);
     }
      while (key [KEY_SPACE] == 0);
    }
     else
       rotate_sprite(temp_bmp, pretile_m_bmp [tmaze] [index2], 0, 0, itofix(rotation));
   }    

   if (index >= NO_PRETILE_M_BMPS || pretile_m_bmp [tmaze] [index] == NULL)
   {
    do
    {
     textprintf_centre_ex(screen, small_font, 200, 200, COLOUR_WHITE, COLOUR_GREY1, "Pretile m error index %i, %i: Space to continue", index, tmaze);
    }
     while (key [KEY_SPACE] == 0);
   }
    else
      rotate_sprite(temp_bmp, pretile_m_bmp [tmaze] [index], 0, 0, itofix(rotation));

  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  if (colourise1 != 0)
   rectfill(temp_bmp, 0, 0, 50, 50, colourise1);
  if (colourise2 != 0)
   rectfill(temp_bmp, 0, 0, 50, 50, colourise2);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

 RLE_SPRITE *return_sprite = get_rle_sprite(temp_bmp);
 
 destroy_bitmap(temp_bmp);

 return return_sprite;



}

// index must be in pretile array; index2 must be in pretile_m array referred to by tmaze
RLE_SPRITE *make_complex_tile(int index, int index2, int tmaze, int colourise1, int colourise2, int colourise2_1, int colourise2_2, int rotation)
{

 BITMAP *temp_bmp = make_bmp(50, 50, "make_complex_tile (temp_bmp)");
 clear_bitmap(temp_bmp);
 BITMAP *temp_bmp2 = make_bmp(50, 50, "make_complex_tile (temp_bmp2)");
 clear_bitmap(temp_bmp2);

   if (index2 != -1)
   {
    if (index2 >= NO_PRETILE_M_BMPS || pretile_m_bmp [tmaze] [index2] == NULL)
    {
     do
     {
      textprintf_centre_ex(screen, small_font, 200, 200, COLOUR_WHITE, COLOUR_GREY1, "make_complex_tile pretile m error index2 %i: Space to continue", index2);
     }
      while (key [KEY_SPACE] == 0);
    }
     else
       rotate_sprite(temp_bmp, pretile_m_bmp [tmaze] [index2], 0, 0, itofix(rotation));
   }    

  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  if (colourise1 != 0)
   rectfill(temp_bmp, 0, 0, 50, 50, colourise1);
  if (colourise2 != 0)
   rectfill(temp_bmp, 0, 0, 50, 50, colourise2);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);


   if (index >= NO_PRETILE_BMPS || pretile_bmp [index] == NULL)
   {
    do
    {
     textprintf_centre_ex(screen, small_font, 200, 200, COLOUR_WHITE, COLOUR_GREY1, "make_complex_tile pretile error index %i: Space to continue", index);
    }
     while (key [KEY_SPACE] == 0);
   }
    else
      rotate_sprite(temp_bmp2, pretile_bmp [index], 0, 0, itofix(rotation));

  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  if (colourise1 != 0)
   rectfill(temp_bmp2, 0, 0, 50, 50, colourise2_1);
  if (colourise2 != 0)
   rectfill(temp_bmp2, 0, 0, 50, 50, colourise2_2);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

  draw_sprite(temp_bmp, temp_bmp2, 0, 0);
// circle(temp_bmp2, 25, 25, 5, COLOUR_GREEN6);

 RLE_SPRITE *return_sprite = get_rle_sprite(temp_bmp);
 
 destroy_bitmap(temp_bmp);
 destroy_bitmap(temp_bmp2);

 return return_sprite;


}

BITMAP *make_bmp(int x, int y, const char errtxt [])
{

 BITMAP *bmp_made = create_bitmap(x, y);
 
 if (bmp_made == NULL)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  allegro_message("Bitmap creation error  \n%s\n\n\r%s", allegro_error, errtxt);
  exit(1);
 }
 
 return bmp_made;
}



void tile_progress_update(const char progtxt [])
{
 static int ypos = 10, xpos = 10;
 textprintf_centre_ex(screen, small_font, 320, ypos, COLOUR_GREEN8, -1, progtxt);
 ypos += 10;  
 if (ypos > 470)
 {
  if (xpos > 620)
  {
   xpos = 10;
   ypos = 10;
  }
  ypos = 10;
  xpos += 30;
 }
}


void make_tile_background_rle(void)
{

 if (tile_background != NULL)
  destroy_rle_sprite(tile_background);

// BITMAP *tile_back_temp = make_bmp(scr_x + GRID_WIDTH, scr_y + GRID_WIDTH, "make_tile_background_rle");
 BITMAP *tile_back_temp = make_bmp(scr_x + GRID_WIDTH, scr_y + GRID_WIDTH, "make_tile_background_rle");
 
 int i, j, xw;
 
 xw = (scr_x + GRID_WIDTH) / GRID_WIDTH;
 
 if (game.users == 2)
 {
  xw /= 2;
  xw ++; // even in 640x480
 } 
 
 if (options.resolution != GMODE_800_600)
  xw ++;
 
 for (i = 0; i < xw; i ++)
 {
  for (j = 0; j < (scr_y + GRID_WIDTH) / GRID_WIDTH + 1; j ++)
  {
     draw_rle_sprite(tile_back_temp, tile_rle [0], i * GRID_WIDTH, j * GRID_WIDTH);
//     circlefill(tile_back_temp, i * GRID_WIDTH, j * GRID_WIDTH, 7, COLOUR_WHITE);
  } 
 }

 tile_background = get_rle_sprite(tile_back_temp);

 destroy_bitmap(tile_back_temp);
}



