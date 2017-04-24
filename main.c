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
    
File: Main.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - main()
 - various initialisation functions
 - miscellaneous stuff


*/

/*

Your guide to source files:
(for more information visit the files themselves)

actor.c
 - functions for dealing with actors (ie players' ships)
bullet.c
 - creation, movement and collision of bullets
cloud.c
 - clouds - ie non-bullet special effects
cmds.c
 - makes actors implement commands issued in input.c
displ_in.c
 - initialises the display and loads bitmaps
display.c
 - shows things on the screen
eclass.c
 - big struct containing enemy data
enemy.c
 - horrible code for all enemies
game.c
 - the game loop and a few special functions
input.c
 - takes keyboard input
levels.c
 - initialises and manages the levels, enemy spawning etc
light.c
 - like cloud.c but for lights 
main.c
 - this file
menu.c
 - the front-end interface
palette.c
 - the palette and transparency functions
pickup.c
 - pickups are symbols/repair/weapons
score.c 
 - deals with scoring
sound.c
 - initialises and plays sounds
stuff.c
 - small, very generic functions (like grand)
tile.c
 - generates the background graphics, which are displayed in display.c


Most .h files just contain function definitions. But some are special:

config.h
 - structs, enums and #defines
globvar.h
 - extern declarations of global variables
sound.h
 - the WAV_xxx sound enum
palette.h
 - the COLOUR_xxx and TRANS_xxx enums
display.h
 - all the graphics enums

*/


#include "config.h"

#include "allegro.h"

//#include <conio.h>
#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <pc.h>
#include <math.h>


#include "palette.h"

#include "displ_in.h"
#include "display.h"
#include "cmds.h"
#include "input.h"
#include "actor.h"
//#include "stars.h"
#include "bullet.h"
#include "cloud.h"
//#include "ships.h"
#include "enemy.h"
#include "levels.h"
#include "menu.h"
#include "sound.h"
#include "stuff.h"

// Global variables:

struct gamestruct game;

struct playerstruct player [NO_PLAYERS];

struct actorstruct actor [NO_ACTORS];

struct bulletstruct bullet [NO_BULLETS];

struct cloudstruct cloud [NO_CLOUDS];

struct starstruct star [NO_STARS];

struct arenastruct arena;

struct enemystruct enemy [NO_ENEMIES];

unsigned char counter;

struct pickupstruct pickup [NO_PICKUPS];

//   --- end global variables

// timer interupt functions and variables:
void framecount(void);

volatile int framecounter;
volatile int frames_per_second;

volatile int inputcounter = 0;
volatile int inputs_per_second = 0;

volatile int turncounter = 0;
volatile int turns_per_second = 0;

void tickover(void);

volatile unsigned char ticked;
//volatile unsigned char tick_counter;
int slacktime;
// --- end timer interupt

extern int grid_offset_x_2p_finetune;

// init functions
void init_at_startup(void);
void begin_game(void);
// --- end init functions


void game_loop(void);

struct optionstruct options;


void framecount(void)
{
   frames_per_second = framecounter;
   framecounter = 0;
//   turns_per_second = turncounter;
//   turncounter = 0;
//   inputs_per_second = inputcounter;
//   inputcounter = 0;
//  arena.time_left --;
}
END_OF_FUNCTION (framecount);


void tickover(void)
{
 ticked ++;
// tick_counter++; // assumes it'll wrap at 256
}
END_OF_FUNCTION (tickover);



int main(void)
{

int allint =  allegro_init();
   if (allint == -1)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Failed to initialise Allegro! This isn't going to work, sorry.");
      exit(1);
   }

   set_config_file("lacew.cfg");
   
   install_keyboard();
   install_timer();

   three_finger_flag = 0;
   key_led_flag = 0;


 init_at_startup();

 menu_loop();
 
 return 0;

}

END_OF_MAIN();


void init_at_startup(void)
{


   LOCK_FUNCTION (framecount);
   LOCK_FUNCTION (tickover);
   LOCK_VARIABLE (ticked);
//   LOCK_VARIABLE (tick_counter);
   LOCK_VARIABLE (frames_per_second);
   LOCK_VARIABLE (framecounter);
   LOCK_VARIABLE (turns_per_second);
   LOCK_VARIABLE (turncounter);
//   LOCK_VARIABLE (inputs_per_second);
//   LOCK_VARIABLE (inputcounter);

   install_int (framecount, 1000);
   install_int (tickover, 30);

   set_color_depth(8);

   set_config_file("overgod.cfg");

   options.resolution = get_config_int("Options", "Resolution", 0);
   
   int randseed = get_config_int("Misc", "Seed", 0);
   srand(randseed);
   
   int windowed = GFX_AUTODETECT_FULLSCREEN;
   
   switch(options.resolution)
   {
//     case 640:
     case 0:  
      graphics_mode = GMODE_640_480;
      break;
     case 1:
      graphics_mode = GMODE_800_600;
      break;
     case 2:  
      graphics_mode = GMODE_640_480;
      windowed = GFX_AUTODETECT_WINDOWED;
      break;
     case 3:
      graphics_mode = GMODE_800_600;
      windowed = GFX_AUTODETECT_WINDOWED;
      break;
/*     case 1024:
      graphics_mode = GMODE_1024_768;
      break;*/
   }
   
   scr_x = 640;
   scr_y = 480;
   tp_window_width = 315;
   grid_offset_x_1p = 0;//35000;
   grid_offset_x_2p = -3;//32000;
//   grid_offset_x_2p = 32000 + 850000;
   grid_offset_x_2p_finetune = -13;
   grid_offset_y = 0;
   special_600_y = 0;
   text_offset_x_1p = 0;
   text_offset_x_2p = 0;
   text_offset_x = 0;
   text_offset_y = 0;
   grid_finetune_x_1p = 0;
   grid_finetune_y = 0;
   visible_grids_y = 15;
      
   switch(graphics_mode)
   {
    case GMODE_800_600:
     scr_x = 800;
     scr_y = 600;
     tp_window_width = 395;
     grid_offset_x_1p = 3;//5000;
     grid_offset_x_2p = -3;//8000;//-2000;
     grid_finetune_x_1p = 31;
     grid_finetune_x_2p = 10;
     grid_offset_x_2p_finetune = -22;
     grid_finetune_y = -90;
     grid_offset_y = -10000;
     special_600_y = -5;
     text_offset_y = 60;   
     text_offset_x = 80;   
     visible_grids_y = 60;
     special_600_y = 2;
     break;
/*    case GMODE_1024_768:
     scr_x = 1024;
     scr_y = 768;
     tp_window_width = 507;
     grid_offset_x_1p = 43000;
     grid_offset_x_2p = 2000;
     grid_offset_y = 6000;
     special_600_y = 0;
     text_offset_y = 144;   
     text_offset_x = 192;   
     break;
not working     
     */
   }

   if (set_gfx_mode(windowed, scr_x, scr_y, 0, 0) != 0)
//   if (set_gfx_mode(GFX_DIRECTX_FULLSCREEN, scr_x, scr_y, 0, 0) != 0)
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to set %ix%i display mode\n%s\n", scr_x, scr_y, allegro_error);
      exit(1);
   }

   init_trig(); // can't use any integer trig before this!
   
   init_palette();

   init_display();
   init_menus_once_only();

   init_sound(); // must come after init_menus_once_only, as that's where
    // options.sound_enabled is set.
 
 

}



