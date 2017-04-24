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
    
File: palette.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - palette and transparency stuff.
Various enums are in palette.h.

*/

/*
Lacewing
Copyright (C) 2003 Captain Pork

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

    You should be able to reach Captain Pork by sending an email to
    captainpork@fastmail.fm or sending a letter to
    Captain Pork, 3/4 Kensington Rd, Rose Park, SA 5067 Australia
    but my lease is a short-term one so email is probably your best bet.

File: Palette.c
History:
1/5/03 - Started (Capt Pork)

This file contains:
The palette functions

*/

#include "config.h"

#include "allegro.h"


#include "palette.h"
#include "globvars.h"

COLOR_MAP trans_table;
int blend_function(int x, int y, RGB *rgbl);
int limit_colour(int colour_input);
int average_colours(int colour1, int colour2);

RGB other_palet [1324];
RGB palet [256];
RGB light_palet [256];
RGB dark_palet [256];
RGB other_palet2 [2048];
// what seems to be a weird bug in Allegro forces me to put padding around
//  the palet array, or it becomes corrupted. The same thing happened in
//  World of Violence and I have no idea why.

int adjust_lit_colour(int col, int lit);
int add_light_to_colour(int col, int lit);
void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal);
int get_lower_colour(int y);
int colour_illumination(int y);
int base_colour(int y);
int colour_brightness(int y);
void init_light_palette(void);
void init_dark_palette(void);

void colour_table(const char *which_call);
//void colour_table(void);

//RGB mono_palet [256];


int base_palette [48] [3] =
{
// DON't modify this palette - it may mess up image loading
{0, 0, 0},
{0, 0, 1},
{10, 10, 10},
{20, 20, 20},
{30, 30, 30},
{40, 40, 40},
{50, 50, 50},
{63, 63, 63}, // greys
{0, 8, 0},
{0, 16, 0},
{3, 24, 3},
{6, 32, 6},
{8, 40, 8},
{12, 48, 12},
{16, 56, 16},
{20, 63, 20}, // Greens
{8, 8, 0},
{16, 16, 0},
{24, 24, 4},
{32, 32, 8},
{40, 40, 12},
{48, 48, 16},
{56, 56, 20},
{63, 63, 24}, // yellows
// DON't modify this palette - it may mess up image loading
{8, 3, 0},
{16, 6, 0},
{24, 9, 0},
{32, 12, 0},
{40, 15, 0},
{48, 18, 0},
{56, 21, 0},
{63, 24, 0}, // Oranges
{8, 0, 0},
{16, 0, 0},
{24, 0, 0},
{32, 3, 3},
{40, 5, 5},
{48, 7, 7},
{56, 9, 9},
{63, 11, 11}, // Reds
{0, 0, 8},
{0, 0, 16},
{4, 4, 24},
{8, 8, 32},
{12, 12, 40},
{16, 16, 48},
{20, 20, 56},
{24, 24, 63} // Blues
// DON't modify this palette - it may mess up image loading

};

int base_palette2 [48] [3] =
{
// Modifying this palette is okay
{0, 0, 0},
{0, 0, 1},
{10, 10, 10},
{20, 20, 20},
{33, 33, 33},
{46, 46, 46},
{55, 55, 55},
{63, 63, 63}, // greys
{2, 8, 2},
{4, 16, 4},
{8, 24, 8},
{16, 32, 16},
{24, 40, 24},
{30, 48, 30},
{33, 56, 33},
{36, 63, 36}, // Greens
{8, 6, 3},
{16, 17, 6},
{24, 20, 12},
{32, 30, 16},
{40, 37, 22},
{48, 45, 30},
{56, 52, 36},
{63, 60, 40}, // yellows
/*
{8, 4, 1},
{16, 6, 3},
{24, 9, 6},
{32, 12, 9},
{40, 20, 12},
{48, 25, 15},
{56, 30, 18},
{63, 35, 21}, // Oranges
{8, 3, 3},
{16, 6, 6},
{24, 9, 9},
{32, 12, 12},
{40, 15, 15},
{48, 18, 18},
{56, 21, 21},
{63, 25, 25}, // Reds
*/
{8, 5, 3},
{16, 9, 7},
{24, 13, 9},
{32, 17, 13},
{40, 24, 16},
{48, 28, 19},
{56, 34, 21},
{63, 40, 25}, // Oranges
{8, 3, 3},
{16, 6, 6},
{24, 9, 9},
{32, 12, 12},
{40, 15, 15},
{48, 18, 18},
{56, 21, 21},
{63, 25, 25}, // Reds
{4, 4, 8},
{8, 8, 16},
{15, 15, 24},
{20, 20, 32},
{24, 24, 40},
{28, 28, 48},
{36, 36, 56},
{42, 42, 63} // Blues
// Modifying this palette is okay

};


#define END_TRANSPARENCIES 104

/*
0 - black
1-104 - transparencies
105-152 - dark
153-200 - lighter
201-248 - brightest
249-255 - ???

Colours:
1 - grey/white
2 - Green
3 - yellow
4 - orange
5 - red
6 - blue

Transparencies:
1 - white
2 - purple
3 - lblue
4 - dblue
5 - yellow
6 - lgreen
7 - dgreen
8 - orange
9 - red
10 - dred
11 - white2
12 - lgrey
13 - dgrey


13 * 8 = 104
256 - 104 = 152
152 / 48 = 3 + 8
*/

void process_palette(RGB *proc_palet)
{
// int i;
}


void init_palette(void)
{

int i;


// colour_table("First");

 for (i = 0; i < 48; i ++)
 {
/*    // base (ambient)
    palet[i + 105].r = limit_colour(base_palette [i] [0] * 3) / 4;
    palet[i + 105].g = limit_colour(base_palette [i] [1] * 3) / 4;
    palet[i + 105].b = limit_colour(base_palette [i] [2] * 3) / 4;

    // bright
    palet[i + 153].r = limit_colour(base_palette [i] [0]);
    palet[i + 153].g = limit_colour(base_palette [i] [1]);
    palet[i + 153].b = limit_colour(base_palette [i] [2]);

    // brightest
    palet[i + 201].r = limit_colour((base_palette [i] [0] * 4) / 3);
    palet[i + 201].g = limit_colour((base_palette [i] [1] * 4) / 3);
    palet[i + 201].b = limit_colour((base_palette [i] [2] * 4) / 3);
    
*/    
    
    // base (ambient)
    palet[i + 105].r = limit_colour(((base_palette [i] [0] * 4) / 5) - 5);
    palet[i + 105].g = limit_colour(((base_palette [i] [1] * 4) / 5) - 5);
    palet[i + 105].b = limit_colour(((base_palette [i] [2] * 4) / 5) - 5);

    // bright
    palet[i + 153].r = limit_colour(base_palette [i] [0]);
    palet[i + 153].g = limit_colour(base_palette [i] [1]);
    palet[i + 153].b = limit_colour(base_palette [i] [2]);

    // brightest
    palet[i + 201].r = limit_colour(((base_palette [i] [0] * 5) / 4));
    palet[i + 201].g = limit_colour(((base_palette [i] [1] * 5) / 4));
    palet[i + 201].b = limit_colour(((base_palette [i] [2] * 5) / 4));
    
    
  }


 palet [252].r = 23;
 palet [252].g = 53;
 palet [252].b = 23;
 palet [255].r = 53;
 palet [255].g = 23;
 palet [255].b = 23;

 for (i = 0; i < 8; i ++)
 {

// NOT REAL PALETTE - see below

    // White - should be same as WHITE2 (below)
    palet[i + 1].r = limit_colour(48 + i * 2);
    palet[i + 1].g = limit_colour(48 + i * 2);
    palet[i + 1].b = limit_colour(48 + i * 2);

    // Yellow
    palet[i + 9].r = limit_colour(47 + i * 3);
    palet[i + 9].g = limit_colour(47 + i * 3);
    palet[i + 9].b = limit_colour(10 + i * 3);

    // LGreen
    palet[i + 17].r = limit_colour(i * 3);
    palet[i + 17].g = limit_colour(32 + i * 4);
    palet[i + 17].b = limit_colour(i * 3);

    // Orange
    palet[i + 25].r = limit_colour(47 + i * 3);
    palet[i + 25].g = limit_colour(20 + i * 3);
    palet[i + 25].b = limit_colour(5 + i * 2);

    // Red
    palet[i + 33].r = limit_colour(41 + i * 4);
    palet[i + 33].g = limit_colour(15 + i * 2);
    palet[i + 33].b = limit_colour(5 + i * 2);

    // DGreen
    palet[i + 41].r = limit_colour(i * 2);
    palet[i + 41].g = limit_colour(21 + i * 4);
    palet[i + 41].b = limit_colour(i * 2);

    // DRed
    palet[i + 49].r = limit_colour(20 + i * 4);
    palet[i + 49].g = limit_colour(i * 2);
    palet[i + 49].b = limit_colour(i * 2);

    // Purple
    palet[i + 57].r = limit_colour(32 + i * 4);
    palet[i + 57].g = limit_colour(i * 3);
    palet[i + 57].b = limit_colour(32 + i * 4);

    // LBlue
    palet[i + 65].r = limit_colour(10 + i * 3);
    palet[i + 65].g = limit_colour(10 + i * 3);
    palet[i + 65].b = limit_colour(47 + i * 2);

    // DBlue
    palet[i + 73].r = limit_colour(i * 2);
    palet[i + 73].g = limit_colour(i * 2);
    palet[i + 73].b = limit_colour(21 + i * 4);

    // White2 - should be same as WHITE (above)
    palet[i + 81].r = limit_colour(48 + i * 2);
    palet[i + 81].g = limit_colour(48 + i * 2);
    palet[i + 81].b = limit_colour(48 + i * 2);

    // LGrey
    palet[i + 89].r = limit_colour(24 + i * 4);
    palet[i + 89].g = limit_colour(24 + i * 4);
    palet[i + 89].b = limit_colour(24 + i * 4);

    // DGrey
    palet[i + 97].r = limit_colour(12 + i * 4);
    palet[i + 97].g = limit_colour(12 + i * 4);
    palet[i + 97].b = limit_colour(12 + i * 4);

 }

  palet[0].r = 0;
  palet[0].g = 0;
  palet[0].b = 0;


/* for (i = 1; i < 256; i ++)
 {
  if (palet[i].r < 20 
      && palet[i].b < 20 
      && palet[i].g < 20)
      {
        light_palet[i].r = palet[i].r;
        light_palet[i].g = palet[i].g;
        light_palet[i].b = palet[i].b;
      }
       else
       {
        light_palet[i].r = limit_colour((palet[i].r * 32) / 63 + 25);
        light_palet[i].b = limit_colour((palet[i].b * 32) / 63 + 25);
        light_palet[i].g = limit_colour((palet[i].g * 32) / 63 + 25);
       } 
 }
*/
/*  for (i = 0; i < 192; i ++)
  {
   palet[i].r = 0;
   palet[i].b = 0;
   palet[i].g = 0;
  }*/


   set_palette(palet);

   pork_create_color_table(&trans_table, palet);

   color_map = &trans_table;

   set_palette(palet);

   init_light_palette();
   init_dark_palette(); // after light_palette

/*   BITMAP *pbmp = create_bitmap(16, 16);

   int x, y;

   for (x = 0; x < 16; x ++)
   {
    for (y = 0; y < 16; y ++)
    {
     putpixel(pbmp, y, x, y + (x * 16));
//     if (y + (x * 16) < 192)
//      putpixel(pbmp, y, x, 0);
    }
   }

   save_bitmap("palgod.pcx", pbmp, palet);
*/
/*   for (x = 192; x < 256; x ++)
   {
    rectfill(screen, (x - 192) * 6, 1, (x - 192) * 6 + 6, 10, x);
   }

   do
   {

   } while (key [KEY_U] == 0);
*/
// int j;

/* for (i = 0; i < 256; i ++)
 {
  j = limit_colour((palet[i].r + palet[i].g + palet[i].b) / 3);
  mono_palet [i].r = j;
  mono_palet [i].g = j;
  mono_palet [i].b = j;
 }

 mono_palet [222].r = 60;
 mono_palet [222].g = 40;
 mono_palet [222].b = 0;
 mono_palet [223].r = 0;
 mono_palet [223].g = 60;
 mono_palet [223].b = 10; // these colours used for 'you win'/'game over'
*/
}

int limit_colour(int colour_input)
{

 if (colour_input < 0) return 0;
 if (colour_input > 63) return 63;
 return colour_input;

}

int average_colours(int colour1, int colour2)
{

 int avge = colour1 + colour2 / 2;
 if (avge < 0) return 0;
 if (avge > 63) return 63;
 return avge;

}


int blend_function(int x, int y, RGB *rgbl)
{

// if (x % 8 == 0)
// {
//  return y;
// }



 if (x == TRANS_REVERSE)
 {
  return ((int) y / 8) * 8 + (8 - ((y-1) % 8));
 }

 if (x == TRANS_DARKEN)
 {
//  return ((int) y / 8) * 8 + (((y-1) % 8) / 2);
  if ((y-1) % 8 < 1)
   x = 1;
    else x = 7;

  if (y < GC_BLACK)
   return ((int) y / 8) * 8 + x;
   
  return COLOUR_BLACK + x; 
 }

/* if (x == TRANS_GREY)
 {
  if (y < 192)
   return y;

  return y - 64;
 }*/

// if (x >= TRANS_WHITE && x < TRANS_END)
/* if (x == TRANS_WHITE
  || x == TRANS_PURPLE
  || x == TRANS_LBLUE
  || x == TRANS_DBLUE
  || x == TRANS_YELLOW
  || x == TRANS_LGREEN
  || x == TRANS_DGREEN
  || x == TRANS_ORANGE
  || x == TRANS_LRED
  || x == TRANS_DRED
  || x == TRANS_WHITE2
  || x == TRANS_LGREY
  || x == TRANS_DGREY)*/
  
 if (x > GC_BLACK && x <= COLOUR_BLUE8) // ie isn't a transparency or a bright lit
 {
  if (x % 8 == 1 && x < COLOUR_BLACK) // ie is a GC (lowest-light) colour
  {
//   if (y > GC_BLACK && y <= GC_WHITE) // ie is part of GC grey scale
//    y = x + ((y-1) % 8) - 4;
   if (y > GC_BLACK && y <= GC_BLUE8) // ie is part of GC grey scale
    y = x + ((y-5) % 8);
   return y;
//   if (x == CONVERT_YELLOW1 && y % 8 != 0)
//    y --; 
  }
  if (x > COLOUR_BLACK && x % 8 == 1 && x <= COLOUR_BLUE8) // ie is a COLOUR (middle-light) colour
  {
   if (y > COLOUR_BLACK && y <= COLOUR_BLUE8) // ie is part of COLOUR grey scale
//    y = x + ((y-5) % 8) - 48;// - 48;
    y = x + ((y-5) % 8) - 48;// - 48;
//   if (x == CONVERT_YELLOW2 && y % 8 != 0)
//    y --; 
   return y;
  }
 }
  
 if (x <= END_TRANSPARENCIES && x % 8 == 0 && x > 0)
 {

//  if ((y - 1) / 8 < ((x / 8) - TRANS_WHITE)
  if (y < x - 7
      && y < END_TRANSPARENCIES)
   return y; // don't overwrite lower transparencies*/


//return 100;

//  if ((y - 1) % 8 == 0)
//   return (y - 1) % 8 + (x - TRANS_WHITE) * 8 + 1;
   return (y - 1) % 8 + x - 7;


//   return (y - 1) % 8 + (x - TRANS_WHITE) * 8 + 1;

 }

 if (x == LIGHT_1)
 {
  if (y <= END_TRANSPARENCIES)
   return y; // leaves transparencies alone
  if (y >= 105 && y <= 152)
  {
   return y + 48;
  } // only lightens areas only lit by ambient light.
  return y;  
 }

 if (x == LIGHT_2)
 {
  if (y <= END_TRANSPARENCIES)
   return y; // leaves transparencies alone
  if (y >= 105 && y <= 152)
  {
   return y + 96;
  } // lightens level 1 light to level 3
  if (y >= 153 && y <= 200)
  {
   return y + 48;
  } // lightens level 2 light to level 3
  return y;  
 }

 return y;

}



int get_lower_colour(int y)
{

  if (y % 32 == COLOUR_WHITE % 32)
   return 7;
  
  if (y % 32 >= COLOUR_BLACK % 32 && y % 32 <= COLOUR_WHITE % 32)
   return (y % 32);
   
  if (y % 32 >= COLOUR_ORANGE1 % 32 && y % 32 <= COLOUR_BLUE4 % 32)
   return ((((y % 32) - (COLOUR_ORANGE1 % 32)) % 4 + 1) * 3) / 2;

 return 7;

}

int colour_to_trans(int y)
{

 switch(y)
 {
  case COLOUR_GREY1:
  case COLOUR_GREY2:
  case COLOUR_GREY3:
  case COLOUR_GREY4:
   return TRANS_LGREY;
  case COLOUR_GREY5:
  case COLOUR_GREY6:
  case COLOUR_WHITE:
   return TRANS_WHITE;
  case COLOUR_ORANGE1:
  case COLOUR_ORANGE2:
  case COLOUR_ORANGE3:
  case COLOUR_ORANGE4:
   return TRANS_ORANGE;
  case COLOUR_ORANGE5:
  case COLOUR_ORANGE6:
  case COLOUR_ORANGE7:
  case COLOUR_ORANGE8:
   return TRANS_ORANGE;
  case COLOUR_BLUE1:
  case COLOUR_BLUE2:
  case COLOUR_BLUE3:
  case COLOUR_BLUE4:
   return TRANS_DBLUE;
  case COLOUR_BLUE5:
  case COLOUR_BLUE6:
  case COLOUR_BLUE7:
  case COLOUR_BLUE8:
   return TRANS_LBLUE;
  case COLOUR_RED1:
  case COLOUR_RED2:
  case COLOUR_RED3:
  case COLOUR_RED4:
   return TRANS_DRED;
  case COLOUR_RED5:
  case COLOUR_RED6:
  case COLOUR_RED7:
  case COLOUR_RED8:
   return TRANS_LRED;
  case COLOUR_YELLOW1:
  case COLOUR_YELLOW2:
  case COLOUR_YELLOW3:
  case COLOUR_YELLOW4:
   return TRANS_YELLOW;
  case COLOUR_YELLOW5:
  case COLOUR_YELLOW6:
  case COLOUR_YELLOW7:
  case COLOUR_YELLOW8:
   return TRANS_YELLOW;
  case COLOUR_GREEN1:
  case COLOUR_GREEN2:
  case COLOUR_GREEN3:
  case COLOUR_GREEN4:
   return TRANS_DGREEN;
  case COLOUR_GREEN5:
  case COLOUR_GREEN6:
  case COLOUR_GREEN7:
  case COLOUR_GREEN8:
   return TRANS_LGREEN;
/*  case COLOUR_PURPLE1:
  case COLOUR_PURPLE2:
  case COLOUR_PURPLE3:
  case COLOUR_PURPLE4:
  case COLOUR_PURPLE5:
  case COLOUR_PURPLE6:
  case COLOUR_PURPLE7:
  case COLOUR_PURPLE8:
   return TRANS_PURPLE;*/

 default:
  return TRANS_WHITE;

 }

}


int base_colour(int y)
{
 switch(y % 32)
 {
  case COLOUR_GREY1:
  case COLOUR_GREY2:
  case COLOUR_GREY3:
  case COLOUR_GREY4:
  case COLOUR_GREY5:
  case COLOUR_GREY6:
  case COLOUR_WHITE:
   return COLOUR_GREY1;
  case COLOUR_ORANGE1:
  case COLOUR_ORANGE2:
  case COLOUR_ORANGE3:
  case COLOUR_ORANGE4:
   return COLOUR_ORANGE1;
  case COLOUR_BLUE1:
  case COLOUR_BLUE2:
  case COLOUR_BLUE3:
  case COLOUR_BLUE4:
   return COLOUR_BLUE1;
  case COLOUR_RED1:
  case COLOUR_RED2:
  case COLOUR_RED3:
  case COLOUR_RED4:
   return COLOUR_RED1;
  case COLOUR_YELLOW1:
  case COLOUR_YELLOW2:
  case COLOUR_YELLOW3:
  case COLOUR_YELLOW4:
   return COLOUR_YELLOW1;
  case COLOUR_GREEN1:
  case COLOUR_GREEN2:
  case COLOUR_GREEN3:
  case COLOUR_GREEN4:
   return COLOUR_GREEN1;
 }

 return COLOUR_BLACK;

}


/*
This function had to be modified from the allegro create_color_table
because the allegro version used bestfit_color, whereas we need
specific color values (eg so that there can be multiple blacks to carry
information in areas of the screen that haven't been lightsourced yet)
*/
void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal)
//void pork_create_color_table(COLOR_MAP *table, AL_CONST PALETTE pal, (int *)(blend)(AL_CONST PALETTE pal, int x, int y, RGB *rgb), void (*callback)(int pos))
{
   int x, y, z;
   RGB c;

   for (x=0; x<PAL_SIZE; x++) {
      for (y=0; y<PAL_SIZE; y++) {
   z = blend_function(x, y, &c);

//   if (rgb_map)
      table->data[x][y] = z;//rgb_map->data[c.r>>1][c.g>>1][c.b>>1];
//   else
//      table->data[x][y] = bestfit_color(pal, c.r, c.g, c.b);
      }

   }
}



void init_light_palette(void)
{

int i;


 for (i = 0; i < 48; i ++)
 {
    
    // base (ambient)
    light_palet[i + 105].r = limit_colour(((base_palette2 [i] [0] * 4) / 5) - 5);
    light_palet[i + 105].g = limit_colour(((base_palette2 [i] [1] * 4) / 5) - 5);
    light_palet[i + 105].b = limit_colour(((base_palette2 [i] [2] * 4) / 5) - 5);

    // bright
    light_palet[i + 153].r = limit_colour(base_palette2 [i] [0]);
    light_palet[i + 153].g = limit_colour(base_palette2 [i] [1]);
    light_palet[i + 153].b = limit_colour(base_palette2 [i] [2]);

    // brightest
    light_palet[i + 201].r = limit_colour(((base_palette2 [i] [0] * 5) / 4));
    light_palet[i + 201].g = limit_colour(((base_palette2 [i] [1] * 5) / 4));
    light_palet[i + 201].b = limit_colour(((base_palette2 [i] [2] * 5) / 4));
    
    
  }


 light_palet [252].r = 23;
 light_palet [252].g = 53;
 light_palet [252].b = 23;
 light_palet [255].r = 53;
 light_palet [255].g = 23;
 light_palet [255].b = 23;

 for (i = 0; i < 8; i ++)
 {

    // White - should be same as WHITE2 (below)
    light_palet[i + 1].r = limit_colour(48 + i * 2);
    light_palet[i + 1].g = limit_colour(48 + i * 2);
    light_palet[i + 1].b = limit_colour(48 + i * 2);

    // Yellow
    light_palet[i + 9].r = limit_colour(47 + i * 3);
    light_palet[i + 9].g = limit_colour(43 + i * 3);
    light_palet[i + 9].b = limit_colour(10 + i * 3);

    // LGreen
    light_palet[i + 17].r = limit_colour(i * 3);
    light_palet[i + 17].g = limit_colour(32 + i * 4);
    light_palet[i + 17].b = limit_colour(i * 3);

    // Orange
    light_palet[i + 25].r = limit_colour(47 + i * 3);
    light_palet[i + 25].g = limit_colour(20 + i * 3);
    light_palet[i + 25].b = limit_colour(5 + i * 2);

    // Red
    light_palet[i + 33].r = limit_colour(41 + i * 4);
    light_palet[i + 33].g = limit_colour(15 + i * 2);
    light_palet[i + 33].b = limit_colour(5 + i * 2);

    // DGreen
    light_palet[i + 41].r = limit_colour(i * 2);
    light_palet[i + 41].g = limit_colour(21 + i * 4);
    light_palet[i + 41].b = limit_colour(i * 2);

    // DRed
    light_palet[i + 49].r = limit_colour(20 + i * 4);
//    light_palet[i + 49].g = limit_colour(i * 2);
//    light_palet[i + 49].b = limit_colour(i * 2);
    light_palet[i + 49].g = limit_colour(5 + i * 2);
    light_palet[i + 49].b = limit_colour(1 + i * 2);

    // Purple
    light_palet[i + 57].r = limit_colour(32 + i * 4);
    light_palet[i + 57].g = limit_colour(i * 3);
    light_palet[i + 57].b = limit_colour(32 + i * 4);

    // LBlue
    light_palet[i + 65].r = limit_colour(10 + i * 3);
    light_palet[i + 65].g = limit_colour(10 + i * 3);
    light_palet[i + 65].b = limit_colour(47 + i * 2);

    // DBlue
    light_palet[i + 73].r = limit_colour(i * 2);
    light_palet[i + 73].g = limit_colour(i * 2);
    light_palet[i + 73].b = limit_colour(21 + i * 4);

    // White2 - should be same as WHITE (above)
    light_palet[i + 81].r = limit_colour(48 + i * 2);
    light_palet[i + 81].g = limit_colour(48 + i * 2);
    light_palet[i + 81].b = limit_colour(48 + i * 2);

    // LGrey
    light_palet[i + 89].r = limit_colour(24 + i * 4);
    light_palet[i + 89].g = limit_colour(24 + i * 4);
    light_palet[i + 89].b = limit_colour(24 + i * 4);

    // DGrey
    light_palet[i + 97].r = limit_colour(12 + i * 4);
    light_palet[i + 97].g = limit_colour(12 + i * 4);
    light_palet[i + 97].b = limit_colour(12 + i * 4);

 }

  light_palet[0].r = 0;
  light_palet[0].g = 0;
  light_palet[0].b = 0;

}


void init_dark_palette(void)
{

 int i;
 
 for (i = 0; i < 256; i ++)
 {
  dark_palet [i].r = light_palet [i].r;
  dark_palet [i].g = light_palet [i].g;
  dark_palet [i].b = light_palet [i].b;
  if (i >= GC_BLACK && i <= GC_BLUE8)
  {
//   if (i % 8 <= 5)
//   {
    dark_palet[i].r = 1;
    dark_palet[i].g = 1;
    dark_palet[i].b = 1;
//   }
//   if (i % 8 >= 6)// || i % 8 == 5)
//   {
//    dark_palet[i].r = limit_colour(light_palet [i].r / 2 - 2);
//    dark_palet[i].g = limit_colour(light_palet [i].g / 2 - 2);
//    dark_palet[i].b = limit_colour(light_palet [i].b / 2 - 2);
//   }
  }
  if (i >= COLOUR_BLACK && i <= COLOUR_BLUE8)
  {
    dark_palet[i].r = limit_colour(((light_palet[i].r * 4) / 5) - 3);
    dark_palet[i].g = limit_colour(((light_palet[i].g * 4) / 5) - 3);
    dark_palet[i].b = limit_colour(((light_palet[i].b * 4) / 5) - 3);
  }  
 }
}


void set_dark_palette(void)
{
 vsync();
 set_palette(dark_palet);
}

void set_light_palette(void)
{
 vsync();
 set_palette(light_palet);
}

