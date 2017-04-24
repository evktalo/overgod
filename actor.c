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
    
File: actor.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - functions relating to the player ships

*/


/*
UPGRADES:

Primary:
Square - recycle (autofire)
Circle - number
Triangle - speed (proj)

Secondary:
Square - Damage
Circle - Number
Triangle - Size/Speed

Drive:
Square - Speed
Circle - Turn
Triangle - Drift

Defence:
Square - Armour
Circle - Shield capacity
Triangle - Shield recharge



SHIPS:

Pointy: extra-damage darts
Horseshoe: Fast secondary recycle
round: armour self-repairs
Retro: retro-rocket instead of drag field
small: smaller collision radius
fins: backfire
large rear wings: orbital
curve: bounces off enemies (ram)

*/

#include "config.h"

#include "allegro.h"

#include <math.h>
//#include <stdlib.h>


#include "globvars.h"

#include "stuff.h"
#include "enemy.h"
#include "cloud.h"
#include "palette.h"
#include "pickup.h"
#include "bullet.h"
#include "sound.h"
#include "display.h"
#include "cmds.h"

//#include "upgrades.h"

#define ORBITAL_DISTANCE 25000

#define DISTINCT_WEAPON 11


void drag_actor(int i);
void move_actor(int i);
int detect_collision_actor_enemies(int a);
int detect_collision_actor_pickups(int a);
int hurt_actor(int a, int source, int hurty);
void actor_destroyed(int a, int source);
void actor_explodes(int a);
void score_duel_kill(int a, int source);
void spawn_actor(int a);
void gain_upgrade_points(int who, int how_many);
int get_upgrade_maximum(int a);
void extra_ship(void);

int acquire_target(int a);
int acquire_turret_target(int a);
void actor_orbital(int a);
void actor_shield(int a);
void actor_sidekicks(int a);
int gain_symbol(int a, int abil, int symb);
int new_weapon(int a);
int new_secondary(int a);
int duel_armour(int armour, int h);

int closest_enemy(int x, int y, int range, int hide_invisible);
int closest_enemy_or_actor(int a, int x, int y, int range, int hide_invisible);
void reset_actor(int a);

void calulate_total_power(int a);

void upgrade_sidekicks(int a);
void game_is_over(void);


void run_actors(void)
{
 int i;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].type == ACTORTYPE_NONE)
   continue;
  if (actor[i].in_play == 0)
  {
   
   actor[i].x_speed = 0;
   actor[i].y_speed = 0;
   if (actor[i].spawn_delay > 0)
   {
    actor[i].spawn_delay --;
    continue;
   }
   if (game.ships_left > 0)
   {
    if (arena.game_over == 0)
     spawn_actor(i); // ships_left will be non-zero for time attack games.
   } 
     else
      continue;
  }
//  actor[i].energy += actor[i].recharge;
//  if (actor[i].energy >= actor[i].max_energy)
//   actor[i].energy = actor[i].max_energy;

   if (actor[i].screen_shake_time > 0)
   {
    if (actor[i].screen_shake_time > 4)
    {
     player[actor[i].controller].screen_shake_x = grand(7501) - 3250;
     player[actor[i].controller].screen_shake_y = grand(7501) - 3250;
    }
     else
     {
      player[actor[i].controller].screen_shake_x = grand(3500) - 1750;
      player[actor[i].controller].screen_shake_y = grand(3500) - 1750;
     } 
    actor[i].screen_shake_time --;
   }
    else
    {
     player[actor[i].controller].screen_shake_x = 0;
     player[actor[i].controller].screen_shake_y = 0;
    }
// screen_shake needs to be first, as many things below can affect it and we don't want them wiped.
   
  if (actor[i].repairing > 0)
  {
   actor[i].repairing --;
   if (actor[i].armour < actor[i].max_armour)
    actor[i].armour += 5;
   if (actor[i].armour > actor[i].max_armour)
    actor[i].armour = actor[i].max_armour;
// + play a sound?
  }

  if (actor[i].ship == SHIP_ROUND && counter % 7 == 0)
  {
   if (actor[i].armour < actor[i].max_armour)
    actor[i].armour += 1;
  }


  if (actor[i].grace_period > 0)
   actor[i].grace_period --;
  if (actor[i].hurt_pulse > 0)
   actor[i].hurt_pulse --;
  if (actor[i].recycle1 > 0)
   actor[i].recycle1 --;
  if (actor[i].recycle2 > 0)
   actor[i].recycle2 --;
   
//  if (actor[i].turret_recycle > 0)
//   actor[i].turret_recycle --;
//  if (actor[i].sidekick_recycle > 0)
//   actor[i].sidekick_recycle --;
//  if (actor[i].heavy_recycle > 0)
//   actor[i].heavy_recycle --;
//  if (actor[i].backfire_recycle > 0)
//   actor[i].backfire_recycle --;
   
  if (actor[i].secondary_burst > 0)
   continue_secondary_burst(i); 
   
//  if (actor[i].just_upgraded_timeout > 0)
//   actor[i].just_upgraded_timeout --;
  if (actor[i].just_collided > 0)
   actor[i].just_collided --;
  move_actor(i);
  detect_collision_actor_enemies(i);
  detect_collision_actor_pickups(i);
  if (actor[i].upgraded_system [UPG_SEEKER] > 0)
   acquire_target(i);
//  if (actor[i].upgraded_system [UPG_TURRET] > 0)
//   acquire_turret_target(i);
//  if (actor[i].ship == SHIP_ORBITAL && actor[i].total_power > 0)
//   actor_orbital(i);
  if (actor[i].ability [ABILITY_DEFENCE] [SYMBOL_CIRCLE] > 0)
   actor_shield(i);
//  if (actor[i].upgraded_system [UPG_SIDEKICK] > 0)
//   actor_sidekicks(i);
  if (actor[i].drive_sound [DRIVE_THRUST] > 0)
   actor[i].drive_sound [DRIVE_THRUST] --;
  if (actor[i].drive_sound [DRIVE_SLIDE] > 0)
   actor[i].drive_sound [DRIVE_SLIDE] --;
  if (actor[i].drive_sound [DRIVE_RETRO] > 0)
   actor[i].drive_sound [DRIVE_RETRO] --;
 }

}

void move_actor(int i)
{

  if (actor[i].x + actor[i].x_speed <= actor[i].edge_radius
      || actor[i].x + actor[i].x_speed >= arena.max_x - actor[i].edge_radius - 2000)// - 3000)
  {
   actor[i].x_speed *= -1;
  }
  if (actor[i].y + actor[i].y_speed <= actor[i].edge_radius
      || actor[i].y + actor[i].y_speed >= arena.max_y - actor[i].edge_radius - 4000)// - 4000)
  {
   actor[i].y_speed *= -1;
  }
  actor[i].x += actor[i].x_speed;
  actor[i].y += actor[i].y_speed;
  if (game.drag != 0)
   drag_actor(i);

}

/*
void drag_actor(int i)
{

 int drag_x, drag_y;
 int new_x_speed, new_y_speed;

 new_x_speed = ((float) actor[i].x_speed * game[0].drag);// / 10000;
 new_y_speed = ((float) actor[i].y_speed * game[0].drag);// / 10000;
 
 drag_x = abs(actor[i].x_speed - new_x_speed);
 drag_y = abs(actor[i].y_speed - new_y_speed);

 actor[i].drag_amount = hypot(drag_x, drag_y);

 if (actor[i].drag_amount < 2)
  return;


 actor[i].x_speed = new_x_speed;
 actor[i].y_speed = new_y_speed;
 
}

*/



void drag_actor(int i)
{

// int drag_x, drag_y;
// int new_x_speed, new_y_speed;

/* float drag_mult = 0.015 - (actor[i].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 0.0015);//game.drag;
 if (actor[i].dragging > 0)
 {
  drag_mult *= (float) actor[i].brake_strength * actor[i].dragging / 20;
 }*/
 //float drag_mult = 0.020 - (actor[i].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 0.0020);//game.drag;
 
 float drag_mult = 0.025 - (actor[i].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 0.0025);//game.drag;

 if (actor[i].dragging > 0)
 {
  drag_mult *= (float) actor[i].brake_strength * actor[i].dragging / 23;
 }

 int angle = radians_to_angle(atan2(actor[i].y_speed, actor[i].x_speed));
 actor[i].moving_angle = angle;
 float dragged = hypot(abs(actor[i].x_speed), abs(actor[i].y_speed)) * drag_mult;

 if (actor[i].dragging == 0 && actor[i].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] != 0)
 {
  if (dragged < 10 + actor[i].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 5)
  {
   dragged /= 5 + actor[i].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE];
  }
 }

 float change = cos(angle_to_radians(angle)) * dragged;
 actor[i].x_speed -= change;// * game[0].drag;

 change = sin(angle_to_radians(angle)) * dragged;
 actor[i].y_speed -= change;// * game[0].drag;

 if (actor[i].dragging == 20)
 {
  if (abs(actor[i].x_speed) < 20 && abs(actor[i].y_speed) < 20)
  {
   actor[i].x_speed = 0;
   actor[i].y_speed = 0;
  }
 }


 if (actor[i].dragging == 20 && grand(100) + 70 < dragged)
 {
  int passing_colours [5] = {COLOUR_YELLOW8, COLOUR_ORANGE8, COLOUR_RED8, COLOUR_RED6, COLOUR_RED4};
  create_cloud(CLOUD_SPECK,
  actor[i].x + grand(10001) - 5000,
  actor[i].y + grand(10001) - 5000,
   0, 0,
//   actor[a].x_speed + x_accel * 20 + (300 - grand(601)),
//   actor[a].y_speed + y_accel * 20 + (300 - grand(601)),
   0, 0,
   10 + grand(11),1,0, 0, 0, 0, passing_colours);
 }

 if (dragged > 250 - (actor[i].dragging == 20) * 150)
 {
   player[actor[i].controller].screen_shake_x += grand((dragged - 100) * 60) - ((dragged - 100) * 30);
   player[actor[i].controller].screen_shake_y += grand((dragged - 100) * 60) - ((dragged - 100) * 30);
//   player[actor[i].controller].screen_shake_x += grand(3500) - 1750;
//   player[actor[i].controller].screen_shake_y += grand(3500) - 1750;
//  if (dragged > 60)
// {
   
// }
 }

 actor[i].drag_amount = dragged;// * game[0].drag;
/*
 if (actor[i].drag_amount < 2)
  return;


 actor[i].x_speed = new_x_speed;
 actor[i].y_speed = new_y_speed;
*/
}


int acquire_target(int a)
{

 actor[a].seek_x = actor[a].x + cos(angle_to_radians(actor[a].angle - ANGLE_QUARTER)) * 75000;
 actor[a].seek_y = actor[a].y + sin(angle_to_radians(actor[a].angle - ANGLE_QUARTER)) * 75000;
 if (game.type == GAME_DUEL)
 {
  actor[a].lock = closest_enemy_or_actor(a, actor[a].seek_x, actor[a].seek_y,
                   35000 + actor[a].upgraded_system [UPG_SEEKER] * 30000, 1);
 }
  else
   actor[a].lock = closest_enemy(actor[a].seek_x, actor[a].seek_y,
                   35000 + actor[a].upgraded_system [UPG_SEEKER] * 30000, 1);
 return 0;

}

int acquire_turret_target(int a)
{

 if (game.type == GAME_DUEL)
 {
  actor[a].turret_lock = closest_enemy_or_actor(a, actor[a].x, actor[a].y,
                   80000 + actor[a].upgraded_system [UPG_TURRET] * 20000, 1);
 }
  else
   actor[a].turret_lock = closest_enemy(actor[a].x, actor[a].y,
                   80000 + actor[a].upgraded_system [UPG_TURRET] * 20000, 1);
 return 0;

}

int closest_enemy(int x, int y, int range, int hide_invisible)
{

 int i;
 int dist;
 int retval = -1;


 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
  if (hide_invisible == 1)
  {
   if ((enemy[i].type == ENEMY_SHADOW1
     || enemy[i].type == ENEMY_SHADOW2)
     && enemy[i].attribute [0] == 0)
      continue;
  }
  dist = hypot(y - enemy[i].y, x - enemy[i].x);
  if (dist < range)
  {
   retval = i;
   range = dist;
  }
 }

 return retval;

}

int closest_enemy_or_actor(int a, int x, int y, int range, int hide_invisible)
{

 int i;
 int dist;
 int retval = -1;


 for (i = 0; i < 2; i ++)
 {
  if (i == a)
   continue;
  if (actor[i].type == ENEMY_NONE)
   continue;
  if (actor[i].in_play == 0)
   continue;
  dist = hypot(y - actor[i].y, x - actor[i].x);
  if (dist < range)
  {
   if (i == 0)
    retval = LOCK_ACTOR0;
     else
      retval = LOCK_ACTOR1;
   range = dist;
  }
 }

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
//  if (enemy[i].type == ENEMY_DEAD_WANDERER)
//   continue; // dead fighters are still dangerous, so lock onto them
//  if (hide_invisible == 1)
//  {
//   if ((enemy[i].type == ENEMY_CLOAKER
//     || enemy[i].type == ENEMY_CLOAKER2)
//     && enemy[i].attribute [ATTRIB_CLOAKER_PULSE] == 0)
//      continue;
//  }
  dist = hypot(y - enemy[i].y, x - enemy[i].x);
  if (dist < range)
  {
   retval = i;
   range = dist;
  }
 }

 return retval;

}


void actor_orbital(int a)
{
  if (actor[a].grace_period > 0)
   return;
// this could make boss enemies too easy

  int colours [4], ox, oy;

//  if (actor[a].upgraded_system [UPG_ORBITAL] >= 4)
//   actor[a].orbital_spin = 25;
//    else
     actor[a].orbital_spin = 18;

  actor[a].orbital_angle += actor[a].orbital_spin;
  if (actor[a].orbital_angle > ANGLE_FULL)
   actor[a].orbital_angle -= ANGLE_FULL;
  if (actor[a].orbital_angle < 0)
   actor[a].orbital_angle += ANGLE_FULL;

  int damage = 500 + actor[a].total_power * 35;
  int special1 = 1 + actor[a].total_power / 5;
  
  if (special1 > 7) special1 = 7;
  
    colours [0] = TRANS_YELLOW;
    colours [1] = TRANS_LGREEN;
    colours [2] = TRANS_LGREEN;
    colours [3] = TRANS_DGREEN;
//    colours [0] = TRANS_YELLOW;
//    colours [1] = TRANS_ORANGE;
//    colours [2] = TRANS_LRED;
//    colours [3] = TRANS_DRED;
//    colours [0] = COLOUR_YELLOW8;
//    colours [1] = COLOUR_YELLOW8;
//    colours [2] = COLOUR_YELLOW6;
//    colours [3] = COLOUR_YELLOW4;

  ox = actor[a].x + xpart(actor[a].orbital_angle, ORBITAL_DISTANCE);
  oy = actor[a].y + ypart(actor[a].orbital_angle, ORBITAL_DISTANCE);

  create_bullet(BULLET_NICE_ORBITAL, ox, oy,
   0, 0,
   a, damage, 2, 100, 0,
   0, 0, colours, 1,special1,0,0,0,0);

  ox = actor[a].x + xpart(actor[a].orbital_angle + ANGLE_HALF, ORBITAL_DISTANCE);
  oy = actor[a].y + ypart(actor[a].orbital_angle + ANGLE_HALF, ORBITAL_DISTANCE);

  create_bullet(BULLET_NICE_ORBITAL, ox, oy,
   0, 0,
   a, damage, 2, 100, 0,
   0, 0, colours, 1,special1,0,0,0,0);


}

void actor_shield(int a)
{

 actor[a].max_shield = (actor[a].ability [ABILITY_DEFENCE] [SYMBOL_CIRCLE]) * 70; //100;

/* if (counter % 3 == 0)
 {
  if (actor[a].shield < actor[a].max_shield)
//  actor[a].shield += actor[a].upgraded_system [UPG_SHIELD] + 1;
   actor[a].shield += 3 + actor[a].upgraded_system [UPG_SHIELD]; // if can be increased by > 1, must not be > shield_max or layers look odd.
  if (actor[a].shield > actor[a].max_shield)
   actor[a].shield = actor[a].max_shield;
 }*/
 if (actor[a].shield < actor[a].max_shield)
 {
//  if (counter % 3 == 0)
  actor[a].shield_recharge += 2 + actor[a].ability [ABILITY_DEFENCE] [SYMBOL_TRIANGLE] * 2;
//  if (counter % 6 - actor[a].upgraded_system [UPG_SHIELD] == 0)
//   actor[a].shield ++;
  while(actor[a].shield_recharge > 10)
  {
   actor[a].shield ++;
   actor[a].shield_recharge -= 10;
  };
  if (actor[a].shield > actor[a].max_shield)
   actor[a].shield = actor[a].max_shield;
 }
// if (actor[a].shield > actor[a].max_shield)
//  actor[a].shield = actor[a].max_shield;
 
 if (actor[a].shield_pulse > 0 && counter % 3 == 0)
  actor[a].shield_pulse --;

}
/*

void actor_sidekicks(int a)
{

 int i, xcos2, ysin2;

 int passing_colours [5];

 if (actor[a].sidekick_recycle > 0)
  actor[a].sidekick_recycle --;

 int xcos = cos(angle_to_radians(actor[a].angle)) * GRAIN;
 int ysin = sin(angle_to_radians(actor[a].angle)) * GRAIN;
// int xcos2;// = cos(angle_to_radians(actor[a].angle)) * GRAIN;
// int ysin2;// = sin(angle_to_radians(actor[a].angle)) * GRAIN;

// if (actor[a].sidekicks >= 2)
// {
//  xcos2 = cos(angle_to_radians(actor[a].angle)) * GRAIN;
//  ysin2 = sin(angle_to_radians(actor[a].angle)) * GRAIN;
// }
 
 int target_x, target_y;
 float speed_mult;
 int accel = 0;

 for (i = 0; i < actor[a].sidekicks; i ++)
 {
  switch(i)
  {
   case 0:
    target_x = actor[a].x + (cos(angle_to_radians(actor[a].angle - ANGLE_1_EIGHTH)) * GRAIN * 40);
    target_y = actor[a].y + (sin(angle_to_radians(actor[a].angle - ANGLE_1_EIGHTH)) * GRAIN * 40);
    accel = 22;
    passing_colours [0] = COLOUR_YELLOW8;
    passing_colours [1] = COLOUR_GREEN8;
    passing_colours [2] = COLOUR_GREEN6;
    passing_colours [3] = COLOUR_GREEN4;
    passing_colours [4] = COLOUR_GREEN2;
    break;
   case 1:
    target_x = actor[a].x + (cos(angle_to_radians(actor[a].angle - ANGLE_QUARTER - ANGLE_1_EIGHTH)) * GRAIN * 40);
    target_y = actor[a].y + (sin(angle_to_radians(actor[a].angle - ANGLE_QUARTER - ANGLE_1_EIGHTH)) * GRAIN * 40);
//    target_x = actor[a].x - xcos * 20;
//    target_y = actor[a].y - ysin * 20;
    accel = 22;
    break;
   case 2:
    target_x = actor[a].x + xcos * 40;
    target_y = actor[a].y + ysin * 40;
    accel = 12;
    passing_colours [0] = COLOUR_YELLOW8;
    passing_colours [1] = COLOUR_YELLOW6;
    passing_colours [2] = COLOUR_YELLOW4;
    passing_colours [3] = COLOUR_YELLOW3;
    passing_colours [4] = COLOUR_YELLOW2;
    break;
   case 3:
    target_x = actor[a].x - xcos * 40;
    target_y = actor[a].y - ysin * 40;
    accel = 12;
    break;
  }

  speed_mult = hypot(actor[a].sidekick_y [i] - target_y, actor[a].sidekick_x [i] - target_x) / 600;

  if (speed_mult > 200)
   speed_mult = 200;
   
  speed_mult *= accel;
  speed_mult /= 10;
/ *  if (speed_mult < 20 && abs(actor[a].sidekick_x_speed [i]) + abs(actor[a].sidekick_y_speed [i]) < 2000)
  {
   actor[a].sidekick_angle [i] = turn_towards_angle(actor[a].sidekick_angle [i], actor[a].angle + ANGLE_QUARTER, 8);
   actor[a].sidekick_x_speed [i] = 0;
   actor[a].sidekick_y_speed [i] = 0;
  }
   else
   {
    actor[a].sidekick_angle [i] = turn_towards_xy(actor[a].sidekick_x [i], actor[a].sidekick_y [i], target_x, target_y, actor[a].sidekick_angle [i], 16);

//  actor[a].sidekick_x_speed [i] = cos(angle_to_radians(actor[a].sidekick_angle [i])) * speed_mult;
//  actor[a].sidekick_y_speed [i] = sin(angle_to_radians(actor[a].sidekick_angle [i])) * speed_mult;

//  actor[a].sidekick_x_speed [i] += cos(angle_to_radians(actor[a].sidekick_angle [i])) * speed_mult;
//  actor[a].sidekick_y_speed [i] += sin(angle_to_radians(actor[a].sidekick_angle [i])) * speed_mult;
    xcos2 = cos(angle_to_radians(actor[a].sidekick_angle [i])) * speed_mult;
    ysin2 = sin(angle_to_radians(actor[a].sidekick_angle [i])) * speed_mult;
    actor[a].sidekick_x_speed [i] += xcos2;
    actor[a].sidekick_y_speed [i] += ysin2;
   }

  actor[a].sidekick_x_speed [i] *= 0.97;
  actor[a].sidekick_y_speed [i] *= 0.97;

  actor[a].sidekick_x [i] += actor[a].sidekick_x_speed [i];
  actor[a].sidekick_y [i] += actor[a].sidekick_y_speed [i];

  if (grand(12) == 0 && abs(actor[a].sidekick_x_speed [i]) + abs(actor[a].sidekick_y_speed [i]) > 1000)
  {
   create_cloud(CLOUD_SPECK,
    actor[a].sidekick_x [i] - xcos2 * 37,
    actor[a].sidekick_y [i] - ysin2 * 37,
    0, 0,
    actor[a].sidekick_x_speed [i] - (xcos2 * 10) + (100 - grand(51)),
    actor[a].sidekick_y_speed [i] - (ysin2 * 10) + (100 - grand(51)),
    10 + grand(11),1,0, 0, 0, 0, passing_colours);
  }
  

 }


}

*/

int gain_symbol(int a, int abil, int symb)
{
 if (actor[a].ability [abil] [symb] >= 5)
 {
  // random_symbol(a); 
  return 0;
 }

 actor[a].ability [abil] [symb] ++;

 switch(abil)
 {
  case ABILITY_PRIMARY:
   switch(symb)
   {
    case SYMBOL_SQUARE:
//     send_message(actor[a].controller, "Dart Reload", COLOUR_YELLOW8);
//   actor[a].primary = new_weapon(a);
     break;
    case SYMBOL_CIRCLE:
//     send_message(actor[a].controller, "Multiple Darts", COLOUR_YELLOW8);
     break;
    case SYMBOL_TRIANGLE:
//     send_message(actor[a].controller, "Fast Darts", COLOUR_YELLOW8);
     break;
   }
   break;
  case ABILITY_SECONDARY:
   switch(symb)
   {
    case SYMBOL_SQUARE:
//     send_message(actor[a].controller, "Power", COLOUR_ORANGE8);
//   actor[a].primary = new_weapon(a);
     break;
    case SYMBOL_CIRCLE:
//     send_message(actor[a].controller, "Number", COLOUR_ORANGE8);
     break;
    case SYMBOL_TRIANGLE:
//     send_message(actor[a].controller, "Range", COLOUR_ORANGE8);
     break;
   }
//   actor[a].secondary = new_secondary(a);
   break; 
  case ABILITY_DEFENCE:
   switch(symb)
   {
    case SYMBOL_SQUARE:
//     send_message(actor[a].controller, "Armour", COLOUR_BLUE8);
     actor[a].max_armour = 1000 + ARMOUR_UPGRADE * actor[a].ability [ABILITY_DEFENCE] [SYMBOL_SQUARE];
      if (game.type == GAME_DUEL)
      {
       actor[a].max_armour = duel_armour(actor[a].max_armour, game.duel_handicap [actor[a].controller]);
       actor[a].armour += duel_armour(ARMOUR_UPGRADE, game.duel_handicap [actor[a].controller]);
      } 
        else actor[a].armour += ARMOUR_UPGRADE;
      if (actor[a].armour > actor[a].max_armour)
       actor[a].armour = actor[a].max_armour;
     break;
     case SYMBOL_CIRCLE:
//      send_message(actor[a].controller, "Ward", COLOUR_BLUE8);
      break;
     case SYMBOL_TRIANGLE:
//      send_message(actor[a].controller, "Ward Restore", COLOUR_BLUE8);
      break;
   }
   break;
  case ABILITY_DRIVE:
   switch(symb)
   {
    case SYMBOL_SQUARE:
//     send_message(actor[a].controller, "Drive", COLOUR_GREEN8);
     break;  
    case SYMBOL_CIRCLE:
//     send_message(actor[a].controller, "Turn", COLOUR_GREEN8);
     break;  
    case SYMBOL_TRIANGLE:
//     send_message(actor[a].controller, "Drift", COLOUR_GREEN8);
     break;  
   }
   break;
//    case SYMBOL_:
//     send_message(actor[a].controller, "++", COLOUR_GREEN8);
//     break;  
 
 
 }
 
 calulate_total_power(a);

 return 1;
}

int new_weapon(int a)
{

 return WPN_DARTS;

 int highest = 0;
 if (actor[a].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE] > highest)
  highest = actor[a].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE];
 if (actor[a].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] > highest)
  highest = actor[a].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
 if (actor[a].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] > highest)
  highest = actor[a].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE];

// if (highest < 2)
//  return WPN_DARTS;

// int wpn = WPN_DAR;
 if (actor[a].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE] == highest)
 {
  if (highest * 10 > actor[a].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] * DISTINCT_WEAPON
      && highest * 10 > actor[a].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * DISTINCT_WEAPON)
       return WPN_SPINES;
 } else
  {
    if (actor[a].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] == highest)
    {
     if (highest * 10 > actor[a].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE] * DISTINCT_WEAPON
         && highest * 10 > actor[a].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * DISTINCT_WEAPON)
          return WPN_BURST;
    } else
      if (actor[a].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] == highest)
      {
       if (highest * 10 > actor[a].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] * DISTINCT_WEAPON
           && highest * 10 > actor[a].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE] * DISTINCT_WEAPON)
            return WPN_TEETH;
      }
  }

 return WPN_DARTS;
 
}



int new_secondary(int a)
{

 int highest = 0;
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] > highest)
  highest = actor[a].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE];
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] > highest)
  highest = actor[a].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE];
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] > highest)
  highest = actor[a].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE];
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_CROSS] > highest)
  highest = actor[a].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];

 int wpn = 0;
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] == highest) wpn += 1;
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] == highest) wpn += 10;
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] == highest) wpn += 100;
 if (actor[a].ability [ABILITY_SECONDARY] [SYMBOL_CROSS] == highest) wpn += 1000;
 
 switch(wpn)
 {
  default:
  case 0:
   return SECOND_NONE;
  case 1:
   return SECOND_FURIOUS_ORB;
  case 10:
   return SECOND_EYE_DESOLATION;
  case 100:
   return SECOND_WORMS_SORROW;
  case 1000:
   return SECOND_FROZEN_STARS;
  case 11:
   return SECOND_BURNING_EYE;
  case 101:
   return SECOND_LARVA_NEST;
  case 1001:
   return SECOND_MANIFOLD_ORB;
  case 110:
   return SECOND_TERROR_EELS;
  case 1010:
   return SECOND_PANIC_EELS;
  case 1100:
   return SECOND_WORMS_AGONY;
  case 111:
//   return SECOND_ORB_RECTIFICATION;
//  case 1011:
   return SECOND_NONE;
  case 1101:
   return SECOND_NONE;
  case 1110:
   return SECOND_NONE;
//  case 1111:
//   return SECOND_CIRCLE_PAIN; 
 
 }

 return SECOND_NONE;
 
}





int detect_collision_actor_enemies(int a)
{
 if (actor[a].grace_period > 0 || actor[a].in_play == 0)
  return -1;

 int x2 = actor[a].x - actor[a].x_speed;
 int y2 = actor[a].y - actor[a].y_speed;
 int i;

 int angle1, angle2, angle3;
 int total_speed;
 int difference;
 int collision_velocity = 0;
 int hurty = 0;
 int passing_colours [5];

 int xs2, ys2;

 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE)
   continue;
  if (enemy[i].radius == 0)
  // enemy has no collision radius - dead wanderer etc
   continue;

  if (actor[a].just_collided > 0 && enemy[i].just_collided > 0)
   continue;

  if (hypot(actor[a].x - enemy[i].x, actor[a].y - enemy[i].y) <= actor[a].radius + enemy[i].radius
      || hypot(x2 - enemy[i].x, y2 - enemy[i].y) <= actor[a].radius + enemy[i].radius)
  {
/*   if (actor[a].x_speed == 0)
    tangent = 1000;
     else
      tangent = tan(actor[a].y_speed / actor[a].x_speed);
*/
   switch(enemy[i].type)
   {
/*    case ENEMY_SWERVER:
    case ENEMY_SWERVER2:
    case ENEMY_SWERVER3:
    case ENEMY_BOUNCER:
    case ENEMY_BOUNCER2:
    case ENEMY_BOUNCER3:
    case ENEMY_BOUNCER4:
    case ENEMY_BOUNCER5:
     hurt_enemy(i, 99999, -1, 1);
     return -1;*/

   }


// Really dodgy collision physics...

   collision_velocity = hypot(abs(actor[a].x_speed - enemy[i].x_speed), abs(actor[a].y_speed - enemy[i].y_speed));

// First calculate actor's post-collision velocity:
   angle1 = radians_to_angle(atan2(actor[a].y_speed, actor[a].x_speed));
   angle2 = radians_to_angle(atan2(actor[a].y - enemy[i].y, actor[a].x - enemy[i].x));

   total_speed = hypot(actor[a].y_speed, actor[a].x_speed);

   angle1 += ANGLE_HALF;
   if (angle2 > ANGLE_FULL)
    angle2 -= ANGLE_FULL;

   angle2 += ANGLE_QUARTER;
   if (angle2 > ANGLE_FULL)
    angle2 -= ANGLE_FULL;

/*   if (angle1 < angle2)
   {
    difference = angle2 - angle1;
   }
    else
    {
     difference = angle1 - angle2;
    }

   angle3 = angle1 - difference;*/

   difference = angle2 - angle1;
   angle3 = angle2 - difference; // or +?

   xs2 = actor[a].x_speed;
   ys2 = actor[a].y_speed;

   actor[a].x_speed = cos(angle_to_radians(angle3)) * total_speed;
   actor[a].y_speed = sin(angle_to_radians(angle3)) * total_speed;
//   actor[a].x_speed = cos(angle_to_radians(angle3)) * total_speed;
//   actor[a].y_speed = sin(angle_to_radians(angle3)) * total_speed;

   actor[a].x_speed += (enemy[i].x_speed / 2) * 100 / actor[a].mass;
   actor[a].y_speed += (enemy[i].y_speed / 2) * 100 / actor[a].mass;

// Now the enemy's
   angle1 = radians_to_angle(atan2(enemy[i].y_speed, enemy[i].x_speed));
   angle2 = radians_to_angle(atan2(enemy[i].y - actor[a].y, enemy[i].x - actor[a].x));

   total_speed = hypot(enemy[i].y_speed, enemy[i].x_speed);

   angle1 += ANGLE_HALF;
   if (angle2 > ANGLE_FULL)
    angle2 -= ANGLE_FULL;

   angle2 += ANGLE_QUARTER;
   if (angle2 > ANGLE_FULL)
    angle2 -= ANGLE_FULL;

   difference = angle2 - angle1;
   angle3 = angle2 - difference; // or +?

   enemy[i].x_speed = xpart(angle3, total_speed);//cos(angle_to_radians(angle3)) * total_speed;
   enemy[i].y_speed = ypart(angle3, total_speed);//sin(angle_to_radians(angle3)) * total_speed;

   enemy[i].x_speed += (xs2 * 25) / enemy[i].mass * actor[a].mass / 100;
   enemy[i].y_speed += (ys2 * 25) / enemy[i].mass * actor[a].mass / 100;


   int sanity = 0;
          
   do
   {
    actor[a].x_speed += xpart(angle2, 100);//grand(101) - 50;
    actor[a].y_speed += ypart(angle2, 100);//grand(101) - 50;
    move_actor(a);
    sanity ++;
    if (sanity >= 5000)
     break; // hmm
   } while (hypot(actor[a].x - enemy[i].x, actor[a].y - enemy[i].y) < actor[a].radius + enemy[i].radius);

   actor[a].just_collided = 7;
   enemy[i].just_collided = 7;

   if (actor[a].ship == SHIP_CURVE)
   {
      passing_colours [0] = TRANS_DGREEN;
      passing_colours [1] = TRANS_LGREEN;
      passing_colours [2] = TRANS_YELLOW;
       
      place_explosion(actor[a].x, actor[a].y, 0,0,
       300 + crandom(150), passing_colours);
      blast(actor[a].x, actor[a].y, 40000, 100 + actor[a].total_power * 15, 20000, a);
      blast(enemy[i].x, enemy[i].y, 40000, 0, 2000, OWNER_ENEMY);
      play_wavf(NWAV_BURSTZ, 300 + grand(300));
//   blast(bullet[b].x, bullet[b].y, 30000, 200, 1200, bullet[b].owner);
   }
    else
    {
      passing_colours [0] = TRANS_DGREY;
      passing_colours [1] = TRANS_LGREY;
      passing_colours [2] = TRANS_WHITE;
       
      place_explosion(actor[a].x, actor[a].y, 0,0,
       200 + crandom(150), passing_colours);
     }  

// we cannot have any references to enemy[i] below as it may have been destroyed by the blast
//  call above.
       
   place_explosion(actor[a].x, actor[a].y, 0,0,
    200 + crandom(150), passing_colours);

   play_wavf(NWAV_BUMP, 700 + grand(800));

   hurty = collision_velocity / 30;
   if (hurty > 50)
    hurty = 50;
   
   if (check_shielder(i) == 0 && actor[a].ship != SHIP_CURVE)
   {
    hurt_enemy(i, hurty, a, 1);
    if (enemy[i].hurt_pulse < 4)
     enemy[i].hurt_pulse = 4;
   }
   
   hurt_actor(a, a, hurty);

   break;
/*
   if ((angle1 < angle2 && angle2 > angle1 + ANGLE_HALF)
       || (angle1 > angle2 && angle2 > angle1 - ANGLE_HALF))
   {
    difference =
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
*/

  }
 
 }

 return -1;

}


int detect_collision_actor_pickups(int a)
{

 int x2 = actor[a].x - actor[a].x_speed;
 int y2 = actor[a].y - actor[a].y_speed;
 int i;
 char destroy = 0;

 for (i = 0; i < NO_PICKUPS; i ++)
 {
  if (pickup[i].type == PICKUP_NONE)
   continue;

  if (hypot(actor[a].x - pickup[i].x, actor[a].y - pickup[i].y) <= actor[a].radius + pickup[i].radius
      || hypot(x2 - pickup[i].x, y2 - pickup[i].y) <= actor[a].radius + pickup[i].radius)
  {
   destroy = 1;
   switch(pickup[i].type)
   {
    case PICKUP_PRESYMBOL:
     destroy = 0;
     break;
    case PICKUP_PRESECONDARY:
     destroy = 0;
     break;
    case PICKUP_SECONDARY:
     actor[a].secondary = pickup[i].subtype;
     actor[a].secondary_burst = 0;
     actor[a].secondary_burst_recycle = 0;
     play_wavf(NWAV_REPAIR, 1700);
     break;
    case PICKUP_SQUARE:
//     gain_upgrade_points(a, 1);
     destroy = gain_symbol(a, pickup[i].subtype, SYMBOL_SQUARE);
     if (destroy)
//      play_soundf(WAV_PICKUP_UPGRADE, 1000);
      play_wavf(NWAV_SYMBOL, 1000);
     break;
    case PICKUP_TRIANGLE:
     destroy = gain_symbol(a, pickup[i].subtype, SYMBOL_TRIANGLE);
//     gain_upgrade_points(a, 2);
     if (destroy)
      play_wavf(NWAV_SYMBOL, 1700);
//      play_soundf(WAV_PICKUP_UPGRADE, 1500);
     break;
    case PICKUP_CIRCLE:
     destroy = gain_symbol(a, pickup[i].subtype, SYMBOL_CIRCLE);
//     gain_upgrade_points(a, 3);
     if (destroy)
      play_wavf(NWAV_SYMBOL, 1300);
//      play_soundf(WAV_PICKUP_UPGRADE, 2000);
     break;
    case PICKUP_REPAIR:
//     actor[a].armour = actor[a].max_armour;
     actor[a].repairing += 150; // actor[a].max_armour / 10;
     play_wav(NWAV_REPAIR);
     break;
    case PICKUP_GRACE:
     actor[a].grace_period = 333;
     break;
    case PICKUP_SHIP:
     extra_ship();
     break;
   }
   if (destroy)
    pickup_explodes(i, 1);
  }
 
 }

 return -1;

}


void extra_ship(void)
{
 if (game.ships_left <= 20)
 {
  game.ships_left ++;
  play_wav(NWAV_EXTRA);
  if (game.users == 1)
  {
//   send_message(game.single_player, "Extra!", STYLE_UPGRADE5);
  }
   else
   {
//    send_message(0, "Extra!", STYLE_UPGRADE5);
//    send_message(1, "Extra!", STYLE_UPGRADE5);
   }
 }
}



int hurt_actor(int a, int source, int hurty)
{

 if (actor[a].in_play == 0)
  return 0;
  
//  hurty *= 2;
  
 if (game.type == GAME_DUEL && game.duel_leader_handicap > 0)
 {
  if (player[actor[a].controller].duel_score > player[actor[a].controller == 0].duel_score)
  {
   hurty *= 10 + (player[actor[a].controller].duel_score - player[actor[a].controller == 0].duel_score) * game.duel_leader_handicap;
   hurty /= 10;
  }
 }

 if (actor[a].ability [ABILITY_DEFENCE] [SYMBOL_CIRCLE] > 0
     && actor[a].shield > 0)
 {
  if (actor[a].shield >= 20)
   play_wavf(NWAV_SHIELD, 400 + actor[a].shield * 2);
  if (hurty > actor[a].shield)
  {
   hurty -= actor[a].shield;
   actor[a].shield = 0;
   actor[a].shield_pulse = 2 + actor[a].ability [ABILITY_DEFENCE] [SYMBOL_CIRCLE];
  }
   else
   {
//    play_wavf(NWAV_SHIELD, 400 + actor[a].shield * 2);
    actor[a].shield -= hurty;
    actor[a].shield_pulse = 5 + actor[a].ability [ABILITY_DEFENCE] [SYMBOL_CIRCLE];
    return 0;
   }
//  play_soundf(WAV_SHIELD, 1000);
 }

// return 0;
// shields go here...

// int passing_colours [5];

 actor[a].hurt_pulse = 4;

 if (hurty >= actor[a].armour)
 {
  actor[a].armour = 0;
  actor_explodes(a);
  actor_destroyed(a, source);
  return 0;
 }

 play_wavf(NWAV_BUMP2, 900 + grand(300));

 actor[a].armour -= hurty;
 return 1;

}

void actor_destroyed(int a, int source)
{

 actor[a].in_play = 0;
 actor[a].spawn_delay = 50;
// actor[a].lock = -1;
// actor[a].turret_lock = -1;

// actor[a].x = grand(arena.max_x - 100000) + 50000;
// actor[a].y = grand(arena.max_y - 100000) + 50000;
 actor[a].next_spawn_x = grand(arena.max_x - 100000) + 50000;
 actor[a].next_spawn_y = grand(arena.max_y - 100000) + 50000;


 int e;

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  if (enemy[e].type == ENEMY_NONE)
   continue;
  if (enemy[e].attacking == a)
   enemy[e].attacking = ATTACK_NONE;
 }

 if (game.type == GAME_DUEL)
 {
  score_duel_kill(a, source);
  return;
 }

 if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
 {
  arena.time_left -= 33.333 * 30;
  if (arena.time_left < 1)
   arena.time_left = 1;
  return;
 }

  
 if (game.ships_left == 0)
 {
  if (game.users == 1)
   game_is_over();
    else
    {
     if (actor[0].in_play == 0 && actor[1].in_play == 0)
      game_is_over();
    }
 }
 
}

void game_is_over(void)
{
 arena.game_over = 132;
}


void score_duel_kill(int a, int source)
{
 if (arena.game_over > 0)
  return;

 if (a == source || source == OWNER_ENEMY)
 {
  if (player[actor[a].controller].duel_score > 0)
   player[actor[a].controller].duel_score --;
  return;
 }

 int p = actor[source].controller;

 player[p].duel_score ++;

 if ((player[p].duel_score >= 10 && game.duel_mode == DUEL_10_POINTS)
     || (player[p].duel_score >= 30 && game.duel_mode == DUEL_30_POINTS))
 {
  game.duel_winner = p;
  arena.game_over = 132;
 }
 
}



void actor_explodes(int a)
{

 int passing_colours [5];

 passing_colours[0] = TRANS_DRED;
 passing_colours[1] = TRANS_LRED;
 passing_colours[2] = TRANS_YELLOW;

 place_explosion(actor[a].x, actor[a].y, 0,0,//actor[a].x_speed, actor[a].y_speed,
  1500 + crandom(250), passing_colours);
 place_burstlet_burst(actor[a].x, actor[a].y, 0, 0, 4 + grand(3), 2, 6, 1000, 1500,
  4, passing_colours);

 int i;

 if (actor[a].sidekicks != 0)
 {
  for (i = 0; i < actor[a].sidekicks; i ++)
  {
   place_explosion(actor[a].sidekick_x [i], actor[a].sidekick_y [i], 0,0,
    500 + crandom(250), passing_colours);
   place_burstlet_burst(actor[a].sidekick_x [i], actor[a].sidekick_y [i], 0, 0, 2 + grand(3), 2, 6, 1000, 1500,
    4, passing_colours);
  }
  actor[a].sidekicks = 0;
 }

 kill_drive_sounds();
 play_wavf(NWAV_BLAST, 500 + grand(200));

}


// called at start of each game
void init_actor(int a, int type)
{
 int i,j;

 actor[a].type = ACTORTYPE_SHIP; // redundant
 actor[a].ship = type;
 actor[a].radius = 9000;
 actor[a].edge_radius = 5000;
// actor[a].turning = 70; now calculated dynamically
 actor[a].thrust = 0;
 actor[a].slide = 50;
 actor[a].mass = 100;
 actor[a].x = grand(494000) + 50000;
 actor[a].y = grand(495000) + 50000;
 actor[a].next_spawn_x = actor[a].x;
 actor[a].next_spawn_y = actor[a].y;

 for (i = 0; i < 4; i ++)
 {
  for (j = 0; j < 4; j ++)
  {
   actor[a].ability [i] [j] = 0;  
  } 
 }
 
 actor[a].total_power = 0;

// for (j = 0; j < NO_UPGRADES; j ++)
// {
//  actor[a].upgrades [j] = 0;
//  actor[a].upgrade_specials [j] = 0;
// }
// for (j = 0; j < MAX_UPGRADES; j ++)
// {
//actor[a].upgraded_system [j] = 0;
// }
// for (j = 0; j < 9; j ++)
// {
//  actor[a].upgrade_structure [j] = upgrades [actor[a].ship] [j];
//  actor[a].upgrade_specials [j] = 0;
// }
/* actor[a].upgrade_structure [0] = UPG_NONE;
 actor[a].upgrade_structure [1] = UPG_SPEED;
 actor[a].upgrade_structure [2] = UPG_SLIDE;
 actor[a].upgrade_structure [3] = UPG_RETRO;
 actor[a].upgrade_structure [4] = UPG_SIDEKICK;
 actor[a].upgrade_structure [5] = UPG_ROCKET;
 actor[a].upgrade_structure [6] = UPG_PROJECT;
 actor[a].upgrade_structure [7] = UPG_WARHEAD;
 actor[a].upgrade_structure [8] = UPG_SPECIAL;*/
 actor[a].upgrade_slot = 0;
 actor[a].brakes = BRAKES_DRAG;
 actor[a].brake_strength = 5;
 actor[a].max_armour = 1000;
 if (game.type == GAME_DUEL)
 {
  actor[a].max_armour = duel_armour(actor[a].max_armour, game.duel_handicap [actor[a].controller]);
 }
 actor[a].armour = actor[a].max_armour;
 actor[a].max_shield = 0;
 actor[a].shield = 0;
 actor[a].spawn_delay = 50;
 actor[a].grace_period = 0;
 actor[a].secondary = SECOND_NONE;//D_ORB;//SECONDARY_NONE;
 actor[a].sidekicks = 0;

// actor[a].exhaust_distance_x = 14;
// actor[a].exhaust_distance_y = 15;
 actor[a].exhaust_distance_x = 10;
 actor[a].exhaust_distance_y = 10;
 actor[a].flash_distance = 5000;

 switch(actor[a].ship)
 {
  case SHIP_SMALL:
   actor[a].radius = 7000;
   actor[a].edge_radius = 3000;
   actor[a].exhaust_distance_x = 7;
   actor[a].exhaust_distance_y = 7;
   break;
  case SHIP_HORSESHOE:
   actor[a].exhaust_distance_x = 4;
   actor[a].exhaust_distance_y = 4;
   break;
  case SHIP_FINS:
   actor[a].exhaust_distance_x = 9;
   actor[a].exhaust_distance_y = 9;
   break;
  case SHIP_RETRO:
   actor[a].exhaust_distance_x = 9;
   actor[a].exhaust_distance_y = 9;
   break;
  case SHIP_ROUND:
   actor[a].exhaust_distance_x = 9;
   actor[a].exhaust_distance_y = 9;
   break;
  case SHIP_ORBITAL:
   actor[a].exhaust_distance_x = 1;
   actor[a].exhaust_distance_y = 1;
   actor[a].mass = 400;
   break;
  case SHIP_CURVE:
   actor[a].exhaust_distance_x = 3;
   actor[a].exhaust_distance_y = 3;
   break;
   
 /*
  case SHIP_LACEWING:
  actor[a].exhaust_distance_x = 1;
  actor[a].exhaust_distance_y = 0;
  actor[a].flash_distance = 4000;
  break;
  case SHIP_AETHER:
  actor[a].exhaust_distance_x = 1;
  actor[a].exhaust_distance_y = 0;
  actor[a].flash_distance = 7000;
  break;
  case SHIP_DESPOT:
  actor[a].exhaust_distance_x = 1;
  actor[a].exhaust_distance_y = 0;
  break;
  case SHIP_CAPYBARA:
  actor[a].exhaust_distance_x = 6;
  actor[a].exhaust_distance_y = 5;
  actor[a].flash_distance = 6000;
  break;
  case SHIP_LENTIL:
  actor[a].exhaust_distance_x = 6;
  actor[a].exhaust_distance_y = 5;
  break;
  case SHIP_HOOKWORM:
  actor[a].exhaust_distance_x = 8;
  actor[a].exhaust_distance_y = 7;
  actor[a].flash_distance = 3000;
  break;
  case SHIP_PORKUPINE:
  actor[a].exhaust_distance_x = 8;
  actor[a].exhaust_distance_y = 7;
  actor[a].flash_distance = 4000;
  break;
  case SHIP_SCORPION:
  actor[a].exhaust_distance_x = 4;
  actor[a].exhaust_distance_y = 3;
  actor[a].flash_distance = 4000;
  break;
  case SHIP_TORTFEASOR:
  actor[a].exhaust_distance_x = 4;
  actor[a].exhaust_distance_y = 3;
  actor[a].flash_distance = 9000;
  break;
  case SHIP_RUMSFELD:
  actor[a].exhaust_distance_x = 4;
  actor[a].exhaust_distance_y = 3;
  break;
  case SHIP_PRONG:
  actor[a].flash_distance = 1000;
  break;
  case SHIP_GODBOTHERER:
  actor[a].exhaust_distance_x = 3;
  actor[a].exhaust_distance_y = 2;
  actor[a].flash_distance = 5000;
  break;
  case SHIP_BOTULUS:
  actor[a].exhaust_distance_x = 3;
  actor[a].exhaust_distance_y = 2;
  actor[a].flash_distance = 6000;
  break;
  case SHIP_SLIDEWINDER:
  actor[a].exhaust_distance_x = 1;
  actor[a].exhaust_distance_y = 0;
  actor[a].flash_distance = 4000;
  break;
  case SHIP_DOOM:
  actor[a].exhaust_distance_x = 7;
  actor[a].exhaust_distance_y = 6;
  actor[a].flash_distance = 8000;
  break;
  */
 }

 
 actor[a].retro_distance_x = 5;
 actor[a].retro_distance_y = 4;
 actor[a].slide_distance_x = 5;
 actor[a].slide_distance_y = 4;
// actor[a].exhaust_displacement = 0;

 actor[a].primary = WPN_DARTS;
}

int duel_armour(int armour, int h)
{
  switch(h)
  {
   default:
    armour *= 100;
    break;
   case 0:
    armour *= 75;
    break;
   case 1:
    armour *= 100;
    break;
   case 2:
    armour *= 120;
    break;
   case 3:
    armour *= 150;
    break;
  }
  armour /= 100;

  return armour;
}


void calulate_total_power(int a)
{
 int i, j;
 
 actor[a].total_power = 0;
 
 for (i = 0; i < 4; i ++)
 {
  for (j = 0; j < 4; j ++)
  {
   actor[a].total_power += actor[a].ability [i] [j];
  }
 } 
}


// called when an actor spawns after being destroyed
void spawn_actor(int a)
{

 actor[a].in_play = 1;
// actor[a].x = arena.max_x / 2; //grand(arena.max_x - 100000) + 50000;
// actor[a].y = arena.max_y / 2; //grand(arena.max_y - 100000) + 50000;
 actor[a].x = actor[a].next_spawn_x;
 actor[a].y = actor[a].next_spawn_y;

      int passing_colours [5];

      passing_colours [0] = COLOUR_WHITE;

      create_cloud(CLOUD_SHRINKING_CIRCLE,
       actor[a].x,
       actor[a].y,
       0, 0,
       0, 0,
       500, -300, 10, 0, 0, 0, passing_colours);
       
      passing_colours [0] = TRANS_DBLUE;
      passing_colours [1] = TRANS_LBLUE;
      passing_colours [2] = TRANS_WHITE;
  
      place_explosion(actor[a].x, actor[a].y, 0,0,
       700 + crandom(250), passing_colours);


/* actor[a].x_speed = 0;
 actor[a].y_speed = 0;
 actor[a].angle = 0;
 actor[a].base_angle = 0;
 actor[a].dragging = 0;
 actor[a].drag_amount = 0;
// actor[a].in_play = 0;
 actor[a].moving_angle = actor[0].angle;
 actor[a].just_collided = 0;
 
// actor[a].recharge = ships[ship].recharge;
 actor[a].just_upgraded = 0;
 actor[a].just_upgraded_timeout = 0;
 actor[a].shield_pulse = 0;
 actor[a].recycle1 = 0;
 actor[a].recycle2 = 0;
 actor[a].heavy_recycle = 0;
 actor[a].turret_recycle = 0;
 actor[a].backfire_recycle = 0;
 actor[a].cannon_status = 0;
 actor[a].orbital_angle = 0;
 actor[a].bomb_status = 0;
 actor[a].upgrade_slot = 0;
 actor[a].spawn_delay = 0;
 actor[a].grace_period = 169;
 actor[a].armour = actor[a].max_armour;
 actor[a].max_shield = 0;
 actor[a].shield = 0;
 actor[a].lock = -1;
 actor[a].turret_lock = -1;*/

 reset_actor(a);

 actor[a].secondary = SECOND_NONE; // not in reset_actor as we want to retain this between levels.

 actor[a].sidekicks = 0;
// if (actor[a].upgraded_system [UPG_SIDEKICK] > 0)
//  upgrade_sidekicks(a);
 
 play_wavf(NWAV_SPAWN, 800);

 if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP || game.type == GAME_DUEL)
 {
  calculate_beat();
 }

 if (game.type != GAME_DUEL && game.type != GAME_TIME_ATTACK && game.type != GAME_TIME_ATTACK_COOP)
  game.ships_left --;
}

void actor_new_level(int a)
{

 actor[a].in_play = 1;
// actor[a].x = arena.max_x / 2; //grand(arena.max_x - 100000) + 50000;
// actor[a].y = arena.max_y / 2; //grand(arena.max_y - 100000) + 50000;
 actor[a].x = grand(arena.max_x - 100000) + 50000;
 actor[a].y = grand(arena.max_y - 100000) + 50000;
 actor[a].next_spawn_x = actor[a].x;
 actor[a].next_spawn_y = actor[a].y;

// actor[a].sidekicks = 0;
// if (actor[a].upgraded_system [UPG_SIDEKICK] > 0)
//  upgrade_sidekicks(a);
 
 reset_actor(a);
 
}


void reset_actor(int a)
{
 int j;
 
 actor[a].x_speed = 0;
 actor[a].y_speed = 0;
 actor[a].angle = 0;
 actor[a].base_angle = 0;
 actor[a].dragging = 0;
 actor[a].drag_amount = 0;
 actor[a].moving_angle = actor[0].angle;
 actor[a].just_collided = 0;
 actor[a].lock = -1;
 
 actor[a].just_upgraded = 0;
 actor[a].just_upgraded_timeout = 0;
 actor[a].shield_pulse = 0;
 actor[a].recycle1 = 0;
 actor[a].recycle2 = 0;
 actor[a].heavy_recycle = 0;
 actor[a].turret_recycle = 0;
 actor[a].backfire_recycle = 0;
 actor[a].sidekick_recycle = 0;
 actor[a].orbital_angle = 0;
 actor[a].lock = -1;
 actor[a].turret_lock = -1;
 actor[a].cannon_status = 0;
 actor[a].bomb_status = 0;
// actor[a].upgrade_slot = 0;
 actor[a].spawn_delay = 0;
 actor[a].grace_period = 169;
 actor[a].repairing = 0;
 actor[a].armour = actor[a].max_armour;
 actor[a].max_shield = 0;
 actor[a].shield = 0;
 actor[a].shield_recharge = 0;
 actor[a].hurt_pulse = 0;
 actor[a].secondary_burst = 0;
 actor[a].secondary_burst_recycle = 0;
 actor[a].screen_shake_time = 0;

 for (j = 0; j < 5; j ++)
 {
  actor[a].sidekick_x [j] = actor[a].x;
  actor[a].sidekick_y [j] = actor[a].y;
  actor[a].sidekick_x_speed [j] = 0;
  actor[a].sidekick_y_speed [j] = 0;
 }

 actor[a].drive_sound [DRIVE_THRUST] = 0;
 actor[a].drive_sound [DRIVE_SLIDE] = 0;
 actor[a].drive_sound [DRIVE_RETRO] = 0;
 actor[a].thrust = 0;
 
}


void gain_score(int p, int sc)
{
 if (arena.game_over > 0)
  return;
 if (game.type == GAME_DUEL)
  return;

 int old_score = player[p].score;
 player[p].score += sc;
 if (player[p].score > MAX_SCORE)
  player[p].score = MAX_SCORE;

/* if (game.type != GAME_COOP)
 {
  if (old_score < 1000 && player[p].score >= 1000)
   extra_ship();
 }*/

 if (old_score < 1000 && player[p].score >= 1000)
  extra_ship();

// if (old_score < 5000 && player[p].score >= 5000)
//  extra_ship();

// if (old_score < 10000 && player[p].score > 10000)
//  extra_ship();

 if (old_score / 5000 != player[p].score / 5000)
  extra_ship();
  
}



void shake_all_screens(int amount)
{

 actor[0].screen_shake_time += amount;
 if (actor[0].screen_shake_time > 15)
  actor[0].screen_shake_time = 15;
 actor[1].screen_shake_time += amount;
 if (actor[1].screen_shake_time > 15)
  actor[1].screen_shake_time = 15;

}
