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
    
File: levels.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - initialising and managing of the levels

*/

#include "allegro.h"

#include "config.h"
#include "globvars.h"



#include "stuff.h"

#include "palette.h"
#include "enemy.h"
#include "display.h"
#include "displ_in.h"
#include "cloud.h"
#include "bullet.h"
#include "enemy.h"
#include "pickup.h"
#include "actor.h"
#include "sound.h"
#include "light.h"
#include "tile.h"

#ifdef SANITY_CHECK
extern FONT *small_font;
int e_index = 0;
#endif

int test_ta_enemies;

int choose_enemy(int e1, int e2, int e3, int e4);

void init_time_attack_level(void);
void run_time_attack_level(void);
void time_attack_enemies(void);

void init_duel_level(void);
void run_duel_level(void);

void muster_enemies(int enemy_type [15], int types, int role, int enemy_level);
void make_enemy_list(void);
void tilemap(void);

extern BITMAP *level_bmp;

int extra_list [3];

void start_at_level_1(void)
{
 arena.level = 0;
}

void init_level(void)
{

/* if (level_bmp != NULL)
 {
  destroy_bitmap(level_bmp);
  level_bmp = NULL;
 }*/

 level_announcement();
 // in display.c. currently just clears the player display bmps

 if (game.type == GAME_DUEL)
 {
  init_duel_level();
  return;
 }
 
 if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
 {
  init_time_attack_level(); // only called once per game.
  return;
 }

 arena.level ++;

 arena.new_level_sign = 80;

 init_bullets();
 init_clouds();
 init_enemies();
 reset_lights();
 init_effects(); // cloud.c

 calculate_beat();

 arena.max_x = 594000 + (((int) (arena.level / 3)) + grand(arena.level / 2)) * 100000;
 arena.max_y = 595000 + (((int) (arena.level / 3)) + grand(arena.level / 2)) * 100000;
// placement of actor in actor.c depends on level 1 being 594000x595000
 
/* if (arena.level % 4 == 0)
 {
  arena.max_x = 794000;
  arena.max_y = 795000;
 }*/
 if (arena.max_x > 1000000)
 {
  arena.max_x = 994000;
 }
 if (arena.max_y > 1000000)
 {
  arena.max_y = 995000;
 }
/* if (arena.max_x > 1300000)
 {
  arena.max_x = 1294000;
 }
 if (arena.max_y > 1300000)
 {
  arena.max_y = 1295000;
 }*/
//  arena.max_x = 994000;
//  arena.max_y = 995000;
 
 arena.max_targets_in_level = 4 + (arena.level - 1) / 5;
 arena.max_non_targets_in_level = 4 + (arena.level - 1) / 5;
 if (arena.level == 1)
  arena.max_non_targets_in_level = 0;
 if (arena.level == 2)
  arena.max_non_targets_in_level = 2;
 make_enemy_list();

 arena.time_left = 33.333 * 180;
 if (arena.level < 11)
  arena.time_left = 33.33 * 150;
 if (arena.level < 6)
  arena.time_left = 33.33 * 120;
//  arena.time_left = 33.33 * 120;

//   arena.time_left = 600;
//   arena.time_left = 50;
//   arena.time_left = 280;
   arena.level_finished = 0;
   arena.next_target = grand(3) + 2;
   arena.next_non_target = grand(4) + 15;
   arena.hostile = 0;
   init_pickups();
   
   tilemap();
#ifdef SANITY_CHECK
e_index = 0;
#endif   
   
}


void tilemap(void)
{
   int lx, ly;
   
   lx = arena.max_x / 50000 + 5; // if changed, change in open_eyes in tile.c also
   ly = arena.max_y / 50000 + 5;
/*
   level_bmp = create_bitmap(lx, ly);

   if (!level_bmp)
   {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message("Bitmap creation error (level_bmp %i x %i) \n%s\n", lx, ly, allegro_error);
    exit(1);
   }
*/   
   clear_to_color(level_bmp, 1);
   
   make_tile_map(lx, ly);
   

}

// called about every second
void run_level(void)
{

 if (game.type == GAME_DUEL)
 {
  run_duel_level();
  return;
 }

 if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
 {
  run_time_attack_level();
  return;
 }

 int i;
 int current_targets = 0;
 int current_non_targets = 0;
 int subtype = SUBTYPE_NONE;

 arena.next_target --;
 arena.next_non_target --;

 if (arena.next_target > 0 && arena.next_non_target > 0)
  return;
  // Don't bother placing new enemies if now's not the time

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
  if (enemy[i].target == TARGET_PRIMARY)
   current_targets ++;
  if (enemy[i].target == TARGET_EXTRA)
   current_non_targets ++;
 }


 if (arena.next_target <= 0)
 {
  if (arena.targets_left_total > 0 
      && current_targets < arena.max_targets_in_level
      && arena.enemy_list [arena.list_index] != ENEMY_LIST_END
      && (arena.enemy_list [arena.list_index] != ENEMY_LIST_WAIT
           || current_targets == 0))
  {
    while (arena.enemy_list [arena.list_index] == ENEMY_LIST_WAIT)
    {
     arena.list_index ++;
    }
     
//    spawn_enemy(arena.enemy_list [arena.list_index], subtype, PICKUP_NONE, TARGET_PRIMARY);
    if (spawn_enemy(arena.enemy_list [arena.list_index], subtype, PICKUP_NONE, TARGET_PRIMARY) != -1)
     arena.list_index ++;
     
    while (arena.enemy_list [arena.list_index] == ENEMY_LIST_EMPTY)
    {
     arena.list_index ++;
    }
//     arena.targets_left_total --;
  }
  arena.next_target = 2;
 }
 
 int etype = -1;
  
 if (arena.next_non_target <= 0)
 {
  if (current_non_targets < arena.max_non_targets_in_level)
  {
    etype = extra_list [grand(2)];
/*   extern FONT *small_font; 
   textprintf_ex(screen, small_font, 150, 150, COLOUR_WHITE, -1, "type %i", etype);
   do
   {
   
   }
    while (key [KEY_K] == 0);*/
    spawn_enemy(etype, 0, PICKUP_NONE, TARGET_EXTRA);
//    if (spawn_enemy(arena.extra_list [grand(2)], subtype, PICKUP_NONE, TARGET_NO) != -1)
//     arena.list_index ++;
  }
  arena.next_non_target = 30;
 }
 
/* 
 if (arena.next_non_target == 0)
 {
  if (current_non_targets < arena.max_non_targets_in_level
      && arena.enemy_list [arena.list_index] != ENEMY_LIST_END) // this last shouldn't happen
  {
    if (spawn_enemy(arena.enemy_list [arena.list_index], subtype, PICKUP_NONE, TARGET_PRIMARY) != -1)
    {
     arena.targets_left_total --;
     arena.list_index ++;
    } 
   }
  }
  arena.next_non_target = 5;
 }
*/

}


void make_enemy_list(void)
{

 arena.list_index = 0;
 arena.symbol_index = 0;
 int i;
 int enemy_type [15];
 int miniboss_type [15];
 
 arena.battle_type = BATTLE_NORMAL;
 if (arena.level == 5 || arena.level == 10 || arena.level == 15)
  arena.battle_type = BATTLE_BOSS;

 if (game.mode_god == 1 && arena.level > 3 && grand(3) == 0)
  arena.battle_type = BATTLE_BOSS;

 int csym = 0;
 int total_actual_targets = 0;

 for (i = 0; i < 50; i ++)
 {
  arena.symbol_list [i] = 0;
 }

 for (i = 0; i < 25; i ++)
 {
  arena.enemy_list [i] = ENEMY_LIST_END;
 }  

 switch(arena.battle_type)
 {
  default:
  case BATTLE_NORMAL:
// first process the targets:  
   arena.total_targets = 9;// + arena.level / 5;
   arena.targets_left_total = 9;// + arena.level / 5;
   arena.max_targets_in_level = 4 + arena.level / 5;
   muster_enemies(enemy_type, 3, ROLE_TARGET, arena.level);
   muster_enemies(miniboss_type, 3, ROLE_MINIBOSS, arena.level);
   for (i = 0; i < arena.total_targets; i ++)
   {
    if (arena.level == 1)
    {
      arena.enemy_list [i] = ENEMY_MULTI1;
      if (grand(2) == 0) arena.enemy_list [i] = ENEMY_GUARDIAN1;
      if (grand(3) == 0) arena.enemy_list [i] = ENEMY_CURVE1;
//      total_actual_targets ++;
//      arena.enemy_list [i] = ENEMY_CRUISER1;
    }  
      else
      {
       if (arena.battle_type == BATTLE_BOSS)
       {
        if (i == 5)
        {
         arena.enemy_list [i] = ENEMY_BOSS1_1 + grand(3);
         if (arena.level > 7)
         {
          switch(grand(3))
          {
           case 0: arena.enemy_list [i] = ENEMY_BOSS2; break;
           case 1: arena.enemy_list [i] = ENEMY_BOSS2_2; break;
           case 2: arena.enemy_list [i] = ENEMY_BOSS2_3; break;
          } 
         } 
         if (arena.level > 12)
         {
          switch(grand(3))
          {
           case 0: arena.enemy_list [i] = ENEMY_BOSS3_1; break;
           case 1: arena.enemy_list [i] = ENEMY_BOSS3_2; break;
           case 2: arena.enemy_list [i] = ENEMY_BOSS3_3; break;
          } 
         } 
         arena.enemy_list [i + 1] = ENEMY_LIST_EMPTY;
         arena.enemy_list [i + 2] = ENEMY_LIST_EMPTY;
         arena.enemy_list [i + 3] = ENEMY_LIST_EMPTY;
         arena.enemy_list [i + 4] = ENEMY_LIST_EMPTY;
         i = arena.total_targets;
//         total_actual_targets ++;
         break; // out of i loop
        }
       }
        else
        {
        // maybe a miniboss or two...
         if ((arena.level > 2 || game.mode_god == 1)
          && ((i == arena.total_targets - 4 && (grand(5) == 0 || game.mode_god == 1))
          || (i > 2 && i < arena.total_targets - 3 && (grand(20) == 0 || (grand(4) == 0 && game.mode_god == 1)))))
          {
           arena.enemy_list [i] = miniboss_type [grand(3)];
           arena.enemy_list [i + 1] = ENEMY_LIST_EMPTY;
           arena.enemy_list [i + 2] = ENEMY_LIST_EMPTY;
           i += 2; // gets +1 from just going through the loop
//           total_actual_targets ++;
           continue; // through i loop
          }
        }
       arena.enemy_list [i] = enemy_type [grand(3)]; 
// we have to guarantee at least 5 enemies per level, as 4 symbols are given in a coop game.
//       total_actual_targets ++;
      } 
   }

//     arena.enemy_list [0] = ENEMY_SPIKEY4;
//     arena.enemy_list [1] = ENEMY_OVERSPIKEY;
/*     arena.enemy_list [1] = ENEMY_SHADOW1;
     arena.enemy_list [2] = ENEMY_SHADOW2;*/

/*     arena.enemy_list [0] = ENEMY_TRIANGLER1;
     arena.enemy_list [1] = ENEMY_TRIANGLER1;
     arena.enemy_list [2] = ENEMY_TRIANGLER2;
     arena.enemy_list [3] = ENEMY_TRIANGLER2;
     arena.enemy_list [4] = ENEMY_TRIANGLER3;*/

/*     arena.enemy_list [0] = ENEMY_DEFENDER1;
     arena.enemy_list [1] = ENEMY_LIST_WAIT;
     arena.enemy_list [2] = ENEMY_DEFENDER2;
     arena.enemy_list [3] = ENEMY_LIST_WAIT;
     arena.enemy_list [4] = ENEMY_DEFENDER3;
     arena.enemy_list [5] = ENEMY_LIST_WAIT;
     arena.enemy_list [6] = ENEMY_OVERTRIANGLER;*/
/*     arena.enemy_list [0] = ENEMY_SPIKEY2;
     arena.enemy_list [1] = ENEMY_UNDERSPIKEY;
     arena.enemy_list [2] = ENEMY_UNDERSPIKEY2;
     arena.enemy_list [3] = ENEMY_UNDERSPIKEY3;*/
/*     arena.enemy_list [0] = ENEMY_CRUISER1;
     arena.enemy_list [1] = ENEMY_CRUISER1;
     arena.enemy_list [1] = ENEMY_CRUISER1;
     arena.enemy_list [2] = ENEMY_CRUISER1 + grand(5);
     arena.enemy_list [3] = ENEMY_CRUISER1 + grand(5);
     arena.enemy_list [4] = ENEMY_CRUISER1 + grand(5);
     arena.enemy_list [5] = ENEMY_CRUISER1 + grand(5);*/
//     arena.enemy_list [0] = ENEMY_BOSS3_1 + grand(3);
/*     arena.enemy_list [0] = ENEMY_GUARDIAN1;
     arena.enemy_list [1] = ENEMY_GUARDIAN2;
     arena.enemy_list [2] = ENEMY_GUARDIAN3;
     arena.enemy_list [3] = ENEMY_GUARDIAN4;
     arena.enemy_list [4] = ENEMY_GUARDIAN5;*/
//   arena.enemy_list [3] = ENEMY_LIST_WAIT;
   break;
 }  
// should this be in the i loop?
//  I'm not sure.
   arena.enemy_list [i] = ENEMY_LIST_END;
   arena.next_target_upgrade = grand(arena.total_targets / 3) + 1;
// now process the extras:   
   muster_enemies(enemy_type, 3, ROLE_EXTRA, arena.level);
   extra_list [0] = enemy_type [0];
   extra_list [1] = enemy_type [1];

 total_actual_targets = 0;
 
 for (i = 0; i < 25; i ++)
 {
  if (arena.enemy_list [i] == ENEMY_LIST_END)
   break;
  if (arena.enemy_list [i] != ENEMY_LIST_EMPTY && arena.enemy_list [i] != ENEMY_LIST_WAIT)
   total_actual_targets ++;
 }

 int total_symbols = 3;

 if (arena.level > 5)
  total_symbols = 2;
  
 if (arena.level > 10)
  total_symbols = 1;
  
 if (game.type == GAME_COOP)
  total_symbols ++; 
  
 for (i = 0; i < total_symbols; i ++)
 {
  do
  {
    csym = grand(total_actual_targets - 1); // never choose last index - last enemy shouldn't produce a symbol as there mightn't be enough time to collect it.
  } while (arena.symbol_list [csym] == 1); 
//            || arena.enemy_list [csym] == ENEMY_LIST_WAIT 
//            || arena.enemy_list [csym] == ENEMY_LIST_EMPTY 
//            || arena.enemy_list [csym] == ENEMY_LIST_END);
            
            arena.symbol_list [csym] = 1;
 }

/* switch(arena.level)
 {
  case 5: 
   arena.enemy_list [0] = ENEMY_BOSS1_1 + grand(3);
   arena.enemy_list [1] = ENEMY_LIST_END;
   arena.total_targets = 1;
   arena.targets_left_total = 1;
   arena.max_targets_in_level = 1;
   break;
  case 10: 
  case 15: 
  arena.battle_type = BATTLE_BOSS; break;
 }
*/

}


void muster_enemies(int enemy_type [15], int types, int role, int enemy_level)
{

 int muster [400]; // arbitrarily high value

 int i, j, m, k = 0;

 m = 0;

 for (i = 0; i < NO_ENEMY_TYPES; i ++)
 {
  if (eclass[i].common == 0 || eclass[i].role != role)
   continue;
//  if (i != ENEMY_BOSS1_3) //i != ENEMY_CRUISER2 && i != ENEMY_CRUISER3 && i != ENEMY_CRUISER4)
//   continue;
  if (abs(eclass[i].level - (enemy_level + k)) < eclass[i].common)
  {
   for (j = 0; j < eclass[i].common - abs(eclass[i].level - enemy_level); j ++)
   {
    if (m > 200)
     break;
    muster [m] = i;
    m ++;
   }
  }
 }

 for (k = 0; k < types; k ++)
 {
   enemy_type [k] = muster [grand(m)];
 }


}




int choose_enemy(int e1, int e2, int e3, int e4)
{
 switch(grand(4))
 {
  case 0: return e1;
  case 1: return e2;
  case 2: return e3;
  case 3: return e4;
 }

 return e1;

}

void finish_level(void)
{
   arena.level_finished = 1;
   arena.seconds_left_when_finished = 0;
   arena.time_bonus = 0;
   if (arena.hostile == 0)
   {
    arena.seconds_left_when_finished = arena.time_left / 33.3333;
//    arena.time_bonus = arena.seconds_left_when_finished * (arena.level + 2); //(arena.seconds_left_when_finished) * (arena.level + 4);
    arena.time_bonus = arena.seconds_left_when_finished * 15;
    if (arena.level <= 10)
     arena.time_bonus = arena.seconds_left_when_finished * 10;
    if (arena.level <= 5)
     arena.time_bonus = arena.seconds_left_when_finished * 5;
    if (game.users == 1)
     gain_score(game.single_player, arena.time_bonus);
      else
      {
       gain_score(0, arena.time_bonus);
       gain_score(1, arena.time_bonus);
      }
   }
   arena.time_left = 166;
   arena.colour1 = COLOUR_GREY2;
   arena.colour2 = COLOUR_GREY6;
   if (arena.hostile > 0)
   {
    arena.hostile = 0;
    arena.colour3 = COLOUR_GREY1;
   }

// play_gameover_loop(2000);
//  play_wavf(NWAV_SUCCESS, 300 + arena.level * 30);
// play_sound(WAV_LEVEL_END);
}





// is called once at the start of each game
void init_time_attack_level(void)
{
/*
 if (level_bmp != NULL)
 {
  destroy_bitmap(level_bmp); // assumes that it'll have been set to NULL at startup
  level_bmp = NULL;
 }*/

 arena.new_level_sign = 80;
 arena.total_targets = 9;

 game.ta_level = 1;
 game.ta_symbol_count = 0;
 game.symbols_given = 0;
 game.ta_total_time = 0;
 game.ta_enemy_index = 0;

 init_bullets();
 init_clouds();
 init_enemies();
 reset_lights();
 init_effects(); // cloud.c

 calculate_beat();

 arena.max_x = 794000;
 arena.max_y = 795000;
 
 game.ta_enemy_index = 0; 
 time_attack_enemies(); // ie make_enemy_list

 arena.time_left = 33.333 * 60;

 arena.level_finished = 0;
 arena.hostile = 0;
 arena.next_target = grand(3) + 2;
 arena.next_non_target = grand(4) + 15;
 init_pickups();
   
 tilemap();
   
}

void run_time_attack_level(void)
{

 int i;

 int current_targets = 0;
 int current_non_targets = 0;

 arena.next_target --;
 if (game.ta_level > 1)
  arena.next_non_target --;

 if (arena.next_target > 0 && arena.next_non_target > 0)
  return;
  // Don't bother placing new enemies if now's not the time

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
  if (enemy[i].target == TARGET_PRIMARY)
  {
   current_targets ++;
   if (eclass[enemy[i].type].role == ROLE_MINIBOSS)
    current_targets += 2; // ie +3 in total
  } 
  if (enemy[i].target == TARGET_EXTRA)
   current_non_targets ++;
 }


 if (arena.next_target <= 0)
 {
  if (current_targets < 5)
  {
#ifdef SANITY_CHECK
if (game.ta_enemy_index >= 20 || game.ta_enemy_index <= -1 || game.ta_enemies [game.ta_enemy_index] < 0)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "run_ta_level %i", game.ta_enemy_index);
     if (game.ta_enemy_index >= 0)
      textprintf_centre_ex(screen, small_font, 400, 260, COLOUR_WHITE, -1, "%i", game.ta_enemies [game.ta_enemy_index]);
     rest(2000);
     return;
//  exit(type);
    } 
#endif    
    if (spawn_enemy(game.ta_enemies [game.ta_enemy_index], 0, PICKUP_NONE, TARGET_PRIMARY) != -1)
     game.ta_enemy_index ++;
    if (game.ta_enemies [game.ta_enemy_index] == ENEMY_LIST_END)
    {
      if (game.ta_level < 13)
       game.ta_level ++; // 13 rather than 15 because it gives a wider range of enemies
      game.ta_enemy_index = 0; 
      time_attack_enemies();
    } 
  }
  arena.next_target = 3;
 }

 int etype;

 if (arena.next_non_target <= 0)
 {
  if (current_non_targets < 3)
  {
    etype = game.ta_extra_list [grand(2)];
#ifdef SANITY_CHECK
if (etype <= -1 || etype >= NO_ENEMY_TYPES)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "run_ta_level extra %i", etype);
     rest(2000);
     return;
} 
#endif    
    spawn_enemy(etype, 0, PICKUP_NONE, TARGET_EXTRA);
  }
  arena.next_non_target = 30;
 }
 


}


void time_attack_enemies(void)
{

 int enemy_type [15];
 int miniboss_type [15];

 muster_enemies(enemy_type, 3, ROLE_TARGET, game.ta_level);
 muster_enemies(miniboss_type, 3, ROLE_MINIBOSS, game.ta_level);
 muster_enemies(game.ta_extra_list, 2, ROLE_EXTRA, game.ta_level);
 
/* enemy_type [0] = ENEMY_CRUISER1;
 enemy_type [1] = ENEMY_CRUISER2;
 enemy_type [2] = ENEMY_CRUISER3;*/


 int i, list_index = 0;
 
 for (i = 0; i < 9; i ++)
 {
#ifdef SANITY_CHECK
if (list_index > 10)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "time_attack_enemies %i", list_index);
     rest(2000);
     return;
//  exit(type);
    } 
#endif    

  game.ta_enemies [list_index] = enemy_type [grand(3)];

  
/*

  do
  {
   e_index++;
  }
   while (eclass[e_index].role != ROLE_TARGET || eclass[e_index].common == 0);
  game.ta_enemies [list_index] = e_index;
  list_index ++;
  continue;
*/
  
  if (game.ta_level == 1)
  {
   if (grand(2) == 0)
    game.ta_enemies [list_index] = ENEMY_GUARDIAN1;
     else
      game.ta_enemies [list_index] = ENEMY_MULTI1;
  }
  if (game.ta_level >= 3)
  {
   if (i == 7 && (grand(5) == 0 || game.mode_god == 1))
   {
    game.ta_enemies [list_index] = miniboss_type [grand(3)];
    game.ta_enemies [list_index + 1] = ENEMY_LIST_END;
    i += 2;
   } 
   if (i == 3 && (grand(2) == 0 || (game.mode_god == 1 && grand(3) != 0))) // was (5)
   {
    game.ta_enemies [list_index] = miniboss_type [grand(3)];
    i += 2;
   }
  } 
#ifdef SANITY_CHECK
if (game.ta_enemies [list_index] <= -1 || game.ta_enemies [list_index] >= NO_ENEMY_TYPES)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "time_attack_enemies ta_enemies %i", game.ta_enemies [list_index]);
     rest(2000);
     return;
//  exit(type);
    } 
#endif    
/*
 do
 {
  test_ta_enemies ++;
 } while(eclass[test_ta_enemies].common == 0);
  game.ta_enemies [list_index] = test_ta_enemies;
*/

  list_index ++;
 }

  game.ta_enemies [list_index] = ENEMY_LIST_END;
 
}


void init_duel_level(void)
{

 init_bullets();
 init_clouds();
 init_enemies();
 reset_lights();
 
 player[0].duel_score = 0;
 player[1].duel_score = 0;
 game.symbols_given = 0;
 
 game.duel_level = 0;

 calculate_beat();

//   arena.colour1 = COLOUR_YELLOW2;
//   arena.colour2 = COLOUR_YELLOW6;
//   arena.colour3 = COLOUR_GREY1;

   arena.max_x = 794000 + grand(6) * 100000;
   arena.max_y = 795000 + grand(6) * 100000;
   if (arena.max_x > 1200000)
   {
    arena.max_x = 1194000;
   }
   if (arena.max_y > 1200000)
   {
    arena.max_y = 1195000;
   }
   switch(game.duel_crawlies)
   {
    case 0:
     arena.max_non_targets_in_level = 0;
     arena.between_crawly_upgrades = 0;
     break;
    case 1:
     arena.max_non_targets_in_level = 3;
     arena.between_crawly_upgrades = 2;
     break;
    case 2:
     arena.max_non_targets_in_level = 6;
     arena.between_crawly_upgrades = 2;
     break;
    case 3:
     arena.max_non_targets_in_level = 9;
     arena.between_crawly_upgrades = 2;
     break;
   }
   arena.time_left = 33.333 * 180;
   if (game.duel_mode == DUEL_10_MINUTES)
    arena.time_left = 33.333 * 600;

   arena.level_finished = 0;
   arena.next_non_target = grand(5) + 1;
//   arena.next_crawly_upgrade = grand(arena.between_crawly_upgrades) + 1;
   arena.hostile = 0;
   arena.crawlies_created = 0;
   init_pickups();
   
 make_enemy_list();
 
 tilemap();
   
}



// called about every second
void run_duel_level(void)
{

 int i;
 int current_enemies = 0;
// int subtype = SUBTYPE_NONE;
// int carrying_pickup = PICKUP_NONE;
 int target_index = -1;
 int level_raised = 0;

 arena.next_non_target --;

 if (arena.next_non_target > 0 || game.duel_crawlies == 0)
  return;

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
//  if (enemy[i].target == TARGET_CRAWLY)
   current_enemies ++;
 }

  if (current_enemies < arena.max_non_targets_in_level)
  {
//   if (grand(3) == 0)
   switch(grand(4))
   {
    case 0: target_index = ENEMY_SPINNER1; break;
    case 1: target_index = ENEMY_SPIKEY1; break;
    case 2: target_index = ENEMY_FIGHTER1; break;
    case 3: target_index = ENEMY_BLATTER1; break;
   }
   if (game.duel_level > 8)
   {
//    level_raised = grand((game.duel_level - 8) / 2 + 1);
    level_raised = grand((game.duel_level - 8) / 3 + 1);
    if (level_raised > 4)
     level_raised = 4;
    target_index += level_raised;
   } 
   if (game.duel_level > 15)
   {
    if (grand(6) == 0)
     target_index = ENEMY_MINEFIELDER1;
   } 
   spawn_enemy(target_index, 0, 0, TARGET_NO);
   game.duel_level++;
  }
  switch(game.duel_crawlies)
  {
   default:
   case 0:
    arena.next_non_target = 0;
    break;
   case 1:
    arena.next_non_target = 24;
    break;
   case 2:
    arena.next_non_target = 12;
    break;
   case 3:
    arena.next_non_target = 6;
    break;
  }
  
  
/* if (arena.next_crawly == 0)
 {
  if (current_crawlies < arena.max_crawlies_in_level)
  {
   arena.next_crawly_upgrade --;
   if (arena.next_crawly_upgrade == 0)
   {
    arena.next_crawly_upgrade = grand(arena.between_crawly_upgrades) + 1;
    carrying_pickup = PICKUP_SQUARE + grand(4);
   }
    else
    {
      if (grand(12) == 0)
      carrying_pickup = PICKUP_REPAIR;
    }
   target_index = ENEMY_CRAWLER;
   if (grand(3) == 0 && arena.crawlies_created > 16)
    target_index = ENEMY_CRAWLER3;
   if (grand(3) == 0 && arena.crawlies_created > 8)
    target_index = ENEMY_CRAWLER2;
   if (grand(3) == 0)
    target_index = ENEMY_FALLER;
    
   subtype = SUBTYPE_GREEN;
   subtype += grand(arena.crawlies_created) / 8;
   if (subtype > SUBTYPE_PURPLE)
    subtype = SUBTYPE_PURPLE;
   spawn_enemy(target_index, subtype, carrying_pickup, TARGET_CRAWLY);
  }
  switch(game.duel_crawlies)
  {
   case 0:
    arena.next_crawly = 0;
    break;
   case 1:
    arena.next_crawly = 24;
    break;
   case 2:
    arena.next_crawly = 12;
    break;
   case 3:
    arena.next_crawly = 6;
    break;
  }
 }

*/
}



