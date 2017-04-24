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
    
File: light.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - functions relating to light sources

*/

#include "config.h"

#include "allegro.h"

#include <math.h>

#include "globvars.h"

#include "stuff.h"
//#include "palette.h"

#include "light.h"

struct lightstruct light [NO_LIGHTS];

void reset_lights(void)
{
 light[0].type = LIGHT_NONE;
}

int add_light(int ltype, int lsize, int lx, int ly)
{
 int i = 0;

 do
 {
   if (light[i].type == LIGHT_NONE)
   {
    light[i].x = lx;
    light[i].y = ly;
    light[i].type = ltype;
    light[i].size = lsize;
    light[i + 1].type = LIGHT_NONE;
    return 1;
   }
  if (i >= NO_LIGHTS - 2)
   return -1;
  i++; 
 }
  while(TRUE);

 return -1;

}




