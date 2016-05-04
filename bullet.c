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
    
File: bullet.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - functions relating to bullets and dangerous projectiles

*/

#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvars.h"

#include "stuff.h"
#include "cloud.h"
#include "enemy.h"
#include "actor.h"
#include "sound.h"
#include "light.h"
#include "stuff.h"
#include "display.h"

#include "palette.h"

#ifdef SANITY_CHECK
extern FONT *small_font;
#endif

extern int var1, var2, var3; // debug display vars, externed as necessary


int create_bullet(int type, int x, int y,
int x_speed, int y_speed, int owner,
int damage, int timer, int mass, int angle,
int status, int size, int colours [4], int speed_div,
int special1, int special2, int special3, int special4, int special5);


void manage_bullet(int b);
int move_bullet(int mbull, char limited);
int detect_collision(int b, int things [2]);
int detect_enemy_collision(int b, int things [2]);
void bullet_impact(int b, int k, int hit_what);

void destroy_bullet(int b);
//void apply_force_to_actor(int a, int fx, int fy);
//void apply_force_to_enemy(int e, int fx, int fy);

void blast(int x, int y, int radius, int damage, int force, int owner);

void drag_bullet(int b, float drag_amount);

extern int inflicteda, inflictede;

int enemy_bullet_track_target(int b, int attacking, int turning);

int closest_target(int x, int y);
int nearby_target(int range, int x, int y);

void bullet_sound(int b, int sound);
void bullet_soundf(int b, int sound, int freq);
void bullet_soundvf(int b, int sound, int vol, int freq);
void enemy_sparks(int e, int b, int bx, int by, int bxs, int bys, int damage);

void shield_line(int e, int bx, int by);
void shielder_pulse(int e, int dam);
int check_shielder(int e);

void line_blast(int x, int y, int type, int target);
void teleport_actor(int a);
int clear_space(int x, int y, int rad);
int radius_effect(int x, int y, int radius, int effect);

void actor_in_beam(int i, int angle, int damage, int force, int colour1, int colour2);

enum
{
RADEFFECT_TELEPORT,
RADEFFECT_MAX
};

enum
{
BSOUND_BOMB,
BSOUND_MISSILE,
BSOUND_MINE,
BSOUND_SEEKMINE,
BSOUND_ORBITAL,
BSOUND_SEEKBLOB_BANG1,
BSOUND_SEEKBLOB_BANG2,
BSOUND_TUBE,
BSOUND_MINE2,
BSOUND_HIT
};

enum
{
HIT_NOTHING,
HIT_ACTOR,
HIT_ENEMY,
HIT_EDGE,
HIT_WAVE
};


void init_bullets(void)
{

 int b;

 for (b = 0; b < NO_BULLETS; b++)
 {
  bullet[b].type = BULLET_NONE;
 }

}

/*
If this function is crashing mysteriously, it's probably being called
with a zero value for speed_div.
*/
int create_bullet(int type, int x, int y,
int x_speed, int y_speed, int owner,
int damage, int timer, int mass, int angle,
int status, int size, int colours [4], int speed_div,
int special1, int special2, int special3, int special4, int special5)
{

#ifdef SANITY_CHECK
if (type <= BULLET_NONE || speed_div <= 0)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "create_bullet %i %i", type, speed_div);
     rest(1000);
//  exit(type);
    } 
#endif    


    if (owner < -1 || owner >= NO_ACTORS)
     return -1;

   int b = 0;

   for (b = 1; b < NO_BULLETS; b++)
   {
/*    if ((bullet_type == BULLET_SPARK || bullet_type == BULLET_SPECK)
         && bcounter > MAX_BULLETS / 2)
          return -1; // these bullets just for decoration*/
    if (b == NO_BULLETS - 1) return -1;
    if (bullet[b].type == BULLET_NONE) break;
   }

   bullet[b].type = type;
//   bullet[b].x = x + (x_speed - actor[owner].x_speed);
//   bullet[b].y = y + (y_speed - actor[owner].y_speed);
   if (owner == OWNER_ENEMY)
   {
     bullet[b].x = x + (x_speed / speed_div);
     bullet[b].y = y + (y_speed / speed_div);
   }
    else
    {
     bullet[b].x = x + (x_speed - actor[owner].x_speed) / speed_div;
     bullet[b].y = y + (y_speed - actor[owner].y_speed) / speed_div;
//     bullet[b].x = x + (x_speed) / speed_div;
//     bullet[b].y = y + (y_speed) / speed_div;
    } 
   bullet[b].x2 = x;
   bullet[b].y2 = y;
   bullet[b].x_speed = x_speed;
   bullet[b].y_speed = y_speed;
   bullet[b].owner = owner;
   bullet[b].size = size;
   bullet[b].timeout = timer;
//   bullet[b].status = status;
   bullet[b].damage = damage;
   bullet[b].left_owner = 0;
   bullet[b].mass = mass;
   bullet[b].angle = angle;
   bullet[b].colours [0] = colours [0];
   bullet[b].colours [1] = colours [1];
   bullet[b].colours [2] = colours [2];
   bullet[b].colours [3] = colours [3];
   bullet[b].special1 = special1;
   bullet[b].special2 = special2;
   bullet[b].special3 = special3;
   bullet[b].special4 = special4;
   bullet[b].special5 = special5;

   switch(type)
   { 
    case BULLET_PANIC_EEL:
     if (grand(2) == 0)
      bullet[b].special2 *= -1;
      bullet[b].special3 = 3 + grand(2);
      break;
    case BULLET_CURVE:
     if (grand(2) == 0)
      bullet[b].angle = angle + ANGLE_HALF;
//       else
//        bullet[b].angle = angle - ANGLE_QUARTER;
     bullet[b].angle %= ANGLE_FULL;
     if (grand(10) == 0)
       bullet[b].angle = grand(ANGLE_FULL);
     break;
    case BULLET_TORPEDO2:
     bullet[b].x_speed = 0;
     bullet[b].y_speed = 0;
     break;

   }
   
   return b;

}



void run_bullets(void)
{

 int b;

 for (b = 1; b < NO_BULLETS; b++)
 {
  if (bullet[b].type != BULLET_NONE) manage_bullet(b);
 }

}


void manage_bullet(int b)
{

//   int x_gain = 0;
//   int y_gain = 0;
//   char angle_change;

   int cx, cy, xa, ya, xb;
   int c2x, c2y;

   int passing_colour [5];

   if (bullet[b].left_owner < 100)
    bullet[b].left_owner ++;
   if (bullet[b].timeout > 0)
   {
    bullet[b].timeout --;
    if (bullet[b].timeout <= 0)
    {
     bullet_impact(b, -1, HIT_NOTHING);
     return;
    }
   }

   switch(bullet[b].type)
   {
    case BULLET_TOXIC_SUN:
     drag_bullet(b, 0.08);
     bullet[b].special2 --;
     if (bullet[b].special2 <= 0)
     {
      cx = bullet[b].angle + grand(75) - grand(75) - ANGLE_QUARTER;
      create_bullet(BULLET_TOXIC_FLARE, 
       bullet[b].x - xpart(cx, bullet[b].special4), bullet[b].y - ypart(cx, bullet[b].special4),
       xpart(cx, bullet[b].special4 + grand(3000)), ypart(cx, bullet[b].special4 + grand(3000)), 
       bullet[b].owner,
       bullet[b].special1, 30 + grand(10), 10, cx, 0, 0, bullet[b].colours, 1, 
       0, 0, 0, 0, 0);
      bullet[b].special2 = 3;
      bullet_soundvf(b, NWAV_JET, 100, 300 + grand(100) + bullet[b].special4 / 500); 
     }
     passing_colour [0] = TRANS_YELLOW; //bullet[b].colours [0];
     passing_colour [1] = TRANS_LGREEN; //bullet[b].colours [1];
     passing_colour [2] = TRANS_DGREEN; //bullet[b].colours [2];
     passing_colour [3] = TRANS_DGREY; //bullet[b].colours [3];
      cx = grand(ANGLE_FULL);
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + xpart(cx, 4000 + grand(7000)),
       bullet[b].y + ypart(cx, 4000 + grand(7000)),
       0, 0, 
       bullet[b].x_speed, bullet[b].y_speed,
       25 + grand(10), 1, 1, 0, 0, 0, passing_colour);
     break;
    case BULLET_TOXIC_FLARE:
     passing_colour [0] = TRANS_YELLOW; //bullet[b].colours [0];
     passing_colour [1] = TRANS_LGREEN; //bullet[b].colours [1];
     passing_colour [2] = TRANS_DGREEN; //bullet[b].colours [2];
     passing_colour [3] = TRANS_DGREY; //bullet[b].colours [3];
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + grand(5001) - 2500,
       bullet[b].y + grand(5001) - 2500,
       0, 0, 
       bullet[b].x_speed, bullet[b].y_speed,
       25 + grand(10), 1, 1, 0, 0, 0, passing_colour);
     break;
//    case BULLET_NUMEROUS_BLADE:
    case BULLET_SLIVER:
//     drag_bullet(b, 0.03);
     bullet[b].special1 += bullet[b].special2;
     bullet[b].special1 &= 1023;     
     break;
    case BULLET_EVIL_STAR: 
    case BULLET_FROZEN_STAR:
     drag_bullet(b, 0.05);
//     add_light(LIGHT_NORMAL, 2 + grand(3), bullet[b].x + bullet[b].x_speed, bullet[b].y + bullet[b].y_speed);
     break;
    case BULLET_ZAP_DRAG:
     drag_bullet(b, 0.06);
     if (bullet[b].special1 < 2)
     {
      bullet[b].x2 = bullet[b].x;
      bullet[b].y2 = bullet[b].y;
      bullet[b].special1 ++;
     }
      else
      {
       bullet[b].x2 = bullet[b].x - bullet[b].x_speed * 2;
       bullet[b].y2 = bullet[b].y - bullet[b].y_speed * 2;
      }
     if (abs(bullet[b].x_speed) + abs(bullet[b].y_speed) > 500)
      bullet[b].timeout = 3;
     break; 
    case BULLET_ICE_DART:
    case BULLET_ICE_DART_SMALL:
     drag_bullet(b, 0.09);
     break;
    case BULLET_SNOW_DART:
    case BULLET_SNOW_DART_SMALL:
     if (grand(3) == 0)
     {
      passing_colour[0] = COLOUR_GREY5;
      passing_colour[1] = COLOUR_BLUE8;
      passing_colour[2] = COLOUR_BLUE6;
      passing_colour[3] = COLOUR_BLUE4;
      passing_colour[4] = COLOUR_BLUE2;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(2001) - 1000, bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 24, 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.09);
     break;
    case BULLET_FROZEN_BREATH:
     if (grand(2) == 0)
      create_cloud(CLOUD_BLUE_BLOB,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 2 + crandom(10),1 + grand(2),0, 0, 0, 0, passing_colour);
      drag_bullet(b, 0.09);
      break; 
    case BULLET_BURNING_DRAGGED:
     drag_bullet(b, 0.04);
    case BULLET_BURNING_SPIRIT:
     if (grand(2) == 0)
      create_cloud(CLOUD_ORANGE_BLOB,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 2 + crandom(10),1 + grand(2),0, 0, 0, 0, passing_colour);
//      add_light(LIGHT_NORMAL, 7, bullet[b].x + bullet[b].x_speed, bullet[b].y + bullet[b].y_speed);
      break; 
    case BULLET_BURST:
      create_cloud(CLOUD_ORANGE_BLOB,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 2 + crandom(10),1 + grand(2),0, 0, 0, 0, passing_colour);
      break; 
    case BULLET_PUFFY3:
      create_cloud(CLOUD_GREEN_BLOB,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 2 + crandom(10),1 + grand(2),0, 0, 0, 0, passing_colour);
      break; 
    case BULLET_PUFFY4:
      create_cloud(CLOUD_BLUE_BLOB,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 2 + crandom(10),1 + grand(2),0, 0, 0, 0, passing_colour);
      break; 
    case BULLET_GREEN_BLAT:
    case BULLET_BLUE_BLAT:
/*     if (grand(2) == 0)
      create_cloud(CLOUD_GREEN_BLOB,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 2 + crandom(10),1 + grand(2),0, 0, 0, 0, passing_colour);*/
     drag_bullet(b, 0.07);
      break; 
   case BULLET_FLOWER:
     drag_bullet(b, 0.1);
      break; 
   case BULLET_PETAL1:
    add_light(LIGHT_NORMAL, 12 + (bullet[b].special2 / 3) + grand(4), bullet[b].x, bullet[b].y);
    drag_bullet(b, 0.03);
    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + grand(6000) - 3000,
       bullet[b].y + grand(6000) - 3000,
       0, 0,
       bullet[b].x_speed,
       bullet[b].y_speed,
       30 + grand(20), 3, 0, 0, 10 + grand(10), 0, passing_colour);
     break;
    case BULLET_WORM_SORROW:
    case BULLET_WORM_AGONY:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
       if ((bullet[b].timeout == 143 + bullet[b].special5)|| (bullet[b].special3 == -1 && bullet[b].timeout % 5 == 0))
       {
          if (game.type == GAME_DUEL)
          {
             bullet[b].special3 = closest_enemy_or_actor(bullet[b].owner, bullet[b].x, bullet[b].y,
                     500000, 1);
          }
           else
             bullet[b].special3 = closest_enemy(bullet[b].x, bullet[b].y, 1000000, 1);
          if (bullet[b].timeout == 143)   
           bullet[b].timeout -= grand(10);
       }
       
     if (bullet[b].special3 == LOCK_ACTOR0 || bullet[b].special3 == LOCK_ACTOR1)
     {
     
      if (bullet[b].special3 == LOCK_ACTOR0)
       cx = 0;
        else
         cx = 1;
      if (hypot(bullet[b].y - actor[cx].y, bullet[b].x - actor[cx].x)
           < 30000)
//          < 5000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x, actor[cx].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x + (actor[cx].x_speed * bullet[b].special5), actor[cx].y + (actor[cx].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
     }
      else
       if (bullet[b].special3 >= 0)
       {
        // destroyed enemies have locks cleared in register_destroyed (enemy.c)
        if (hypot(bullet[b].y - enemy[bullet[b].special3].y, bullet[b].x - enemy[bullet[b].special3].x)
            < 30000)
//          < 5000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x, enemy[bullet[b].special3].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x + (enemy[bullet[b].special3].x_speed * bullet[b].special5), enemy[bullet[b].special3].y + (enemy[bullet[b].special3].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
// What this does: if missile is close to enemy, heads straight for it.
//  Otherwise, leads it slightly (to an extent specified by bullet[b].special5)
//  For this reason, enemy[].x_speed must be set correctly even for enemies for whom it's irrelevant
//  (eg crawlers). <--- old lacewing stuff
        if ((enemy[bullet[b].special3].type == ENEMY_SHADOW1 || enemy[bullet[b].special3].type == ENEMY_SHADOW2)
            && enemy[bullet[b].special3].attribute [0] == 0)
             bullet[b].special3 = -1;
       }
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
//     drag_bullet(b, 0.07);
     if (bullet[b].type == BULLET_WORM_SORROW) 
      drag_bullet(b, 0.24); // 14
       else
        drag_bullet(b, 0.11);
     break;
    case BULLET_CLAW:
    if (bullet[b].special3 > 0)
    {
       passing_colour [0] = TRANS_DGREY;
       passing_colour [1] = TRANS_DGREEN;
       passing_colour [2] = TRANS_LGREEN;
       passing_colour [3] = TRANS_YELLOW;
       bullet[b].special3 --;
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
    } 
     else
     {
       passing_colour [0] = TRANS_DRED;
       passing_colour [1] = TRANS_LRED;
       passing_colour [2] = TRANS_ORANGE;
       passing_colour [3] = TRANS_YELLOW;
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
      cx = xpart(bullet[b].special2 - ANGLE_QUARTER, GRAIN);
      cy = ypart(bullet[b].special2 - ANGLE_QUARTER, GRAIN);
      c2x = (cx * bullet[b].special1) / GRAIN;
      c2y = (cy * bullet[b].special1) / GRAIN;
      bullet[b].x_speed += c2x;
      bullet[b].y_speed += c2y;
     }
       drag_bullet(b, 0.1); // 14
       break;
    case BULLET_SPORE:
       if (bullet[b].special3 == -1 && bullet[b].timeout % 5 == 0)
       {
          if (game.type == GAME_DUEL)
          {
             bullet[b].special3 = closest_enemy_or_actor(bullet[b].owner, bullet[b].x, bullet[b].y,
                     500000, 1);
          }
           else
             bullet[b].special3 = closest_enemy(bullet[b].x, bullet[b].y, 1000000, 1);
       }
       
     if (bullet[b].special3 == LOCK_ACTOR0 || bullet[b].special3 == LOCK_ACTOR1)
     {
     
      if (bullet[b].special3 == LOCK_ACTOR0)
       cx = 0;
        else
         cx = 1;
      if (hypot(bullet[b].y - actor[cx].y, bullet[b].x - actor[cx].x)
           < 30000)
//          < 5000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x, actor[cx].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x + (actor[cx].x_speed * bullet[b].special5), actor[cx].y + (actor[cx].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
        if ((enemy[bullet[b].special3].type == ENEMY_SHADOW1 || enemy[bullet[b].special3].type == ENEMY_SHADOW2)
            && enemy[bullet[b].special3].attribute [0] == 0)
             bullet[b].special3 = -1;
     }
      else
       if (bullet[b].special3 >= 0)
       {
        // destroyed enemies have locks cleared in register_destroyed (enemy.c)
        if (hypot(bullet[b].y - enemy[bullet[b].special3].y, bullet[b].x - enemy[bullet[b].special3].x)
            < 30000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x, enemy[bullet[b].special3].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x + (enemy[bullet[b].special3].x_speed * bullet[b].special5), enemy[bullet[b].special3].y + (enemy[bullet[b].special3].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
       }
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.1);
     break;
    case BULLET_ATTRACTOR_LINE:
    case BULLET_CHARGE_LINE:
    case BULLET_HOLE_LINE:
       if (bullet[b].special2 != -1)
       {
        c2y = 1; // if 0, bullet isn't attracted.
        if (bullet[b].type == BULLET_ATTRACTOR_LINE)
        {
         if (enemy[bullet[b].special2].type != ENEMY_ATTRACTOR)
          c2y = 0;
         cx = enemy[bullet[b].special2].x;
         cy = enemy[bullet[b].special2].y;
         c2x = 15000;
        }
        if (bullet[b].type == BULLET_CHARGE_LINE)
        {
         if (enemy[bullet[b].special2].type != ENEMY_BOSS2_2)
          c2y = 0;
         cx = enemy[bullet[b].special2].x + xpart(enemy[bullet[b].special2].angle, 35000);
         cy = enemy[bullet[b].special2].y + ypart(enemy[bullet[b].special2].angle, 35000);
         c2x = 7000;
        }
        if (bullet[b].type == BULLET_HOLE_LINE)
        {
         if (bullet[bullet[b].special2].type != BULLET_HOLE)
          c2y = 0;
         cx = bullet[bullet[b].special2].x;
         cy = bullet[bullet[b].special2].y;
         c2x = 7000;
        }
        if (c2y == 1)
        {
//             {
                  xb = hypot(bullet[b].y - cy, bullet[b].x - cx) + 1;
                  if (xb < c2x)
                  {
                   destroy_bullet(b);
                   return; // hmmm.
                  }
                  if (TRUE) //xb < 300000)
                  {
                   xa = radians_to_angle(atan2(bullet[b].y - cy, bullet[b].x - cx));
                   ya = 120000 / xb;
                   if (ya > 100)
                       ya = 100;
                   bullet[b].x_speed -= xpart(xa, ya * GRAIN / 3);
                   bullet[b].y_speed -= ypart(xa, ya * GRAIN / 3);
/*                   cx = xpart(xa, ya * GRAIN);
                   cy = ypart(xa, ya * GRAIN);
                   c2x = (cx * bullet[b].special1) / GRAIN;
                   c2y = (cy * bullet[b].special1) / GRAIN;
                   bullet[b].x_speed += c2x;
                   bullet[b].y_speed += c2y;*/
                  }
//             }
//              else
//               bullet[b].special2 = -1; 
        }    
       } // end if bullet[b].special2 != -1 
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_TRANS_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
     drag_bullet(b, 0.07);// .11
     break;
  case BULLET_DISRUPT2:
/*    passing_colour [0] = TRANS_REVERSE;
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       bullet[b].x + grand(5000) - grand(5000), bullet[b].y + grand(5000) - grand(5000),
       0, 0, 0, 0, 500 + grand(400), -3, 7, 0, 0, 0, passing_colour);*/
   break; 
  case BULLET_DISRUPT3:
/*    passing_colour [0] = TRANS_DARKEN;
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       bullet[b].x + grand(5000) - grand(5000), bullet[b].y + grand(5000) - grand(5000),
       0, 0, 0, 0, 500 + grand(400), -7, 7, 0, 0, 0, passing_colour);*/
   break;      
    case BULLET_EVIL_WORM:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
       if (bullet[b].timeout == bullet[b].special2 || (bullet[b].special3 == -1 && bullet[b].timeout % 5 == 0))
       {
          bullet[b].special3 = closest_target(bullet[b].x, bullet[b].y);
          if (bullet[b].timeout == bullet[b].special2)   
           bullet[b].timeout -= grand(10);
       }
       
     if (bullet[b].special3 != -1) //== LOCK_ACTOR0 || bullet[b].special3 == LOCK_ACTOR1)
     {
      cx = bullet[b].special3;
//      if (bullet[b].special3 == LOCK_ACTOR0)
//       cx = 0;
//        else
//         cx = 1;
//      if (hypot(bullet[b].y - actor[cx].y, bullet[b].x - actor[cx].x)
//           < 30000)
//         {  
          bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x, actor[cx].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
//         }
//          else
//           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x + (actor[cx].x_speed), actor[cx].y + (actor[cx].y_speed), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
     }
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.05);
     break;
    case BULLET_SWIRL2:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
 
     if (bullet[b].special3 != -1)
     {
      if (bullet[bullet[b].special3].type != BULLET_SWIRL1)
       bullet[b].special3 = -1;
     }
     if (bullet[b].special5 > 0)
      bullet[b].special5 --;
         
     if (bullet[b].special3 != -1 && bullet[b].special5 == 0)
     {
      cx = bullet[b].special3;
          bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, bullet[cx].x, bullet[cx].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
     }
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.05);
     break;
    case BULLET_FROZEN_TOOTH:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
     cx = xpart(bullet[b].special5 - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].special5 - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special4) / GRAIN;
     c2y = (cy * bullet[b].special4) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
//     drag_bullet(b, 0.07);
     drag_bullet(b, 0.03);
//     add_light(LIGHT_NORMAL, 3 + grand(4), bullet[b].x + bullet[b].x_speed, bullet[b].y + bullet[b].y_speed);
     break;
   case BULLET_BOLT:  
//     if (bullet[b].timeout < 5)
//      break;
      passing_colour [0] = bullet[b].colours [3];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [0];
//      passing_colour [3] = TRANS_DRED;
//      passing_colour [4] = TRANS_DRED;
      place_rocket_trail(bullet[b].x + grand(4001) - 2000, bullet[b].y + grand(4001) - 2000, 0, 0, 150, passing_colour);
//      place_rocket_trail(bullet[b].x + grand(3001) - 1500, bullet[b].y + grand(3001) - 1500, 0, 0, 250, passing_colour);
/*     if (bullet[b].timeout % 4 != 0) 
      break;
     passing_colour [0] = bullet[b].colours [0];
     passing_colour [1] = bullet[b].colours [1];
     passing_colour [2] = bullet[b].colours [2];
     place_explosion_no_light(bullet[b].x, bullet[b].y, 0, 0, 10 + grand(5), passing_colour);*/
//     place_explosion(bullet[b].x + grand(bullet[b].special1 * 2) - bullet[b].special1, bullet[b].y + grand(bullet[b].special1 * 2) - bullet[b].special1, 0, 0, 100 + grand(50 + bullet[b].special2), passing_colour);
     break;
   case BULLET_YELLOW_PULSE:
    add_light(LIGHT_NORMAL, 9 + grand(5), bullet[b].x, bullet[b].y);
    if (grand(2) == 0)
     break;
    passing_colour [0] = bullet[b].colours [0];
      cx = 36 + grand(12);
      create_cloud(CLOUD_SMALL_FADING_CIRCLE,
       bullet[b].x + grand(4001) - 2000,
       bullet[b].y + grand(4001) - 2000,
       0, 0,
       0,
       0,
       cx, 3, 0, 0, cx, 0, passing_colour);
    break;   
    case BULLET_WINGS1:
     if (bullet[b].timeout <= 1)
     {
      bullet[b].type = BULLET_WINGS2;
      bullet[b].timeout = 30;
     }
     break;
    case BULLET_WINGS2:
    if (bullet[b].special1 != ATTACK_NONE)
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 16);
     if (bullet[b].timeout <= 1)
     {
      bullet[b].type = BULLET_WINGS3;
      bullet[b].timeout = 80 + grand(20);
      bullet_soundf(b, NWAV_JET, 1000 + grand(300)); 
     }
     break;
    case BULLET_WINGS3:
     bullet[b].angle += 5 - grand(11);
     bullet[b].angle &= 1023;
     cx = xpart(bullet[b].angle, 300);
     cy = ypart(bullet[b].angle, 300);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     drag_bullet(b, 0.03);
     if (grand(2) == 0)
     {
      passing_colour [0] = TRANS_DRED;
      passing_colour [1] = TRANS_LRED;
      passing_colour [2] = TRANS_YELLOW;
//      passing_colour [3] = TRANS_DRED;
//      passing_colour [4] = TRANS_DRED;
//      place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 14, bullet[b].y + grand(3001) - 1500 - cy * 14, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 140, passing_colour);
      place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 14, bullet[b].y + grand(3001) - 1500 - cy * 14, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 110, passing_colour);
     }
     add_light(LIGHT_NORMAL, 4 + grand(3), bullet[b].x, bullet[b].y);
     break;
    case BULLET_BIGWINGS1:
     if (bullet[b].timeout <= 1)
     {
      bullet[b].type = BULLET_BIGWINGS2;
      bullet[b].timeout = 30;
     }
     break;
    case BULLET_BIGWINGS2:
    if (bullet[b].special1 != ATTACK_NONE)
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 16);
     if (bullet[b].timeout <= 1)
     {
      bullet[b].type = BULLET_BIGWINGS3;
      bullet[b].timeout = 80 + grand(20);
      bullet_soundf(b, NWAV_JET, 600 + grand(100)); 
     }
     break;
    case BULLET_BIGWINGS3:
     bullet[b].angle += 9 - grand(19);
     bullet[b].angle &= 1023;
     cx = xpart(bullet[b].angle, 300);
     cy = ypart(bullet[b].angle, 300);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     drag_bullet(b, 0.03);
     if (grand(3) != 0)
     {
      passing_colour [0] = TRANS_DGREEN;
      passing_colour [1] = TRANS_LGREEN;
      passing_colour [2] = TRANS_YELLOW;
//      passing_colour [3] = TRANS_DRED;
//      passing_colour [4] = TRANS_DRED;
//      place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 14, bullet[b].y + grand(3001) - 1500 - cy * 14, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 140, passing_colour);
      place_rocket_trail(bullet[b].x + grand(4001) - 2000 - cx * 14, bullet[b].y + grand(4001) - 2000 - cy * 14, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 210, passing_colour);
     }
     add_light(LIGHT_NORMAL, 7 + grand(4), bullet[b].x, bullet[b].y);
     break;
    case BULLET_CIRCLES:
     if (counter % 3 != 0)
      break;
      passing_colour [0] = bullet[b].colours [0];
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       500,-50,10, 0, 0, 0, passing_colour);
     break;
    case BULLET_CIRCLES2:
     if (counter % 2 != 0)
      break;
      passing_colour [0] = bullet[b].colours [0];
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       500,-50,10, 0, 0, 0, passing_colour);
     break;
    case BULLET_BIGCIRCLES:
     if (counter % 2 != 0)
      break;
      passing_colour [0] = bullet[b].colours [0];
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       bullet[b].x + grand(8000) - grand(8000),
       bullet[b].y + grand(8000) - grand(8000),
       0, 0,
       0, 0,
       500 + grand(200),-50,10, 0, 0, 0, passing_colour);
     break;
    case BULLET_BLOCKS:
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      create_cloud(CLOUD_BLOCK1,
       bullet[b].x - bullet[b].x_speed,
       bullet[b].y - bullet[b].y_speed,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       6 + grand(8),1,0, bullet[b].angle, 0, 0, passing_colour);
     break;
    case BULLET_OVERBLOCKS:
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      create_cloud(CLOUD_BLOCK1,
       bullet[b].x - bullet[b].x_speed,
       bullet[b].y - bullet[b].y_speed,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       14 + ypart(bullet[b].special1, 7),1,0, bullet[b].angle, 0, 0, passing_colour);
      bullet[b].size = (5 + ypart(bullet[b].special1, 8)) * GRAIN;
      bullet[b].special1 += 145;
      bullet[b].special1 %= ANGLE_FULL;
     break;
    case BULLET_ZIGZAG2:
/*      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      create_cloud(CLOUD_BLOCK5,
       bullet[b].x - bullet[b].x_speed,
       bullet[b].y - bullet[b].y_speed,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       90, 5 + grand(10), 0, bullet[b].angle, 5 + grand(5), 0, passing_colour);*/
/*      create_cloud(CLOUD_BLOCK5,
       bullet[b].x - bullet[b].x_speed,
       bullet[b].y - bullet[b].y_speed,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       90, 10, 0, bullet[b].angle, 7, 0, passing_colour);*/
      bullet[b].special2--;
      if (bullet[b].special2 == 0)
      {
       bullet[b].special2 = 12;
       if (bullet[b].special5 == 1)
        bullet[b].special2 = 6 + grand(10);
//       bullet[b].x_speed += xpart(bullet[b].special4 + bullet[b].special1, bullet[b].special3);
//       bullet[b].y_speed += ypart(bullet[b].special4 + bullet[b].special1, bullet[b].special3);
       bullet[b].x_speed += xpart(bullet[b].angle + bullet[b].special1, bullet[b].special3);
       bullet[b].y_speed += ypart(bullet[b].angle + bullet[b].special1, bullet[b].special3);
       bullet[b].special4 = radians_to_angle(atan2(bullet[b].y_speed, bullet[b].x_speed));
       bullet[b].special1 *= -1;
       if (bullet[b].special3 == 7000)
        bullet[b].special3 = 14000;
       bullet[b].x2 = bullet[b].x; 
       bullet[b].y2 = bullet[b].y; 
/*      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
        create_cloud(CLOUD_BLOCK1,
         bullet[b].x - bullet[b].x_speed,
         bullet[b].y - bullet[b].y_speed,
         bullet[b].x,
         bullet[b].y,
         0, 0,
         19, 1,0, bullet[b].special4, 0, 0, passing_colour);*/
      } 
      bullet[b].x2 -= bullet[b].x_speed; 
      bullet[b].y2 -= bullet[b].y_speed; 
     break;
    case BULLET_CURVEY:
//    if (counter % 2 == 0)
//     break;
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      cx = bullet[b].x - xpart(bullet[b].angle, 1 * GRAIN);
      cy = bullet[b].y - ypart(bullet[b].angle, 1 * GRAIN);
      create_cloud(CLOUD_BLOCK1,
       cx,
       cy,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       6 + grand(7),1,0, bullet[b].angle, 0, 0, passing_colour);
     break;
    case BULLET_LINE_PULSE:
//     passing_colour [0] = COLOUR_ORANGE8;
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
/*      if (counter % 2 != 0)
       break;
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      cx = bullet[b].x - xpart(bullet[b].angle, 2 * GRAIN);
      cy = bullet[b].y - ypart(bullet[b].angle, 2 * GRAIN);
      create_cloud(CLOUD_BLOCK1,
       cx,
       cy,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       6 + grand(7), 1, 0, bullet[b].angle, 0, 0, passing_colour);*/
     create_cloud(CLOUD_LINE_SHADOW,
      bullet[b].x,
      bullet[b].y,
      0, 0,
      0,
      0,
      30 + grand(60),3 + grand(7),0, bullet[b].angle, 0, 0, passing_colour);
     break;
    case BULLET_THICK_SHOCK:
      bullet[b].x2 = bullet[b].x;
      bullet[b].y2 = bullet[b].y;
      bullet[b].x += grand(28001) - 14000;
      bullet[b].y += grand(28001) - 14000;
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      cx = 8 + grand(3);
      create_cloud(CLOUD_BLOCK4,
       bullet[b].x2 - bullet[b].x_speed,
       bullet[b].y2 - bullet[b].y_speed,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       cx,1,0, bullet[b].angle, 0, 0, passing_colour);
      create_cloud(CLOUD_THICK_SHOCK_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       cx,1,0, bullet[b].angle, 0, 0, passing_colour);
       add_light(LIGHT_NORMAL, 15 + grand(15), bullet[b].x, bullet[b].y);
     break;
    case BULLET_NOVA:
     blast(bullet[b].x, bullet[b].y, 30000, 50, 300, bullet[b].owner);
     if (bullet[b].timeout <= 3)
     {
      passing_colour [0] = TRANS_LRED;
      passing_colour [1] = TRANS_YELLOW;
      passing_colour [2] = TRANS_WHITE;
      place_explosion(bullet[b].x, bullet[b].y, 0, 0, 600 + grand(300), passing_colour);
      blast(bullet[b].x, bullet[b].y, 70000, 300, 30000, bullet[b].owner);
      place_burstlet_burst(bullet[b].x, bullet[b].y, 0, 0, 6,
       6, 4, 1500, 1500, 7, passing_colour);
      simple_shockwave(TRANS_YELLOW, bullet[b].x, bullet[b].y, 0, 0, 800 + crandom(50), 30);
      bullet_soundf(b, NWAV_BURSTZL, 700 + grand(500)); 
//      simple_shockwave(TRANS_YELLOW, bullet[b].x, bullet[b].y, 0, 0, 1400 + crandom(50), 25);
//      simple_shockwave(TRANS_YELLOW, bullet[b].x, bullet[b].y, 0, 0, 1800 + crandom(50), 50);
      for (xb = 0; xb < 12; xb ++)
      {
       cx = grand(ANGLE_FULL);
       cy = 5000 + grand(20000);
       create_bullet(BULLET_HOLE_LINE, 
        bullet[b].x + xpart(cx, cy), bullet[b].y + ypart(cx, cy),
        xpart(cx, cy), ypart(cx, cy), 
        bullet[b].owner,
        0, 90 + grand(10), 10, 0, 0, 0, bullet[b].colours, 1, 
        0, b, 0, 0, 0);
      }  
      bullet[b].type = BULLET_HOLE;
      bullet[b].timeout = 180;
      break;
     }
     drag_bullet(b, 0.1);
     c2y = grand(4000);
     bullet[b].size = 18000;
        passing_colour [0] = TRANS_WHITE;
        passing_colour [1] = TRANS_YELLOW;
        passing_colour [2] = TRANS_LRED;
        passing_colour [3] = TRANS_DRED;
     if (bullet[b].timeout <= 40)
     {
//      cx = grand(xpart(bullet[b].timeout * 7 - ANGLE_QUARTER, 70000));
      cx = xpart(bullet[b].timeout * 7 - ANGLE_QUARTER, 70000);
      bullet[b].size = xpart(bullet[b].timeout * 7 - ANGLE_QUARTER, 60000);
      c2y = grand(4000) + (60 - bullet[b].timeout) * 80;
      passing_colour [0] = TRANS_ORANGE;
      passing_colour [1] = TRANS_LRED;
      passing_colour [2] = TRANS_DRED;
      passing_colour [3] = TRANS_DRED;
     } 
       else
       {
        cx = 25000;
/*        passing_colour [0] = TRANS_WHITE;
        passing_colour [1] = TRANS_YELLOW;
        passing_colour [2] = TRANS_LRED;
        passing_colour [3] = TRANS_DRED;*/
      }
      for (xb = 0; xb < 5; xb ++)
      {
       xa = grand(ANGLE_FULL);
       c2x = grand(cx);
       create_cloud(CLOUD_BLAST_CIRCLE,
        bullet[b].x + xpart(xa, c2x),
        bullet[b].y + ypart(xa, c2x),
        0, 0, 
        xpart(xa, c2y), ypart(xa, c2y),
        25 + grand(10), 2, 0, 0, 0, 0, passing_colour);
       } 
     break; 
    case BULLET_HOLE: 
     if (bullet[b].timeout == 1)
     {
      simple_shockwave(TRANS_YELLOW, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50), 20);
      blast(bullet[b].x, bullet[b].y, 30000, 100, 4000, bullet[b].owner);
      break;
     }
     blast(bullet[b].x, bullet[b].y, 15000, 50, 0, bullet[b].owner);
     blast(bullet[b].x, bullet[b].y, 600000, 0, -100, OWNER_ENEMY);
     if (bullet[b].timeout < 10)
      break;
      cx = grand(ANGLE_FULL);
      cy = 10000 + grand(50000);
      create_bullet(BULLET_HOLE_LINE, 
       bullet[b].x + xpart(cx, cy), bullet[b].y + ypart(cx, cy),
       0, 0, 
       bullet[b].owner,
       0, 30 + grand(10), 10, 0, 0, 0, bullet[b].colours, 1, 
       0, b, 0, 0, 0);
       break;
    case BULLET_ORBIT:
//      bullet[b].x2 = bullet[b].x;
//      bullet[b].y2 = bullet[b].y;
//      bullet[b].x += grand(28001) - 14000;
//      bullet[b].y += grand(28001) - 14000;
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      cx = 5 + grand(7); //bullet[b].special3 + grand(bullet[b].special4);
      create_cloud(CLOUD_THICK_SHOCK_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       cx,1,0, bullet[b].angle, 0, 0, passing_colour);
      if (bullet[b].colours [0] == TRANS_WHITE)
      {
       passing_colour [0] = TRANS_LGREY;
       create_cloud(CLOUD_BLOCK4,
        bullet[b].special1,// - bullet[b].x_speed,
        bullet[b].special2,// - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0, 0,
        cx,1,0, bullet[b].angle, 0, 0, passing_colour);
      }  
       add_light(LIGHT_NORMAL, 20 + grand(8), bullet[b].x, bullet[b].y);
     break;
/*    case BULLET_E_BOMB:
     drag_bullet(b, 0.03);
     if (counter % 3 != 0)
      break;
      passing_colour [0] = COLOUR_YELLOW8; //bullet[b].colours [0];
      create_cloud(CLOUD_SMALL_SHRINKING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
//       300,-30,15, 0, 0, 0, passing_colour);
       400,30,3, 0, 0, 0, passing_colour);
     break;*/
    case BULLET_E_BOMB2:
     drag_bullet(b, 0.06);
     if (abs(bullet[b].x_speed) + abs(bullet[b].y_speed) < 2000)
      break;
     // note indices reversed
       passing_colour [3] = COLOUR_WHITE;//bullet[b].colours [0];
       passing_colour [2] = COLOUR_GREY5;//bullet[b].colours [1];
       passing_colour [1] = COLOUR_BLUE8;//bullet[b].colours [2];
       passing_colour [0] = COLOUR_BLUE7;//bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 3,0, 0, 0, 0, passing_colour);
     break;
    case BULLET_DISRUPT1_DROP:
     bullet[b].special1 ++;
//     blast(bullet[b].x, bullet[b].y, 150000, 3, 300, bullet[b].owner);
     blast(bullet[b].x, bullet[b].y, 100000, 3, 300, bullet[b].owner);
     break; 
    case BULLET_FORK1:
    case BULLET_FORK2:
    case BULLET_FORK3:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2 - (bullet[b].type == BULLET_FORK1),0, 0, 0, 0, passing_colour);
     break;
    case BULLET_EVIL_EEL:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
     if (bullet[b].special3 <= 0)
     {
      bullet[b].special3 = 7 + grand(3);
      bullet[b].special2 *= -1;
     }
     bullet[b].special3 --;
     bullet[b].angle += bullet[b].special2;    
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     xa = bullet[b].special1 / 2 + grand(bullet[b].special1) / 2;
     c2x = (cx * xa) / GRAIN;
     c2y = (cy * xa) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.15);
     break;
    case BULLET_PANIC_EEL:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
     if (bullet[b].special5 > 0)
     {
      bullet[b].special5 --;
      drag_bullet(b, 0.05);
      break;
     }
     if (bullet[b].special3 <= 0)
     {
      bullet[b].special3 = 7 + grand(3);
      bullet[b].special2 *= -1;
     }
     bullet[b].special3 --;
     bullet[b].special4 += bullet[b].special2;    
     cx = xpart(bullet[b].special4 - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].special4 - ANGLE_QUARTER, GRAIN);
     xa = bullet[b].special1 / 2 + grand(bullet[b].special1) / 2;
     c2x = (cx * xa) / GRAIN;
     c2y = (cy * xa) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.15);
     break;
    case BULLET_TURN_WORM:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
     bullet[b].angle += bullet[b].special2;    
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.15);
     break;
    case BULLET_SQUIRMY:
       passing_colour [0] = bullet[b].colours [0];
       passing_colour [1] = bullet[b].colours [1];
       passing_colour [2] = bullet[b].colours [2];
       passing_colour [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colour);
     if (bullet[b].special3 <= 0)
     {
      bullet[b].special3 = bullet[b].special4;
      bullet[b].special2 *= -1;
     }
     bullet[b].special3 --;
     bullet[b].angle += bullet[b].special2;    
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.15);
     break;
   case BULLET_SEEKER1:
   case BULLET_SEEKER2:
   case BULLET_SEEKER3:
    add_light(LIGHT_NORMAL, (bullet[b].special2 / 3) + grand(4), bullet[b].x, bullet[b].y);
    if (bullet[b].special1 != ATTACK_NONE)
    {
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, bullet[b].special3);
     cx = xpart(bullet[b].angle, bullet[b].special4);
     cy = ypart(bullet[b].angle, bullet[b].special4);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     drag_bullet(b, 0.03);
    } 
    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];
/*     passing_colour [0] = bullet[b].colours [0];
     passing_colour [1] = bullet[b].colours [1];
     passing_colour [2] = bullet[b].colours [2];
     place_explosion_no_light(bullet[b].x + grand(bullet[b].special5) - bullet[b].special5 / 2, bullet[b].y + grand(bullet[b].special5) - bullet[b].special5 / 2, 
     bullet[b].x_speed, bullet[b].y_speed, 1, passing_colour);//bullet[b].special2 * 2 + grand(20), passing_colour);
*/

      cx = bullet[b].special2 + grand(12);
//      create_cloud(CLOUD_SMALL_FADING_CIRCLE,
      create_cloud(CLOUD_BLAST_CIRCLE, //MED_TRANS_FADING_CIRCLE,
       bullet[b].x + grand(bullet[b].special5) - bullet[b].special5 / 2,
       bullet[b].y + grand(bullet[b].special5) - bullet[b].special5 / 2,
       0, 0,
       bullet[b].x_speed,
       bullet[b].y_speed,
       cx, 3, 0, 0, cx, 0, passing_colour);
     break;
   case BULLET_BIGSEEKER:
    add_light(LIGHT_NORMAL, 25 + grand(5), bullet[b].x, bullet[b].y);
    if (bullet[b].special1 != ATTACK_NONE)
    {
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 12);
     cx = xpart(bullet[b].angle, 210);
     cy = ypart(bullet[b].angle, 210);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     drag_bullet(b, 0.023);
    } 
     if (bullet[b].timeout % 4 != 0)
      break;
/*    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];*/
     passing_colour [0] = bullet[b].colours [2];
     passing_colour [1] = bullet[b].colours [1];
     passing_colour [2] = bullet[b].colours [0];
     place_explosion_no_light(bullet[b].x + grand(9000) - grand(9000), bullet[b].y + grand(9000) - grand(9000), 
      bullet[b].x_speed, bullet[b].y_speed, 1, passing_colour);//bullet[b].special2 * 2 + grand(20), passing_colour);
/*      cx = bullet[b].special2 + grand(12);
      create_cloud(CLOUD_BLAST_CIRCLE, //MED_TRANS_FADING_CIRCLE,
       bullet[b].x + grand(bullet[b].special5) - bullet[b].special5 / 2,
       bullet[b].y + grand(bullet[b].special5) - bullet[b].special5 / 2,
       0, 0,
       bullet[b].x_speed,
       bullet[b].y_speed,
       cx, 3, 0, 0, cx, 0, passing_colour);*/
     break;
   case BULLET_CHARGE:
    add_light(LIGHT_NORMAL, 25 + grand(5), bullet[b].x, bullet[b].y);
//     if (bullet[b].timeout % 3 != 0)
//      break;
     passing_colour [0] = bullet[b].colours [2];
     passing_colour [1] = bullet[b].colours [1];
     passing_colour [2] = bullet[b].colours [0];
   passing_colour [0] = bullet[b].colours [0];
   cx = bullet[b].x + grand(9001) - 4500;
   cy = bullet[b].y + grand(9001) - 4500;
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    cx, cy, 0, 0,
    0, 0, 800 + grand(400),50,2, 0, 0, 0, passing_colour);
   passing_colour [0] = bullet[b].colours [1];
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    cx, cy, 0, 0,
    0, 0, 800 + grand(400),40,1, 0, 0, 0, passing_colour);
   passing_colour [0] = bullet[b].colours [2];
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    cx, cy, 0, 0,
    0, 0, 800 + grand(400),30,1, 0, 0, 0, passing_colour);
//     place_explosion_no_light(bullet[b].x + grand(9000) - grand(9000), bullet[b].y + grand(9000) - grand(9000), 
//      0, 0, 1, passing_colour);//bullet[b].special2 * 2 + grand(20), passing_colour);
     break;
   case BULLET_SWIRL1:
    add_light(LIGHT_NORMAL, 25 + grand(5), bullet[b].x, bullet[b].y);
    if (bullet[b].timeout % 2 == 0)
     break;
//     passing_colour [0] = bullet[b].colours [2];
//     passing_colour [1] = bullet[b].colours [1];
//     passing_colour [2] = bullet[b].colours [0];
   passing_colour [0] = bullet[b].colours [0];
   cx = bullet[b].x + grand(3001) - 1500;
   cy = bullet[b].y + grand(3001) - 1500;
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    cx, cy, 0, 0,
    0, 0, 600 + grand(300),40,2, 0, 0, 0, passing_colour);
   passing_colour [0] = bullet[b].colours [1];
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    cx, cy, 0, 0,
    0, 0, 600 + grand(300),30,1, 0, 0, 0, passing_colour);
   passing_colour [0] = bullet[b].colours [3];
   create_cloud(CLOUD_MED_TRANS_CIRCLE,
    cx, cy, 0, 0,
    0, 0, 600 + grand(300),20,1, 0, 0, 0, passing_colour);
//     place_explosion_no_light(bullet[b].x + grand(9000) - grand(9000), bullet[b].y + grand(9000) - grand(9000), 
//      0, 0, 1, passing_colour);//bullet[b].special2 * 2 + grand(20), passing_colour);
     break;
   case BULLET_PULSE1:
//    add_light(LIGHT_NORMAL, (7) + grand(5), bullet[b].x, bullet[b].y);
    if (bullet[b].special1 != ATTACK_NONE)
    {
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 16);
     cx = xpart(bullet[b].angle, 260);
     cy = ypart(bullet[b].angle, 260);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
    } 
    drag_bullet(b, 0.15);
    break;
   case BULLET_PULSE2:
//    add_light(LIGHT_NORMAL, (7) + grand(5), bullet[b].x, bullet[b].y);
    if (bullet[b].special1 != ATTACK_NONE)
    {
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 32);
     cx = xpart(bullet[b].angle, 500);
     cy = ypart(bullet[b].angle, 500);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
    } 
    drag_bullet(b, 0.20);
    break;
   case BULLET_OVERPULSE:
    if (bullet[b].special1 != ATTACK_NONE)
    {
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 32);
     cx = xpart(bullet[b].angle, 400);
     cy = ypart(bullet[b].angle, 400);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
    } 
    drag_bullet(b, 0.17);
    cx = (bullet[b].timeout % 64) * 16;
    if (bullet[b].special2 == 1)
     cx = ANGLE_FULL - cx;
    bullet[b].size = 12000 + xpart(cx, 3);
    break;
    case BULLET_EYE_DESOLATION:
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special4) / GRAIN;
     c2y = (cy * bullet[b].special4) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.03);
//     if (crandom(bullet[b].special1) > 100)
     {
      passing_colour [0] = TRANS_DRED;//bullet[b].colours [0];
      passing_colour [1] = TRANS_ORANGE;//bullet[b].colours [2]; // 1->1
      passing_colour [2] = TRANS_YELLOW;//bullet[b].colours [3]; // 2->3
      place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 3, bullet[b].y + grand(3001) - 1500 - cy * 3, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 140, passing_colour);
     }
     add_light(LIGHT_NORMAL, 8 + grand(4), bullet[b].x, bullet[b].y);
     break;
    case BULLET_TRI1:
     cx = xpart(bullet[b].angle, GRAIN);
     cy = ypart(bullet[b].angle, GRAIN);
     c2x = (cx * 100) / GRAIN;
     c2y = (cy * 100) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.007);
     passing_colour [0] = TRANS_DRED;//bullet[b].colours [0];
     passing_colour [1] = TRANS_ORANGE;//bullet[b].colours [2]; // 1->1
     passing_colour [2] = TRANS_YELLOW;//bullet[b].colours [3]; // 2->3
     place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 3, bullet[b].y + grand(3001) - 1500 - cy * 3, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 110, passing_colour);
     add_light(LIGHT_NORMAL, 8 + grand(4), bullet[b].x, bullet[b].y);
     break;
    case BULLET_TRI2:
     cx = xpart(bullet[b].angle, GRAIN);
     cy = ypart(bullet[b].angle, GRAIN);
     c2x = (cx * 140) / GRAIN;
     c2y = (cy * 140) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.006);
     passing_colour [0] = TRANS_DBLUE;//bullet[b].colours [0];
     passing_colour [1] = TRANS_LBLUE;//bullet[b].colours [2]; // 1->1
     passing_colour [2] = TRANS_WHITE;//bullet[b].colours [3]; // 2->3
     place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 3, bullet[b].y + grand(3001) - 1500 - cy * 3, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 110, passing_colour);
     add_light(LIGHT_NORMAL, 10 + grand(4), bullet[b].x, bullet[b].y);
     break;
    case BULLET_TRI3:
     cx = xpart(bullet[b].angle, GRAIN);
     cy = ypart(bullet[b].angle, GRAIN);
     c2x = (cx * 160) / GRAIN;
     c2y = (cy * 160) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.005);
     passing_colour [0] = TRANS_ORANGE;//bullet[b].colours [0];
     passing_colour [1] = TRANS_YELLOW;//bullet[b].colours [2]; // 1->1
     passing_colour [2] = TRANS_WHITE;//bullet[b].colours [3]; // 2->3
     place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 3, bullet[b].y + grand(3001) - 1500 - cy * 3, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 110, passing_colour);
     add_light(LIGHT_NORMAL, 12 + grand(4), bullet[b].x, bullet[b].y);
     break;
    case BULLET_OVERTRI:
     cx = xpart(bullet[b].angle, GRAIN);
     cy = ypart(bullet[b].angle, GRAIN);
     c2x = (cx * 100) / GRAIN;
     c2y = (cy * 100) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.003);
     passing_colour [0] = TRANS_DGREEN;//bullet[b].colours [0];
     passing_colour [1] = TRANS_LGREEN;//bullet[b].colours [2]; // 1->1
     passing_colour [2] = TRANS_YELLOW;//bullet[b].colours [3]; // 2->3
     place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 3, bullet[b].y + grand(3001) - 1500 - cy * 3, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 180, passing_colour);
     add_light(LIGHT_NORMAL, 18 + grand(6), bullet[b].x, bullet[b].y);
     break;
    case BULLET_BURNING_EYE:
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special4) / GRAIN;
     c2y = (cy * bullet[b].special4) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.03);
     {
      passing_colour [0] = TRANS_DRED;//bullet[b].colours [0];
      passing_colour [1] = TRANS_LRED;//bullet[b].colours [2]; // 1->1
      passing_colour [2] = TRANS_YELLOW;//bullet[b].colours [3]; // 2->3
      place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx * 3, bullet[b].y + grand(3001) - 1500 - cy * 3, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 90, passing_colour);
     }
     add_light(LIGHT_NORMAL, 4 + grand(3), bullet[b].x + bullet[b].x_speed, bullet[b].y + bullet[b].y_speed);
     break;
    case BULLET_MISSILE:
/* 
    KEEP for WORMS!
    
      passing_colour [0] = COLOUR_GREY1;
       passing_colour [1] = COLOUR_GREY3;
       passing_colour [2] = COLOUR_GREY5;
       passing_colour [3] = COLOUR_WHITE;
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 1,0, 0, 0, 0, passing_colour);*/
     if (bullet[b].special3 == LOCK_ACTOR0 || bullet[b].special3 == LOCK_ACTOR1)
     {
     
      if (bullet[b].special3 == LOCK_ACTOR0)
       cx = 0;
        else
         cx = 1;
      if (hypot(bullet[b].y - actor[cx].y, bullet[b].x - actor[cx].x)
           < 30000)
//          < 5000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x, actor[cx].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[cx].x + (actor[cx].x_speed * bullet[b].special5), actor[cx].y + (actor[cx].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
     }
      else
       if (bullet[b].special3 >= 0)
       {
        // destroyed enemies have locks cleared in register_destroyed (enemy.c)
        if (hypot(bullet[b].y - enemy[bullet[b].special3].y, bullet[b].x - enemy[bullet[b].special3].x)
            < 30000)
//          < 5000)
         bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x, enemy[bullet[b].special3].y, bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
          else
           bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, enemy[bullet[b].special3].x + (enemy[bullet[b].special3].x_speed * bullet[b].special5), enemy[bullet[b].special3].y + (enemy[bullet[b].special3].y_speed * bullet[b].special5), bullet[b].angle - ANGLE_QUARTER, bullet[b].special4) + ANGLE_QUARTER;
// What this does: if missile is close to enemy, heads straight for it.
//  Otherwise, leads it slightly (to an extent specified by bullet[b].special5)
//  For this reason, enemy[].x_speed must be set correctly even for enemies for whom it's irrelevant
//  (eg crawlers).
       }
     cx = xpart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     cy = ypart(bullet[b].angle - ANGLE_QUARTER, GRAIN);
     c2x = (cx * bullet[b].special1) / GRAIN;
     c2y = (cy * bullet[b].special1) / GRAIN;
     bullet[b].x_speed += c2x;
     bullet[b].y_speed += c2y;
     drag_bullet(b, 0.03);
     
//      special1 = 200 + actor[sactor].upgraded_system [UPG_ROCKET] * 50;
     if (crandom(bullet[b].special1) > 100)
     {
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [2]; // 1->1
      passing_colour [2] = bullet[b].colours [3]; // 2->3

//      place_rocket_trail(bullet[b].x, bullet[b].y, bullet[b].x_speed + cx*3, bullet[b].y_speed + cy*3, 5 + grand(5), passing_colour);
//      place_rocket_trail(bullet[b].x, bullet[b].y, bullet[b].x_speed + cx*3, bullet[b].y_speed + cy*3, 1, passing_colour);
      place_rocket_trail(bullet[b].x + grand(3001) - 1500 - cx, bullet[b].y + grand(3001) - 1500 - cy, bullet[b].x_speed - cx*2, bullet[b].y_speed - cy*2, 140, passing_colour);
     }
     add_light(LIGHT_NORMAL, 10, bullet[b].x, bullet[b].y);
     break;
   case BULLET_BLAST:
//    add_light(LIGHT_NORMAL, 8 + grand(4), bullet[b].x, bullet[b].y);
    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + grand(5001) - 2500,
       bullet[b].y + grand(5001) - 2500,
       0, 0, 
       bullet[b].x_speed, bullet[b].y_speed,
       25 + grand(10), 1, 1, 0, 0, 0, passing_colour);
     break;
   case BULLET_BIGBALL1:
//    add_light(LIGHT_NORMAL, 8 + grand(4), bullet[b].x, bullet[b].y);
    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];
    if (grand(2) == 0)
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + grand(4001 + bullet[b].special2 * 1000) - (2000 + bullet[b].special2 * 500),
       bullet[b].y + grand(4001 + bullet[b].special2 * 1000) - (2000 + bullet[b].special2 * 500), //grand(10001) - 5000,
       0, 0, 
       bullet[b].x_speed, bullet[b].y_speed,
       35 + grand(20), 1, 0, 0, 0, 0, passing_colour);
     break;
   case BULLET_FLAME:
     passing_colour [0] = bullet[b].colours [0];
     cx = bullet[b].x + grand(8001) - 2000;
     cy = bullet[b].y + grand(8001) - 2000;
     c2x = 200; //300 - bullet[b].timeout * 10;
     add_light(LIGHT_NORMAL, 30 + grand(10), cx, cy);
     create_cloud(CLOUD_FLAME_CIRCLE,
      cx, cy, 0,0, bullet[b].x_speed, bullet[b].y_speed,
      c2x + crandom(200),30 + grand(3),1, 0, 0, 0, passing_colour);
     passing_colour [0] = bullet[b].colours [1];
     create_cloud(CLOUD_FLAME_CIRCLE,
      cx, cy, 0,0, bullet[b].x_speed, bullet[b].y_speed,
      c2x + crandom(200),20 + grand(3),0, 0, 0, 0, passing_colour);
     passing_colour [0] = bullet[b].colours [2];
     create_cloud(CLOUD_FLAME_CIRCLE,
      cx, cy, 0,0, 0,0, //bullet[b].x_speed, bullet[b].y_speed,
      c2x + crandom(200),10 + grand(3),0, 0, 0, 0, passing_colour);
   blast(bullet[b].x, bullet[b].y, 20000, 75, 1000, bullet[b].owner);
     break;
   case BULLET_EYE_DESOLATION_SMALL:
    passing_colour [0] = TRANS_YELLOW;
    passing_colour [1] = TRANS_ORANGE;
    passing_colour [2] = TRANS_LRED;
    passing_colour [3] = TRANS_DRED;
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + grand(5001) - 2500,
       bullet[b].y + grand(5001) - 2500,
       0, 0, 0, 0,
       30 + grand(10), 1, 1, 0, 0, 0, passing_colour);
     add_light(LIGHT_NORMAL, 7 + grand(3), bullet[b].x, bullet[b].y);
     drag_bullet(b, 0.02);
     break;
   case BULLET_TWIRLY1:
    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];
    cx = (bullet[b].timeout % 32) * 32;
    if (bullet[b].special1 == 1)
     cx = ANGLE_FULL - cx;
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + xpart(cx, 10000) + grand(5001) - 2500,
       bullet[b].y + ypart(cx, 10000) + grand(5001) - 2500,
       0, 0, 0, 0,
       30 + grand(10), 3, 0, 0, 0, 0, passing_colour);
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + xpart(cx + ANGLE_HALF, 10000) + grand(5001) - 2500,
       bullet[b].y + ypart(cx + ANGLE_HALF, 10000) + grand(5001) - 2500,
       0, 0, 0, 0,
       30 + grand(10), 3, 0, 0, 0, 0, passing_colour);
       break;
   case BULLET_TWIRLY2:
    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];
    cx = (bullet[b].timeout % 32) * 32;
    if (bullet[b].special1 == 1)
     cx = ANGLE_FULL - cx;
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + xpart(cx, 6000) + grand(5001) - 2500,
       bullet[b].y + ypart(cx, 6000) + grand(5001) - 2500,
       0, 0, 0, 0,
       20 + grand(10), 2, 0, 0, 0, 0, passing_colour);
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + xpart(cx + ANGLE_HALF, 6000) + grand(5001) - 2500,
       bullet[b].y + ypart(cx + ANGLE_HALF, 6000) + grand(5001) - 2500,
       0, 0, 0, 0,
       20 + grand(10), 2, 0, 0, 0, 0, passing_colour);
       break;
   case BULLET_TWIRLY3:
    passing_colour [0] = bullet[b].colours [0];
    passing_colour [1] = bullet[b].colours [1];
    passing_colour [2] = bullet[b].colours [2];
    passing_colour [3] = bullet[b].colours [3];
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x + grand(9001) - 4500,
       bullet[b].y + grand(9001) - 4500,
       0, 0, 0, 0,
       30 + grand(15), 2, 0, 0, 0, 0, passing_colour);       
       break;
    case BULLET_TWISTY:
    passing_colour [0] = bullet[b].colours [1];
    passing_colour [1] = bullet[b].colours [2];
    passing_colour [2] = bullet[b].colours [3];
    passing_colour [3] = bullet[b].colours [3];
     c2x = (bullet[b].special1 % 32) * 32;
     c2y = xpart(c2x, 13 * GRAIN);
     bullet[b].size = abs(c2y) + 2000;
     cx = bullet[b].x + xpart(bullet[b].angle + ANGLE_QUARTER, c2y);
     cy = bullet[b].y + ypart(bullet[b].angle + ANGLE_QUARTER, c2y);
      create_cloud(CLOUD_BLAST_CIRCLE,
       cx + grand(5001) - 2500,
       cy + grand(5001) - 2500,
       0, 0, 0, 0,
       30 + grand(15), 3, 0, 0, 0, 0, passing_colour);
     cx = bullet[b].x + xpart(bullet[b].angle - ANGLE_QUARTER, c2y);
     cy = bullet[b].y + ypart(bullet[b].angle - ANGLE_QUARTER, c2y);
      create_cloud(CLOUD_BLAST_CIRCLE,
       cx + grand(5001) - 2500,
       cy + grand(5001) - 2500,
       0, 0, 0, 0,
       30 + grand(15), 3, 0, 0, 0, 0, passing_colour);
     bullet[b].special1 ++;
     break;   
    case BULLET_CURVE:
     cx = xpart(bullet[b].angle + ANGLE_QUARTER, 200);
     cy = ypart(bullet[b].angle + ANGLE_QUARTER, 200);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     break;
    case BULLET_PLASMA:
     passing_colour [0] = bullet[b].colours [grand(4)];
     cx = bullet[b].x_speed / 3;
     cy = bullet[b].y_speed / 3;
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + grand(4001) - 2000,
      bullet[b].y + grand(4001) - 2000,
      cx, cy,
      0,
      0,
      250 + crandom(200),70,0, 0, 0, 0, passing_colour);
     break;
    case BULLET_TORPEDO:
//     bullet[b].angle += grand(11) - 5;
     cx = xpart(bullet[b].angle, 205);
     cy = ypart(bullet[b].angle, 205);
     bullet[b].x_speed += cx;
     bullet[b].y_speed += cy;
     drag_bullet(b, 0.01);
     if (bullet[b].timeout % 4 != 0)
      break;
      passing_colour [0] = COLOUR_YELLOW6;
      create_cloud(CLOUD_GROWING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
//       bullet[b].x_speed - cx*5, bullet[b].y_speed - cy*5,
       0, 0,
       100,3,0, 0, 100, 0, passing_colour);
     break;
    case BULLET_BOSS4:
     passing_colour [0] = TRANS_YELLOW;//bullet[b].colours [grand(4)];
     if (grand(3) == 0)
      passing_colour [0] = TRANS_WHITE;
     cx = bullet[b].x_speed / 3;
     cy = bullet[b].y_speed / 3;
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + grand(8001) - 4000,
      bullet[b].y + grand(8001) - 4000,
      cx, cy,
      0,
      0,
      500 + crandom(300),70,0, 0, 0, 0, passing_colour);
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + xpart(bullet[b].timeout * 40 + bullet[b].angle, 30000),
      bullet[b].y + ypart(bullet[b].timeout * 40 + bullet[b].angle, 30000),
      0, 0,
      0,
      0,
      300 + crandom(200),50,0, 0, 0, 0, passing_colour);
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + xpart(ANGLE_FULL - (bullet[b].timeout * 40 + bullet[b].angle), 30000),
      bullet[b].y + ypart(ANGLE_FULL - (bullet[b].timeout * 40 + bullet[b].angle), 30000),
      0, 0,
      0,
      0,
      300 + crandom(200),50,0, 0, 0, 0, passing_colour);
//     if (bullet[b].timeout % 16 == 0)
//      bullet_sound(b, BSOUND_MINE2);
     break;
    case BULLET_FLAK:
     if (counter % 3 != 0)
      break;
      passing_colour [0] = COLOUR_RED8;
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       500,-50,10, 0, 0, 0, passing_colour);
     break;
    case BULLET_LINES:
     passing_colour [0] = COLOUR_GREY6;
//     cx = bullet[b].x_speed / 3;
//     cy = bullet[b].y_speed / 3;
     create_cloud(CLOUD_LINE_SHADOW,
      bullet[b].x,
      bullet[b].y,
      0, 0,
      0,
      0,
      20 + grand(60),3 + grand(7),0, bullet[b].angle, 0, 0, passing_colour);
     break;
    case BULLET_MINE1:
    case BULLET_MINE2:
    case BULLET_MINE3:
//     if (grand(4) == 0)
     if (grand(3) == 0 && grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 2000)
     {
      passing_colour[0] = COLOUR_YELLOW8;
      passing_colour[1] = COLOUR_YELLOW6;
      passing_colour[2] = COLOUR_ORANGE6;
      passing_colour[3] = COLOUR_RED5;
      passing_colour[4] = COLOUR_RED4;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(4001) - 2000, bullet[b].y + grand(4001) - 2000,
       0, 0, 0, 0, 16 + grand(9), 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.08);
     if (abs(bullet[b].x_speed) + abs(bullet[b].y_speed) < 1000)
     {
      bullet[b].x_speed = 0;
      bullet[b].y_speed = 0;
     }  
     break;
    case BULLET_BFLAK:
     if (grand(3) != 0 && grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 1900)
     {
      passing_colour[0] = COLOUR_WHITE;
      passing_colour[1] = COLOUR_BLUE8;
      passing_colour[2] = COLOUR_BLUE7;
      passing_colour[3] = COLOUR_BLUE5;
      passing_colour[4] = COLOUR_BLUE3;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(4001) - 2000, bullet[b].y + grand(4001) - 2000,
       0, 0, 0, 0, 16 + grand(9), 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.06);
     break;
    case BULLET_BFLAK2:
     if (grand(3) != 0 && grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 1900)
     {
      passing_colour[0] = COLOUR_WHITE;
      passing_colour[1] = COLOUR_YELLOW8;
      passing_colour[2] = COLOUR_YELLOW7;
      passing_colour[3] = COLOUR_ORANGE6;
      passing_colour[4] = COLOUR_RED5;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(4001) - 2000, bullet[b].y + grand(4001) - 2000,
       0, 0, 0, 0, 16 + grand(9), 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.06);
     break;
//    case BULLET_MBOMB:
    case BULLET_E_BOMB:
    if (grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 1500)
     {
      passing_colour[0] = COLOUR_YELLOW8;
      passing_colour[1] = COLOUR_YELLOW6;
      passing_colour[2] = COLOUR_ORANGE6;
      passing_colour[3] = COLOUR_RED5;
      passing_colour[4] = COLOUR_RED4;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(4001) - 2000, bullet[b].y + grand(4001) - 2000,
       0, 0, 0, 0, 16 + grand(9), 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.03);
     break;
    case BULLET_PRESEEKMINE:
//     if (grand(4) == 0)
     if (grand(3) == 0 && grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 2000)
     {
      passing_colour[0] = COLOUR_WHITE;
      passing_colour[1] = COLOUR_BLUE8;
      passing_colour[2] = COLOUR_BLUE6;
      passing_colour[3] = COLOUR_BLUE4;
      passing_colour[4] = COLOUR_BLUE2;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(6001) - 3000, bullet[b].y + grand(6001) - 3000,
       0, 0, 0, 0, 24, 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.08);
     if (abs(bullet[b].x_speed) + abs(bullet[b].y_speed) < 1000)
      bullet[b].timeout = 1;
       else
        bullet[b].timeout ++;
     break;
    case BULLET_PREMINE:
//     if (grand(4) == 0 && abs(bullet[b].x_speed) + abs(bullet[b].y_speed) > 2000)
     if (grand(3) == 0 && grand(abs(bullet[b].x_speed) + abs(bullet[b].y_speed)) > 2000)
     {
      passing_colour[0] = COLOUR_YELLOW8;
      passing_colour[1] = COLOUR_RED8;
      passing_colour[2] = COLOUR_RED6;
      passing_colour[3] = COLOUR_RED4;
      passing_colour[4] = COLOUR_RED2;
      create_cloud(CLOUD_SPECK, bullet[b].x + grand(6001) - 3000, bullet[b].y + grand(6001) - 3000,
       0, 0, 0, 0, 24, 1,
       0, 0, 0, 0, passing_colour);
     }
     drag_bullet(b, 0.08);
     if (abs(bullet[b].x_speed) + abs(bullet[b].y_speed) < 1000)
      bullet[b].timeout = 1;
       else
        bullet[b].timeout ++;
     break;
    case BULLET_SEEKMINE:
    case BULLET_MINE:
     bullet[b].x_speed = 0;
     bullet[b].y_speed = 0;
     break;
    case BULLET_SEEKER_BLOB:
     if (bullet[b].special1 == ATTACK_NONE)
      break;
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 4);
//     cx = cos(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * 1200;
//     cy = sin(angle_to_radians(bullet[b].angle) + ANGLE_QUARTER) * 1200;
     cx = xpart(bullet[b].angle, 1900);
     cy = ypart(bullet[b].angle, 1900);
     bullet[b].x_speed = cx;
     bullet[b].y_speed = cy;
     break;
    case BULLET_PRONG:
     bullet[b].x_speed += grand(201) - 100;
     bullet[b].y_speed += grand(201) - 100;
     bullet[b].angle += 64;
     if (bullet[b].angle > ANGLE_FULL)
      bullet[b].angle -= ANGLE_FULL;
     break;
    case BULLET_SEEKER_BLOB3:
     passing_colour [0] = TRANS_LBLUE;
     if (grand(3) == 0)
      passing_colour [0] = TRANS_DBLUE;
//     if (grand(4) == 0)
//      passing_colour [0] = TRANS_WHITE;
     create_cloud(CLOUD_SMALL_TRANS_CIRCLE,
      bullet[b].x + grand(4001) - 2000,
      bullet[b].y + grand(4001) - 2000,
      0, 0, 0, 0, 250 + crandom(200),10,5, 0, 0, 0, passing_colour);
     if (bullet[b].special1 == ATTACK_NONE)
      break;
     bullet[b].angle = enemy_bullet_track_target(b, bullet[b].special1, 8);
     cx = xpart(bullet[b].angle, 2300);
     cy = ypart(bullet[b].angle, 2300);
     bullet[b].x_speed = cx;
     bullet[b].y_speed = cy;
     break;
    case BULLET_STING:
     if (bullet[b].timeout % 3 != 0)
      break;
     passing_colour[0] = bullet[b].colours [1];
     passing_colour[1] = bullet[b].colours [1];
     passing_colour[2] = bullet[b].colours [2];
     passing_colour[3] = bullet[b].colours [2];
     passing_colour[4] = bullet[b].colours [3];
//     create_cloud(CLOUD_SPECK, bullet[b].x, bullet[b].y, 0, 0,
//      0, 0, 15 + grand(15), 1,
//      1, 0, 0, 0, passing_colour);
     create_cloud(CLOUD_SPECK, bullet[b].x + grand(5001) - 2500, bullet[b].y + grand(5001) - 2500, 0, 0,
      0, 0, 15 + grand(15), 1,
      1, 0, 0, 0, passing_colour);
      break;
/*    case BULLET_PRONG:
     bullet[b].x_speed += grand(201) - 100;
     bullet[b].y_speed += grand(201) - 100;
     bullet[b].angle += 64;
     if (bullet[b].angle > ANGLE_FULL)
      bullet[b].angle -= ANGLE_FULL;
     break;*/
   }
    // other switch after move_bullet

//   if (counter % 5 != 0)
//    return;

   if (move_bullet(b, 1) == 0)
    return;

   // other switch before move_bullet
   switch(bullet[b].type)
   {
    case BULLET_HOSTILE:
    case BULLET_SHOCK:
//      bullet[b].x2 = bullet[b].x;
//      bullet[b].y2 = bullet[b].y;
      bullet[b].x += grand(20001) - 10000;
      bullet[b].y += grand(20001) - 10000;
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      cx = bullet[b].x;// + grand(16001) - 8000;
      cy = bullet[b].y;// + grand(16001) - 8000;
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x2 - bullet[b].x_speed,
       bullet[b].y2 - bullet[b].y_speed,
       bullet[b].x,
       bullet[b].y,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
       bullet[b].x2 = cx;
       bullet[b].y2 = cy;
     break;
    case BULLET_ZIGZAG1:
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
      if (bullet[b].timeout % 2 == 0)
      {
       cx = bullet[b].x + xpart(bullet[b].angle + ANGLE_QUARTER, bullet[b].special1);
       cy = bullet[b].y + ypart(bullet[b].angle + ANGLE_QUARTER, bullet[b].special1);
      }
       else
       {
        cx = bullet[b].x + xpart(bullet[b].angle - ANGLE_QUARTER, bullet[b].special1);
        cy = bullet[b].y + ypart(bullet[b].angle - ANGLE_QUARTER, bullet[b].special1);
       }
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x2 - bullet[b].x_speed,
       bullet[b].y2 - bullet[b].y_speed,
       cx,
       cy,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
       bullet[b].x2 = cx;
       bullet[b].y2 = cy;
     break;
/*  case BULLET_SHOCK_PATH:
    case BULLET_GOLDEN_PATH:
//    case BULLET_SHOCK:
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
       cx = bullet[b].x + grand(12001) - 6000;
       cy = bullet[b].y + grand(12001) - 6000;
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x2 - bullet[b].x_speed,
       bullet[b].y2 - bullet[b].y_speed,
       cx,
       cy,
       0,
       0,
       10, 2,0, 0, 0, 0, passing_colour);
       bullet[b].x2 = cx;
       bullet[b].y2 = cy;
     break;
    case BULLET_GOLDEN_PATH_SMALL:
    case BULLET_SHOCK_PATH_SMALL:
      passing_colour [0] = bullet[b].colours [0];
      passing_colour [1] = bullet[b].colours [1];
      passing_colour [2] = bullet[b].colours [2];
      passing_colour [3] = bullet[b].colours [3];
       cx = bullet[b].x + grand(6001) - 3000;
       cy = bullet[b].y + grand(6001) - 3000;
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x2 - bullet[b].x_speed,
       bullet[b].y2 - bullet[b].y_speed,
       cx,
       cy,
       0,
       0,
       10, 3,0, 0, 0, 0, passing_colour);
       bullet[b].x2 = cx;
       bullet[b].y2 = cy;
     break;
    case BULLET_TORPEDO2:
      if (bullet[b].timeout < 35)
      {
       if (bullet[b].timeout == 34)
        bullet_sound(b, BSOUND_TUBE);
       cx = xpart(bullet[b].angle, 400 + (35 - bullet[b].timeout) * 20);
       cy = ypart(bullet[b].angle, 400 + (35 - bullet[b].timeout) * 20);
       bullet[b].x_speed += cx;
       bullet[b].y_speed += cy;
//       drag_bullet(b, 0.01);
       passing_colour [0] = COLOUR_GREY1;
       passing_colour [1] = COLOUR_GREY3;
       passing_colour [2] = COLOUR_GREY5;
       passing_colour [3] = COLOUR_WHITE;
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x2 - bullet[b].x_speed,
        bullet[b].y2 - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 1,0, 0, 0, 0, passing_colour);
        bullet[b].x2 = bullet[b].x;
        bullet[b].y2 = bullet[b].y;
      }
     break;
    case BULLET_SUPERSTING:
//     if (bullet[b].timeout % 2 == 0)
     {
      passing_colour [3] = bullet[b].colours [0];
      passing_colour [2] = bullet[b].colours [1];
      passing_colour [1] = bullet[b].colours [2];
      passing_colour [0] = bullet[b].colours [3];
       c2x = xpart(bullet[b].angle + ANGLE_QUARTER, 3000);
       c2y = ypart(bullet[b].angle + ANGLE_QUARTER, 3000);
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x2 - (bullet[b].x_speed * 1) + c2x,
       bullet[b].y2 - (bullet[b].y_speed * 1) + c2y,
       bullet[b].x - c2x,
       bullet[b].y - c2y,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
      create_cloud(CLOUD_FADING_LINE,
       bullet[b].x - (bullet[b].x_speed * 1) - c2x,
       bullet[b].y - (bullet[b].y_speed * 1) - c2y,
       bullet[b].x + c2x,
       bullet[b].y + c2y,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
     }*/


       
//       bullet[b].x2 = bullet[b].x + c2x;
//       bullet[b].y2 = bullet[b].y + c2y;
/*       c2x = cos(angle_to_radians(bullet[b].angle + ANGLE_QUARTER)) * 5000;
       c2y = sin(angle_to_radians(bullet[b].angle + ANGLE_QUARTER)) * 5000;
//       cx = bullet[b].x + c2x;
//       cy = bullet[b].y + c2y;
      create_cloud(CLOUD_FADING_LINE,
//       bullet[b].x2 - bullet[b].x_speed + c2x,
//       bullet[b].y2 - bullet[b].y_speed + c2y,
       bullet[b].x2 + c2x,
       bullet[b].y2 + c2y,
       bullet[b].x - c2x,
       bullet[b].y - c2y,
       0,
       0,
       10, 1,0, 0, 0, 0, passing_colour);
       bullet[b].x2 = bullet[b].x - c2x;
       bullet[b].y2 = bullet[b].y - c2y;*/
     break;

   }



   int things [2];
   int k, sh;

   if (bullet[b].owner == OWNER_ENEMY || game.type == GAME_DUEL)
   {
    k = detect_collision(b, things);

    if (k != -1)
    {
     bullet[b].x = things [0];
     bullet[b].y = things [1];
// bullet_hits_actor
     if (bullet[b].mass > 0)
     {
      actor[k].x_speed += (bullet[b].x_speed * bullet[b].mass) / actor[k].mass;
      actor[k].y_speed += (bullet[b].y_speed * bullet[b].mass) / actor[k].mass;
      actor[k].screen_shake_time += bullet[b].mass / 20;
      if (actor[k].screen_shake_time > 5)
       actor[k].screen_shake_time = 5;
     }
     // hurt_actor must be after force, as actor may be destroyed in it.
     hurt_actor(k, bullet[b].owner, bullet[b].damage);
     bullet_impact(b, k, HIT_ACTOR);
     return;
    }
   }
//    else
   
   if (bullet[b].owner != OWNER_ENEMY) // || game.type == GAME_DUEL)
    {
     k = detect_enemy_collision(b, things);

     if (k != -1)
     {
      bullet[b].x = things [0];
      bullet[b].y = things [1];
      if (bullet[b].mass > 0)
      {
       enemy[k].x_speed += (bullet[b].x_speed * bullet[b].mass) / enemy[k].mass;
       enemy[k].y_speed += (bullet[b].y_speed * bullet[b].mass) / enemy[k].mass;
      }
      sh = check_shielder(k);
      if (!sh)
      {
       enemy_sparks(k, b, bullet[b].x, bullet[b].y, bullet[b].x_speed, bullet[b].y_speed, bullet[b].damage);
       int pulse = bullet[b].damage / 6 + 1;
       if (pulse > 8)
        pulse = 8;
       hurt_enemy(k, bullet[b].damage, bullet[b].owner, pulse);
       bullet_soundf(b, NWAV_TWING, enemy_sound_pitch(enemy[k].type)); //800 + grand(200) - enemy_pitch(enemy[k].type));
      }
       else
       {
        shield_line(k, bullet[b].x, bullet[b].y);
        bullet_soundf(b, NWAV_SHIELDE, enemy_sound_pitch(enemy[k].type));
       } 
      bullet_impact(b, k, HIT_ENEMY);
// bullet_hits_enemy
      return;
     }
    }

}

int check_shielder(int e)
{
 if (enemy[e].type == ENEMY_SHIELDER1
      || enemy[e].type == ENEMY_DEFENDER3_TURRET1)
  return 0; // note enemy[e]! (not i)
  
 int i, distance, retval = 0;
 
 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type != ENEMY_SHIELDER1
       && enemy[i].type != ENEMY_DEFENDER3_TURRET1)
   continue; // note enemy[i]! (not e)

  distance = hypot(enemy[e].x - enemy[i].x, enemy[e].y - enemy[i].y);

  if (distance <= 300000)
  {
        shielder_pulse(i, 100);
        retval = 1;
  }  
 }
 
 return retval;
 
}

void shield_line(int e, int bx, int by)
{
/*
TODO:
 blasts
 collisions
*/
 int sangle = radians_to_angle(atan2(enemy[e].y - by, enemy[e].x - bx));
 
 int pcol [5];
 
 pcol [4] = TRANS_WHITE;
 pcol [3] = TRANS_LBLUE;
 pcol [2] = TRANS_LBLUE;
 pcol [1] = TRANS_LBLUE;
 pcol [0] = TRANS_DBLUE;
/* pcol [3] = TRANS_YELLOW;
 pcol [2] = TRANS_LGREEN;
 pcol [1] = TRANS_DGREEN;
 pcol [0] = TRANS_DGREY;
*/   
/* create_cloud(CLOUD_SHIELD_LINE, enemy[e].x, enemy[e].y, 0, 0,
0,0, 100, 7,
//enemy[e].x_speed, enemy[e].y_speed, 100, 7,
0, sangle,
enemy[e].radius, 0, pcol);*/
 
create_cloud(CLOUD_SHIELD_LINE, bx, by, 0, 0,
0,0, 100, 7,
//enemy[e].x_speed, enemy[e].y_speed, 100, 7,
0, sangle,
enemy[e].radius, 0, pcol);


}


void shielder_pulse(int e, int dam)
{
 enemy[e].attribute [0] = 15;
 return;
}

void enemy_sparks(int e, int b, int bx, int by, int bxs, int bys, int damage)
{

// int i;
 int msparks = 2 + grand(damage) / 50;
 if (msparks > 5) msparks = 5 - grand(2);
 int passing_colours [5];
// int csx, csy;
// int angle = 0;
// int timer = 0;

 passing_colours [0] = (enemy[e].colours [0] / 8) * 8 + 7;
 passing_colours [1] = passing_colours [0] - 1; 
 passing_colours [2] = passing_colours [0] - 2; 
 passing_colours [3] = passing_colours [0] - 3; 
 passing_colours [4] = passing_colours [0] - 4;
/* passing_colours [1] = passing_colours [0] - 1; 
 passing_colours [2] = passing_colours [0] - 2; 
 passing_colours [3] = passing_colours [0] - 3; 
 passing_colours [4] = passing_colours [0] - 4; */

// for (i = 0; i < msparks; i ++)
// {
//      angle = grand(ANGLE_FULL);
//      csx = enemy[e].x_speed + xpart(angle, 200 + grand(500));
//      csy = enemy[e].y_speed + ypart(angle, 200 + grand(500));
//      csx = xpart(angle, 200 + grand(500));
//      csy = ypart(angle, 200 + grand(500));
//      timer = 10 + grand(10);
//      create_cloud(CLOUD_FLECK, bx, by,
//       0, 0, csx, csy, timer, 
//       1, 0, angle, 0, 0, passing_colours);
//      create_cloud(CLOUD_SPECK, bx, by,
//       0, 0, enemy[e].x_speed + grand(1001) - 500, enemy[e].y_speed + grand(1001) - 500, 
//       10 + grand(15), 1, 0, 0, 0, 0, passing_colours);
   place_speck_burst(bx, by, enemy[e].x_speed, enemy[e].y_speed, msparks, 10, 15, 900, passing_colours);
// }

}

void bullet_impact(int b, int k, int hit_what)
{
 int j;
 int xs = bullet[b].x_speed;
 int ys = bullet[b].y_speed;
 int m, xa, xb, cx, cy;

 int passing_colours [5];
 
 if (hit_what == HIT_ACTOR && k != -1)
 {
  xs = actor[k].x_speed;
  ys = actor[k].y_speed;
 }
  else
  {
   if (hit_what == HIT_ENEMY && k != -1)
   {
    xs = enemy[k].x_speed;
    ys = enemy[k].y_speed;
   }
    else
    {
     xs = 0;
     ys = 0;
    }
  }

 switch(bullet[b].type)
 {
  case BULLET_NOVA:
   bullet[b].x_speed = 0;
   bullet[b].y_speed = 0;
   return;
  case BULLET_DISRUPT3:
   passing_colours [0] = TRANS_DARKEN;
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 0, 0, 3500, -3, 3, 0, 0, 0, passing_colours);
   blast(bullet[b].x, bullet[b].y, 50000, 250, 1000, bullet[b].owner);
   radius_effect(bullet[b].x, bullet[b].y, 50000, RADEFFECT_TELEPORT);
   bullet_soundf(b, NWAV_CHIME, 200 + grand(20)); 
   break;
  case BULLET_HOSTILE:
  case BULLET_SHOCK:
  case BULLET_ZIGZAG1:
   passing_colours [0] = TRANS_WHITE;
   passing_colours [1] = bullet[b].colours [3];//COLOUR_BLUE8;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 4 + grand(4), 15, 15, 1200, 300, passing_colours);
   break;
  case BULLET_FURIOUS_ORB:
//   bullet_sound(b, BSOUND_BOMB);
  case BULLET_MANIFOLD_ORB_SMALL:
  case BULLET_FURIOUS_ORB_SMALL:
   passing_colours [0] = bullet[b].colours [0];
   passing_colours [1] = bullet[b].colours [1];
   passing_colours [2] = bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(300) + bullet[b].special2 * 10, passing_colours);
   blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1, bullet[b].special4, bullet[b].owner);
   if (bullet[b].type == BULLET_FURIOUS_ORB)
//    bullet_soundf(b, NWAV_BURSTZL, 400 + grand(150)); 
    bullet_soundf(b, NWAV_BURST, 300 + grand(10)); 
     else
      bullet_soundvf(b, NWAV_BURST, 80, 400 + grand(150)); 

/*   for (j = 0; j < bullet[b].damage / 12 + 1; j ++)
   {
    xs = bullet[b].x + grand(40001) - 20000;
    ys = bullet[b].y + grand(40001) - 20000;
    place_explosion_no_light(xs, ys, 0, 0, 100 + grand(150), passing_colours);
//    blast(xs, ys, 15000, bullet[b].damage * 5, 2000, bullet[b].owner);
   }*/
//   passing_colours [3] = TRANS_WHITE2;
//   passing_colours [2] = TRANS_LGREY;
//   passing_colours [1] = TRANS_LGREY;
//   passing_colours [0] = TRANS_DGREY;
//      create_cloud(CLOUD_SHOCKWAVE, bullet[b].x, bullet[b].y, 0, 0, 0, 0,
//       200 + crandom(50) + bullet[b].special2 / 2,15,0, 0, 250 + grand(100) + bullet[b].special2 / 2, 0, passing_colours);
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50) + bullet[b].special2 * 2, 15);
   break;
  case BULLET_MANIFOLD_ORB:
   passing_colours [0] = bullet[b].colours [0];
   passing_colours [1] = bullet[b].colours [1];
   passing_colours [2] = bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(300) + bullet[b].special2 * 10, passing_colours);
   blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1, bullet[b].special4, bullet[b].owner);
//   bullet_soundf(b, NWAV_BURSTZL, 300 + grand(150)); 
   bullet_soundf(b, NWAV_BURST, 300 + grand(10)); 
   if (bullet[b].special5 > 0 && hit_what != HIT_WAVE)
   {
    for (j = 0; j < bullet[b].special5; j ++)
    {    
      create_bullet(BULLET_MANIFOLD_ORB_SMALL, bullet[b].x, bullet[b].y,
       grand(5001) - 2500, grand(5001) - 2500, bullet[b].owner,
       1, 20 + grand(40), 100, 0, 0, 0, bullet[b].colours, 1, 
       bullet[b].special1 / 2, bullet[b].special2 / 3, bullet[b].special3 / 3, bullet[b].special4 / 3, bullet[b].special5 / 3);
    }
   }
/*   for (j = 0; j < bullet[b].damage / 12 + 1; j ++)
   {
    xs = bullet[b].x + grand(40001) - 20000;
    ys = bullet[b].y + grand(40001) - 20000;
    place_explosion_no_light(xs, ys, 0, 0, 100 + grand(150), passing_colours);
//    blast(xs, ys, 15000, bullet[b].damage * 5, 2000, bullet[b].owner);
   }*/
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50) + bullet[b].special2 * 2, 15);
   break;
  case BULLET_CIRCLER:
   passing_colours [0] = TRANS_DRED;//bullet[b].colours [0];
   passing_colours [1] = TRANS_ORANGE;//bullet[b].colours [1];
   passing_colours [2] = TRANS_YELLOW;//bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(300), passing_colours);
   blast(bullet[b].x, bullet[b].y, 40000, 300, 5000, bullet[b].owner);
   for (j = 0; j < 5; j ++)
   {
    xs = bullet[b].x + grand(40001) - 20000;
    ys = bullet[b].y + grand(40001) - 20000;
    place_explosion_no_light(xs, ys, 0, 0, 100 + grand(150), passing_colours);
   }
   place_burstlet_burst(bullet[b].x, bullet[b].y, 0, 0, 6,
      6, 4, 1500, 1500, 7, passing_colours);
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50));
   bullet_soundf(b, NWAV_BURST, 1500 + grand(350)); 
   break;
  case BULLET_FORK2: 
  case BULLET_FORK1:
       passing_colours [0] = bullet[b].colours [0];
       passing_colours [1] = bullet[b].colours [1];
       passing_colours [2] = bullet[b].colours [2];
       passing_colours [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colours);
  if (hit_what == HIT_NOTHING)
  {
   passing_colours [0] = bullet[b].colours [0];
   passing_colours [1] = bullet[b].colours [1];
   passing_colours [2] = bullet[b].colours [2];
   m = 2 + grand(3);
    for (j = 0; j < m; j ++)
    {    
      create_bullet(BULLET_FORK3 - (bullet[b].type == BULLET_FORK1 && grand(3) != 0), bullet[b].x, bullet[b].y,
//       bullet[b].x_speed + grand(9001) - 4500, bullet[b].y_speed + grand(9001) - 4500, 
       bullet[b].x_speed + grand(5001) - 2500, bullet[b].y_speed + grand(5001) - 2500, 
       bullet[b].owner, bullet[b].damage, 20 + grand(40), bullet[b].mass, 0, 0, 0, bullet[b].colours, 1, 
       bullet[b].special1, 0, 0, 0, 0);
    }
    break; 
   } 
// fall-through:
  case BULLET_FORK3:
   passing_colours [0] = bullet[b].special1;
      j = 36 + grand(12);
      create_cloud(CLOUD_SMALL_FADING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       j, 3, 0, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x,
       bullet[b].y,
       0, 0, 0, 0, 420 + crandom(105),60,0, 0, 0, 0, passing_colours);
   break;
  case BULLET_WORM_SORROW:
  case BULLET_WORM_AGONY:
  case BULLET_EVIL_EEL:
  case BULLET_PANIC_EEL:
  case BULLET_SQUIRMY:
  case BULLET_EVIL_WORM:
  case BULLET_TURN_WORM:
  case BULLET_SWIRL2:
       passing_colours [0] = bullet[b].colours [0];
       passing_colours [1] = bullet[b].colours [1];
       passing_colours [2] = bullet[b].colours [2];
       passing_colours [3] = bullet[b].colours [3];
       create_cloud(CLOUD_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colours);
//   if (hit_what != HIT_NOTHING)
//    break;   
   passing_colours [0] = bullet[b].colours [3];
      j = 36 + grand(12);
      create_cloud(CLOUD_SMALL_FADING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       j, 3, 0, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x,
       bullet[b].y,
       0, 0, 0, 0, 420 + crandom(105),60,0, 0, 0, 0, passing_colours);
    break;   
  case BULLET_CLAW:
       passing_colours [0] = bullet[b].colours [3];
       passing_colours [1] = bullet[b].colours [2];
       passing_colours [2] = bullet[b].colours [1];
       passing_colours [3] = bullet[b].colours [0];
       create_cloud(CLOUD_TRANS_FADING_LINE,
        bullet[b].x - bullet[b].x_speed,
        bullet[b].y - bullet[b].y_speed,
        bullet[b].x,
        bullet[b].y,
        0,
        0,
        10, 2,0, 0, 0, 0, passing_colours);
// passing_colours [0] = bullet[b].colours [3];
       passing_colours [0] = bullet[b].colours [0];
       passing_colours [1] = bullet[b].colours [1];
       passing_colours [2] = bullet[b].colours [2];
       passing_colours [3] = bullet[b].colours [3];
      j = 46 + grand(17);
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       j, 3, 0, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x,
       bullet[b].y,
       0, 0, 0, 0, 420 + crandom(105),60,0, 0, 0, 0, passing_colours);
    break;   
   case BULLET_CIRCLES: 
   case BULLET_CIRCLES2: 
      passing_colours [0] = bullet[b].colours [0];
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       1500,-150,20, 0, 0, 0, passing_colours);
     break;
   case BULLET_BIGCIRCLES: 
      passing_colours [0] = bullet[b].colours [0];
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
       2500,-190,30, 0, 0, 0, passing_colours);
     break;
    case BULLET_BLOCKS:
      passing_colours [0] = bullet[b].colours [0];
      passing_colours [1] = bullet[b].colours [1];
      passing_colours [2] = bullet[b].colours [2];
      passing_colours [3] = bullet[b].colours [3];
      create_cloud(CLOUD_BLOCK3,
       bullet[b].x,
       bullet[b].y,
       0, 0,
       0, 0,
//       100 + grand(50),-2 - grand(4),1, bullet[b].angle, grand(2), 0, passing_colours);
       100 + grand(50),3,0, bullet[b].angle, grand(2), 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x, bullet[b].y,
       0, 0, 0, 0, 1600 + crandom(800),48,0, 0, 0, 0, passing_colours);
     break;
    case BULLET_OVERBLOCKS:
      passing_colours [0] = bullet[b].colours [0];
      passing_colours [1] = bullet[b].colours [1];
      passing_colours [2] = bullet[b].colours [2];
      passing_colours [3] = bullet[b].colours [3];
       create_cloud(CLOUD_BLOCK1,
        bullet[b].x + xpart(bullet[b].angle, 25000),
        bullet[b].y + ypart(bullet[b].angle, 25000),
        bullet[b].x - xpart(bullet[b].angle, 25000),
        bullet[b].y - ypart(bullet[b].angle, 25000),
        0, 0,
        50 + grand(8),1,0, bullet[b].angle, 0, 0, passing_colours);
      passing_colours [0] = bullet[b].colours [1];
      for (j = 0; j < 8; j ++)
      {
       xs = grand(ANGLE_FULL); 
       ys = 2500 + grand(2500);
       create_cloud(CLOUD_BLOCK1,
        bullet[b].x + xpart(xs, 15000),
        bullet[b].y + ypart(xs, 15000),
        bullet[b].x - xpart(xs, 15000),
        bullet[b].y - ypart(xs, 15000),
        xpart(xs, ys),
        ypart(xs, ys),
        40 + grand(20),2,0, xs, 0, 0, passing_colours);
       } 
     break;
    case BULLET_ZIGZAG2:
      passing_colours [0] = bullet[b].colours [0];
      passing_colours [1] = bullet[b].colours [1];
      passing_colours [2] = bullet[b].colours [2];
      passing_colours [3] = bullet[b].colours [3];
        create_cloud(CLOUD_BLOCK1,
         bullet[b].x - bullet[b].x_speed * 3,
         bullet[b].y - bullet[b].y_speed * 3,
         bullet[b].x,
         bullet[b].y,
         0, 0,
         40, 1,0, bullet[b].special4, 30, 0, passing_colours);
//         200, 5,0, bullet[b].special4, 30, 0, passing_colours);
      break;   
     case BULLET_CURVEY:
      simple_shockwave(TRANS_WHITE, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50), 20);
      break; 
//  case BULLET_SHOCK:  
/*  case BULLET_SHOCK_PATH:
  case BULLET_SHOCK_PATH_SMALL:
  case BULLET_GOLDEN_PATH:
  case BULLET_GOLDEN_PATH_SMALL:
   passing_colours[0] = bullet[b].colours [3];
   passing_colours[1] = bullet[b].colours [3];
   passing_colours[2] = bullet[b].colours [2];
   passing_colours[3] = bullet[b].colours [1];
   passing_colours[4] = bullet[b].colours [0];
   place_speck_burst(bullet[b].x, bullet[b].y, xs,
    ys, 3 + grand(3), 10, 21, 450, passing_colours);
   break;
  case BULLET_SNOW_DART:
  case BULLET_ICE_DART:
  case BULLET_FROZEN_BREATH:
      create_cloud(CLOUD_BLUE_BLOB2,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 8 + crandom(3),1,0, 0, 0, 0, passing_colours);
    break;   
  case BULLET_BURNING_DRAGGED:*/
  case BULLET_BURNING_SPIRIT:
      create_cloud(CLOUD_ORANGE_BLOB2,
       bullet[b].x,// + grand(2001) - 1000,
       bullet[b].y,// + grand(2001) - 1000,
       0, 0, 0, 0, 12 + crandom(5),1,0, 0, 0, 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x,
       bullet[b].y,
       0, 0, 0, 0, 550 + crandom(105),40,0, 0, 0, 0, passing_colours);
    break;   
  case BULLET_BURST:
   passing_colours [0] = TRANS_DRED;//bullet[b].colours [0];
   passing_colours [1] = TRANS_LRED;//bullet[b].colours [1];
   passing_colours [2] = TRANS_YELLOW;//bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(300) + bullet[b].special2 * 10, passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 50, 8200, bullet[b].owner);
   for (j = 0; j < 6 + grand(4); j ++)
   {    
      create_bullet(BULLET_BURNING_DRAGGED, bullet[b].x, bullet[b].y,
       grand(15001) - 7500, grand(15001) - 7500, bullet[b].owner,
       100, 20 + grand(40), 10, 0, 0, 0, bullet[b].colours, 1, 
       0, 0, 0, 0, 0);
   }
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50), 15);
   bullet_soundf(b, NWAV_BURST, 800 + grand(150)); 
   break;
  case BULLET_PUFFY3:
   passing_colours [0] = TRANS_DGREY;//bullet[b].colours [0];
   passing_colours [1] = TRANS_DGREEN;//bullet[b].colours [1];
   passing_colours [2] = TRANS_LGREEN;//bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(100), passing_colours);
   blast(bullet[b].x, bullet[b].y, 20000, 1, 3200, bullet[b].owner);
   for (j = 0; j < 10 + grand(6); j ++)
   {    
      xa = grand(ANGLE_FULL);
      m = 500 + grand(7000);
      create_bullet(BULLET_EVIL_STAR, bullet[b].x, bullet[b].y,
       xpart(xa, m), ypart(xa, m), bullet[b].owner,
       50, 70 + grand(80), 10, 0, 0, 0, bullet[b].colours, 1, 
       0, 0, 0, 0, 0);
   }
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 250 + crandom(50), 15);
   bullet_soundf(b, NWAV_DNO, 400 + grand(100)); 
   break;
  case BULLET_PUFFY4:
   passing_colours [0] = TRANS_DBLUE;//bullet[b].colours [0];
   passing_colours [1] = TRANS_LBLUE;//bullet[b].colours [1];
   passing_colours [2] = TRANS_WHITE;//bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(100), passing_colours);
   blast(bullet[b].x, bullet[b].y, 20000, 1, 3200, bullet[b].owner);
   for (j = 0; j < 10 + grand(6); j ++)
   {    
      xa = grand(ANGLE_FULL);
      m = 2000 + grand(13000);
      create_bullet(BULLET_ZAP_DRAG, bullet[b].x, bullet[b].y,
       xpart(xa, m), ypart(xa, m), bullet[b].owner,
       30, 20 + grand(10), 30, 0, 0, 0, bullet[b].colours, 1, 
       0, 0, 0, 0, 0);
   }
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 250 + crandom(50), 15);
   bullet_soundf(b, NWAV_BURST, 300 + grand(150)); 
   break;
  case BULLET_BFLAK:
/*   passing_colours [0] = TRANS_DRED;
   passing_colours [1] = TRANS_LRED;
   passing_colours [2] = TRANS_YELLOW;*/
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 350 + grand(300), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 200, 8200, bullet[b].owner);
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50), 15);
   bullet_soundf(b, NWAV_BURSTZ, 1900 + grand(300)); 
   break;
  case BULLET_BFLAK2:
   passing_colours [0] = TRANS_ORANGE;
   passing_colours [1] = TRANS_YELLOW;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 350 + grand(100), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 150, 3200, bullet[b].owner);
   for (j = 0; j < 8 + grand(5); j ++)
   {    
      xa = grand(ANGLE_FULL);
      m = 2000 + grand(9000);
      create_bullet(BULLET_ZAP_DRAG, bullet[b].x, bullet[b].y,
       xpart(xa, m), ypart(xa, m), bullet[b].owner,
       50, 20 + grand(10), 30, 0, 0, 0, bullet[b].colours, 1, 
       0, 0, 0, 0, 0);
   }
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 250 + crandom(50), 15);
   bullet_soundf(b, NWAV_BURSTZ, 1600 + grand(300)); 
   break;
  case BULLET_MBOMB:
   passing_colours [0] = TRANS_ORANGE;
   passing_colours [1] = TRANS_YELLOW;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 1100 + grand(200), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 200, 3200, bullet[b].owner);
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 500 + crandom(50), 15);
   break;
  case BULLET_MINE1:
   passing_colours [0] = TRANS_DRED;
   passing_colours [1] = TRANS_LRED;
   passing_colours [2] = TRANS_YELLOW;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(100), passing_colours);
   blast(bullet[b].x, bullet[b].y, 20000, 200, 8200, bullet[b].owner);
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 200 + crandom(50), 15);
   bullet_soundf(b, NWAV_SHORTBURST, 1000 + grand(100)); 
   break;
  case BULLET_MINE2:
   passing_colours [0] = TRANS_DRED;
   passing_colours [1] = TRANS_LRED;
   passing_colours [2] = TRANS_YELLOW;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(100), passing_colours);
   blast(bullet[b].x, bullet[b].y, 5000, 20, 1200, bullet[b].owner);
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 200 + crandom(50));
   passing_colours [0] = TRANS_DRED;
   passing_colours [1] = TRANS_LRED;
   passing_colours [2] = TRANS_YELLOW;
   passing_colours [3] = TRANS_YELLOW;
    for (j = 0; j < 6; j ++)
    {
      xs = xpart(((ANGLE_FULL / 6) * j) + bullet[b].angle, 7000);
      ys = ypart(((ANGLE_FULL / 6) * j) + bullet[b].angle, 7000);
      create_bullet(BULLET_BOLT, bullet[b].x + xs, bullet[b].y + ys,
       xs, ys, bullet[b].owner,
       200, 10 + grand(5), 40, 0, 0, 0, passing_colours, 1, 
       2500,30,0,0,0);
    }
   bullet_soundf(b, NWAV_SHORTBURST, 1200 + grand(300)); 
   break;
  case BULLET_MINE3:
/*   passing_colours [0] = TRANS_ORANGE;
   passing_colours [1] = TRANS_YELLOW;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(100), passing_colours);
   blast(bullet[b].x, bullet[b].y, 5000, 20, 1200, bullet[b].owner);
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 200 + crandom(50));
   passing_colours [0] = COLOUR_YELLOW8;
//      xs = xpart(((ANGLE_FULL / 6) * j) + bullet[b].angle, 7000);
//      ys = ypart(((ANGLE_FULL / 6) * j) + bullet[b].angle, 7000);
      create_bullet(BULLET_SEEKER1, bullet[b].x, bullet[b].y,
       0, 0, bullet[b].owner,
       200, 60 + grand(30), 50, grand(ANGLE_FULL), 0, 0, passing_colours, 1, 
       closest_target(bullet[b].x, bullet[b].y),20,6,200,4001);*/
   passing_colours [0] = TRANS_ORANGE;//bullet[b].colours [0];
   passing_colours [1] = TRANS_YELLOW;//bullet[b].colours [1];
   passing_colours [2] = TRANS_WHITE;//bullet[b].colours [2];
   bullet[b].colours [3] = COLOUR_ORANGE4;
   bullet[b].colours [2] = COLOUR_ORANGE6;
   bullet[b].colours [1] = COLOUR_YELLOW7;
   bullet[b].colours [0] = COLOUR_YELLOW8;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(100), passing_colours);
   blast(bullet[b].x, bullet[b].y, 20000, 1, 2500, bullet[b].owner);
   for (j = 0; j < 10 + grand(6); j ++)
   {    
      xa = grand(ANGLE_FULL);
      m = 2000 + grand(12000);
      create_bullet(BULLET_ZAP_DRAG, bullet[b].x, bullet[b].y,
       xpart(xa, m), ypart(xa, m), bullet[b].owner,
       40, 20 + grand(10), 30, 0, 0, 0, bullet[b].colours, 1, 
       0, 0, 0, 0, 0);
   }
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 250 + crandom(50), 15);
   bullet_soundf(b, NWAV_SHORTBURST, 700 + grand(50)); 
   break;
  case BULLET_WINGS1:
  case BULLET_WINGS2:
  case BULLET_WINGS3:
   passing_colours [0] = TRANS_DRED;
   passing_colours [1] = TRANS_LRED;
   passing_colours [2] = TRANS_YELLOW;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 100 + grand(50), passing_colours);
   j = grand(ANGLE_FULL);
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   j += ANGLE_FULL / 3;
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   j += ANGLE_FULL / 3;
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   blast(bullet[b].x, bullet[b].y, 15000, 250, 6000, bullet[b].owner);
   bullet_soundvf(b, NWAV_BURST, 70, 600 + grand(100)); 
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50));
   break;
  case BULLET_BIGWINGS1:
  case BULLET_BIGWINGS2:
  case BULLET_BIGWINGS3:
   passing_colours [0] = TRANS_DGREEN;
   passing_colours [1] = TRANS_LGREEN;
   passing_colours [2] = TRANS_YELLOW;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(50), passing_colours);
   j = grand(ANGLE_FULL);
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   j += ANGLE_FULL / 5;
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   j += ANGLE_FULL / 5;
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   j += ANGLE_FULL / 5;
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   j += ANGLE_FULL / 5;
   place_explosion(bullet[b].x + xpart(j, 5000), bullet[b].y + ypart(j, 5000), xpart(j, 1000), ypart(j, 1000), 5 + grand(5), passing_colours);
   blast(bullet[b].x, bullet[b].y, 20000, 300, 9000, bullet[b].owner);
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50));
   bullet_soundvf(b, NWAV_BURST, 120, 400 + grand(100)); 
   break;
  case BULLET_E_BOMB:
   passing_colours [0] = TRANS_DRED;//bullet[b].colours [0];
   passing_colours [1] = TRANS_LRED;//bullet[b].colours [1];
   passing_colours [2] = TRANS_YELLOW;//bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 700 + grand(300), passing_colours);
   blast(bullet[b].x, bullet[b].y, 30000, 200, 11000, bullet[b].owner);
   passing_colours [0] = TRANS_YELLOW;
   passing_colours [1] = COLOUR_YELLOW8;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 7 + grand(4), 15, 15, 2500, 300, passing_colours);
//   simple_shockwave(TRANS_YELLOW, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50));
   bullet_soundf(b, NWAV_SHORTBURST, 1500 + grand(100)); 
   break;
  case BULLET_E_BOMB2:
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;//bullet[b].colours [1];
   passing_colours [2] = TRANS_WHITE;//bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(50), passing_colours);
   blast(bullet[b].x, bullet[b].y, 10000, 100, 8500, bullet[b].owner);
   passing_colours [0] = TRANS_WHITE;
   passing_colours [1] = COLOUR_BLUE8;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 4 + grand(4), 15, 15, 1200, 300, passing_colours);
//   simple_shockwave(TRANS_YELLOW, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50));
   bullet_soundf(b, NWAV_SHORTBURST, 1500 + grand(100)); 
   break;
  case BULLET_THICK_SHOCK:
    passing_colours [0] = bullet[b].colours [0];
      create_cloud(CLOUD_LARGE_TRANS_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 0, 0, 1200 + grand(350), -3, 8, 0, 300 + grand(50), 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x, bullet[b].y,
       0, 0, 0, 0, 1600 + crandom(800),0,12, 0, 0, 0, passing_colours);
       break;
  case BULLET_YELLOW_PULSE:
    passing_colours [0] = bullet[b].colours [0];
      j = bullet[b].special2 + grand(12);
      create_cloud(CLOUD_LARGE_FADING_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 0, 0, j, -3, 1, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LARGE_FADING_CIRCLE,
       bullet[b].x + xpart(bullet[b].angle + ANGLE_QUARTER, 19000), 
       bullet[b].y + ypart(bullet[b].angle + ANGLE_QUARTER, 19000), 
       0, 0, 0, 0, j, -3, 3, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LARGE_FADING_CIRCLE,
       bullet[b].x + xpart(bullet[b].angle - ANGLE_QUARTER, 19000), 
       bullet[b].y + ypart(bullet[b].angle - ANGLE_QUARTER, 19000), 
       0, 0, 0, 0, j, -3, 3, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x, bullet[b].y,
       0, 0, 0, 0, 1420 + crandom(105),30,7, 0, 0, 0, passing_colours);
   blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1, bullet[b].special4, bullet[b].owner);
   break;
  case BULLET_SEEKER1:
    passing_colours [0] = bullet[b].colours [2];
      j = 764 + grand(120);
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 0, 0, j, -3, 9, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x, bullet[b].y,
       0, 0, 0, 0, 680 + crandom(105),-3,9, 0, 0, 0, passing_colours);
   break;
  case BULLET_SEEKER2:
    passing_colours [0] = bullet[b].colours [2];
      j = 1550 + grand(212);
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 0, 0, j, -3, 7, 0, j, 0, passing_colours);
      create_cloud(CLOUD_LIGHT,
       bullet[b].x, bullet[b].y,
       0, 0, 0, 0, 1280 + crandom(105),-3,7, 0, 0, 0, passing_colours);
   break;
  case BULLET_SEEKER3:
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 1500 + grand(300), passing_colours);
   blast(bullet[b].x, bullet[b].y, 50000, 150, 500, bullet[b].owner);
   passing_colours [0] = TRANS_WHITE2;
   passing_colours [1] = COLOUR_WHITE;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5 + grand(3), 15, 15, 2500, 300, passing_colours);
   bullet_soundf(b, NWAV_BURSTZ, 800 + grand(100)); 
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50));
   break;
  case BULLET_BIGSEEKER:
//   passing_colours [0] = //TRANS_DRED;
//   passing_colours [1] = //TRANS_LRED;
//   passing_colours [2] = //TRANS_YELLOW;
   passing_colours [0] = TRANS_LBLUE;
   passing_colours [1] = TRANS_PURPLE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 2200 + grand(500), passing_colours);
   blast(bullet[b].x, bullet[b].y, 50000, 300, 1000, bullet[b].owner);
   simple_shockwave(TRANS_PURPLE, bullet[b].x, bullet[b].y, 0, 0, 700 + crandom(50), 15);
//   passing_colours [0] = TRANS_WHITE2;
//   passing_colours [1] = COLOUR_WHITE;
//   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5 + grand(3), 15, 15, 2500, 300, passing_colours);
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 400 + crandom(50));
   bullet_soundf(b, NWAV_BURSTZ, 500 + grand(100)); 
   break;
  case BULLET_CHARGE: 
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 2200 + grand(500), passing_colours);
   blast(bullet[b].x, bullet[b].y, 80000, 450, 1000, bullet[b].owner);
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 700 + crandom(50), 15);
   bullet_soundf(b, NWAV_BANG, 700 + grand(100)); 
   break;
  case BULLET_SWIRL1: 
   passing_colours [0] = TRANS_LRED;
   passing_colours [1] = TRANS_ORANGE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 2200 + grand(500), passing_colours);
   break;
  case BULLET_DISRUPT1:
      create_bullet(BULLET_DISRUPT1_DROP, bullet[b].x, bullet[b].y,
       0, 0, bullet[b].owner,
       0, 60, 50, 0, 0, 0, passing_colours, 1, 
       10,0,0,0,0);
   bullet_soundf(b, NWAV_CYMBAL, 400 + grand(20)); 
   break;
  case BULLET_DISRUPT2:
    passing_colours [0] = TRANS_REVERSE;
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 0, 0, 3500, -3, 3, 0, 0, 0, passing_colours);
   blast(bullet[b].x, bullet[b].y, 100000, 100, -4000, bullet[b].owner);
   bullet_soundf(b, NWAV_CHIME, 300 + grand(20)); 
   break;

  case BULLET_GREEN_BLAT:
     if (hit_what == HIT_NOTHING)
       {
        create_cloud(CLOUD_GREEN_BLOB2,
         bullet[b].x,// + grand(2001) - 1000,
         bullet[b].y,// + grand(2001) - 1000,
         0, 0, 0, 0, 12 + crandom(5),1,0, 0, 0, 0, passing_colours);
        create_cloud(CLOUD_LIGHT,
         bullet[b].x,
         bullet[b].y,
         0, 0, 0, 0, 400 + crandom(105),30,0, 0, 0, 0, passing_colours);
       }
        else
       {
         passing_colours[0] = TRANS_DGREEN;
         passing_colours[1] = TRANS_LGREEN;
         passing_colours[2] = TRANS_YELLOW;
         place_explosion(bullet[b].x, bullet[b].y, 0, 0, 250 + grand(150), passing_colours);
        create_cloud(CLOUD_LIGHT,
         bullet[b].x,
         bullet[b].y,
         0, 0, 0, 0, 450 + crandom(105),40,0, 0, 0, 0, passing_colours);
//          passing_colours [1] = TRANS_LBLUE;
//         passing_colours [2] = TRANS_WHITE;
//          place_burstlet_burst(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(3),
//              4, 2, 1000, 1500, 4, passing_colours);
       } 
/*      create_cloud(CLOUD_GREEN_BLOB2,
       bullet[b].x,// + grand(2001) - 1000,
       bullet[b].y,// + grand(2001) - 1000,
       0, 0, 0, 0, 12 + crandom(5),1,0, 0, 0, 0, passing_colours);
//     if (hit_what != HIT_NOTHING)
//      {
//          passing_colours [1] = TRANS_LGREEN;
//          passing_colours [2] = TRANS_YELLOW;
//          place_burstlet_burst(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(3),
//              4, 2, 1000, 1500, 4, passing_colours);
//       } 
      create_cloud(CLOUD_LIGHT,
       bullet[b].x,
       bullet[b].y,
       0, 0, 0, 0, 550 + crandom(105),40,0, 0, 0, 0, passing_colours);*/
    break;   
  case BULLET_BLUE_BLAT:
     if (hit_what == HIT_NOTHING)
       {
        create_cloud(CLOUD_BLUE_BLOB2,
         bullet[b].x,// + grand(2001) - 1000,
         bullet[b].y,// + grand(2001) - 1000,
         0, 0, 0, 0, 12 + crandom(5),1,0, 0, 0, 0, passing_colours);
        create_cloud(CLOUD_LIGHT,
         bullet[b].x,
         bullet[b].y,
         0, 0, 0, 0, 550 + crandom(105),40,0, 0, 0, 0, passing_colours);
       }
        else
       {
         passing_colours[0] = TRANS_DBLUE;
         passing_colours[1] = TRANS_LBLUE;
         passing_colours[2] = TRANS_WHITE;
         place_explosion(bullet[b].x, bullet[b].y, 0, 0, 250 + grand(150), passing_colours);
        create_cloud(CLOUD_LIGHT,
         bullet[b].x,
         bullet[b].y,
         0, 0, 0, 0, 450 + crandom(105),40,0, 0, 0, 0, passing_colours);
//          passing_colours [1] = TRANS_LBLUE;
//         passing_colours [2] = TRANS_WHITE;
//          place_burstlet_burst(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(3),
//              4, 2, 1000, 1500, 4, passing_colours);
       } 
    break;   
  case BULLET_SNOW_DART_SMALL:
  case BULLET_ICE_DART_SMALL:
      create_cloud(CLOUD_BLUE_BLOB2,
       bullet[b].x + grand(2001) - 1000,
       bullet[b].y + grand(2001) - 1000,
       0, 0, 0, 0, 6 + crandom(3),1,0, 0, 0, 0, passing_colours);
    break;   
  case BULLET_NUMEROUS_BLADE:
   passing_colours[0] = bullet[b].colours [0];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [1];
   passing_colours[3] = bullet[b].colours [2];
   passing_colours[4] = bullet[b].colours [2];
   place_speck_burst(bullet[b].x, bullet[b].y, xs,
    ys, 1 + grand(2), 10, 21, 450, passing_colours);
   passing_colours [0] = bullet[b].colours [0];
      j = 16 + grand(12);
      create_cloud(CLOUD_SMALL_FADING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       j, 1, 0, 0, j, 0, passing_colours);
   break;
  case BULLET_CRYSTAL_TOOTH:
  case BULLET_CRYSTAL_SPINE:
   passing_colours[0] = bullet[b].colours [0];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [1];
   passing_colours[3] = bullet[b].colours [2];
   passing_colours[4] = bullet[b].colours [2];
   place_speck_burst(bullet[b].x, bullet[b].y, xs,
    ys, 2 + grand(3), 10, 21, 450, passing_colours);
// fall-through...
  case BULLET_SILVER_TOOTH:
  case BULLET_BRASS_TOOTH:
  case BULLET_GOLDEN_NEEDLE:
  case BULLET_NUMEROUS_DART:
  case BULLET_ZAP:
  case BULLET_ZAP_DRAG:
  case BULLET_SLIVER:
//   if (hit_what != HIT_NOTHING)
//    break;
   passing_colours [0] = bullet[b].colours [0];
      j = 30 + grand(20);
      create_cloud(CLOUD_SMALL_FADING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       j, -1, 1, 0, j, 0, passing_colours);
   break;
  case BULLET_SILVER_TOOTH_SMALL:
  case BULLET_GOLDEN_NEEDLE_SMALL:
  case BULLET_BRASS_TOOTH_SMALL:
   passing_colours [0] = bullet[b].colours [0];
      j = 20 + grand(15);
      create_cloud(CLOUD_SMALL_FADING_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       j, 0, 1, 0, j, 0, passing_colours);
   break;
  case BULLET_PARTICLE_SPITTER:
   passing_colours [0] = COLOUR_GREEN8;
      create_cloud(CLOUD_SMALL_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       200 + crandom(120),15,15, 0, 0, 0, passing_colours);
   break;
  case BULLET_FAR_SPITTER:
   passing_colours [0] = COLOUR_YELLOW8;
      create_cloud(CLOUD_SMALL_CIRCLE,
       bullet[b].x,
       bullet[b].y,
       xs, ys,
       0,
       0,
       200 + crandom(120),15,15, 0, 0, 0, passing_colours);
   break;
  case BULLET_BLAST:
  case BULLET_BOLT:
   passing_colours[0] = bullet[b].colours [0];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_BIGBALL1:
   passing_colours[0] = bullet[b].colours [2];
   passing_colours[1] = bullet[b].colours [1];
   passing_colours[2] = bullet[b].colours [0];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 400 + grand(150) + bullet[b].special1 * 50, passing_colours);
   break;
  case BULLET_TRI1:
    passing_colours [3] = TRANS_DRED; //bullet[b].colours [0];
    passing_colours [2] = TRANS_LRED; //bullet[b].colours [1];
    passing_colours [1] = TRANS_ORANGE; //bullet[b].colours [2];
    passing_colours [0] = TRANS_YELLOW; //bullet[b].colours [3];
    for (j = 0; j < 10; j ++)
    {
     xs = grand(ANGLE_FULL);
      create_cloud(CLOUD_BIG_BLAST_CIRCLE,
       bullet[b].x,
       bullet[b].y, //grand(10001) - 5000,
       0, 0, 
//       bullet[b].x_speed + xpart(xs, 5000 + grand(6000)), 
//       bullet[b].y_speed + ypart(xs, 5000 + grand(6000)),
         xpart(xs, 4000 + grand(6000)), 
         ypart(xs, 4000 + grand(6000)),
       85 + grand(50), 3, 0, 0, 0, 0, passing_colours);
    }
    passing_colours [0] = TRANS_DRED; //bullet[b].colours [0];
    passing_colours [1] = TRANS_LRED; //bullet[b].colours [1];
    passing_colours [2] = TRANS_YELLOW; //bullet[b].colours [2];
    place_explosion(bullet[b].x, bullet[b].y, 0, 0, 400 + grand(300), passing_colours);
    blast(bullet[b].x, bullet[b].y, 50000, 150, 3000, bullet[b].owner);
    bullet_soundf(b, NWAV_BURST, 500 + grand(100)); 
    break;
  case BULLET_TRI2:
    passing_colours [3] = TRANS_DBLUE; //bullet[b].colours [0];
    passing_colours [2] = TRANS_DBLUE; //bullet[b].colours [1];
    passing_colours [1] = TRANS_LBLUE; //bullet[b].colours [2];
    passing_colours [0] = TRANS_WHITE; //bullet[b].colours [3];
    for (j = 0; j < 10; j ++)
    {
     xs = grand(ANGLE_FULL);
      create_cloud(CLOUD_BIG_BLAST_CIRCLE,
       bullet[b].x,
       bullet[b].y, //grand(10001) - 5000,
       0, 0, 
//       bullet[b].x_speed + xpart(xs, 5000 + grand(6000)), 
//       bullet[b].y_speed + ypart(xs, 5000 + grand(6000)),
         xpart(xs, 4000 + grand(6000)), 
         ypart(xs, 4000 + grand(6000)),
       85 + grand(50), 3, 0, 0, 0, 0, passing_colours);
    }
    passing_colours [0] = TRANS_DBLUE; //bullet[b].colours [0];
    passing_colours [1] = TRANS_LBLUE; //bullet[b].colours [1];
    passing_colours [2] = TRANS_WHITE; //bullet[b].colours [2];
    place_explosion(bullet[b].x, bullet[b].y, 0, 0, 400 + grand(300), passing_colours);
    blast(bullet[b].x, bullet[b].y, 50000, 200, 4000, bullet[b].owner);
    bullet_soundf(b, NWAV_BURST, 450 + grand(50)); 
    break;
  case BULLET_TRI3:
    passing_colours [3] = TRANS_LRED; //bullet[b].colours [0];
    passing_colours [2] = TRANS_ORANGE; //bullet[b].colours [1];
    passing_colours [1] = TRANS_YELLOW; //bullet[b].colours [2];
    passing_colours [0] = TRANS_WHITE; //bullet[b].colours [3];
    for (j = 0; j < 14; j ++)
    {
     xs = grand(ANGLE_FULL);
      create_cloud(CLOUD_BIG_BLAST_CIRCLE,
       bullet[b].x,
       bullet[b].y, //grand(10001) - 5000,
       0, 0, 
//       bullet[b].x_speed + xpart(xs, 5000 + grand(6000)), 
//       bullet[b].y_speed + ypart(xs, 5000 + grand(6000)),
         xpart(xs, 4000 + grand(6000)), 
         ypart(xs, 4000 + grand(6000)),
       85 + grand(50), 3, 0, 0, 0, 0, passing_colours);
    }
    passing_colours [0] = TRANS_ORANGE; //bullet[b].colours [0];
    passing_colours [1] = TRANS_YELLOW; //bullet[b].colours [1];
    passing_colours [2] = TRANS_WHITE; //bullet[b].colours [2];
    place_explosion(bullet[b].x, bullet[b].y, 0, 0, 400 + grand(300), passing_colours);
    blast(bullet[b].x, bullet[b].y, 50000, 250, 5000, bullet[b].owner);
    bullet_soundf(b, NWAV_BURST, 400 + grand(10)); 
    break;
  case BULLET_OVERTRI:
    passing_colours [3] = TRANS_DGREEN; //bullet[b].colours [0];
    passing_colours [2] = TRANS_LGREEN; //bullet[b].colours [1];
    passing_colours [1] = TRANS_YELLOW; //bullet[b].colours [2];
    passing_colours [0] = TRANS_WHITE; //bullet[b].colours [3];
    for (j = 0; j < 24; j ++)
    {
     xs = grand(ANGLE_FULL);
      create_cloud(CLOUD_BIG_BLAST_CIRCLE,
       bullet[b].x,
       bullet[b].y, //grand(10001) - 5000,
       0, 0, 
         xpart(xs, 4000 + grand(6000)), 
         ypart(xs, 4000 + grand(6000)),
       85 + grand(80), 3, 0, 0, 0, 0, passing_colours);
    }
    passing_colours [0] = TRANS_LGREEN; //bullet[b].colours [0];
    passing_colours [1] = TRANS_YELLOW; //bullet[b].colours [1];
    passing_colours [2] = TRANS_WHITE; //bullet[b].colours [2];
    place_explosion(bullet[b].x, bullet[b].y, 0, 0, 800 + grand(300), passing_colours);
    blast(bullet[b].x, bullet[b].y, 70000, 500, 6000, bullet[b].owner);
    bullet_soundf(b, NWAV_BURST, 200 + grand(10)); 
    break;
  case BULLET_EYE_DESOLATION_SMALL:
   passing_colours[2] = TRANS_YELLOW;
   passing_colours[1] = TRANS_ORANGE;
   passing_colours[0] = TRANS_DRED;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_EYE_DESOLATION:
   passing_colours [0] = bullet[b].colours [0];
   passing_colours [1] = bullet[b].colours [1];
   passing_colours [2] = bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(300) + bullet[b].special2 * 10, passing_colours);
   blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1, 2000 + bullet[b].special1 * 10, bullet[b].owner);
   if (bullet[b].special5 > 0 && hit_what != HIT_WAVE)
   {
    m = ANGLE_FULL / bullet[b].special5;
    for (j = 0; j < bullet[b].special5; j ++)
    {
      xs = xpart((m * j) + bullet[b].angle, 800 + bullet[b].special4 * 9);
      ys = ypart((m * j) + bullet[b].angle, 800 + bullet[b].special4 * 9);
      create_bullet(BULLET_EYE_DESOLATION_SMALL, bullet[b].x + xs * 3, bullet[b].y + ys * 3,
       xs, ys, bullet[b].owner,
       bullet[b].special1, 30 + grand(10), 100, 0, 0, 0, bullet[b].colours, 1, 
       0,0,0,0,0);
    }
   }
   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50) + bullet[b].special2 * 2, 15);
   bullet_soundf(b, NWAV_BURSTZL, 300 + grand(100)); 
   break;
  case BULLET_BURNING_EYE:
   passing_colours [0] = bullet[b].colours [0];
   passing_colours [1] = bullet[b].colours [1];
   passing_colours [2] = bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 200 + grand(300) + bullet[b].special2 * 10, passing_colours);
   blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1, 2000 + bullet[b].special1 * 10, bullet[b].owner);
   passing_colours [0] = TRANS_ORANGE;
   passing_colours [1] = COLOUR_ORANGE8;
   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5, 15, 15, 1200 + (bullet[b].special1 * 3), 300, passing_colours);
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50) + bullet[b].special2 * 2);
   bullet_soundf(b, NWAV_SHORTBURST, 1300 + grand(100)); 
   break;
  case BULLET_FLOWER:
  if (hit_what != HIT_NOTHING)
   {
    passing_colours [0] = TRANS_DGREEN;
    passing_colours [1] = TRANS_LGREEN;
    passing_colours [2] = TRANS_YELLOW;
    place_explosion(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(2), passing_colours);
    break;
   }
   switch(grand(5))
   {
    case 0:
     passing_colours [0] = TRANS_DRED;
     passing_colours [1] = TRANS_LRED;
     passing_colours [2] = TRANS_YELLOW;
     break;
    case 1:
     passing_colours [0] = TRANS_DGREEN;
     passing_colours [1] = TRANS_LGREEN;
     passing_colours [2] = TRANS_YELLOW;
     break;
    case 2:
     passing_colours [0] = TRANS_DBLUE;
     passing_colours [1] = TRANS_LBLUE;
     passing_colours [2] = TRANS_WHITE;
     break;
    case 3:
     passing_colours [0] = TRANS_ORANGE;
     passing_colours [1] = TRANS_YELLOW;
     passing_colours [2] = TRANS_WHITE;
     break;
    case 4:
     passing_colours [0] = TRANS_DBLUE;
     passing_colours [1] = TRANS_LBLUE;
     passing_colours [2] = TRANS_PURPLE;
     break;
   }  
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 80 + grand(20) + bullet[b].special1 / 2, passing_colours);
//   blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1, 2000 + bullet[b].special1 * 10, bullet[b].owner);
   blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1 / 2, -2000 - bullet[b].special1 * 10, bullet[b].owner);
//   passing_colours [1] = TRANS_DRED;
//   passing_colours [2] = TRANS_DRED;
   xs = bullet[b].special5 / 3; // cannot == 0!!
   int pc2 [4];
   for (j = 0; j < xs; j ++)
   {
    pc2 [0] = passing_colours [2];
    pc2 [1] = passing_colours [2];
    pc2 [2] = passing_colours [2];
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 
       bullet[b].x_speed + xpart(bullet[b].angle + j * (ANGLE_FULL / xs), 9000 + bullet[b].special1 * 9), 
       bullet[b].y_speed + ypart(bullet[b].angle + j * (ANGLE_FULL / xs), 9000 + bullet[b].special1 * 9),
       40 + grand(5) + bullet[b].special1 / 3, -20, 3, 0, 0, 0, pc2);
    pc2 [0] = passing_colours [1];
    pc2 [1] = passing_colours [1];
    pc2 [2] = passing_colours [1];
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 
       bullet[b].x_speed + xpart(bullet[b].angle + j * (ANGLE_FULL / xs), 8000 + bullet[b].special1 * 8), 
       bullet[b].y_speed + ypart(bullet[b].angle + j * (ANGLE_FULL / xs), 8000 + bullet[b].special1 * 8),
       50 + grand(10) + bullet[b].special1 / 3, -30, 3, 0, 0, 0, pc2);
   }
/*   for (j = 0; j < 3; j ++)
   {
    passing_colours [0] = TRANS_LRED;
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 
       bullet[b].x_speed + xpart(bullet[b].angle + j * (ANGLE_FULL / 3), 6900 + bullet[b].special1 * 9), 
       bullet[b].y_speed + ypart(bullet[b].angle + j * (ANGLE_FULL / 3), 6900 + bullet[b].special1 * 9),
       40 + grand(5) + bullet[b].special1 / 3, -20, 3, 0, 0, 0, passing_colours);
    passing_colours [0] = TRANS_DRED;
      create_cloud(CLOUD_BLAST_CIRCLE,
       bullet[b].x, bullet[b].y, 0, 0, 
       bullet[b].x_speed + xpart(bullet[b].angle + j * (ANGLE_FULL / 3), 6000 + bullet[b].special1 * 8), 
       bullet[b].y_speed + ypart(bullet[b].angle + j * (ANGLE_FULL / 3), 6000 + bullet[b].special1 * 8),
       50 + grand(10) + bullet[b].special1 / 3, -30, 3, 0, 0, 0, passing_colours);
   }*/
   bullet[b].type = BULLET_FLOWER2;
   bullet[b].timeout = 15;
   bullet_soundf(b, NWAV_BURSTZL, 800 + grand(10)); 
//   bullet_soundf(b, NWAV_BURST, 200 + grand(100)); 
   return; // NOTE return
  case BULLET_FLOWER2:
/*   if (hit_what != HIT_NOTHING)
   {
    passing_colours [0] = TRANS_DGREEN;
    passing_colours [1] = TRANS_LGREEN;
    passing_colours [2] = TRANS_YELLOW;
    place_explosion(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(2), passing_colours);
    break;
   }*/
//   passing_colours [0] = TRANS_ORANGE;
//   passing_colours [1] = TRANS_YELLOW;
//   passing_colours [2] = TRANS_WHITE;
   switch(grand(5))
   {
    case 0:
     bullet[b].colours [2] = TRANS_DRED;
     bullet[b].colours [1] = TRANS_LRED;
     bullet[b].colours [0] = TRANS_YELLOW;
     break;
    case 1:
     bullet[b].colours [2] = TRANS_DGREEN;
     bullet[b].colours [1] = TRANS_LGREEN;
     bullet[b].colours [0] = TRANS_YELLOW;
     break;
    case 2:
     bullet[b].colours [2] = TRANS_DBLUE;
     bullet[b].colours [1] = TRANS_LBLUE;
     bullet[b].colours [0] = TRANS_WHITE;
     break;
    case 3:
     bullet[b].colours [2] = TRANS_ORANGE;
     bullet[b].colours [1] = TRANS_YELLOW;
     bullet[b].colours [0] = TRANS_WHITE;
     break;
    case 4:
     bullet[b].colours [2] = TRANS_DBLUE;
     bullet[b].colours [1] = TRANS_LBLUE;
     bullet[b].colours [0] = TRANS_PURPLE;
     break;
   }  
   if (bullet[b].special5 > 0)
   {
    m = ANGLE_FULL / bullet[b].special5;
    for (j = 0; j < bullet[b].special5; j ++)
    {
      xs = xpart((m * j) + bullet[b].angle + ANGLE_QUARTER, 1200 + bullet[b].special4 * 15);
      ys = ypart((m * j) + bullet[b].angle + ANGLE_QUARTER, 1200 + bullet[b].special4 * 15);
      create_bullet(BULLET_PETAL1, //bullet[b].x, bullet[b].y, //
       bullet[b].x + xs, bullet[b].y + ys,
       xs, ys, bullet[b].owner,
       bullet[b].special1, 30 + grand(10), 100, 0, 0, 0, bullet[b].colours, 1, 
       0,0,0,0,0);
    }
   }
   bullet_soundf(b, NWAV_BURST, 200 + grand(100)); 
//   bullet_soundf(b, NWAV_BURSTZL, 200 + grand(10)); 
   break; 
  case BULLET_FROZEN_STAR:
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(2), passing_colours);
   break;
  case BULLET_SPORE:
   passing_colours [0] = TRANS_ORANGE;
   passing_colours [1] = TRANS_YELLOW;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(2), passing_colours);
   break;
  case BULLET_EVIL_STAR:
   passing_colours [0] = bullet[b].colours [0];
   passing_colours [1] = bullet[b].colours [1];
   passing_colours [2] = bullet[b].colours [2];
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 3 + grand(2), passing_colours);
   break;
  case BULLET_FROZEN_TOOTH:
   passing_colours [0] = TRANS_DBLUE;
   passing_colours [1] = TRANS_LBLUE;
   passing_colours [2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 20 + grand(10) + bullet[b].special2, passing_colours);
   //blast(bullet[b].x, bullet[b].y, bullet[b].special3, bullet[b].special1, 2000 + bullet[b].special1 * 10, bullet[b].owner);
//   passing_colours [0] = TRANS_LBLUE;
//   passing_colours [1] = COLOUR_WHITE;
//   place_line_burst(bullet[b].x, bullet[b].y, 0, 0, 5, 15, 15, 1200 + (bullet[b].special1 * 3), 300, passing_colours);
//   simple_shockwave(TRANS_WHITE2, bullet[b].x, bullet[b].y, 0, 0, 300 + crandom(50) + bullet[b].special2 * 2);
   break;
  case BULLET_BALL1:
  case BULLET_BALL2:
   passing_colours[0] = TRANS_ORANGE;
   passing_colours[1] = TRANS_YELLOW;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_PULSE1:
   passing_colours[0] = TRANS_ORANGE;
   passing_colours[1] = TRANS_YELLOW;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_PULSE2:
   passing_colours[0] = TRANS_DBLUE;
   passing_colours[1] = TRANS_LBLUE;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 300 + grand(150), passing_colours);
   break;
  case BULLET_OVERPULSE:
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 1300 + grand(450), passing_colours);
   break;
  case BULLET_TWIRLY1:
  case BULLET_TWIRLY2:
   passing_colours[0] = TRANS_ORANGE;
   passing_colours[1] = TRANS_YELLOW;
   passing_colours[2] = TRANS_WHITE;
   place_explosion(bullet[b].x, bullet[b].y, 0, 0, 900 + grand(100), passing_colours);
   break;
  case BULLET_TWISTY: 
    passing_colours [0] = bullet[b].colours [0];
    passing_colours [1] = bullet[b].colours [1];
    passing_colours [2] = bullet[b].colours [2];
     xa = (bullet[b].special1 % 32) * 32;
     xb = xpart(xa, 13 * GRAIN);
     cx = bullet[b].x + xpart(bullet[b].angle + ANGLE_QUARTER, xb);
     cy = bullet[b].y + ypart(bullet[b].angle + ANGLE_QUARTER, xb);
     place_explosion(cx, cy, 0, 0, 600 + grand(100), passing_colours);
     cx = bullet[b].x + xpart(bullet[b].angle - ANGLE_QUARTER, xb);
     cy = bullet[b].y + ypart(bullet[b].angle - ANGLE_QUARTER, xb);
     place_explosion(cx, cy, 0, 0, 600 + grand(100), passing_colours);
     break;   
  case BULLET_TWIRLY3:   
      create_cloud(CLOUD_TWIRLY_CIRCLE,
       bullet[b].x, bullet[b].y,
       0, 0,
       0, 0, 
       50, 1, 0, 0, 0, 0, passing_colours);
   break;
   case BULLET_CRYSTAL1: 
     create_cloud(CLOUD_SHATTER1, bullet[b].x, bullet[b].y, 0, 0,
      0, 0, 35, 1,
      0, 0, 0, 0, passing_colours);
     blast(bullet[b].x, bullet[b].y, 30000, 200, 6500, bullet[b].owner);
    bullet_soundf(b, NWAV_SHADOW, 700 + grand(100)); 
    break;
   case BULLET_CRYSTAL2: 
     create_cloud(CLOUD_SHATTER2, bullet[b].x, bullet[b].y, 0, 0,
      0, 0, 35, 2,
      0, bullet[b].angle, 0, 0, passing_colours);
     blast(bullet[b].x, bullet[b].y, 35000, 250, 7000, bullet[b].owner);
    bullet_soundf(b, NWAV_SHADOW, 500 + grand(100)); 
    break;
   case BULLET_LINE_PULSE: 
    passing_colours [0] = bullet[b].colours [0];
    for (j = 0; j < 9; j ++)
    {
     xa = bullet[b].angle + j * ANGLE_FULL / 9 + grand(ANGLE_1_SIXTEENTH) - grand(ANGLE_1_SIXTEENTH);
     create_cloud(CLOUD_LINE_SHADOW,
      bullet[b].x + xpart(xa, 11 * GRAIN),
      bullet[b].y + ypart(xa, 11 * GRAIN),
      0, 0,
      xpart(xa, 1200),
      ypart(xa, 1200),
      140 + grand(40),6 + grand(3),0, xa + ANGLE_QUARTER, 0, 0, passing_colours);
     } 
     break; 


  case BULLET_YELLOW_FLAK:
   blast(bullet[b].x, bullet[b].y, 22000, 200, 5000, bullet[b].owner);
      create_cloud(CLOUD_FLAK_BURST,
       bullet[b].x, bullet[b].y,
       0, 0, 0, 0,
       60,5,0, 0, 3, 0, passing_colours);
   bullet_soundvf(b, NWAV_SHORTBURST, 100, 1500 + grand(100)); 
   break;
 }

// Note: not all bullets get this far (eg mines)

 destroy_bullet(b);

}


void enemy_beam(int x, int y, int angle, int power, int type, int e)
{

 int x2, y2, angle2;
 int actor_size;
 int i, a, c;
 int a1, adiff, t_angle;


 if (power > 100)
    a = (150 - power) * 6;
     else
      a = 300;  
   if (power < 25)
    a = power * 12;
       
 if (power > 100)
    c = (150 - power) / 12 + 1;// + pulsate(64, 2, effect[dr].things [2]);
     else
      c = 5 + grand(3);// + pulsate(64, 1, effect[dr].things [2]);
   if (power < 40)
    c = power / 8 + grand(3);   
   if (c < 1)
    c = 1;
 
  for (i = 0; i < NO_ACTORS; i ++)
  {
   if (actor[i].in_play == 0)
    continue;
   if (actor[i].grace_period > 0)
    continue;

   actor_size = 7;
   if (actor[i].ship == SHIP_SMALL) 
    actor_size = 5;

   if (hypot(actor[i].x - (x + xpart(angle, 21 * GRAIN + a * GRAIN)), actor[i].y - (y + ypart(angle, 21 * GRAIN + a * GRAIN))) < (12 + actor_size) * GRAIN)
   {
          if (type == EFFECT_REDBEAM)
           actor_in_beam(i, angle, 20, 600, TRANS_YELLOW, TRANS_DRED);
            else
             actor_in_beam(i, angle, 20, 800, TRANS_WHITE, TRANS_LBLUE);
    continue;
   }

   if (hypot(actor[i].x - (enemy[e].x + xpart(enemy[e].angle, 20000)), 
             actor[i].y - (enemy[e].y + ypart(enemy[e].angle, 20000)))
             > hypot(actor[i].x - (enemy[e].x - xpart(enemy[e].angle, 20000)), 
             actor[i].y - (enemy[e].y - ypart(enemy[e].angle, 20000))))
              continue;

   
   if (hypot(actor[i].x - (x + xpart(angle, 21 * GRAIN)), actor[i].y - (y + ypart(angle, 21 * GRAIN))) > a * GRAIN)
    continue;
    
   x2 = x - xpart(angle, 200 * GRAIN);
   y2 = y - ypart(angle, 200 * GRAIN);
   angle2 = radians_to_angle(atan2(actor[i].y - y2, actor[i].x - x2));


 a1 = abs(radians_to_angle(atan2(y + ypart(angle, a * GRAIN) + ypart(angle - ANGLE_QUARTER, (c * 2 + 7) * GRAIN) - y2,
     x + xpart(angle, a * GRAIN) + xpart(angle - ANGLE_QUARTER, (c * 2 + actor_size) * GRAIN) - x2)));

 if (y + ypart(angle, a * GRAIN) + ypart(angle - ANGLE_QUARTER, (c * 2 + actor_size) * GRAIN) < y2)
  a1 = ANGLE_FULL - a1;

 angle2 = abs(angle2);

 if (actor[i].y < y2)
  angle2 = (ANGLE_FULL - angle2) % ANGLE_FULL;

  t_angle = angle;
  if (angle > ANGLE_HALF)
   t_angle = ANGLE_FULL - angle;
  t_angle = abs(t_angle);

  var1 = angle_difference(abs(angle2),
         abs(radians_to_angle(atan2(actor[i].y - y, actor[i].x - x))));
  var2 = abs(t_angle);
  var3 = abs(radians_to_angle(atan2(actor[i].y - y, actor[i].x - x)));

  adiff = abs(radians_to_angle(atan2(actor[i].y - y, actor[i].x - x)));
  if (adiff > ANGLE_HALF)
   adiff = ANGLE_FULL - adiff;
   
  var3 = adiff; 
 
 if (angle_difference(abs(angle2), abs(angle)) < angle_difference(abs(angle), abs(a1)))
 {
 
       {
        if (angle_difference(abs(t_angle), adiff) < ANGLE_QUARTER)
         {
          if (type == EFFECT_REDBEAM)
           actor_in_beam(i, angle, 20, 600, TRANS_YELLOW, TRANS_DRED);
            else
             actor_in_beam(i, angle, 20, 800, TRANS_WHITE, TRANS_LBLUE);
          continue;
         }
        }
   
  }

 }

}

void actor_in_beam(int i, int angle, int damage, int force, int colour1, int colour2)
{
 int passing_colours [5];

            passing_colours [0] = colour2;
   
                  create_cloud(CLOUD_MED_TRANS_CIRCLE,
                      actor[i].x,
                      actor[i].y,
                      0, 0,
                      0,
                      0,
                      1200 + grand(400),400, 0, 0, 0, 0, passing_colours);
            passing_colours [0] = colour1;
   
                  create_cloud(CLOUD_MED_TRANS_CIRCLE,
                      actor[i].x,
                      actor[i].y,
                      0, 0,
                      0,
                      0,
                      1000 + grand(300),400, 0, 0, 0, 0, passing_colours);
          actor[i].x_speed += xpart(angle, force);
          actor[i].y_speed += ypart(angle, force);
          actor[i].screen_shake_time += 2;
          if (actor[i].screen_shake_time > 12)
           actor[i].screen_shake_time = 12;
          hurt_actor(i, OWNER_ENEMY, damage);
}

void destroy_bullet(int b)
{
 bullet[b].type = BULLET_NONE;
}


int move_bullet(int mbull, char limited)
{


if (bullet[mbull].x_speed == 0 && bullet[mbull].y_speed == 0) return 1;
/*
 if (bullet[mbull].x + bullet[mbull].x_speed <= 0
      || bullet[mbull].x + bullet[mbull].x_speed >= arena.max_x - 3000)
 {
  bullet_impact(mbull, -1);
  return 0;
 }
 if (bullet[mbull].y + bullet[mbull].y_speed <= 0
     || bullet[mbull].y + bullet[mbull].y_speed >= arena.max_y - 4000)
 {
  bullet_impact(mbull, -1);
  return 0;
 }
*/

 if (bullet[mbull].x <= 0
      || bullet[mbull].x >= arena.max_x - 3000)
 {
  bullet_impact(mbull, -1, HIT_EDGE);
  return 0;
 }
 if (bullet[mbull].y <= 0
     || bullet[mbull].y >= arena.max_y - 4000)
 {
  bullet_impact(mbull, -1, HIT_EDGE);
  return 0;
 }

bullet[mbull].x2 += bullet[mbull].x_speed;
bullet[mbull].y2 += bullet[mbull].y_speed;

bullet[mbull].x += bullet[mbull].x_speed;
bullet[mbull].y += bullet[mbull].y_speed;

if (arena.waver_on_level != 0 && bullet[mbull].owner != OWNER_ENEMY)
{
 int e, d1, d2, nd;
 
 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_WAVER1 || enemy[e].type == ENEMY_WAVER2)
  {
   d1 = hypot(enemy[e].x - bullet[mbull].x, enemy[e].y - bullet[mbull].y);
   d2 = hypot(enemy[e].x - bullet[mbull].x - bullet[mbull].x_speed, enemy[e].y - bullet[mbull].y - bullet[mbull].y_speed);
   nd = 147 * GRAIN;
   if (enemy[e].type == ENEMY_WAVER2)
    nd = 197 * GRAIN;
   if ((d2 < nd && d1 >= nd)
       || (d1 >= nd - 8 * GRAIN && d1 <= nd + 8 * GRAIN)
       || (d2 >= nd && d1 < nd))
   {
    bullet_impact(mbull, -1, HIT_WAVE);
    return 0;
   }
  }
 }
}

return 1;

/*
char dir_x = 0, dir_y = 0;

int move_x = bullet[mbull].x_speed;
int move_y = bullet[mbull].y_speed;

int timeout = 0;

if (move_x > 0) dir_x = 10;
if (move_x < 0) dir_x = -10;
if (move_y > 0) dir_y = 10;
if (move_y < 0) dir_y = -10;


int step_x = move_x;
int step_y = move_y;

int oldx = bullet[mbull].x;
int oldy = bullet[mbull].y;


if (step_x == 0)
{
 if (step_y > GRAIN)
  step_y = GRAIN;
 if (step_y < -GRAIN)
  step_y = -GRAIN;
}
 else
if (step_y == 0)
{
 if (step_x > GRAIN)
  step_x = GRAIN;
 if (step_x < -GRAIN)
  step_x = -GRAIN;
}
 else
 if (abs(step_x) > GRAIN || abs(step_y) > GRAIN)
 {
  if (abs(step_x) >= abs(step_y))
  {
   step_y = (step_y * GRAIN) / abs(step_x);
   step_x = (GRAIN * step_x) / abs(step_x);
  } else
  if (abs(step_y) > abs(step_x))
  {
   step_x = (step_x * GRAIN) / abs(step_y);
   step_y = (GRAIN * step_y) / abs(step_y);
  }
 }

// if (move_x == 10 && move_y == 0 && bullet[mbull].damage != -1)
// {
//  if (actor_collision(mbull) == 1) return 0;
// }


while (move_x != 0 || move_y != 0)
{
 timeout ++;
 if (timeout > 1000)
  break;
//  if (actor_collision(mbull) == 1) return 0;



 if (step_x == 0 && step_y == 0) return 1;

 if (step_x == 0) move_x = 0;
 if (step_y == 0) move_y = 0;



 if (move_y != 0 && (abs(move_y + step_y) == move_y + step_y) != (abs(step_y) == step_y))
 {
//  move_y = 0;
  step_y = move_y;
  if (move_x == 0) break;
 }
 
 if (move_x != 0 && (abs(move_x + step_x) == move_x + step_x) != (abs(step_x) == step_x))
 {
//  move_x = 0;
  step_x = move_x;
  if (move_y == 0) break;
 }

 if ((move_x > 0) != (step_x > 0))// && (move_x < 0) != (step_x < 0))
 {
//  move_x = 0;
  step_x = move_x;
  if (move_y == 0) break;
 }

 if ((move_y > 0) != (step_y > 0))// && (move_y < 0) != (step_y < 0))
 {
//  move_y = 0;
  step_y = move_y;
  if (move_x == 0) break;
 }


 if (limited == 1)
 {
  move_x -= step_x;
  move_y -= step_y;
 }

 bullet[mbull].x += step_x;
 bullet[mbull].y += step_y;

* /
} // end of while loop
bullet[mbull].x2 += bullet[mbull].x_speed;
bullet[mbull].y2 += bullet[mbull].y_speed;


if (oldx == bullet[mbull].x && oldy == bullet[mbull].y)
{
 bullet[mbull].x_speed = 0;
 bullet[mbull].y_speed = 0;
}



return 1;
*/
}



int detect_collision(int b, int things [2])
{

 int x2 = bullet[b].x - bullet[b].x_speed;
 int y2 = bullet[b].y - bullet[b].y_speed;
 int i;
/* int xa, ya;
 float bullet_angle;
 float bullet_slope;
 int side_disp;
 int x1 =*/

 if (bullet[b].damage == 0)
  return -1;

 for (i = 0; i < NO_ACTORS; i ++)
 {
// I tried to write an accurate collision detector but failed; all my
//  ideas would've been too slow. So we get this horrible thing which
//  is woefully inaccurate at high relative speeds:
 if (actor[i].in_play == 0)
  continue;
 if (actor[i].grace_period > 0)
  continue;
 if (bullet[b].owner == i)
  continue; // for duels; actors' bullets in non-duel games don't get this far.
 if (hypot(actor[i].x - bullet[b].x, actor[i].y - bullet[b].y) <= actor[i].radius + bullet[b].size)
 {
  things [0] = bullet[b].x;
  things [1] = bullet[b].y;
  return i;
 }
 if (hypot(actor[i].x - x2, actor[i].y - y2) <= actor[i].radius + bullet[b].size)
 {
  things [0] = x2;
  things [1] = y2;
  return i;
 }
 
 
 }

 return -1;

}



int detect_enemy_collision(int b, int things [2])
{

 int x2 = bullet[b].x - bullet[b].x_speed;
 int y2 = bullet[b].y - bullet[b].y_speed;
 int e;

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_NONE)
   continue;
  if (enemy[e].radius == 0) // dead wanderer etc
   continue;
//  if (bullet[b].owner == OWNER_ENEMY)
//   continue;
  if (hypot(enemy[e].x - bullet[b].x, enemy[e].y - bullet[b].y) <= enemy[e].radius + bullet[b].size)
  {
   things [0] = bullet[b].x;
   things [1] = bullet[b].y;
   return e;
  }
  if (hypot(enemy[e].x - x2, enemy[e].y - y2) <= enemy[e].radius + bullet[b].size)
  {
   things [0] = x2;
   things [1] = y2;
   return e;
  }
  x2 = bullet[b].x - bullet[b].x_speed / 2;
  y2 = bullet[b].y - bullet[b].y_speed / 2;
  if (hypot(enemy[e].x - x2, enemy[e].y - y2) <= enemy[e].radius + bullet[b].size)
  {
   things [0] = x2;
   things [1] = y2;
   return e;
  }
 
 }

 return -1;

}





void blast(int x, int y, int radius, int damage, int force, int owner)
{

 int i, damage_done, distance;
 int bangle;
 float proportion;

 if (owner == OWNER_ENEMY || game.type == GAME_DUEL)
 {
  for (i = 0; i < NO_ACTORS; i ++)
  {
   if (actor[i].in_play == 0)
    continue;
   if (actor[i].grace_period > 0)
    continue;
   if (i == owner)
    continue;

   distance = hypot(actor[i].x - x, actor[i].y - y);

   if (distance <= actor[i].radius + radius)
   {
    bangle = radians_to_angle(atan2(actor[i].y - y, actor[i].x - x));
    proportion = ((radius + actor[i].radius) * 50) / (distance + actor[i].radius);
    if (proportion > 100)
     proportion = 100;
//    actor[i].x_speed += cos(bangle) * (float) force * proportion / actor[i].mass;
//    actor[i].y_speed += sin(bangle) * (float) force * proportion / actor[i].mass;
    actor[i].x_speed += xpart(bangle, force * proportion / actor[i].mass);
    actor[i].y_speed += ypart(bangle, force * proportion / actor[i].mass);
    actor[i].screen_shake_time += (force * proportion) / 2000;
    if (actor[i].screen_shake_time > 12)
     actor[i].screen_shake_time = 12;
    damage_done = (float) damage * proportion;
    damage_done /= 100;
//    inflicteda = damage_done;
//    inflictede = proportion;
    if (damage_done > 0)
     hurt_actor(i, owner, damage_done);
   }

   
  }
  if (owner == OWNER_ENEMY)
   return;
 }

 int e, sh;

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_NONE)
   continue;
  if (enemy[e].radius == 0)
   continue; // dead things

   distance = hypot(enemy[e].x - x, enemy[e].y - y);

   if (distance <= enemy[e].radius + radius)
   {
    bangle = radians_to_angle(atan2(enemy[e].y - y, enemy[e].x - x));
//    proportion = (float) (((distance + enemy[e].radius) * 100) / (float) radius + enemy[e].radius);
    proportion = ((radius + enemy[e].radius) * 50) / (distance + enemy[e].radius);
//    (((distance + enemy[e].radius) * 100) / (float) radius + enemy[e].radius);
    if (proportion > 100)
     proportion = 100;
    enemy[e].x_speed += xpart(bangle, force * proportion / enemy[e].mass);
    enemy[e].y_speed += ypart(bangle, force * proportion / enemy[e].mass);
    damage_done = (float) damage * proportion;
    damage_done /= 100;
//    inflicteda = damage_done;
//    inflictede = proportion;
      sh = check_shielder(e);
      if (!sh)
      {
       int pulse = damage_done / 6 + 1;
       if (pulse > 8)
       pulse = 8;
       if (damage_done > 0)
        hurt_enemy(e, damage_done, owner, pulse);
      }
       else
       {
        shield_line(e, 
         enemy[e].x + xpart(bangle + ANGLE_HALF, enemy[e].radius), // / GRAIN),
         enemy[e].y + ypart(bangle + ANGLE_HALF, enemy[e].radius)); // / GRAIN));
       } 
   }
//  if (distance <= enemy[e].radius + radius)
//  {
//  }
 }

}


void line_blast(int x, int y, int type, int target)
{
 int b, xa;
 
 for (b = 0; b < NO_BULLETS; b ++)
 {
  if (bullet[b].type != type)
   continue;
  if (bullet[b].special2 != target)
   continue; 
  xa = radians_to_angle(atan2(bullet[b].y - y, bullet[b].x - x));
  bullet[b].x_speed += xpart(xa, 10000);
  bullet[b].y_speed += ypart(xa, 10000);
  bullet[b].special2 = -1;
 }

}


int radius_effect(int x, int y, int radius, int effect)
{

 int i, distance, retval = 0;

  for (i = 0; i < NO_ACTORS; i ++)
  {
   if (actor[i].in_play == 0)
    continue;
   if (actor[i].grace_period > 0)
    continue;
   distance = hypot(actor[i].x - x, actor[i].y - y);

   if (distance <= actor[i].radius + radius)
   {
    switch(effect)
    {
     case RADEFFECT_TELEPORT:
      teleport_actor(i);
      retval = 1;
      break;
    }
   }
   
  }
  
  return retval;
}



void drag_bullet(int b, float drag_amount)
{
 bullet[b].x_speed *= (float) 1 - drag_amount;
 bullet[b].y_speed *= (float) 1 - drag_amount;
}


void teleport_actor(int a)
{

 int timeout = 2000;
 do
 { 
  actor[a].x = grand(arena.max_x - 100000) + 50000;
  actor[a].y = grand(arena.max_y - 100000) + 50000;
  timeout --;
 } 
  while (clear_space(actor[a].x, actor[a].y, 10000) == 0 && timeout > 0);

  simple_cloud_trans(TRANS_DARKEN, actor[a].x, actor[a].y, 0, 0, 2020);

}

int clear_space(int x, int y, int rad)
{

 int e;
 
 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_NONE)
   continue;
  if (enemy[e].radius == 0) // dead wanderer etc
   continue;
  if (hypot(enemy[e].x - x, enemy[e].y - y) <= enemy[e].radius + rad)
  {
   return 0;
  }
 }
 
 return 1;

}

int enemy_bullet_track_target(int b, int attacking, int turning)
{

 bullet[b].angle = turn_towards_xy(bullet[b].x, bullet[b].y, actor[attacking].x, actor[attacking].y, bullet[b].angle, turning);
 return bullet[b].angle;
}


// returns the nearest actor - assumes max 2 actors
// returns either 1 or 0, or -1 if no actors in place
int closest_target(int x, int y)
{

 if (actor[0].in_play == 0 && actor[1].in_play == 0)
  return ATTACK_NONE;

 if (actor[0].in_play == 0)
  return 1;
 if (actor[1].in_play == 0)
  return 0;

 if (hypot(x - actor[0].x, y - actor[0].y)
      < hypot(x - actor[1].x, y - actor[1].y))
       return 0;
        else
         return 1;

}

// returns a random actor within range. Assumes max 2 actors
// returns 1, 0, or ATTACK_NONE if none within range
// range calculation is quick & dirty
int nearby_target(int range, int x, int y)
{

 if (actor[0].in_play == 0 && actor[1].in_play == 0)
  return ATTACK_NONE;

 if (actor[0].in_play == 1)
 {
  if (abs(actor[0].x - x) + abs(actor[0].y - y) < range)
  {
   if (actor[1].in_play == 1
        && abs(actor[1].x - x) + abs(actor[1].y - y) < range)
   {
    return grand(2);
   }
    else
     return 0;
  }
 }

 if (actor[1].in_play == 1
     && abs(actor[1].x - x) + abs(actor[1].y - y) < range)
  return 1;

 return ATTACK_NONE;

}



/*

void apply_force_to_actor(int a, float rangle, int total_force)
{

   float rangle = atan2(fx, fy);

   int total_force = hypot(fx, fy);

   actor[a].x_speed += cos(angle) * total_force;
   actor[a].y_speed += sin(angle) * total_force;

}


void apply_force_to_enemy(int e, int fx, int fy)
{

   float rangle = atan2(fx, fy);

   int total_force = hypot(fx, fy);

   enemy[e].x_speed += cos(angle) * total_force;
   enemy[e].y_speed += sin(angle) * total_force;


}


*/


void bullet_soundf(int b, int sound, int freq)
{

  play_wav_pos(sound, freq, 255, bullet[b].x, bullet[b].y);

}

void bullet_soundvf(int b, int sound, int vol, int freq)
{

  play_wav_pos(sound, freq, vol, bullet[b].x, bullet[b].y);

}



