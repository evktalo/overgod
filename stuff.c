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
 - a few common functions

*/

#include "config.h"

#include "allegro.h"

#include <math.h>
//#include <stdlib.h>


int turn_towards_angle(int angle, int tangle, int turning);

float lcos(int angle);
float lsin(int angle);
float angle_to_radians(int angle);

// I have no idea why, but the first few elements of cos_table always get corrupted
//  unless I put a big fat decoy array just above. A similar thing happens to the
//  palette arrays; there seems to be a problem with global arrays like these.
float decoy_table [ANGLE_FULL]; // not used
float cos_table [ANGLE_FULL];
float sin_table [ANGLE_FULL];

int xpart(int angle, int length);

void init_trig(void)
{
 int i;
 
 for (i = 0; i < ANGLE_FULL; i ++)
 {
   cos_table [i] = cos(angle_to_radians(i));// * ANGLE_FULL;
   sin_table [i] = sin(angle_to_radians(i));// * ANGLE_FULL;
 }
 
 
}

int xpart(int angle, int length)
{
// return (lcos(angle) * length);// / ANGLE_FULL;
 return (cos_table [angle & 1023] * length);// / ANGLE_FULL;
}

int ypart(int angle, int length)
{
 return (sin_table [angle & 1023] * length);// / ANGLE_FULL;
}

float lcos(int angle)
{
 return cos_table [angle & 1023]; //0x4ffffffff];
 
}

float lsin(int angle)
{
 return sin_table [angle & 1023]; //0x4ffffffff];
 
}


float angle_to_radians(int angle)
{
 return ((float) angle * PI * 2) / ANGLE_FULL;
// return ((float) angle / ANGLE_FULL) * PI * 2;
}

int radians_to_angle(float angle)
{
 return (int) ((angle * ANGLE_FULL) / (PI * 2));
}

fixed angle_to_fixed(int angle)
{
 return itofix(angle / ANGLE_TO_FIXED);
}

int grand(int number)
{
 if (number == 0)
  return 0;
 return ((rand() + (rand() << 16)) & 0x7fffffff) % number; 
}

int crandom(int number)
{
 if (number == 0)
  return 0;
 return ((rand() + (rand() << 16)) & 0x7fffffff) % number; 
// return (rand() + (rand() << 16)) % number; 
}

int turn_towards_angle(int angle, int tangle, int turning)
{

 if ((angle < tangle && tangle > angle + ANGLE_HALF)
     || (angle > tangle && tangle > angle - ANGLE_HALF))
 {
  angle -= turning;
  if (angle < 0)
   angle += ANGLE_FULL;
 }
  else
  {
   angle += turning;
   if (angle > ANGLE_FULL)
    angle -= ANGLE_FULL;
  }
  
 return angle;
 
}


int turn_towards_xy(int x1, int y1, int x2, int y2, int angle, int turning)
{

  int tangle =
    radians_to_angle(atan2((y2 - y1), (x2 - x1)));
   if (tangle < 0)
    tangle += ANGLE_FULL;
   if (tangle > ANGLE_FULL)
    tangle -= ANGLE_FULL;

  return turn_towards_angle(angle, tangle, turning);

}


// speed must be at least 4, and a factor of 1024
int pulsate(int speed, int amount, int county)
{
 return xpart((county * speed) & 1023, amount);
}

void error_message_out(const char *errm)
{
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message(errm);
      exit(1);
}

  
int angle_difference(int a1, int a2)
{
 int d1, d2;
 
 d1 = (a1 - a2 + ANGLE_FULL) % ANGLE_FULL;
 d2 = (a2 - a1 + ANGLE_FULL) % ANGLE_FULL;

 if (d1 < d2)
  return abs(d1);
  
 return abs(d2);
}


