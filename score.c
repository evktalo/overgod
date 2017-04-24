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
    
File: score.c
History:
11/9/2003 - Version 1.0 finalised

This file contains:
 - functions dealing with keeping score

*/

#include "config.h"

#include "allegro.h"

//#include <stdlib.h>
#include <string.h>

#include "globvars.h"

#include "stuff.h"

#include "palette.h"

#include "game.h"
//#include "upgrades.h"
#include "display.h"

#define NO_SCORES 20
// also #def'd in menu.c


struct score_list
{
  int score;
  int ship;
  int level;
  char name [20];
};

extern struct score_list hs_single [NO_SCORES];
extern struct score_list hs_coop [NO_SCORES];

extern struct score_list *hscore;

