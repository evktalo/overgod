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
    
File: pickup.c
History:
11/9/2003 - Version 1.0 finalised

This file contains:
 - pickups (energy, repair etc)

*/

#include "config.h"

#include "allegro.h"

#include "globvars.h"

#include "stuff.h"
#include "palette.h"
#include "cloud.h"

void manage_pickup(int p);

void destroy_pickup(int p);
void pickup_explodes(int p, int taken);



void init_pickups(void)
{

 int p;

 for (p = 0; p < NO_PICKUPS; p++)
 {
  pickup[p].type = PICKUP_NONE;
 }

}

int create_pickup(int type, int subtype, int x, int y, int timeout)
{

   if (arena.hostile > 0)
    return -1;

   int p;

   for (p = 0; p < NO_PICKUPS; p++)
   {
    if (p == NO_PICKUPS - 1) return -1;
    if (pickup[p].type == PICKUP_NONE) break;
   }

   pickup[p].type = type;
   if (subtype == -1)
   {
    switch(type)
    {
     case PICKUP_SQUARE:
     case PICKUP_CIRCLE:
     case PICKUP_TRIANGLE:
      subtype = grand(4);
      break; 
     case PICKUP_SECONDARY:
      subtype = 1 + grand(6);
      break; 
    }
   }
   pickup[p].subtype = subtype;
   pickup[p].x = x;
   pickup[p].y = y;
   pickup[p].radius = 8000; // generous
   pickup[p].timeout = timeout;
   pickup[p].counter = 256 - counter;
   
   return p;

}



void run_pickups(void)
{

 int p;

 for (p = 0; p < NO_PICKUPS; p++)
 {
  if (pickup[p].type != PICKUP_NONE)
   manage_pickup(p);
 }

}


void manage_pickup(int p)
{
   pickup[p].timeout --;
   pickup[p].counter ++;
   if (pickup[p].timeout <= 0)
   {
    switch(pickup[p].type)
    {
     case PICKUP_SQUARE:
//     case PICKUP_CROSS:
     case PICKUP_CIRCLE:
     case PICKUP_TRIANGLE:
      if (pickup[p].subtype == ABILITY_DEFENCE)
       pickup[p].subtype = ABILITY_PRIMARY;
        else
         pickup[p].subtype ++;
      pickup[p].timeout = SYMBOL_TIMEOUT;   
      break;   
     case PICKUP_PRESYMBOL:
      if (arena.hostile > 0)
      {
       pickup_explodes(p, 0);
       break;
      }
      pickup[p].type = pickup[p].subtype;
      pickup[p].subtype = grand(4);
      pickup[p].timeout = SYMBOL_TIMEOUT;   
      int passing_colours [4] = {TRANS_DRED, TRANS_ORANGE, TRANS_YELLOW, TRANS_WHITE};
      create_cloud(CLOUD_SHOCKWAVE,
       pickup[p].x,
       pickup[p].y,
       0, 0,
       0,
       0,
       150,10,0, 0, 150, 0, passing_colours);
      break; 
     case PICKUP_PRESECONDARY:
      if (arena.hostile > 0)
      {
       pickup_explodes(p, 0);
       break;
      }
      pickup[p].type = PICKUP_SECONDARY;
      pickup[p].timeout = 900;
      break; 
     default:     
      pickup_explodes(p, 0);
      return;
    } 
   }
}

void pickup_explodes(int p, int taken)
{
  int passing_colours [4];

  if (pickup[p].type == PICKUP_SECONDARY)
  {
     passing_colours [0] = TRANS_YELLOW;
     passing_colours [1] = TRANS_ORANGE;
     passing_colours [2] = TRANS_LRED;
     passing_colours [3] = TRANS_DRED;
      create_cloud(CLOUD_SHRINKING_FADING_CIRCLE,
       pickup[p].x,
       pickup[p].y,
       0, 0, 0, 0,
       300, -200, 5, 0, 0, 0, passing_colours);
      create_cloud(CLOUD_SHRINKING_FADING_CIRCLE,
       pickup[p].x,
       pickup[p].y,
       0, 0, 0, 0,
       300, -300, 10, 0, 0, 0, passing_colours);
      create_cloud(CLOUD_SHRINKING_FADING_CIRCLE,
       pickup[p].x,
       pickup[p].y,
       0, 0, 0, 0,
       300, -400, 15, 0, 0, 0, passing_colours);
   destroy_pickup(p);
   return;
  }

  if (pickup[p].type == PICKUP_PRESECONDARY 
      || pickup[p].type == PICKUP_PRESYMBOL)
      {
       destroy_pickup(p);
       return;
// presymbol can be exploded if time expires   
      }
   
  if (pickup[p].type == PICKUP_REPAIR)
  {
     passing_colours [0] = TRANS_ORANGE;
     passing_colours [1] = TRANS_ORANGE;
     passing_colours [2] = TRANS_LRED;
     passing_colours [3] = TRANS_DRED;
  }
   else
    switch(pickup[p].subtype)
    {
     case ABILITY_PRIMARY:
     passing_colours [0] = TRANS_YELLOW;
     passing_colours [1] = TRANS_YELLOW;
     passing_colours [2] = TRANS_YELLOW;
     passing_colours [3] = TRANS_ORANGE;
      break;
     case ABILITY_SECONDARY:
     passing_colours [0] = TRANS_DRED;
     passing_colours [1] = TRANS_DRED;
     passing_colours [2] = TRANS_DRED;
     passing_colours [3] = TRANS_DGREY;
      break;
     case ABILITY_DRIVE:
     passing_colours [0] = TRANS_LGREEN;
     passing_colours [1] = TRANS_LGREEN;
     passing_colours [2] = TRANS_DGREEN;
     passing_colours [3] = TRANS_DGREY;
      break;
     case ABILITY_DEFENCE:
     passing_colours [0] = TRANS_LBLUE;
     passing_colours [1] = TRANS_LBLUE;
     passing_colours [2] = TRANS_DBLUE;
     passing_colours [3] = TRANS_DGREY;
      break;
  }
  
  int i, angle, xs;

  for (i = 0; i < 20; i ++)
  {
   angle = grand(ANGLE_FULL);
   xs = 2000 + grand(6000);
      create_cloud(CLOUD_BLAST_CIRCLE,
       pickup[p].x,
       pickup[p].y,
       0, 0,
       xpart(angle, xs),
       ypart(angle, xs),
       55 + grand(20),2,0, 0, 0, 0, passing_colours);
  }  

      create_cloud(CLOUD_LIGHT,
       pickup[p].x,
       pickup[p].y,
       0, 0, 0, 0,
       800 + grand(300),10,3, 0, 0, 0, passing_colours);

  /*
      create_cloud(CLOUD_SHOCKWAVE,
       pickup[p].x,
       pickup[p].y,
       0, 0,
       0,
       0,
       150,10,0, 0, 150, 0, passing_colours);
*/

// can return earlier
 destroy_pickup(p);
 return;
}

void destroy_pickup(int p)
{
 pickup[p].type = PICKUP_NONE;
}


void explode_all_pickups(void)
{

 int p;

 for (p = 0; p < NO_PICKUPS; p++)
 {
  if (pickup[p].type != PICKUP_NONE)
   pickup_explodes(p, 0);
 }

}

/*

Upgrades:


*/
