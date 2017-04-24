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
    
File: cloud.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - functions relating to clouds and transient special effects

*/
#include "config.h"
#include "allegro.h"

#include <math.h>

#include "globvars.h"

#include "stuff.h"
#include "palette.h"

#include "sound.h"
#include "light.h"
#include "actor.h"
#include "display.h"

void manage_cloud(int cl);
int move_cloud(int mc, char limited);

void destroy_cloud(int c);
void drag_cloud(int i, float dragged);

void simple_cloud(int colour, int x, int y, int xs, int ys, int size);
void simple_cloud_trans(int colour, int x, int y, int xs, int ys, int size);
void place_explosion_with_smoke(int x, int y, int xs, int ys, int size, int colours [5]);
void place_explosion(int x, int y, int xs, int ys, int size, int colours [5]);
void place_burstlet_burst(int x, int y, int xs, int ys, int burstlets, int min_timeout, int random_timeout, int burst, int random_burst, int pretravel, int colours [5]);

void place_bang(int x, int y, int xs, int ys, int size, int btype);

int create_cloud_minimum(int type, int x, int y, int x2, int y2,
int x_speed, int y_speed, int timer, int tickrate,
int delta_tickrate, int angle,
int status, unsigned char seed, int minimum, int colours [5]);


void init_clouds(void)
{

 int c;

 for (c = 0; c < NO_CLOUDS; c++)
 {
  cloud[c].type = CLOUD_NONE;
 }

}

// should delta_tickrate be unsigned?
int create_cloud(int type, int x, int y, int x2, int y2,
int x_speed, int y_speed, int timer, int tickrate,
int delta_tickrate, int angle,
int status, unsigned char seed, int colours [5])
{

   int c = 0;

   for (c = 1; c < NO_CLOUDS; c++)
   {
    if (c == NO_CLOUDS - 1) return -1;
    if (cloud[c].type == CLOUD_NONE) break;
   }

   cloud[c].type = type;
   cloud[c].x = x;// + (x_speed - actor[owner].x_speed);
   cloud[c].y = y;// + (y_speed - actor[owner].y_speed);
   cloud[c].x2 = x2;
   cloud[c].y2 = y2;
   cloud[c].x_speed = x_speed;
   cloud[c].y_speed = y_speed;
   cloud[c].seed = seed;
   cloud[c].timeout = timer;
   cloud[c].tickrate= tickrate;
   cloud[c].delta_tickrate= delta_tickrate;
   cloud[c].status = status;
   cloud[c].angle = angle;
   cloud[c].colour [0] = colours [0];
   cloud[c].colour [1] = colours [1];
   cloud[c].colour [2] = colours [2];
   cloud[c].colour [3] = colours [3];
   cloud[c].colour [4] = colours [4];
   
   return c;

}

// just like create_cloud, but sets a minimum for the index.
//  use it for clouds that need to overwrite other clouds on the display (eg dead trianglers)
int create_cloud_minimum(int type, int x, int y, int x2, int y2,
int x_speed, int y_speed, int timer, int tickrate,
int delta_tickrate, int angle,
int status, unsigned char seed, int minimum, int colours [5])
{

   int c = 0;

   for (c = minimum; c < NO_CLOUDS; c++)
   {
    if (c == NO_CLOUDS - 1) return -1;
    if (cloud[c].type == CLOUD_NONE) break;
   }

   cloud[c].type = type;
   cloud[c].x = x;// + (x_speed - actor[owner].x_speed);
   cloud[c].y = y;// + (y_speed - actor[owner].y_speed);
   cloud[c].x2 = x2;
   cloud[c].y2 = y2;
   cloud[c].x_speed = x_speed;
   cloud[c].y_speed = y_speed;
   cloud[c].seed = seed;
   cloud[c].timeout = timer;
   cloud[c].tickrate= tickrate;
   cloud[c].delta_tickrate= delta_tickrate;
   cloud[c].status = status;
   cloud[c].angle = angle;
   cloud[c].colour [0] = colours [0];
   cloud[c].colour [1] = colours [1];
   cloud[c].colour [2] = colours [2];
   cloud[c].colour [3] = colours [3];
   cloud[c].colour [4] = colours [4];
   
   return c;

}



void run_clouds(void)
{

 int c;

 for (c = 1; c < NO_CLOUDS; c++)
 {
  if (cloud[c].type != CLOUD_NONE) manage_cloud(c);
 }

}


void manage_cloud(int c)
{

 int passing_colours [5];

//   int x_gain = 0;
//   int y_gain = 0;
//   char angle_change;

//   if (cloud[c].timeout > 0)
//   {
    cloud[c].timeout -= cloud[c].tickrate;
    cloud[c].tickrate += cloud[c].delta_tickrate;
//   }

   if (cloud[c].timeout <= 0 || cloud[c].timeout >= 10000) // 10000 is sanity check - unfortunately necessary
   {
    destroy_cloud(c);
    return;
   }

   switch(cloud[c].type)
   {
    case CLOUD_PULSER1_V: 
    case CLOUD_PULSER1_H: 
    case CLOUD_PULSER2_V: 
    case CLOUD_PULSER2_H: 
     if (cloud[c].timeout <= 2)
     {
       shake_all_screens(3 + grand(5));
       place_explosion(
          cloud[c].x + (11 * GRAIN), cloud[c].y + (11 * GRAIN),
          0, 0, 300, cloud[c].colour);
       place_burstlet_burst(cloud[c].x, cloud[c].y, 
       0, 0, 4 + grand(3), 2, 5, 1200, 500,
        4, cloud[c].colour);
            play_wav_pos(NWAV_MINEBANG, 200 + grand(200), 200, cloud[c].x, cloud[c].y);
       destroy_cloud(c);
      return;
     }
     break;
    case CLOUD_SQUAREY:
     cloud[c].angle -= cloud[c].status;
     cloud[c].x = cloud[c].x2 + xpart(cloud[c].angle, 30);
     cloud[c].y = cloud[c].y2 + ypart(cloud[c].angle, 30);
     if (cloud[c].timeout <= 2)
     {
       shake_all_screens(3 + grand(5));
       place_explosion(
          cloud[c].x + xpart(cloud[c].angle, 30000), cloud[c].y + ypart(cloud[c].angle, 30000),
          0, 0, 300, cloud[c].colour);
       place_burstlet_burst(cloud[c].x + xpart(cloud[c].angle, 30000), cloud[c].y + ypart(cloud[c].angle, 30000), 
       0, 0, 4 + grand(3), 2, 5, 1200, 500,
        4, cloud[c].colour);

//void place_burstlet_burst(int x, int y, int xs, int ys, int burstlets, int min_timeout, int random_timeout, int burst, int random_burst, int pretravel, int colours [5])


            play_wav_pos(NWAV_MINEBANG, 200 + grand(200), 200, cloud[c].x, cloud[c].y);
        
      destroy_cloud(c);
      return;
     }
     break;
/*    case CLOUD_TRI1: 
    case CLOUD_TRI2: 
    case CLOUD_TRI3: 
     drag_cloud(c, .05);
     if (cloud[c].timeout % 2 == 0)
     {
       int cangle = grand(ANGLE_FULL);
       int cx = cloud[c].x + xpart(cangle, grand(cloud[c].status));
       int cy = cloud[c].y + ypart(cangle, grand(cloud[c].status));
       passing_colours [0] = cloud[c].colour [1];
       create_cloud_minimum(CLOUD_MED_TRANS_CIRCLE,
        cx, cy, 0, 0, cloud[c].x_speed, cloud[c].y_speed,
        200 + grand(350),0,2, 0, 0, 0, c, passing_colours);
       passing_colours [0] = cloud[c].colour [2];
       create_cloud_minimum(CLOUD_MED_TRANS_CIRCLE,
        cx, cy, 0, 0, cloud[c].x_speed, cloud[c].y_speed,
        300 + grand(450),0,6, 0, 0, 0, c, passing_colours);
     }
     if (cloud[c].timeout <= 2)
     {
       shake_all_screens(7 + grand(8));
       place_explosion(
          cloud[c].x, cloud[c].y,
          0, 0, 1900, cloud[c].colour);
       place_burstlet_burst(cloud[c].x, cloud[c].y, 
        0, 0, 4 + grand(3), 2, 6, 2000, 2500,
        4, cloud[c].colour);
       play_wav_pos(NWAV_BANG, 700, 200, cloud[c].x, cloud[c].y);
       destroy_cloud(c);
      return;
     }
     break;*/
    case CLOUD_LIGHT:
     add_light(LIGHT_NORMAL, cloud[c].timeout / 40, cloud[c].x, cloud[c].y);
     break;
    case CLOUD_FLAME_CIRCLE: 
     drag_cloud(c, .2);
     break;
    case CLOUD_BIG_BLAST_CIRCLE: 
     drag_cloud(c, .1);
     break;
    case CLOUD_BLAST_CIRCLE:
     drag_cloud(c, .2);
     break;
    case CLOUD_SEEKER_CIRCLE: 
     drag_cloud(c, .2);
     break;
    case CLOUD_JET_CIRCLE1: 
    case CLOUD_JET_CIRCLE2:
    case CLOUD_DRIVE_CIRCLE:
     drag_cloud(c, .02);
     break; 
    case CLOUD_BURSTLET:
//     place_explosion(cloud[c].x, cloud[c].y, 0, 0, , cloud[c].colours);
     passing_colours [0] = cloud[c].colour [1]; // was [0]
   
     create_cloud(CLOUD_MED_TRANS_CIRCLE,
      cloud[c].x, cloud[c].y, 0, 0, 0, 0,
      100 + grand(450),0,2, 0, 0, 0, passing_colours);
//      50 + grand(300),0,3, 0, 0, 0, passing_colours);
//     passing_colours [0] = cloud[c].colour [1];

//     create_cloud(CLOUD_MED_TRANS_CIRCLE,
//      cloud[c].x, cloud[c].y, 0, 0, 0, 0, size,-40,15, 0, 0, 0, passing_colours);

     passing_colours [0] = cloud[c].colour [2];

     create_cloud(CLOUD_MED_TRANS_CIRCLE,
      cloud[c].x, cloud[c].y, 0, 0, 0, 0,
      50 + grand(300),0,6, 0, 0, 0, passing_colours);
     break;
    case CLOUD_SPAWNER:
// the tickrate of a spawner must be a factor of the seed    
     if (cloud[c].timeout % cloud[c].seed == 0)
     {
       int cangle = grand(ANGLE_FULL);
       int cx = cloud[c].x + xpart(cangle, grand(cloud[c].status * 100));
       int cy = cloud[c].y + ypart(cangle, grand(cloud[c].status * 100));
       shake_all_screens(grand(cloud[c].status) / 200);
       place_explosion(
          cx, cy,
          0, 0, grand(cloud[c].status) + 250, cloud[c].colour);
//       if (cloud[c].timeout % 48 == 0)
//        play_sound_pos(WAV_MINEBANG, 200 + grand(200), 200, cx, cy);
       if (cloud[c].timeout % 48 == 0)
        play_wav_pos(NWAV_BANG, 400 + grand(200), 200, cx, cy);

     }
     break;
   }

   if (move_cloud(c, 1) == 0)
    return;

 drag_cloud(c, game.drag);

}


void destroy_cloud(int c)
{
 cloud[c].type = CLOUD_NONE;
}


int move_cloud(int c, char limited)
{


if (cloud[c].x_speed == 0 && cloud[c].y_speed == 0) return 1;


cloud[c].x2 += cloud[c].x_speed;
cloud[c].y2 += cloud[c].y_speed;

cloud[c].x += cloud[c].x_speed;
cloud[c].y += cloud[c].y_speed;

return 1;

}




void drag_cloud(int i, float dragged)
{

 cloud[i].x_speed *= 1 - dragged;
 cloud[i].y_speed *= 1 - dragged;

}

void simple_cloud(int colour, int x, int y, int xs, int ys, int size)
{

   int passing_colours [5];

   passing_colours [0] = colour;
   
   create_cloud(CLOUD_MED_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-30,10, 0, 0, 0, passing_colours);


}

void simple_cloud_trans(int colour, int x, int y, int xs, int ys, int size)
{

   int passing_colours [5];

   passing_colours [0] = colour;
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-30,10, 0, 0, 0, passing_colours);


}

void simple_light(int x, int y, int xs, int ys, int size)
{

   int passing_colours [5];

   create_cloud(CLOUD_LIGHT,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-30,10, 0, 0, 0, passing_colours);

}

void simple_shockwave(int colour, int x, int y, int xs, int ys, int size, int speed)
{

  int passing_colours [5];

  switch(colour)
  {
   default:
   case TRANS_WHITE2:
    passing_colours [3] = TRANS_WHITE2;
    passing_colours [2] = TRANS_LGREY;
    passing_colours [1] = TRANS_LGREY;
    passing_colours [0] = TRANS_DGREY;
    break;
   case TRANS_YELLOW:
    passing_colours [3] = TRANS_YELLOW;
    passing_colours [2] = TRANS_ORANGE;
    passing_colours [1] = TRANS_LRED;
    passing_colours [0] = TRANS_DRED;
    break;
   case TRANS_PURPLE:
    passing_colours [3] = TRANS_PURPLE;
    passing_colours [2] = TRANS_LBLUE;
    passing_colours [1] = TRANS_LBLUE;
    passing_colours [0] = TRANS_DBLUE;
    break;
   case TRANS_LGREEN:
    passing_colours [3] = TRANS_YELLOW;
    passing_colours [2] = TRANS_LGREEN;
    passing_colours [1] = TRANS_DGREEN;
    passing_colours [0] = TRANS_DGREEN;
    break;
  }
   
   create_cloud(CLOUD_SHOCKWAVE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,speed,0, 0, size + 20 + grand(50), 0, passing_colours);


}

void place_bang(int x, int y, int xs, int ys, int size, int btype)
{

   int passing_colours [5];

   create_cloud(CLOUD_BANG,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-20,3, 0, btype, 0, passing_colours);

}

void place_explosion(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-100,7, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-120,10, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-140,15, 0, 0, 0, passing_colours);

   create_cloud(CLOUD_LIGHT,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-100,7, 0, 0, 0, passing_colours);


}


void place_explosion_no_light(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-100,7, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-120,10, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-140,15, 0, 0, 0, passing_colours);


}

void place_small_explosion(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-15,3, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-20,5, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-23,8, 0, 0, 0, passing_colours);

   create_cloud(CLOUD_LIGHT,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-40,7, 0, 0, 0, passing_colours);


}



void place_explosion_with_smoke(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];

   passing_colours [0] = TRANS_LGREY;

/*   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-30,1, 0, 0, 0, passing_colours);*/

   create_cloud(CLOUD_LIGHT,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-100,7, 0, 0, 0, passing_colours);


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-100,7, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-105,10, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-110,15, 0, 0, 0, passing_colours);

}

void place_rocket_trail(int x, int y, int xs, int ys, int size, int colours [5])
{

   int passing_colours [5];


   passing_colours [0] = colours [0];
   
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-10,2, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [1];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-15,4, 0, 0, 0, passing_colours);

   passing_colours [0] = colours [2];

   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    x,
    y,
    0, 0,
    xs,
    ys,
    size,-20,15, 0, 0, 0, passing_colours);

}


void place_speck_burst(int x, int y, int xs, int ys, int specks, int min_timeout, int random_timeout, int scatter, int colours [5])
{

  int i;

  for (i = 0; i < specks; i ++)
  {
   create_cloud(CLOUD_SPECK,
    x,
    y,
    0, 0,
    xs + grand(scatter * 2 + 1) - scatter,
    ys + grand(scatter * 2 + 1) - scatter,
    min_timeout + grand(random_timeout), 1,0,0, 0, 0, colours);
  }

}

void place_line_burst(int x, int y, int xs, int ys, int specks, int min_timeout, int random_timeout, int burst, int random_burst, int colours [5])
{

  int i, angle, xsp, ysp, burst_speed;

  for (i = 0; i < specks; i ++)
  {
   angle = grand(ANGLE_FULL);
   burst_speed = burst + grand(burst);
   xsp = xs + xpart(angle, burst_speed);
   ysp = ys + ypart(angle, burst_speed);
   x += xsp;
   y += ysp;
   create_cloud(CLOUD_SHRINKING_LINE,
    x,
    y,
    0, 0,
    xsp,
    ysp,
    (min_timeout + grand(random_timeout)) * 10, 10 + grand(6),0,angle, 0, 0, colours);
  }

}


void place_burstlet_burst(int x, int y, int xs, int ys, int burstlets, int min_timeout, int random_timeout, int burst, int random_burst, int pretravel, int colours [5])
{

int i, xs2, ys2, x2, y2;

  for (i = 0; i < burstlets; i ++)
  {
   xs2 = burst + grand(random_burst);
   ys2 = burst + grand(random_burst);
   if (grand(2) == 0)
    xs2 *= -1;
   if (grand(2) == 0)
    ys2 *= -1;
   xs2 += xs;
   ys2 += ys;
   x2 = x + xs2 * pretravel; // or it will just be masked by the main
   y2 = y + ys2 * pretravel; //  explosion

   create_cloud(CLOUD_BURSTLET,
    x2, y2, 0,0, xs2, ys2,
    min_timeout + random_timeout,1,0, 0, 0, 0, colours);
  }


}

// called at start of each level and once per tick
void init_effects(void)
{
 int i;
 
 for (i = 0; i < MAX_EFFECTS; i ++)
 {
  effect[i].type = EFFECT_NONE;
 }

}


int create_effect(int type, int x, int y, int x1, int y1, int x2, int y2, int att1, int att2, int att3, int att4, int att5)
{

 int i;
 
 for (i = 0; i < MAX_EFFECTS + 1; i ++)
 {
  if (i >= MAX_EFFECTS)
   return 0;
  if (effect[i].type == EFFECT_NONE)
   break;
 }

 effect[i].type = type;
 effect[i].x = x;
 effect[i].y = y;
 effect[i].x1 = x1;
 effect[i].y1 = y1;
 effect[i].x2 = x2;
 effect[i].y2 = y2;
 effect[i].things [0] = att1;
 effect[i].things [1] = att2;
 effect[i].things [2] = att3;
 effect[i].things [3] = att4;
 effect[i].things [4] = att5;
 return 1;
}





