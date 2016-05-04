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
    
File: input.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - keyboard input. Commands are passed to cmds.c - see intro to that file
for a bit more information.

*/

#include <math.h>

#include "allegro.h"

#include "config.h"

#include "globvars.h"

#include "sound.h"

//#define DEBUG_KEYS 1

#ifdef DEBUG_KEYS
#include "actor.h"
#include "enemy.h"
#include <string.h>
#include "palette.h"
extern RGB palet [256];
void gain_symbol(int a, int abil, int symb);
extern struct optionstruct options;

int scrs;
int sshot_counter;
extern int debug_info;

#endif


extern int inflicteda, inflictede;

void get_input(void)
{


 int i;
 
 if (player[0].actor_controlled != -1)
 {
  for (i = 0; i < NO_CMDS; i ++)
  {
   if (key [player[0].keys [i]])
    actor[player[0].actor_controlled].actor_cmd [i] = 1;
     else
      actor[player[0].actor_controlled].actor_cmd [i] = 0;
  }
 }
 
 if (player[1].actor_controlled != -1)
 {
  for (i = 0; i < NO_CMDS; i ++)
  {
   if (key [player[1].keys [i]])
    actor[player[1].actor_controlled].actor_cmd [i] = 1;
     else
      actor[player[1].actor_controlled].actor_cmd [i] = 0;
  }
 }
 
 if (key [player[0].keys [CMD_LINK]])
 {
  if (player[0].link_toggle_delay == 0)
  {
   if (player[0].link_fire == 0)
    player[0].link_fire = 1;
     else
      player[0].link_fire = 0;
   player[0].link_toggle_delay = 20;
   play_wav(NWAV_MENU);
  }
 }
 
 if (key [player[1].keys [CMD_LINK]])
 {
  if (player[1].link_toggle_delay == 0)
  {
   if (player[1].link_fire == 0)
    player[1].link_fire = 1;
     else
      player[1].link_fire = 0;
   player[1].link_toggle_delay = 20;
   play_wav(NWAV_MENU);
  }
 }
 


/*
 if (key [KEY_8_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_THRUST] = 1;
 if (key [KEY_4_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_LEFT] = 1;
 if (key [KEY_6_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_RIGHT] = 1;
 if (key [KEY_0_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_FIRE1] = 1;
 if (key [KEY_ENTER_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_FIRE2] = 1;
 if (key [KEY_PLUS_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_UPGRADE] = 1;
 if (key [KEY_1_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_LEFT1] = 1;
 if (key [KEY_3_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_RIGHT1] = 1;
 if (key [KEY_2_PAD])
  actor[player[0].actor_controlled].actor_cmd [CMD_BRAKE] = 1;*/
#ifdef DEBUG_KEYS
 char sfile [20];
 char istr [20];

 if (sshot_counter > 0)
  sshot_counter --;

 if (key [KEY_F1] && sshot_counter <= 0)
 {
  BITMAP *scrshot_bmp;
  if (options.resolution == 0)
  {
   scrshot_bmp = create_bitmap(640, 480);
   blit(screen, scrshot_bmp, 0,0,0,0,640,480);
  }
   else
   {
    scrshot_bmp = create_bitmap(800, 600);
    blit(screen, scrshot_bmp, 0,0,0,0,800,600);
   } 

  strcpy(sfile, "sshot");
  strcat(sfile, itoa(scrs, istr, 10));
  strcat(sfile, ".bmp");
  save_bitmap(sfile, scrshot_bmp, palet);
  clear_to_color(screen, COLOUR_WHITE);
  play_wav(NWAV_MENU);
  scrs ++;
  sshot_counter = 15;
  destroy_bitmap(scrshot_bmp);
 }

 if (key [KEY_F2] && counter % 6 == 0)
 {
  actor[0].secondary ++;
  actor[0].secondary %= TOTAL_SECONDS;
 }
 
 if (key [KEY_MINUS_PAD] && counter % 10 == 0)
 {
  gain_symbol(player[0].actor_controlled, ABILITY_DRIVE, SYMBOL_SQUARE);
  gain_symbol(player[0].actor_controlled, ABILITY_DRIVE, SYMBOL_TRIANGLE);
  gain_symbol(player[0].actor_controlled, ABILITY_DRIVE, SYMBOL_CIRCLE);
  gain_symbol(player[0].actor_controlled, ABILITY_DEFENCE, SYMBOL_SQUARE);
//  gain_upgrade_points(player[0].actor_controlled, 1);
 }
 
 int pl = player[0].actor_controlled;
 
 if (counter % 7 == 0)
 {
  if (key [KEY_SLASH])
  {
   if (debug_info == 0)
    debug_info = 1;
     else
      debug_info = 0;
  }
  if (key [KEY_BACKSPACE])
   arena.time_left = 350;
  if (key [KEY_INSERT])
   arena.time_left = 10000;
  if (key [KEY_1])
   gain_symbol(pl, 0, 0);
  if (key [KEY_2])
   gain_symbol(pl, 0, 1);
  if (key [KEY_3])
   gain_symbol(pl, 0, 2);
  if (key [KEY_4])
   gain_symbol(pl, 0, 3);
  if (key [KEY_5])
   gain_symbol(pl, 1, 0);
  if (key [KEY_6])
   gain_symbol(pl, 1, 1);
  if (key [KEY_7])
   gain_symbol(pl, 1, 2);
  if (key [KEY_8])
   gain_symbol(pl, 1, 3);
  if (key [KEY_Q])
   gain_symbol(pl, 2, 0);
  if (key [KEY_W])
   gain_symbol(pl, 2, 1);
  if (key [KEY_E])
   gain_symbol(pl, 2, 2);
  if (key [KEY_R])
   gain_symbol(pl, 2, 3);
  if (key [KEY_T])
   gain_symbol(pl, 3, 0);
  if (key [KEY_Y])
   gain_symbol(pl, 3, 1);
  if (key [KEY_U])
   gain_symbol(pl, 3, 2);
  if (key [KEY_I])
   gain_symbol(pl, 3, 3);
  if (key [KEY_MINUS])
   player[0].score += 500; 
  if (key [KEY_F12])
   calculate_beat(); 
 }
 
 if (key [KEY_TILDE])
 {
  hurt_actor(0, OWNER_ENEMY, 2000);
 }
 if (key [KEY_TAB])
 {
  arena.targets_left_total = 1;
  for (i = 0; i < NO_ENEMIES; i ++)
  {
   if (enemy[i].type != ENEMY_NONE)
    hurt_enemy(i, 9999, 0,0);
  }
  
 }
/* if (key [KEY_2] && counter % 15 == 0)
 {
  actor[0].type ++;
 }
 if (key [KEY_1] && counter % 15 == 0)
 {
  actor[0].type --;
 }*/
/* if (key [KEY_E])
 {
  inflicteda ++;
 }
 if (key [KEY_R])
 {
  inflicteda --;
 }
 if (key [KEY_D])
 {
  inflictede ++;
 }
 if (key [KEY_F])
 {
  inflictede --;
 }*/
 if (key [KEY_7_PAD])
 {
//  actor[player[0].actor_controlled].armour += 10;
//  if (actor[player[0].actor_controlled].armour > actor[player[0].actor_controlled].max_armour)
//   actor[player[0].actor_controlled].armour = actor[player[0].actor_controlled].max_armour;
    actor[player[0].actor_controlled].repairing = 100;
 }

#endif
// int special [10];
// if (key [KEY_5_PAD] && counter % 10 == 0)
// {
//  create_enemy(ENEMY_BOUNCER, 10000 + random() % 50000, 10000 + random() % 50000,
//   1000 - random() % 2000, 1000 - random() % 2000, 0, special);
//  create_enemy(ENEMY_STINGER, grand(3), 10000 + random() % 300000, 90000,
//   0, 0, 0, special);
// }
/*
 if (key [KEY_UP])
  actor[player[1].actor_controlled].actor_cmd [CMD_THRUST] = 1;
 if (key [KEY_LEFT])
  actor[player[1].actor_controlled].actor_cmd [CMD_LEFT] = 1;
 if (key [KEY_RIGHT])
  actor[player[1].actor_controlled].actor_cmd [CMD_RIGHT] = 1;
 if (key [KEY_DOWN])
  actor[player[1].actor_controlled].actor_cmd [CMD_BRAKE] = 1;
 if (key [KEY_SPACE])
  actor[player[1].actor_controlled].actor_cmd [CMD_FIRE1] = 1;
 if (key [KEY_Z])
  actor[player[1].actor_controlled].actor_cmd [CMD_FIRE2] = 1;
 if (key [KEY_X])
  actor[player[1].actor_controlled].actor_cmd [CMD_UPGRADE] = 1;
*/
}





