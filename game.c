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
    
File: game.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - main game loop and a bit of miscellany

*/

#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvars.h"

#include "stuff.h"

#include "palette.h"

#include "input.h"
#include "bullet.h"
#include "cloud.h"
#include "actor.h"
#include "enemy.h"
#include "actor.h"
#include "levels.h"
#include "display.h"
#include "displ_in.h"
#include "cmds.h"
//#include "ships.h"
#include "pickup.h"
#include "light.h"
#include "sound.h"
#include "tile.h"

void begin_game(void);
void init_structs_etc(void);
void make_arena_hostile(void);
void run_hostile_level();
void quit_query(void);



extern volatile unsigned char ticked;
extern volatile int framecounter;
extern int slacktime;
int long_slacktime;
int long_slacktime_store;


void game_loop(void)
{
 int playing = 1;
 
 counter = 0;

 do
 {
  if (player[0].link_toggle_delay > 0)
    player[0].link_toggle_delay --;
  if (player[1].link_toggle_delay > 0)
    player[1].link_toggle_delay --;

  reset_lights();  
  get_input();
  enact_commands();
  run_actors();
  run_bullets(); // should be between actors and enemies for orbitals to work
  arena.waver_on_level = 0;
  run_clouds();
//  run_stars();
  run_enemies();
  run_pickups();
  play_beats();
  if (counter % 35 == 0)
   run_level();
  if (arena.game_over > 0)
  {
   arena.game_over --;
   if (arena.game_over == 1)
   {
    playing = 0;
    kill_gameover_loop();
   } 
   if (arena.game_over == 120)
    play_gameover_loop(1000);
  } else
   {
    if (arena.new_level_sign > 0)
     arena.new_level_sign --;
    if (arena.time_left > 0)
    {
     if (game.type != GAME_DUEL
       || (game.duel_mode != DUEL_10_POINTS && game.duel_mode != DUEL_30_POINTS))
     arena.time_left --;
     if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
      game.ta_total_time ++;
    }
      else
      {
       if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
       {
         arena.game_over = 132;
         hurt_actor(0, 0, 100000);
         hurt_actor(1, 0, 100000);
         // it's okay to call hurt_actor on non-existent actors.
       }
       if (game.type == GAME_DUEL)
       {
        if (arena.game_over == 0)
        {
         arena.game_over = 132;
         game.duel_winner = -1;
         if (player[0].duel_score > player[1].duel_score)
          game.duel_winner = 0;
         if (player[1].duel_score > player[0].duel_score)
          game.duel_winner = 1;
        }
       }
        else
        {
         if (arena.level_finished == 1)
         {
          if (arena.level == 15)
          {
           arena.level = 16;
//           play_gameover_loop(2200);
//           play_wavf(NWAV_SWARBLE, 1500);
           playing = 0;
           break;
          }
          init_level();
          if (actor[0].in_play == 1)
           actor_new_level(0);
          if (actor[1].in_play == 1)
           actor_new_level(1);
          ticked = 0;
         }
         else
         {
          if (arena.hostile == 0)
          {
           make_arena_hostile();
          }
           else
            {
             if (arena.hostile < 10000)
              arena.hostile ++;
             run_hostile_level();
            }
         }
        }
      }
/*    if (arena.time_left == 334)
    {
     send_message(0, "Hurry} Up}!", STYLE_HURRY);
     send_message(1, "Hurry} Up}!", STYLE_HURRY);
    }*/
    if (arena.time_left == 330 && (game.type != GAME_DUEL
       || (game.duel_mode != DUEL_10_POINTS && game.duel_mode != DUEL_30_POINTS)))
     play_wavf(NWAV_ALARM, 900);
    
    if (arena.hostile == 10 || arena.hostile == 40 || arena.hostile == 70)
     play_wavf(NWAV_ALARM, 700);

     
   }
   
  if (ticked == 0)
  {
   run_display();
   framecounter++;
  }
  
  init_effects();

  slacktime = 0;
  
  do
  {
   slacktime ++;
  } while(ticked == 0);
  ticked --;

  counter ++;
  
  if (counter % 32 == 0)
  {
   long_slacktime_store = long_slacktime;
   long_slacktime = 0;
  } 
   
   long_slacktime += slacktime / 100;
  

  if (key [KEY_ESC])
   quit_query();
  
 } while(playing == 1);

}

void quit_query(void)
{

 if (arena.game_over > 0)
  return;

 display_quit_query();

 do
 {

  if (key [KEY_Y])
  {
   arena.game_over = 44;
   if (game.type == GAME_DUEL)
   {
    game.duel_winner = -1;
    if (player[0].duel_score > player[1].duel_score)
     game.duel_winner = 0;
    if (player[1].duel_score > player[0].duel_score)
     game.duel_winner = 1;
   }
   clear_quit_query();
   return;
  }

  if (key [KEY_N])
  {
   clear_quit_query();
   return;
  }

  play_beats();

  do
  {
   slacktime ++;
   slacktime = 0;
  } while(ticked == 0);

  ticked = 0;

 }
  while(TRUE);
  
}




void make_arena_hostile(void)
{
  if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
   return;
  explode_all_pickups();
  arena.hostile = 1;
//  arena.colour3 = COLOUR_RED1;
//  calculate_beat();
  open_eyes();
  shake_all_screens(150);
  play_wav(NWAV_BANG);
//  send_message(0, "Defences} On}-Line}!", STYLE_HURRY);
//  send_message(1, "Defences} On}-Line}!", STYLE_HURRY);
}



void run_hostile_level()
{
  int hostile_attack, i, angle;
  int x, y, xs, ys;

  
//  colours_for_cloud [0] = TRANS_DRED;

 for (i = 0; i < arena.eyes_on_level; i ++)
 {
  angle = grand(ANGLE_FULL);
  xs = 2000 + grand(2000);
  simple_cloud_trans(arena.eye_colour3, arena.eye_x [i], arena.eye_y [i],
   xpart(angle, xs), ypart(angle, xs), 400);   
 }


//  if (counter % 8 != 0 || grand(3) != 0)
  if (counter % 8 != 0 || grand(2000) > arena.hostile)
   return;

  hostile_attack = grand(5) + arena.hostile / 100;

  if (hostile_attack > 5)
   hostile_attack = 5;

  int colours [4] = {COLOUR_BLUE3, COLOUR_BLUE5, COLOUR_BLUE7, COLOUR_WHITE};

//  play_wavf(WAV_);
//   enemy_soundf(e, NWAV_SZAP, 100 + grand(5));
// play_wav_pos(NWAV_BURSTZL, 600, 255, xs, ys);

//  do
//  {
   i = grand(arena.eyes_on_level);
   if (arena.eye_x [i] == 0)
    return;
//  } while (arena.eye_x [i] != 0); 
  
//  angle = grand(ANGLE_FULL);
  int target = -1;
  if (actor[0].in_play == 1 && actor[1].in_play == 1)
   target = grand(2);
    else
    {
     if (actor[0].in_play == 1)
      target = 0;
     if (actor[1].in_play == 1)
      target = 1;
    }

  if (target == -1)
   return;

  angle = radians_to_angle(atan2(actor[target].y - arena.eye_y [i], actor[target].x - arena.eye_x [i]));
 
  x = arena.eye_x [i] + xpart(angle, 5000);
  y = arena.eye_y [i] + ypart(angle, 5000);

  play_wav_pos(NWAV_SZAP, 50 + grand(10), 200, x, y);

  xs = xpart(angle, 15000);
  ys = ypart(angle, 15000);
  simple_cloud_trans(TRANS_WHITE, x, y, 0, 0, 400);

   create_bullet(BULLET_HOSTILE, x, y,
    xs, ys, OWNER_ENEMY,
    400, 2000, 40, 0,
    0, 0, colours, 3,0,0,0,0,0);

}


void begin_game(void)
{
 int i;

 counter = 0;

 init_structs_etc();

 for (i = 0; i < NO_ACTORS; i ++)
 {
  actor[i].in_play = 0;
 }

 player[0].score = 0;
 player[1].score = 0;

 if (game.type == GAME_SINGLE || game.type == GAME_TIME_ATTACK)
 {
  game.users = 1;

  actor[0].in_play = 0;
  player[game.single_player].actor_controlled = 0;
//  player[0].ships_left = 3;
  game.ships_left = 3;
  actor[0].controller = game.single_player;
  init_actor(0, player[game.single_player].ship); //ACTORTYPE_SHIP);
 }

 if (game.type == GAME_COOP || game.type == GAME_DUEL || game.type == GAME_TIME_ATTACK_COOP)
 {
    game.users = 2;

    actor[0].in_play = 0;
    player[0].actor_controlled = 0;
//    player[0].ships_left = 3;
    actor[0].controller = 0;
    init_actor(0, player[0].ship); //ACTORTYPE_SHIP);
 
    actor[1].in_play = 0;
    player[1].actor_controlled = 1;
//    player[1].ships_left = 3;
    actor[1].controller = 1;
    init_actor(1, player[1].ship); //ACTORTYPE_SHIP);
    game.ships_left = 6;
 }
 
   
 init_bullets();
 init_clouds();
// init_stars();
 init_enemies();
 prepare_display();

 game.drag = 0.013;//955;//9999;
/*
 arena.max_x = 894000;
 arena.max_y = 895000;
*/

 arena.level = 0;
 game.symbols_given = 0;

 init_level();

 long_slacktime = 0;
 arena.waver_on_level = 0;

}

void init_structs_etc(void)
{

 int i, j;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  actor[i].type = ACTORTYPE_NONE;
  actor[i].controller = -1;
  actor[i].in_play = 0;
  for (j = 0; j < NO_CMDS; j ++)
  {
   actor[i].actor_cmd [j] = 0;
  }
 }
 
 for (i = 0; i < NO_PLAYERS; i ++)
 {
  player[i].actor_controlled = -1;
  player[i].user = -1;
  player[i].link_fire = 1;
  player[i].link_toggle_delay = 0;
//  for (j = 0; j < NO_CMDS; j ++)
//  {
//   actor[i].player_cmd [j] = 0;
//  }
 }

 for (i = 0; i < NO_BULLETS; i ++)
 {
  bullet[i].type = BULLET_NONE;
 }
 
 for (i = 0; i < NO_CLOUDS; i ++)
 {
  cloud[i].type = CLOUD_NONE;
 }

 init_pickups();
 
}


