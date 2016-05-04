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
    
File: enemy.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - most functions relating to enemies

*/

#include "allegro.h"

#include <math.h>

#include "config.h"
#include "globvars.h"

#include "stuff.h"
#include "cloud.h"
#include "palette.h"
#include "bullet.h"
#include "pickup.h"
#include "sound.h"
#include "actor.h"
#include "levels.h"
#include "cmds.h"
#include "display.h"

#define HIVE_MIN_RANGE 200000
#define HIVE_MAX_RANGE 500000

#define DAM_GREEN_BLAT 90
#define DAM_SMALL_GREEN_SEEKER 150
#define DAM_LARGE_GREEN_SEEKER 200
#define DAM_YELLOW_SEEKER 200
#define DAM_BLUE_BLAT 70
#define DAM_WORMS1 150
#define DAM_WORMS2 250
#define MINE_SIZE 3000

#define COLOURISE_GREEN_SEEKER1 -1
#define COLOURISE_GREEN_SEEKER2 -2
#define COLOURISE_BLUE_SEEKER -3
#define COLOURISE_YELLOW_SEEKER -4

#define TA_TURRET_TIMEOUT -17



extern FONT *small_font;

int create_enemy(int type, int subtype, int x, int y,
 int x_speed, int y_speed, int angle, int carrying_pickup, int special [10],
 int target);

int create_turret(int type, int base, int turret_counter);

void manage_enemy(int e);
int move_enemy(int e);
int detect_collision(int e);//, int things [2]);
void enemy_explodes(int e, int bang);
void register_destroyed(int e, int cause);

void destroy_enemy(int e);
int metamorphosis(int e);

int enemy_turn_towards_angle(int angle, int tangle, int turning, int forbid);
int enemy_turn_towards_xy(int x1, int y1, int x2, int y2, int angle, int turning, int forbid);
//int turn_towards_angle(int angle, int tangle, int turning);
//int turn_towards_xy(int x1, int y1, int x2, int y2, int angle, int turning);
void accelerate_enemy(int e, int angle, int impulse);
void drag_enemy(int e, float drag_amount);
int track_target(int e, int attacking, int turning, int forbid);
int track_target_any_angle(int x, int y, int target_x, int target_y, int turning, int forbid, int angle);
void standard_attack(int e, int angle);
int near_angle(int angle1, int angle2);
void enemy_vanish(int e);

void find_firebase_start_point(int location [2]);
void find_crawly_start_point(int location [2]);
void find_target_start_point(int location [2], int et);
int spawn_enemy(int type, int subtype, int carrying_pickup, int target);
void move_sun(int e);
int hurt_enemy(int e, int hurty, int source, char pulse);
void score_kill(int etype, int subtype, int p);
int check_for_targets(void);
void check_enemy_pickup_drop(int e);
void worm_new_target(int b);
void manage_turret(int e, int t, int x, int y, int rotate);
void fighter_exhaust(int e, int ax, int ay, int distance [4], int angle [4], int base_colour);
void zap_attack(int e);
int enemy_sound_pitch(int ec);
void puffer_bang(int x, int y, int rot, int number, int dist, int colours [5]);

int move_drifter_ai(int e);
int move_auto_ai(int e);
int move_stinger_ai(int e);
int move_periodic_stinger_ai(int e);
int move_turret_ai(int e);
int move_fighter_ai(int e);
int move_bomber_ai(int e);
int move_cruiser_ai(int e);
int move_leaper_ai(int e);
void run_boss3_1(int e);
void run_boss3_3(int e);

void wormer_pulsates(int e);

void colourise_bullet(int arr [4], int tc);

enum
{
ESOUND_NONE,
ESOUND_EXPLODE1,
ESOUND_EXPLODE2,
ESOUND_WOBBLE,
ESOUND_LONG_WOBBLE,
ESOUND_WARP_IN,
ESOUND_STING,
ESOUND_HARD_ZAP,
ESOUND_BOSS2,
ESOUND_BLORT,
ESOUND_STING4,
ESOUND_BLAST,
ESOUND_ZAPNOTE1,
ESOUND_ZAPNOTE2,
ESOUND_MINE,
ESOUND_SHORTZAP,
ESOUND_SHORTZAP2,
ESOUND_BLAT,
ESOUND_SUN,
ESOUND_BOSS1,
ESOUND_CROAK,
ESOUND_GREENSEEKER1,
ESOUND_GREENSEEKER2,
ESOUND_TORPEDO,
ESOUND_BOSS3_1,
ESOUND_BOSS3_2,
ESOUND_BOSS3_3,
ESOUND_PLASMA,
ESOUND_SEEKMINE,
ESOUND_ORBITAL,
ESOUND_MISSILE,
ESOUND_MINEBANG,
ESOUND_MUTATE

};

void enemy_sound(int e, int sound);
void enemy_soundf(int e, int sound, int freq);
void enemy_soundvf(int e, int sound, int vol, int freq);
void enemy_sound_fire(int e, int sound, int freq, int vol, int dist);

void init_enemies(void)
{

 int e;

 for (e = 0; e < NO_ENEMIES; e++)
 {
  enemy[e].type = ENEMY_NONE;
  enemy[e].target = TARGET_NO;
 }

}


int create_enemy(int type, int subtype, int x, int y,
 int x_speed, int y_speed, int angle, int carrying_pickup, int special [10],
 int target)
{
// REMEMBER: enemies are also created in create_turret()

   int e = 0;
   int i, j;


   for (e = 0; e < NO_ENEMIES; e++)
   {
    if (e == NO_ENEMIES - 1) return -1;
    if (enemy[e].type == ENEMY_NONE) break;
   }

   enemy[e].type = type;
   enemy[e].x = x;
   enemy[e].y = y;
   enemy[e].x_speed = x_speed;
   enemy[e].y_speed = y_speed;
   enemy[e].angle = grand(ANGLE_FULL); //angle;
   enemy[e].moving_angle = angle;
   enemy[e].armour = eclass[type].max_armour;
   enemy[e].hurt_pulse = 0;
   enemy[e].radius = eclass[type].radius;
   enemy[e].edge_radius = eclass[type].edge_radius;
//   enemy[e].shield = eclass[type].max_shield;
   enemy[e].mass = eclass[type].mass;
   enemy[e].drag_amount = 0;
   enemy[e].subtype = subtype;
   enemy[e].attacking = ATTACK_NONE;
   enemy[e].counter = 256 - counter;
   enemy[e].recycle = 50 + grand(50);
//   enemy[e].attacking = 0;
   enemy[e].just_collided = 0;
   enemy[e].carrying_pickup = carrying_pickup;
   enemy[e].target = target;
   enemy[e].burst_fire = 0;
   enemy[e].slew_dir = 0;
   enemy[e].next_impulse = eclass[type].impulse_delay;
   enemy[e].turret_main = -1;
   enemy[e].turret_index = -1;
   if (eclass[enemy[e].type].role == ROLE_TARGET || eclass[enemy[e].type].role == ROLE_EXTRA)
    enemy[e].ta_time_left = 2000;// == 60 seconds
     else
      enemy[e].ta_time_left = 99999;// == 30 seconds
//    enemy[e].ta_time_left = 150;// == 60 seconds

// REMEMBER: enemies are also created in create_turret()
   for (i = 0; i < MAX_TURRETS; i ++)
   {
    enemy[e].turret [i] = -1;
   }

   enemy[e].colours [0] = eclass[type].colour1;
   enemy[e].colours [1] = eclass[type].colour2;

   enemy[e].hurt_pulse_colour1 = enemy[e].colours [0];
   enemy[e].hurt_pulse_colour2 = enemy[e].colours [1];

   for (i = 0; i < NO_ENEMY_ATTRIBUTES; i ++)
   {
    enemy[e].attribute [i] = 0; //eclass[type].default_attributes [i];
   }

// REMEMBER: enemies are also created in create_turret()
   
   switch(enemy[e].type)
   {
    case ENEMY_DEAD_TRI1:
    case ENEMY_DEAD_TRI2:
    case ENEMY_DEAD_TRI3:
     for (i = 0; i < NO_ENEMY_ATTRIBUTES; i ++)
     {
      enemy[e].attribute [i] = special [i];
     }
     break; 

    case ENEMY_MESSENGER:
     enemy[e].burst_fire = 3 + grand(3);
     enemy[e].attribute [4] = special [4];
     break;
    case ENEMY_SPIKEY2:
    case ENEMY_SPINNER4:
    case ENEMY_BLATTER1:
    case ENEMY_BLATTER2:
    case ENEMY_BLATTER3:
    case ENEMY_BLATTER4:
    case ENEMY_BLATTER5:
     enemy[e].burst_fire = 3;
    case ENEMY_BLOATER1:
    case ENEMY_BLOATER2:
    case ENEMY_SPINNER1:
    case ENEMY_SPINNER2:
    case ENEMY_SPINNER3:
    case ENEMY_SPINNER5:
    case ENEMY_SPIKEY1:
    case ENEMY_SPIKEY3:
    case ENEMY_SPIKEY4:
    case ENEMY_SPIKEY5:
    case ENEMY_WAVER1:
    case ENEMY_WAVER2:
     if (grand(2) == 0)
      enemy[e].attribute [1] = -8 - grand(8);
       else
        enemy[e].attribute [1] = 8 + grand(8);
     enemy[e].attribute [0] = grand(ANGLE_FULL);   
     break;   
    case ENEMY_OVERBLATTER:
    case ENEMY_OVERBLATTER2:
//     enemy[e].burst_fire = 6;
     enemy[e].attribute [6] = 0;
     enemy[e].attribute [7] = 0;
     enemy[e].attribute [4] = 0;
    case ENEMY_OVERSPINNER:
    case ENEMY_OVERSPIKEY:
    case ENEMY_UNDERSPIKEY:
    case ENEMY_UNDERSPIKEY2:
    case ENEMY_UNDERSPIKEY3:
    case ENEMY_OVERSPIKEY2:
     if (grand(2) == 0)
      enemy[e].attribute [1] = -6 - grand(6);
       else
        enemy[e].attribute [1] = 6 + grand(6);
     enemy[e].attribute [0] = grand(ANGLE_FULL);   
     enemy[e].attribute [2] = grand(ANGLE_FULL);   
     break;   
    case ENEMY_PUFFER1:
    case ENEMY_PUFFER2:
    case ENEMY_PUFFER3:
    case ENEMY_PUFFER4:
     if (grand(2) == 0)
      enemy[e].attribute [1] = -1;
       else
        enemy[e].attribute [1] = 1;
     break;   
    case ENEMY_BRACKET1:
    case ENEMY_BRACKET3:
    case ENEMY_BRACKET4:
     enemy[e].burst_fire = 3 + grand(3);
     break; 
    case ENEMY_BRACKET2:
     enemy[e].burst_fire = 6 + grand(6);
     break; 
   case ENEMY_WORMER1:
   case ENEMY_WORMER2:
   case ENEMY_WORMER3:
   case ENEMY_WORMER4:
    wormer_pulsates(e);
    break;
    case ENEMY_FIGHTER2:
    case ENEMY_FIGHTER5:
     enemy[e].burst_fire = 3 + grand(3);
     enemy[e].attribute [0] = FIGHTER_MOVE;
     break;
    case ENEMY_LEAPER1:
    case ENEMY_LEAPER2:
     enemy[e].burst_fire = 3;
     enemy[e].attribute [0] = FIGHTER_MOVE;
     break;
    case ENEMY_BOMBER1:
    case ENEMY_BOMBER3:
     enemy[e].burst_fire = 3;
     enemy[e].attribute [0] = FIGHTER_MOVE;
     break;
    case ENEMY_SHIELDER1: // note see also some turrets
     enemy[e].attribute [2] = 6;
     if (grand(2) == 0)
      enemy[e].attribute [2] = -6;
     enemy[e].attribute [0] = 0;
     break;
    case ENEMY_ORBITER1:
     enemy[e].attribute [2] = 12;
     if (grand(2) == 0)
      enemy[e].attribute [2] = -12;
     break;
    case ENEMY_ORBITER2:
     enemy[e].attribute [2] = 12;
     if (grand(2) == 0)
      enemy[e].attribute [2] = -12;
     enemy[e].attribute [4] = 14 + grand(4);
     if (grand(2) == 0)
      enemy[e].attribute [4] = -14 - grand(4);
     break;
    case ENEMY_ORBITER3:
     enemy[e].attribute [2] = 11;
     if (grand(2) == 0)
      enemy[e].attribute [2] = -11;
     break;
    case ENEMY_ZAPPER1:
    case ENEMY_OVERZAPPER:
     enemy[e].attribute [0] = 0;
     enemy[e].attribute [5] = 6;
     if (grand(2) == 0)
      enemy[e].attribute [5] = -6;
     enemy[e].attribute [8] = enemy[e].x; 
     enemy[e].attribute [9] = enemy[e].y; 
     break; 
    case ENEMY_SHADOW1:
    case ENEMY_SHADOW2:
     enemy[e].attribute [0] = 35;
     enemy[e].burst_fire = 3;
     break; 
/*    case ENEMY_DRIFTER2:
     enemy[e].attribute [ATTRIB_DRIFTER_RANGE] = 125000;
     enemy[e].attribute [ATTRIB_DRIFTER_ACCEL] = 185;
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].attribute [ATTRIB_DRIFTER_X_DEST] = grand(arena.max_x - 300000) + 150000;
     enemy[e].attribute [ATTRIB_DRIFTER_Y_DEST] = grand(arena.max_y - 300000) + 150000;
     break;*/
   }
   

   switch(eclass[type].ai_type)
   {
    case AI_DRIFTER:
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].x_dest = grand(arena.max_x - enemy[e].edge_radius * 4) + enemy[e].edge_radius * 2;
     enemy[e].y_dest = grand(arena.max_y - enemy[e].edge_radius * 4) + enemy[e].edge_radius * 2;
     break;
    case AI_AUTO:
     enemy[e].angle = grand(ANGLE_FULL);
     break;
    case AI_PERIODIC_STINGER:
     break; 
    case AI_BOMBER: 
    case AI_FIGHTER:
    case AI_LEAPER:
    case AI_CRUISER:
     enemy[e].angle = grand(ANGLE_FULL);
     enemy[e].x_dest = grand(arena.max_x - enemy[e].edge_radius * 4) + enemy[e].edge_radius * 2;
     enemy[e].y_dest = grand(arena.max_y - enemy[e].edge_radius * 4) + enemy[e].edge_radius * 2;
     break;
   }
   
   if (eclass[type].turrets > 0)
   {
    for (i = 0; i < eclass[type].turrets; i ++)
    {
     switch(type)
     {
      case ENEMY_HEAD1:
       if (i == 0 || grand(10) == 0)
        j = ENEMY_HEAD1_EYE1 + grand(3);
       break;
      case ENEMY_DEFENDER1:
       if (i == 0)// || grand(10) == 0)
        j = ENEMY_DEFENDER1_TURRET1 + grand(3);
       break;
      case ENEMY_DEFENDER2:
       if (i == 0)// || grand(10) == 0)
        j = ENEMY_DEFENDER2_TURRET1 + grand(3);
       break;
      case ENEMY_DEFENDER3:
      case ENEMY_BEAMER2:
      case ENEMY_OVERZAPPER:
      case ENEMY_OVERTRIANGLER:
      case ENEMY_BEAMER1:
       if (i == 0 || grand(20) == 0)
        j = ENEMY_DEFENDER3_TURRET1 + grand(6);
       break;
      case ENEMY_OVERDISRUPTER: // 2 different turrets
       j = ENEMY_DEFENDER3_TURRET1 + grand(6);
       break;
      case ENEMY_BRACKET4:
        j = ENEMY_BRACKET4_TURRET;
       break;
      case ENEMY_CRUISER1:
        j = ENEMY_CRUISER1_TURRET;
       break;
      case ENEMY_CRUISER2:
        j = ENEMY_CRUISER2_TURRET;
       break;
      case ENEMY_CRUISER3:
        j = ENEMY_CRUISER3_TURRET;
       break;
      case ENEMY_CRUISER4:
        j = ENEMY_CRUISER4_TURRET;
       break;
      case ENEMY_MINER3:
        j = ENEMY_MINER3_TURRET;
       break;
      case ENEMY_BOSS1_1:
      case ENEMY_BOSS1_2:
      case ENEMY_BOSS1_3:
       if (i == 0 || grand(10) == 0)
        j = ENEMY_BOSS1_TURRET1 + grand(3);
       break;
      case ENEMY_BOSS2:
      case ENEMY_BOSS2_2:
      case ENEMY_BOSS2_3:
      if (i == 0 || grand(12) == 0)
        j = ENEMY_BOSS2_TURRET1 + grand(4);
//    j = ENEMY_BOSS2_TURRET1;
//          j = ENEMY_BOSS2_TURRET1 + grand(4);
       break;
      case ENEMY_BOSS3_1:
      case ENEMY_BOSS3_2:
      case ENEMY_BOSS3_3:
      if (i == 0 || grand(12) == 0)
        j = ENEMY_BOSS3_TURRET1 + grand(3);
       break; 
     }
     enemy[e].turret [i] = create_turret(j, e, i);
    }   
   }
/*    else
    {
     for (i = 0; i < MAX_TURRETS; i ++)
     {
      enemy[e].turret [i] = -1;
     }
    }*/
   
   return e;

}

int create_turret(int type, int base, int turret_counter)
{

   int e = 0;

   for (e = 0; e < NO_ENEMIES; e++)
   {
    if (e == NO_ENEMIES - 1) return -1;
    if (enemy[e].type == ENEMY_NONE) break;
   }

   enemy[e].type = type;
   enemy[e].x = enemy[base].x;
   enemy[e].y = enemy[base].y;
   enemy[e].x_speed = enemy[base].x_speed;
   enemy[e].y_speed = enemy[base].y_speed;
   enemy[e].angle = enemy[base].angle;
   enemy[e].moving_angle = enemy[base].angle;
   enemy[e].armour = eclass[type].max_armour;
   enemy[e].hurt_pulse = 0;
   enemy[e].radius = eclass[type].radius;
   enemy[e].edge_radius = eclass[type].edge_radius;
//   enemy[e].shield = eclass[type].max_shield;
   enemy[e].mass = eclass[type].mass;
   enemy[e].drag_amount = 0;
   enemy[e].subtype = 0;
   enemy[e].attacking = ATTACK_NONE;
   enemy[e].counter = 256 - counter + turret_counter * 10;
   enemy[e].recycle = enemy[e].counter % 50 + 50;
//   enemy[e].attacking = 0;
   enemy[e].just_collided = 0;
   enemy[e].carrying_pickup = 0;
   enemy[e].target = TARGET_NO;
   enemy[e].burst_fire = 0;
   enemy[e].slew_dir = 0;
   enemy[e].next_impulse = 0;//eclass[type].impulse_delay;
   enemy[e].ta_time_left = 99999;

   int i;
   
   for (i = 0; i < MAX_TURRETS; i ++)
   {
    enemy[e].turret [i] = -1;
   }

//   int i;

//   for (i = 0; i < NO_ENEMY_ATTRIBUTES; i ++)
//   {
//    enemy[e].attribute [i] = eclass[type].default_attributes [i];
//   }

   enemy[e].colours [0] = eclass[type].colour1;
   enemy[e].colours [1] = eclass[type].colour2;
   
   enemy[e].turret_main = base;
   enemy[e].turret_index = turret_counter;
//   enemy[e].attribute [ATTRIB_FI4TURRET_MAIN] = base;
//   enemy[e].attribute [ATTRIB_FI4TURRET_MAIN_DEAD] = 0;

   enemy[e].hurt_pulse_colour1 = enemy[e].colours [0];
   enemy[e].hurt_pulse_colour2 = enemy[e].colours [1];

   enemy[e].angle = grand(ANGLE_FULL);

   switch(enemy[e].type)
   {
    case ENEMY_BOSS2_TURRET2:
    case ENEMY_BOSS3_TURRET2:
     enemy[e].burst_fire = 6 + grand(6);
     break; 
    case ENEMY_MINER3_TURRET:
     enemy[e].burst_fire = 3 + grand(3);
     break;
    case ENEMY_CRUISER1_TURRET:
     enemy[e].burst_fire = 3 + grand(3);
     break; 
    case ENEMY_DEFENDER1_TURRET3:
    case ENEMY_DEFENDER3_TURRET5:
     enemy[e].attribute [0] = 6;
     if (grand(2) == 0)
      enemy[e].attribute [0] = -6;
     break;
    case ENEMY_DEFENDER3_TURRET1:
     enemy[e].attribute [2] = 6;
     if ((base + turret_counter) % 2 == 0)
      enemy[e].attribute [2] = -6;
     enemy[e].attribute [0] = 0;
     break;
    case ENEMY_DEFENDER3_TURRET2:
     enemy[e].attribute [2] = 7;
     if (grand(2) == 0)
      enemy[e].attribute [2] = -7;
     break;
    case ENEMY_DEFENDER3_TURRET3:
     enemy[e].attribute [2] = 5;
     if (grand(2) == 0)
      enemy[e].attribute [2] = -5;
     break;

   }

   return e;

}



void run_enemies(void)
{

 int e;

 for (e = 0; e < NO_ENEMIES; e++)
 {
  if (enemy[e].type != ENEMY_NONE)
   manage_enemy(e);
 }

}


void manage_enemy(int e)
{

   if (enemy[e].hurt_pulse > 0)
    enemy[e].hurt_pulse --;
   if (enemy[e].recycle > 0)
    enemy[e].recycle --;
   if (enemy[e].just_collided > 0)
    enemy[e].just_collided --;

   enemy[e].counter ++; // assumed: this wraps at 256 (unsigned char)

   if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
   {
    enemy[e].ta_time_left --;
    if (enemy[e].ta_time_left <= 0)
    {
     enemy_vanish(e);
     return;
    } 
   }

//   char need_move_enemy = 1;

 switch(eclass[enemy[e].type].ai_type)
 {
  case AI_DRIFTER:
   move_drifter_ai(e);
   break;
  case AI_AUTO:
   move_auto_ai(e);
   break;
  case AI_STINGER:
   move_stinger_ai(e);
   break;
  case AI_PERIODIC_STINGER:
   move_periodic_stinger_ai(e);
   break;
  case AI_TURRET:
   move_turret_ai(e);
   break; 
  case AI_FIGHTER:
   move_fighter_ai(e);
   break; 
  case AI_LEAPER:
   move_leaper_ai(e);
   break; 
  case AI_BOMBER:
   move_bomber_ai(e); 
   break;
  case AI_CRUISER:
   move_cruiser_ai(e); 
   break;
 }

 if (enemy[e].type != ENEMY_NONE)// && need_move_enemy == 1)
   move_enemy(e);

}


int hurt_enemy(int e, int hurty, int source, char pulse)
{

if (e <= -1 || enemy[e].type <= ENEMY_NONE || enemy[e].type >= NO_ENEMY_TYPES)
{
#ifdef SANITY_CHECK
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "hurt e %i, s %i", e, source);
     if (e != -1)
      textprintf_centre_ex(screen, small_font, 400, 260, COLOUR_WHITE, -1, "hurt t %i", enemy[e].type);
     rest(2000);
#endif
     return -1;
//  exit(type);
    } 

 if (enemy[e].type == ENEMY_NONE)
  return 0;

 if (hurty >= enemy[e].armour)
 {
//  switch(enemy[e].type)
//  {
//  }
  register_destroyed(e, source);
  enemy_explodes(e, 1);
  // can also happen in enemy_vanish, just below
  return 0;
 }

 enemy[e].armour -= hurty;
 if (pulse > enemy[e].hurt_pulse)
  enemy[e].hurt_pulse = pulse;
  
 if (enemy[e].type == ENEMY_CURVE2 || enemy[e].type == ENEMY_CURVE3)
 {
  if (enemy[e].armour <= 500)
   enemy[e].attribute [0] = source;
 }
  
 return 1;

}



void enemy_vanish(int e)
{


#ifdef SANITY_CHECK
if (e <= -1 || enemy[e].type <= ENEMY_NONE || enemy[e].type >= NO_ENEMY_TYPES)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "vanish e %i", e);
     if (e != -1)
      textprintf_centre_ex(screen, small_font, 400, 260, COLOUR_WHITE, -1, "vanish t %i", enemy[e].type);
     rest(2000);
     return;
//  exit(type);
    } 
#endif

 int passing_colours [5] = {GC_GREEN8, GC_GREEN8, GC_GREEN8, GC_GREEN8, GC_GREEN8};
 int passing_colours2 [5] = {TRANS_DGREEN, TRANS_LGREEN, TRANS_YELLOW, TRANS_YELLOW, TRANS_YELLOW};

 int cloud_size = 200 + crandom(250) + eclass[enemy[e].type].radius / 10;

     switch(enemy[e].type)
     {
      default:
      passing_colours2 [0] = TRANS_DRED;
      passing_colours2 [1] = TRANS_LRED;
      passing_colours2 [2] = TRANS_YELLOW;
      passing_colours [0] = GC_RED8;
      break;
     }

  
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       enemy[e].x,
       enemy[e].y,
       0, 0,
       0, 0,
       500, -150 - eclass[enemy[e].type].radius / 100, 10, 0, 0, 0, passing_colours);
       
      place_explosion(enemy[e].x, enemy[e].y, 0,0,
       cloud_size, passing_colours2);

  register_destroyed(e, -2);
  enemy_explodes(e, 0);
// see also hurt_enemy.

//  if (standard_sound == 1)
//     enemy_soundf(e, ESOUND_WARP_IN, 800 + grand(800) - eclass[enemy[e].type].mass);
      enemy_soundf(e, NWAV_PHASE, enemy_sound_pitch(enemy[e].type) + grand(250)); //800 + grand(200) - enemy_pitch(enemy[k].type));

}



void enemy_explodes(int e, int bang)
{
 int j, k, l;
// int xs = enemy[e].x_speed;
// int ys = enemy[e].y_speed;

 if (enemy[e].type == ENEMY_NONE)
  return;
  
 if (eclass[enemy[e].type].role == ROLE_TURRET
     && (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
     && enemy[e].ta_time_left == TA_TURRET_TIMEOUT)
      bang = 0;

 int passing_colours [5];
 int bullet_colours [4];
// int special [10];

// int standard_sound = NWAV_BURST;

 if (bang)
 {
 shake_all_screens(enemy[e].mass / 50);

 switch(enemy[e].type)
 {
  case ENEMY_SPIKEY1:
  case ENEMY_SPIKEY2:
  case ENEMY_SPIKEY3:
  case ENEMY_SPIKEY4:
  case ENEMY_SPIKEY5:
  case ENEMY_SPINNER1:
  case ENEMY_SPINNER2:
  case ENEMY_SPINNER3:
  case ENEMY_SPINNER4:
  case ENEMY_SPINNER5:
  case ENEMY_BLATTER1:
  case ENEMY_BLATTER2:
  case ENEMY_BLATTER3:
  case ENEMY_BLATTER4:
  case ENEMY_BLATTER5:
  case ENEMY_GUARDIAN1:
  case ENEMY_GUARDIAN2:
  case ENEMY_GUARDIAN3:
//  case ENEMY_GUARDIAN4:
  case ENEMY_GUARDIAN5:
  case ENEMY_CIRCLER1:
  case ENEMY_OVERTRIANGLER_TURRET:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_GUARDIAN4:
     passing_colours[0] = TRANS_ORANGE;
     passing_colours[1] = TRANS_YELLOW;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1600 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_WAVER1:
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   for (j = 0; j < 4; j ++)
   {
    place_explosion(enemy[e].x + xpart(enemy[e].attribute [0] + j * ANGLE_QUARTER, 43 * GRAIN),
     enemy[e].y + ypart(enemy[e].attribute [0] + j * ANGLE_QUARTER, 43 * GRAIN), 0,0,
     1200 + crandom(250), passing_colours);
   }
   break;
  case ENEMY_WAVER2:
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   for (j = 0; j < 3; j ++)
   {
    place_explosion(enemy[e].x + xpart(enemy[e].attribute [0] + j * ANGLE_FULL / 3, 51 * GRAIN),
     enemy[e].y + ypart(enemy[e].attribute [0] + j * ANGLE_FULL / 3, 51 * GRAIN), 0,0,
     1200 + crandom(250), passing_colours);
   }
   break;
  case ENEMY_DEAD_TRI1: 
  case ENEMY_DEAD_TRI2: 
  case ENEMY_DEAD_TRI3: 
   passing_colours [0] = enemy[e].attribute [2];
   passing_colours [1] = enemy[e].attribute [3];
   passing_colours [2] = enemy[e].attribute [4];
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1800 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   break;
  case ENEMY_TRIANGLER1:
     passing_colours [0] = TRANS_DRED;
     passing_colours [1] = TRANS_LRED;
     passing_colours [2] = TRANS_YELLOW;
/*      create_cloud(CLOUD_TRI1,
       enemy[e].x, enemy[e].y, 0, 0, enemy[e].x_speed, enemy[e].y_speed,
       20 + grand(50), 1, 0, 0, 25000, 0, passing_colours);*/
    enemy[e].attribute [0] = 20 + grand(80);
    enemy[e].attribute [1] = 25000;
    enemy[e].attribute [2] = TRANS_DRED;
    enemy[e].attribute [3] = TRANS_LRED;
    enemy[e].attribute [4] = TRANS_YELLOW;
    create_enemy(ENEMY_DEAD_TRI1, 0, enemy[e].x, enemy[e].y,
      enemy[e].x_speed, enemy[e].y_speed, 0, 0, enemy[e].attribute,
      TARGET_NO);      
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    900 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   break;
  case ENEMY_TRIANGLER2:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_LBLUE;
     passing_colours[2] = TRANS_WHITE;
/*      create_cloud(CLOUD_TRI2,
       enemy[e].x, enemy[e].y, 0, 0, enemy[e].x_speed, enemy[e].y_speed,
       20 + grand(50), 1, 0, 0, 20000, 0, passing_colours);*/
    enemy[e].attribute [0] = 20 + grand(80);
    enemy[e].attribute [1] = 20000;
    enemy[e].attribute [2] = TRANS_DBLUE;
    enemy[e].attribute [3] = TRANS_LBLUE;
    enemy[e].attribute [4] = TRANS_WHITE;
    create_enemy(ENEMY_DEAD_TRI2, 0, enemy[e].x, enemy[e].y,
      enemy[e].x_speed, enemy[e].y_speed, 0, 0, enemy[e].attribute,
      TARGET_NO);      
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1000 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   break;
  case ENEMY_TRIANGLER3:
     passing_colours[0] = TRANS_ORANGE;
     passing_colours[1] = TRANS_YELLOW;
     passing_colours[2] = TRANS_WHITE;
/*      create_cloud(CLOUD_TRI3,
       enemy[e].x, enemy[e].y, 0, 0, enemy[e].x_speed, enemy[e].y_speed,
       20 + grand(50), 1, 0, 0, 18000, 0, passing_colours);*/
    enemy[e].attribute [0] = 20 + grand(80);
    enemy[e].attribute [1] = 18000;
    enemy[e].attribute [2] = TRANS_ORANGE;
    enemy[e].attribute [3] = TRANS_YELLOW;
    enemy[e].attribute [4] = TRANS_WHITE;
    create_enemy(ENEMY_DEAD_TRI3, 0, enemy[e].x, enemy[e].y,
      enemy[e].x_speed, enemy[e].y_speed, 0, 0, enemy[e].attribute,
      TARGET_NO);      
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    900 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   break;
  case ENEMY_CURVE1:
  case ENEMY_CURVE2:
  case ENEMY_MULTI1:
  case ENEMY_MULTI2:
  case ENEMY_MULTI3:
  case ENEMY_ORBITER1:
  case ENEMY_ORBITER2:
  case ENEMY_ORBITER3:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   break;
  case ENEMY_DISRUPTER1:
  case ENEMY_DISRUPTER3:
     passing_colours[0] = TRANS_LRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1200 + crandom(250), passing_colours);
   place_explosion(enemy[e].x - 30000, enemy[e].y, 0,0,
    300 + crandom(150), passing_colours);
   place_explosion(enemy[e].x + 30000, enemy[e].y, 0,0,
    300 + crandom(150), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   break;
  case ENEMY_DISRUPTER2:
     passing_colours[0] = TRANS_LRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1200 + crandom(250), passing_colours);
   place_explosion(enemy[e].x, enemy[e].y - 30000, 0,0,
    300 + crandom(150), passing_colours);
   place_explosion(enemy[e].x, enemy[e].y + 30000, 0,0,
    300 + crandom(150), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   break;
  case ENEMY_ATTRACTOR:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
   simple_shockwave(TRANS_PURPLE, enemy[e].x, enemy[e].y, 0, 0, 500 + crandom(50), 15);
   blast(enemy[e].x, enemy[e].y, 60000, 0, 5000, OWNER_ENEMY);
   line_blast(enemy[e].x, enemy[e].y, BULLET_ATTRACTOR_LINE, e);
   break;

  case ENEMY_PULSER1:
  case ENEMY_PULSER2:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   k = (enemy[e].attribute [0] / 4) % 8;
   l = CLOUD_PULSER1_V;
   if (enemy[e].type == ENEMY_PULSER2)
    l = CLOUD_PULSER2_V;
   if (k >= 4)
    k = 8 - k;
   for (j = 0; j < 2; j ++)
   {
    create_cloud(l, 
     enemy[e].x - (11 + (enemy[e].type == ENEMY_PULSER2)) * GRAIN, 
//          enemy[e].x,
     enemy[e].y + (k + (k * (j == 0) * -1) - (35 * j) + (8 * j == 0)) * GRAIN, 
     enemy[e].x, enemy[e].y,
     enemy[e].x_speed,
     enemy[e].y_speed - (1000 + grand(2000)) * (j - (j == 0)),
     3 + grand(10), 1,
     0, 0,
     j, 0, passing_colours);
    }
   l = CLOUD_PULSER1_H;
   if (enemy[e].type == ENEMY_PULSER2)
    l = CLOUD_PULSER2_H;
   for (j = 0; j < 2; j ++)
   {
    create_cloud(l, 
     enemy[e].x + (k + (k * (j == 0) * -1) - (35 * j) + (8 * j == 0)) * GRAIN, 
     enemy[e].y - (11 + (enemy[e].type == ENEMY_PULSER2)) * GRAIN, 
//     enemy[e].y,
     enemy[e].x, enemy[e].y,
     enemy[e].x_speed - (1000 + grand(2000)) * (j - (j == 0)),
     enemy[e].y_speed,
     3 + grand(10), 1,
     0, 0,
     j, 0, passing_colours);
    } 
   break;
  case ENEMY_FORKER1:
  case ENEMY_FORKER2:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_PURPLE;
     passing_colours[2] = TRANS_ORANGE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_SHADOW1:
  case ENEMY_SHADOW2:
  case ENEMY_CURVE3:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_LBLUE;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    2000 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_SHIELDER1:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_PURPLE;
     passing_colours[2] = TRANS_ORANGE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   for (j = 0; j < 3; j ++)
   {
    create_cloud(CLOUD_SQUAREY, enemy[e].x, enemy[e].y, 
     enemy[e].x - xpart(enemy[e].attribute [1], 30), 
     enemy[e].y - ypart(enemy[e].attribute [1], 30), 
     enemy[e].x_speed + xpart(enemy[e].attribute [1] + ((j * ANGLE_FULL / 3) & 1023), 500 + grand(3000)), //grand(5000) - grand(5000), 
     enemy[e].y_speed + ypart(enemy[e].attribute [1] + ((j * ANGLE_FULL / 3) & 1023), 500 + grand(3000)),//grand(5000) - grand(5000), 
     3 + grand(50), 1,
     0, enemy[e].attribute [1] + ((j * ANGLE_FULL / 3) & 1023),
     enemy[e].attribute [2], 0, passing_colours);
    } 
   break;
  case ENEMY_ZAPPER1:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_LBLUE;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1200 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   for (j = 0; j < 8; j ++)
   {
    k = enemy[e].x + xpart(enemy[e].attribute [4] + (j * ANGLE_1_EIGHTH), 35 * GRAIN);
    l = enemy[e].y + ypart(enemy[e].attribute [4] + (j * ANGLE_1_EIGHTH), 35 * GRAIN);
    place_explosion(k, l, 0,0,
     100 + crandom(50), passing_colours);
    place_burstlet_burst(k, l, 0, 0, 2 + grand(2), 2, 6, 1200, 1800,
     4, passing_colours);
   } 
   break;
  case ENEMY_OVERZAPPER:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_LBLUE;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    3000 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2300, 2800,
    4, passing_colours);
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   for (j = 0; j < 8; j ++)
   {
    k = enemy[e].x + xpart(enemy[e].attribute [4] + (j * ANGLE_1_EIGHTH), 60 * GRAIN);
    l = enemy[e].y + ypart(enemy[e].attribute [4] + (j * ANGLE_1_EIGHTH), 60 * GRAIN);
    place_explosion(k, l, 0,0,
     250 + crandom(50), passing_colours);
    place_burstlet_burst(k, l, 0, 0, 2 + grand(2), 2, 6, 2000, 2500,
     4, passing_colours);
   } 
   break;
  case ENEMY_PUFFER1:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1000 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   puffer_bang(enemy[e].x, enemy[e].y, enemy[e].attribute [2], 3, 17, passing_colours);
/*   place_explosion_no_light(enemy[e].x + xpart((enemy[e].attribute [2] * 8) % 1024, 16 * GRAIN), 
                   enemy[e].y + ypart((enemy[e].attribute [2] * 8) % 1024, 16 * GRAIN), 
                   0 + xpart((enemy[e].attribute [2] * 8) % 1024, GRAIN / 2),
                   0 + ypart((enemy[e].attribute [2] * 8) % 1024, GRAIN / 2),
                   300 + crandom(250), passing_colours);
   place_explosion_no_light(enemy[e].x + xpart(((enemy[e].attribute [2] + 85) * 8) % 1024, 16 * GRAIN), 
                   enemy[e].y + ypart(((enemy[e].attribute [2] + 85) * 8) % 1024, 16 * GRAIN), 
                   0 + xpart(((enemy[e].attribute [2] + 85) * 8) % 1024, GRAIN / 2),
                   0 + ypart(((enemy[e].attribute [2] + 85) * 8) % 1024, GRAIN / 2),
                   300 + crandom(250), passing_colours);
   place_explosion_no_light(enemy[e].x + xpart(((enemy[e].attribute [2] - 85) * 8) % 1024, 16 * GRAIN), 
                   enemy[e].y + ypart(((enemy[e].attribute [2] - 85) * 8) % 1024, 16 * GRAIN), 
                   0 + xpart(((enemy[e].attribute [2] - 85) * 8) % 1024, GRAIN / 2),
                   0 + ypart(((enemy[e].attribute [2] - 85) * 8) % 1024, GRAIN / 2),
                   300 + crandom(250), passing_colours);*/
//   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
//    4, passing_colours);
   break;
  case ENEMY_PUFFER3:
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1100 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   puffer_bang(enemy[e].x, enemy[e].y, enemy[e].attribute [2], 3, 19, passing_colours);
   break;
  case ENEMY_PUFFER4:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1600 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_LBLUE;
     passing_colours[2] = TRANS_WHITE;
   puffer_bang(enemy[e].x, enemy[e].y, enemy[e].attribute [2], 5, 25, passing_colours);
   break;
  case ENEMY_PUFFER2:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1300 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
     passing_colours[0] = TRANS_ORANGE;
     passing_colours[1] = TRANS_YELLOW;
     passing_colours[2] = TRANS_WHITE;
   puffer_bang(enemy[e].x, enemy[e].y, enemy[e].attribute [2], 4, 20, passing_colours);
/*   place_explosion_no_light(enemy[e].x + xpart((enemy[e].attribute [2] * 8) % 1024, 18 * GRAIN), 
                   enemy[e].y + ypart((enemy[e].attribute [2] * 8) % 1024, 18 * GRAIN), 
                   0 + xpart((enemy[e].attribute [2] * 8) % 1024, GRAIN / 2),
                   0 + ypart((enemy[e].attribute [2] * 8) % 1024, GRAIN / 2),
                   300 + crandom(250), passing_colours);
   place_explosion_no_light(enemy[e].x + xpart(((enemy[e].attribute [2] + 32) * 8) % 1024, 18 * GRAIN), 
                   enemy[e].y + ypart(((enemy[e].attribute [2] + 64) * 8) % 1024, 18 * GRAIN), 
                   0 + xpart(((enemy[e].attribute [2] + 64) * 8) % 1024, GRAIN / 2),
                   0 + ypart(((enemy[e].attribute [2] + 64) * 8) % 1024, GRAIN / 2),
                   300 + crandom(250), passing_colours);
   place_explosion_no_light(enemy[e].x + xpart(((enemy[e].attribute [2] - 32) * 18) % 1024, 8 * GRAIN), 
                   enemy[e].y + ypart(((enemy[e].attribute [2] - 64) * 18) % 1024, 8 * GRAIN), 
                   0 + xpart(((enemy[e].attribute [2] - 64) * 8) % 1024, GRAIN / 2),
                   0 + ypart(((enemy[e].attribute [2] - 64) * 8) % 1024, GRAIN / 2),
                   300 + crandom(250), passing_colours);
   place_explosion_no_light(enemy[e].x + xpart(((enemy[e].attribute [2] + 64) * 8) % 1024, 18 * GRAIN), 
                   enemy[e].y + ypart(((enemy[e].attribute [2] + 128) * 8) % 1024, 18 * GRAIN), 
                   0 + xpart(((enemy[e].attribute [2] + 128) * 8) % 1024, GRAIN / 2),
                   0 + ypart(((enemy[e].attribute [2] + 128) * 8) % 1024, GRAIN / 2),
                   300 + crandom(250), passing_colours);
//   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
//    4, passing_colours);*/
   break;
  case ENEMY_BRACKET1:
  case ENEMY_BRACKET2:
     passing_colours[0] = TRANS_LRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1000 + crandom(250), passing_colours);
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_BRACKET3:
  case ENEMY_BRACKET4:
  case ENEMY_BRACKET5:
     passing_colours[0] = TRANS_LRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1900 + crandom(250), passing_colours);
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_WORMER1:
  case ENEMY_WORMER2:
  case ENEMY_WORMER3:
  case ENEMY_WORMER4:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1600 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2500, 2500,
    4, passing_colours);
   break;
  case ENEMY_HEAD1:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1800 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_OVERTRIANGLER:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    3500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0, //enemy[e].x_speed + , enemy[e].y_speed,
    60,2,0, 0, 900, 4, passing_colours);
   break;
  case ENEMY_DEFENDER1:
  case ENEMY_DEFENDER3:
  case ENEMY_BEAMER1:
  case ENEMY_BEAMER2:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    3500 + crandom(250), passing_colours);
   place_explosion(enemy[e].x - 60000, enemy[e].y, 0,0,
    2500 + crandom(250), passing_colours);
   place_explosion(enemy[e].x + 60000, enemy[e].y, 0,0,
    2500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x + 40 * GRAIN, enemy[e].y,
    0, 0, 2 * GRAIN, 0, //enemy[e].x_speed + , enemy[e].y_speed,
    50,2,0, 0, 600, 4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x - 40 * GRAIN, enemy[e].y,
    0, 0, -2 * GRAIN, 0, //enemy[e].x_speed + , enemy[e].y_speed,
    50,2,0, 0, 600, 4, passing_colours);
   break;
  case ENEMY_DEFENDER2:
     passing_colours[0] = TRANS_ORANGE;
     passing_colours[1] = TRANS_YELLOW;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    3500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y + 40 * GRAIN,
    0, 0, 0, 2 * GRAIN, //enemy[e].x_speed + , enemy[e].y_speed,
    50,2,0, 0, 600, 4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y - 40 * GRAIN,
    0, 0, 0, -2 * GRAIN, //enemy[e].x_speed + , enemy[e].y_speed,
    50,2,0, 0, 600, 4, passing_colours);
   break;
  case ENEMY_OVERDISRUPTER:
     passing_colours[0] = TRANS_DGREY;
     passing_colours[1] = TRANS_DBLUE;
     passing_colours[2] = TRANS_LBLUE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    3500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y + 40 * GRAIN,
    0, 0, 0, 2 * GRAIN, //enemy[e].x_speed + , enemy[e].y_speed,
    50,2,0, 0, 600, 4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y - 40 * GRAIN,
    0, 0, 0, -2 * GRAIN, //enemy[e].x_speed + , enemy[e].y_speed,
    50,2,0, 0, 600, 4, passing_colours);
   break;
  case ENEMY_OVERBLATTER:
  case ENEMY_OVERBLATTER2:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    4000 + crandom(550), passing_colours);
//   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 8000, 8500,
//    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    60,2,0, 0, 1000, 4, passing_colours);
/*   for (j = 0; j < 4; j ++)
   {
    place_explosion_no_light(enemy[e].x + xpart((enemy[e].attribute [2] + k * ANGLE_QUARTER) % ANGLE_FULL, 50 * GRAIN), 
                   enemy[e].y + ypart((enemy[e].attribute [2] + k * ANGLE_QUARTER) % ANGLE_FULL, 50 * GRAIN), 
                   0, 0, 400 + crandom(250), passing_colours);
   }*/
//   draw_blatter(bmp, x, y, 5, 50, enemy[dr].attribute [2], 15, GC_YELLOW2, GC_YELLOW5);
   break;
  case ENEMY_OVERSPINNER:
  case ENEMY_UNDERSPIKEY:
  case ENEMY_UNDERSPIKEY2:
  case ENEMY_UNDERSPIKEY3:
  case ENEMY_OVERSPIKEY2:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
  case ENEMY_OVERSPIKEY:
   if (enemy[e].type == ENEMY_OVERSPIKEY)
   {
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   } 
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    5500 + crandom(550), passing_colours);
//   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 8000, 8500,
//    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    120,2,0, 0, 1300, 4, passing_colours);
   break;
  case ENEMY_BOSS1_1:
  case ENEMY_BOSS1_2:
  case ENEMY_BOSS1_3:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_ORANGE;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    8500 + crandom(550), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 8000, 8500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    360,2,0, 0, 1600, 4, passing_colours);
   break;
  case ENEMY_BOSS2:
  case ENEMY_BOSS2_2:
  case ENEMY_BOSS2_3:
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    8500 + crandom(550), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 8000, 8500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    360,2,0, 0, 1600, 4, passing_colours);
   break;
  case ENEMY_BOSS3_1:
  case ENEMY_BOSS3_2:
  case ENEMY_BOSS3_3:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_LBLUE;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    8500 + crandom(550), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 8000, 8500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, 0, 0,
    360,2,0, 0, 1600, 4, passing_colours);
   break;
  case ENEMY_HEAD1_EYE1: 
  case ENEMY_HEAD1_EYE2: 
  case ENEMY_HEAD1_EYE3:
  case ENEMY_BOSS1_TURRET1:
  case ENEMY_BOSS1_TURRET2:
  case ENEMY_BOSS1_TURRET3:
  case ENEMY_BRACKET4_TURRET:
  case ENEMY_CRUISER1_TURRET:
  case ENEMY_CRUISER2_TURRET:
  case ENEMY_CRUISER3_TURRET:
  case ENEMY_CRUISER4_TURRET:
  case ENEMY_MINER3_TURRET:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_DEFENDER1_TURRET1:
  case ENEMY_DEFENDER1_TURRET2:
  case ENEMY_DEFENDER1_TURRET3:
  case ENEMY_DEFENDER2_TURRET1:
  case ENEMY_DEFENDER2_TURRET2:
  case ENEMY_DEFENDER2_TURRET3:
  case ENEMY_DEFENDER3_TURRET1:
  case ENEMY_DEFENDER3_TURRET2:
  case ENEMY_DEFENDER3_TURRET3:
  case ENEMY_DEFENDER3_TURRET4:
  case ENEMY_DEFENDER3_TURRET5:
  case ENEMY_DEFENDER3_TURRET6:
  case ENEMY_BOSS2_TURRET1:
  case ENEMY_BOSS2_TURRET2:
  case ENEMY_BOSS2_TURRET3:
  case ENEMY_BOSS2_TURRET4:
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    500 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_BOSS3_TURRET1:
  case ENEMY_BOSS3_TURRET2:
  case ENEMY_BOSS3_TURRET3:
     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_LBLUE;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    700 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_LEAPER1: 
  case ENEMY_LEAPER2: 
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    800 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, 0, 0, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_FIGHTER1: 
  case ENEMY_FIGHTER2: 
  case ENEMY_FIGHTER3: 
  case ENEMY_FIGHTER5: 
  case ENEMY_MESSENGER: 
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_FIGHTER4: 
     passing_colours[0] = TRANS_DGREEN;
     passing_colours[1] = TRANS_LGREEN;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   break;
  case ENEMY_BOMBER1: 
  case ENEMY_BOMBER2:
  case ENEMY_BOMBER3:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, enemy[e].x_speed, enemy[e].y_speed,
    50 + grand(15) * 2,2,0, 0, 250, 4, passing_colours);
// remember: tickrate must be a factor of 16, and timeout must be even    
   break;
  case ENEMY_CRUISER1:
  case ENEMY_CRUISER2:
  case ENEMY_CRUISER3:
  case ENEMY_CRUISER4:
     passing_colours[0] = TRANS_DRED;
     passing_colours[1] = TRANS_LRED;
     passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    1400 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);
   create_cloud(CLOUD_SPAWNER,
    enemy[e].x, enemy[e].y,
    0, 0, enemy[e].x_speed, enemy[e].y_speed,
    50 + grand(15) * 2,2,0, 0, 250, 4, passing_colours);
// remember: tickrate must be a factor of 16, and timeout must be even    
   break;
  case ENEMY_MINER1:
  case ENEMY_MINEFIELDER1:
  case ENEMY_MINER2:
  case ENEMY_MINER3:
   k = 5 + grand(7);
   l = BULLET_MINE1 + (enemy[e].type == ENEMY_MINER2) + (2 * (enemy[e].type == ENEMY_MINER3));
   for (j = 0; j < k; j ++)
   {
    create_bullet(l, enemy[e].x, enemy[e].y,
     enemy[e].x_speed + grand(12001) - 6000, enemy[e].y_speed + grand(12001) - 6000,
     OWNER_ENEMY, 1, 300 + grand(100), 20, 0,
     0, MINE_SIZE, bullet_colours, 1,0,0,0,0,0);
   }
   passing_colours[0] = TRANS_DRED;
   passing_colours[1] = TRANS_LRED;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
   break;
  case ENEMY_BLOATER1:
   k = 4 + grand(3);
   bullet_colours [0] = TRANS_LGREEN;
   bullet_colours [1] = TRANS_LGREEN;
   bullet_colours [2] = TRANS_LGREEN;
   bullet_colours [3] = TRANS_DGREEN;
   for (j = 0; j < k; j ++)
   {
    create_bullet(BULLET_SEEKER2, enemy[e].x + grand(12001) - 6000, enemy[e].y + grand(12001) - 6000,
     enemy[e].x_speed + grand(6001) - 3000, enemy[e].y_speed + grand(6001) - 3000,
     OWNER_ENEMY, DAM_LARGE_GREEN_SEEKER, 220 + grand(50), 120, grand(ANGLE_FULL),
     0, 0, bullet_colours, 1,closest_target(enemy[e].x, enemy[e].y),38,48,180,7001);
   }
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
   break;
  case ENEMY_BLOATER2:
   k = 4 + grand(3);
   bullet_colours [0] = TRANS_YELLOW;
   bullet_colours [1] = TRANS_ORANGE;
   bullet_colours [2] = TRANS_LRED;
   bullet_colours [3] = TRANS_DRED;
   for (j = 0; j < k; j ++)
   {
    create_bullet(BULLET_SEEKER2, enemy[e].x + grand(12001) - 6000, enemy[e].y + grand(12001) - 6000,
     enemy[e].x_speed + grand(12001) - 6000, enemy[e].y_speed + grand(12001) - 6000,
     OWNER_ENEMY, DAM_LARGE_GREEN_SEEKER + 50, 220 + grand(50), 120, j * ANGLE_FULL / 6, //and(ANGLE_FULL),
     0, 0, bullet_colours, 1,closest_target(enemy[e].x, enemy[e].y),48,48,200,7001);
   }
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);
/*   k = 5 + grand(4);
   bullet_colours [0] = COLOUR_RED5;
   bullet_colours [1] = COLOUR_ORANGE6;
   bullet_colours [2] = COLOUR_YELLOW6;
   bullet_colours [3] = COLOUR_YELLOW8;
   for (j = 0; j < k; j ++)
   {
    create_bullet(BULLET_EVIL_WORM, enemy[e].x + grand(12001) - 6000, enemy[e].y + grand(12001) - 6000,
//     enemy[e].x_speed + grand(6001) - 3000, enemy[e].y_speed + grand(6001) - 3000,
     enemy[e].x_speed, enemy[e].y_speed,
     OWNER_ENEMY, DAM_WORMS2, 180, 80, grand(ANGLE_FULL),
     0, 0, bullet_colours, 1,600,60,closest_target(enemy[e].x, enemy[e].y),8,0);
   }
   passing_colours[0] = TRANS_DGREEN;
   passing_colours[1] = TRANS_LGREEN;
   passing_colours[2] = TRANS_YELLOW;
   place_explosion(enemy[e].x, enemy[e].y, 0, 0,
    1200 + crandom(250), passing_colours);*/
   break;

 }
// if (standard_sound != ESOUND_NONE)
 if (eclass[enemy[e].type].role != ROLE_BOSS)
  enemy_soundf(e, NWAV_BANG, enemy_sound_pitch(enemy[e].type));
   else
    enemy_soundf(e, NWAV_BIGBANG, enemy_sound_pitch(enemy[e].type));

 check_enemy_pickup_drop(e);

 } // end if bang

 destroy_enemy(e);

}

void destroy_enemy(int e)
{
 enemy[e].type = ENEMY_NONE;
// calculate_ambience();
}

void puffer_bang(int x, int y, int rot, int number, int dist, int colours [5])
{

 int i;
 int each_rot = ANGLE_FULL / number;
 
 dist *= GRAIN; 
 
 for (i = 0; i < number; i ++)
 {
   place_explosion_no_light(x + xpart(((rot * 8) + i * each_rot) % 1024, dist), 
                   y + ypart(((rot * 8) + i * each_rot) % 1024, dist), 
                   0 + xpart(((rot * 8) + i * each_rot) % 1024, GRAIN / 2),
                   0 + ypart(((rot * 8) + i * each_rot) % 1024, GRAIN / 2),
                   300 + crandom(250), colours);
 }

}



int enemy_sound_pitch(int ec)
{
 switch (eclass[ec].pitch)
 {
  case PITCH_GUARDIAN:
   return 900 + grand(200); 
  case PITCH_FIGHTER:
   return 1800 + grand(300); 
  case PITCH_SMALL_SPINNER:
   return 1400 + grand(200); 
  case PITCH_LARGE_SPINNER:
   return 700 + grand(100); 
  case PITCH_HEAVY_SPINNER:
   return 600 + grand(100); 
  case PITCH_BOSS1:
   return 400 + grand(50); 
  case PITCH_BOSS2:
   return 350 + grand(50); 
  case PITCH_BOSS3:
   return 320 + grand(50); 
  case PITCH_MINIBOSS1:
   return 600 + grand(50); 
  case PITCH_MINIBOSS2:
   return 500 + grand(50); 
  case PITCH_TURRET1:
   return 1600 + grand(200); 
  case PITCH_TURRET2:
   return 1400 + grand(200); 
  case PITCH_MEDIUM:
   return 1300 + grand(100); 
  case PITCH_HEAVY:
   return 700 + grand(80); 
 }
 return 1000;
}




void check_enemy_pickup_drop(int e)
{
 
 int create_symbol = 0;
 int create_other = 0;
 int symbol_count_needed;

 if (eclass[enemy[e].type].role == ROLE_TURRET)
  return;

/*

 if (enemy[e].carrying_pickup != PICKUP_NONE)
 {
  if (enemy[e].carrying_pickup >= PICKUP_SQUARE
      && enemy[e].carrying_pickup <= PICKUP_TRIANGLE)
    create_pickup(enemy[e].carrying_pickup, -1, enemy[e].x, enemy[e].y, SYMBOL_TIMEOUT);
     else
      create_pickup(enemy[e].carrying_pickup, -1, enemy[e].x, enemy[e].y, 333);
 }
*/
// if (enemy[e].target == TARGET_PRIMARY)
 {
//  if (arena.targets_left_total <= arena.total_targets / 4
//      && arena.targets_left_total + 1 > arena.total_targets / 4)
//      create_pickup(PICKUP_SQUARE + grand(3), -1, enemy[e].x, enemy[e].y, SYMBOL_TIMEOUT); 
//   create_pickup(PICKUP_REPAIR, 0, enemy[e].x, enemy[e].y, 330); 
// char get_second = enemy[e].target == TARGET_PRIMARY && arena.targets_left_total == 7;
 
 if (arena.total_targets > 1)
 {
//   if ((enemy[e].target == TARGET_PRIMARY && arena.targets_left_total % (arena.total_targets / 3) == 1)
   if (game.type == GAME_DUEL  
           || game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
   {
    if (enemy[e].target == TARGET_PRIMARY || game.type == GAME_DUEL)
    {
     game.ta_symbol_count ++;
     if (eclass[enemy[e].type].role == ROLE_MINIBOSS)
      game.ta_symbol_count += 2;
     if (eclass[enemy[e].type].role == ROLE_BOSS)
      game.ta_symbol_count += 3;
     symbol_count_needed = 9; 
     if (game.symbols_given <= 25)
      symbol_count_needed = 5;
     if (game.symbols_given <= 15)
      symbol_count_needed = 3; 
     if (game.type == GAME_TIME_ATTACK_COOP || game.type == GAME_DUEL)
     {
      symbol_count_needed --;
      if (game.symbols_given >= 50)
       symbol_count_needed = 99;      
     } 
      else
       {
        if (game.symbols_given >= 30)
         symbol_count_needed = 99;      
       } 
     if (game.ta_symbol_count >= symbol_count_needed && grand(4) != 0)
     // don't worry, you'll still get the symbol, just a little later.
     {
      create_symbol = 1;
      game.symbols_given ++;
      game.ta_symbol_count -= symbol_count_needed;
     } 
    }
   
   }
    else
    {
//      if ((enemy[e].target == TARGET_PRIMARY && arena.symbol_list [arena.symbol_index - 1] == 1))
      if ((enemy[e].target == TARGET_PRIMARY && arena.symbol_list [arena.symbol_index] == 1))
         create_symbol = 1;
      if (enemy[e].target == TARGET_PRIMARY)
       arena.symbol_index ++;
    }     
    

        
    if (create_symbol)   
     create_pickup(PICKUP_PRESYMBOL, PICKUP_SQUARE + grand(3), enemy[e].x, enemy[e].y, SYMBOL_TIMEOUT); 
      else
      {
       if (grand(10) < eclass[enemy[e].type].generous)
        create_other = 1;
       if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
       {
        if (grand(15) < eclass[enemy[e].type].generous)
         create_other = 1; // increased chance, to make up for lack of between-level repair       
       }
      }
      
    if (create_other)  
    {
     switch(grand(3))
     {
      case 0: create_pickup(PICKUP_REPAIR, 0, enemy[e].x, enemy[e].y, 330); break;
      case 1:
      case 2:
           switch(grand(13))
           {
            case 0: create_pickup(PICKUP_PRESECONDARY, SECOND_FURIOUS_ORB, enemy[e].x, enemy[e].y, 100); break;
            case 1: create_pickup(PICKUP_PRESECONDARY, SECOND_MANIFOLD_ORB, enemy[e].x, enemy[e].y, 100); break;
            case 2: create_pickup(PICKUP_PRESECONDARY, SECOND_PANIC_EELS, enemy[e].x, enemy[e].y, 100); break;
            case 3: //if (grand(2) == 0) 
              create_pickup(PICKUP_PRESECONDARY, SECOND_WORMS_SORROW, enemy[e].x, enemy[e].y, 100); break;
            case 4: create_pickup(PICKUP_PRESECONDARY, SECOND_WORMS_AGONY, enemy[e].x, enemy[e].y, 100); break;
            case 5: create_pickup(PICKUP_PRESECONDARY, SECOND_EYE_DESOLATION, enemy[e].x, enemy[e].y, 100); break;
            case 6: create_pickup(PICKUP_PRESECONDARY, SECOND_FROZEN_TEETH, enemy[e].x, enemy[e].y, 100); break;
//default:
            case 7: create_pickup(PICKUP_PRESECONDARY, SECOND_FROZEN_STARS, enemy[e].x, enemy[e].y, 100); break;
            case 8: create_pickup(PICKUP_PRESECONDARY, SECOND_BURNING_EYE, enemy[e].x, enemy[e].y, 100); break;
            case 9: create_pickup(PICKUP_PRESECONDARY, SECOND_TOXIC_SUN, enemy[e].x, enemy[e].y, 100); break;
            case 10: create_pickup(PICKUP_PRESECONDARY, SECOND_FLOWER, enemy[e].x, enemy[e].y, 100); break;
            case 11: create_pickup(PICKUP_PRESECONDARY, SECOND_SPORES, enemy[e].x, enemy[e].y, 100); break;
            case 12: create_pickup(PICKUP_PRESECONDARY, SECOND_CLAWS, enemy[e].x, enemy[e].y, 100); break;
           }
           break;
     }
    }
 }
/*       else
       {
           switch(grand(6))
           {
            case 0: create_pickup(PICKUP_PRESECONDARY, SECOND_FURIOUS_ORB, enemy[e].x, enemy[e].y, 100); break;
            case 1: create_pickup(PICKUP_PRESECONDARY, SECOND_MANIFOLD_ORB, enemy[e].x, enemy[e].y, 100); break;
            case 2: create_pickup(PICKUP_PRESECONDARY, SECOND_PANIC_EELS, enemy[e].x, enemy[e].y, 100); break;
            case 3: create_pickup(PICKUP_PRESECONDARY, SECOND_WORMS_SORROW, enemy[e].x, enemy[e].y, 100); break;
            case 4: create_pickup(PICKUP_PRESECONDARY, SECOND_WORMS_AGONY, enemy[e].x, enemy[e].y, 100); break;
            default: create_pickup(PICKUP_PRESECONDARY, SECOND_BURNING_EYE, enemy[e].x, enemy[e].y, 100); break;
           }
       }*/
 }
/*  else
  {
    if (grand(10) < eclass[enemy[e].type].generous)
    {
     switch(grand(2))
     {
      case 0: create_pickup(PICKUP_REPAIR, 0, enemy[e].x, enemy[e].y, 330); break;
      case 1:
           switch(grand(6))
           {
            case 0: create_pickup(PICKUP_PRESECONDARY, SECOND_FURIOUS_ORB, enemy[e].x, enemy[e].y, 100); break;
            case 1: create_pickup(PICKUP_PRESECONDARY, SECOND_MANIFOLD_ORB, enemy[e].x, enemy[e].y, 100); break;
            case 2: create_pickup(PICKUP_PRESECONDARY, SECOND_PANIC_EELS, enemy[e].x, enemy[e].y, 100); break;
            case 3: create_pickup(PICKUP_PRESECONDARY, SECOND_WORMS_SORROW, enemy[e].x, enemy[e].y, 100); break;
            case 4: create_pickup(PICKUP_PRESECONDARY, SECOND_WORMS_AGONY, enemy[e].x, enemy[e].y, 100); break;
            default: create_pickup(PICKUP_PRESECONDARY, SECOND_BURNING_EYE, enemy[e].x, enemy[e].y, 100); break;
           }
           break;
     }
    }
  }
*/
}


// REMEMBER this can be called from collision detection in actor.c
int move_enemy(int e)
{

if (enemy[e].x_speed == 0 && enemy[e].y_speed == 0) return 1;

  if (enemy[e].x + enemy[e].x_speed <= enemy[e].radius + 2000 // test - not edge_radius
      || enemy[e].x + enemy[e].x_speed >= arena.max_x - enemy[e].radius - 2000)
  {
   enemy[e].x_speed *= -1;
  }
  if (enemy[e].y + enemy[e].y_speed <= enemy[e].radius + 2000
      || enemy[e].y + enemy[e].y_speed >= arena.max_y - enemy[e].radius - 2000)
  {
   enemy[e].y_speed *= -1;
  }


//enemy[mbull].x2 += bullet[mbull].x_speed;
//bullet[mbull].y2 += bullet[mbull].y_speed;

enemy[e].x += enemy[e].x_speed;
enemy[e].y += enemy[e].y_speed;

// if an enemy is subject to drag, this happens in its own function

return 1;


}

/*
int move_torper(int e)
{

 if (grand(15) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 1000 + grand(1000));
 }

 drag_enemy(e, 0.03);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
 }

  if (enemy[e].attacking != ATTACK_NONE)
  {
    enemy[e].angle = track_target(e, enemy[e].attacking, 32);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].angle);
  }
  
 return 1;

}

*/


/*
int move_stinger2(int e)
{
 if (grand(10) == 0)
 {
  accelerate_enemy(e, grand(ANGLE_FULL + 1), 800 + grand(800));
 }

 drag_enemy(e, 0.02);

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].burst_fire > 0)
   enemy[e].angle = track_target(e, enemy[e].attacking, 16);
    else
     enemy[e].angle = track_target(e, enemy[e].attacking, 32);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }
  
 return 1;

}
*/


/*
int move_swerver3(int e)
{

// int tangle = enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2];
 int tangle = enemy[e].attribute [ATTRIB_SWERVER_ANGLE];

 if (enemy[e].counter % 32 == 0)
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);

 enemy[e].angle = turn_towards_angle(enemy[e].angle,
        enemy[e].attribute [ATTRIB_SWERVER_ANGLE],
        enemy[e].attribute [ATTRIB_SWERVER_TURN]);

 if (enemy[e].attacking != ATTACK_NONE
     && (tangle / 100 == enemy[e].angle / 100
     || tangle / 100 == (enemy[e].angle + ANGLE_HALF) / 100
     || tangle / 100 == (enemy[e].angle - ANGLE_HALF) / 100))
 {
  enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  if (grand(2) == 0)
  {
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] =
    radians_to_angle(atan2((actor[enemy[e].attacking].y - enemy[e].y), (actor[enemy[e].attacking].x - enemy[e].x)));
   if (enemy[e].attribute [ATTRIB_SWERVER_ANGLE] < 0)
    enemy[e].attribute [ATTRIB_SWERVER_ANGLE] += ANGLE_FULL;
   if (enemy[e].attribute [ATTRIB_SWERVER_ANGLE] > ANGLE_FULL)
    enemy[e].attribute [ATTRIB_SWERVER_ANGLE] -= ANGLE_FULL;
  }
 }
 
  enemy[e].x_speed = cos(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_SWERVER_SPEED];
  enemy[e].y_speed = sin(angle_to_radians(enemy[e].angle)) * enemy[e].attribute [ATTRIB_SWERVER_SPEED];

  if (enemy[e].x + enemy[e].x_speed <= enemy[e].edge_radius
      || enemy[e].x + enemy[e].x_speed >= arena.max_x - enemy[e].edge_radius - 4000)
  {
   enemy[e].x_speed *= -1;
   enemy[e].angle += ANGLE_HALF;
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  }
  if (enemy[e].y + enemy[e].y_speed <= enemy[e].edge_radius
      || enemy[e].y + enemy[e].y_speed >= arena.max_y - enemy[e].edge_radius - 5000)
  {
   enemy[e].y_speed *= -1;
   enemy[e].angle -= ANGLE_HALF;
   enemy[e].attribute [ATTRIB_SWERVER_ANGLE] = grand(ANGLE_FULL + 1);
  }

enemy[e].x += enemy[e].x_speed;
enemy[e].y += enemy[e].y_speed;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING] = nearby_target(400000, enemy[e].x, enemy[e].y);
//  exit(2);
 }

//  if (enemy[e].attacking != ATTACK_NONE)
  if (enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING] != ATTACK_NONE)
  {
//  exit(3);
//    enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2] = track_target(e, enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING], 16);
    enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2] = turn_towards_xy(enemy[e].x, enemy[e].y, actor[enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING]].x, actor[enemy[e].attribute [ATTRIB_SWERVER3_ATTACKING]].y, enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2], 16);
    if (enemy[e].recycle == 0)
     standard_attack(e, enemy[e].attribute [ATTRIB_SWERVER3_ANGLE2]);
  }

 return 1;

}
*/
/*
int move_bouncers(int e)
{

 if (enemy[e].type == ENEMY_BOUNCER4)
 {
  if (enemy[e].counter % 16 == 0)
  {
   enemy[e].attacking = nearby_target(500000, enemy[e].x, enemy[e].y);
  }

   if (enemy[e].attacking != ATTACK_NONE)
   {
     enemy[e].attribute [ATTRIB_BOUNCER_ANGLE2] = turn_towards_xy(enemy[e].x, enemy[e].y, actor[enemy[e].attacking].x, actor[enemy[e].attacking].y, enemy[e].attribute [ATTRIB_BOUNCER_ANGLE2], 4);
     if (enemy[e].recycle == 0)
      standard_attack(e, enemy[e].attribute [ATTRIB_BOUNCER_ANGLE2]);
   }
  return 1;
 }

 enemy[e].angle += enemy[e].attribute [ATTRIB_BOUNCER_SPIN];
 if (enemy[e].angle < 0)
  enemy[e].angle += ANGLE_FULL;
 if (enemy[e].angle < ANGLE_FULL)
  enemy[e].angle -= ANGLE_FULL;

 enemy[e].attribute [ATTRIB_BOUNCER_SPIN] += enemy[e].attribute [ATTRIB_BOUNCER_DELTA_SPIN];
 if (enemy[e].attribute [ATTRIB_BOUNCER_SPIN] > 50)
  enemy[e].attribute [ATTRIB_BOUNCER_SPIN] = 50;
 if (enemy[e].attribute [ATTRIB_BOUNCER_SPIN] < -50)
  enemy[e].attribute [ATTRIB_BOUNCER_SPIN] = -50;
  
 if (grand(10) == 0)
 {
  enemy[e].attribute [ATTRIB_BOUNCER_DELTA_SPIN] = grand(5) - 2;
 }

 return 1;

}
*/

void wormer_pulsates(int e)
{
    enemy[e].radius = 9000 + (100000 * (120 - enemy[e].recycle)) / 1500
     + pulsate(48, 1200, enemy[e].recycle);
      if (enemy[e].recycle > 110)
       enemy[e].radius = 17000 - (120 - enemy[e].recycle) * GRAIN
        + pulsate(48, 1200, enemy[e].recycle);
}
/*
void enemy_pulsates(int e, int amount)
{
 pulsate(8, 3500, enemy[e].recycle)
}
*/

int move_drifter_ai(int e)
{
 int ec = enemy[e].type;
 int slew_amount = eclass[ec].slew;
 int bullet_colours [4];
 int i;
 
 if (enemy[e].x / 100000 == enemy[e].x_dest / 100000
     && enemy[e].y / 100000 == enemy[e].y_dest / 100000)
 {
  enemy[e].x_dest = grand(arena.max_x - enemy[e].edge_radius * 2) + enemy[e].edge_radius;
  enemy[e].y_dest = grand(arena.max_y - enemy[e].edge_radius * 2) + enemy[e].edge_radius;
//  enemy[e].x_dest = grand(arena.max_x - enemy[e].edge_radius * 4) + enemy[e].edge_radius * 2;
//  enemy[e].y_dest = grand(arena.max_y - enemy[e].edge_radius * 4) + enemy[e].edge_radius * 2;
//  enemy[e].attribute [ATTRIB_DRIFTER_DELTA_SPIN] = grand(5) - 2;
 }

 float target_angle = atan2(enemy[e].y_dest - enemy[e].y,
                            enemy[e].x_dest - enemy[e].x);

 accelerate_enemy(e, radians_to_angle(target_angle), eclass[ec].acceleration);
/*
 enemy[e].attribute [ATTRIB_DRIFTER_SPIN] += enemy[e].attribute [ATTRIB_DRIFTER_DELTA_SPIN];
 if (enemy[e].attribute [ATTRIB_DRIFTER_SPIN] > 32)
  enemy[e].attribute [ATTRIB_DRIFTER_SPIN] = 32;
 if (enemy[e].attribute [ATTRIB_DRIFTER_SPIN] < -32)
  enemy[e].attribute [ATTRIB_DRIFTER_SPIN] = -32;
 enemy[e].attribute [ATTRIB_DRIFTER_LUMP_ANGLE] += enemy[e].attribute [ATTRIB_DRIFTER_SPIN];
*/ 
 drag_enemy(e, eclass[ec].drag_amount);

 int distance, xa, xb, xc, xd;

 switch(enemy[e].type)
 {
    case ENEMY_CURVE2:
    case ENEMY_CURVE3:
     if (enemy[e].armour <= 500 && enemy[e].counter % 2 == 0 && grand(1000) > enemy[e].armour)
     {
      int passing_colours [5] = {TRANS_DRED, TRANS_LRED, TRANS_YELLOW, 0, 0};
       if (enemy[e].type == ENEMY_CURVE3)
       {
        passing_colours [0] = TRANS_DBLUE;
        passing_colours [1] = TRANS_LBLUE;
        passing_colours [2] = TRANS_WHITE;
       }
       xa = grand(ANGLE_FULL);
       xb = grand(16 * GRAIN);
       place_small_explosion(enemy[e].x + xpart(xa, xb), enemy[e].y + ypart(xa, xb), enemy[e].x_speed, enemy[e].y_speed,
        200 + crandom(200), passing_colours);
       hurt_enemy(e, 10 + grand(20), enemy[e].attribute [0], 1);
     }
     break;
    case ENEMY_BEAMER2:
     if (enemy[e].attribute [0] > 0)
     {
      create_effect(EFFECT_BLUEBEAM, 
       enemy[e].x + xpart(enemy[e].angle, 31 * GRAIN),
       enemy[e].y + ypart(enemy[e].angle, 31 * GRAIN),
       enemy[e].x, enemy[e].y, 
       enemy[e].x + xpart(enemy[e].angle, 400 * GRAIN),
       enemy[e].y + ypart(enemy[e].angle, 400 * GRAIN),
       enemy[e].attribute [0], enemy[e].angle,enemy[e].counter,0,0);
       if (enemy[e].attribute [0] > 100)
        enemy[e].attribute [0] -= 2;
         else
          enemy[e].attribute [0]--;
      enemy_beam(enemy[e].x, enemy[e].y, enemy[e].angle, enemy[e].attribute [0], EFFECT_BLUEBEAM, e);
      slew_amount = 2;
     }
     manage_turret(e, 0, 74 * GRAIN, 0, 0);
     manage_turret(e, 1, -74 * GRAIN, 0, 0);
     break;    
    case ENEMY_BEAMER1:
     if (enemy[e].attribute [0] > 0)
     {
      create_effect(EFFECT_REDBEAM, 
       enemy[e].x + xpart(enemy[e].angle, 31 * GRAIN),
       enemy[e].y + ypart(enemy[e].angle, 31 * GRAIN),
       enemy[e].x, enemy[e].y, 
       enemy[e].x + xpart(enemy[e].angle, 400 * GRAIN),
       enemy[e].y + ypart(enemy[e].angle, 400 * GRAIN),
       enemy[e].attribute [0], enemy[e].angle,enemy[e].counter,0,0);
      enemy[e].attribute [0] --;
      enemy_beam(enemy[e].x, enemy[e].y, enemy[e].angle, enemy[e].attribute [0], EFFECT_REDBEAM, e);
      slew_amount = 1;
     }
     manage_turret(e, 0, 74 * GRAIN, 0, 0);
     manage_turret(e, 1, -74 * GRAIN, 0, 0);
     break;    
    case ENEMY_OVERZAPPER:
     enemy[e].attribute [0] = 0;
     enemy[e].attribute [4] += enemy[e].attribute [5];
     enemy[e].attribute [4] &= 1023;
       if (enemy[e].attacking != -1)
       {
        xa = radians_to_angle(atan2((actor[enemy[e].attacking].y - enemy[e].attribute [9]), (actor[enemy[e].attacking].x - enemy[e].attribute [8])));
        if (xa < 0)
         xa += ANGLE_FULL;
        if (xa > ANGLE_FULL)
         xa -= ANGLE_FULL;
        enemy[e].attribute [8] += xpart(xa, 2000);
        enemy[e].attribute [9] += ypart(xa, 2000);
        if (hypot(enemy[e].attribute [8] - enemy[e].x, enemy[e].attribute [9] - enemy[e].y) > 350000)
        {
         xa = radians_to_angle(atan2((enemy[e].attribute [9] - enemy[e].y), (enemy[e].attribute [8] - enemy[e].x)));
         if (xa < 0)
          xa += ANGLE_FULL;
         if (xa > ANGLE_FULL)
          xa -= ANGLE_FULL;
         enemy[e].attribute [8] -= xpart(xa, 2000);
         enemy[e].attribute [9] -= ypart(xa, 2000);        
        }
       }
       if (enemy[e].attribute [6] <= 0)
       {
        for (xa = 0; xa < NO_ACTORS; xa ++)
        {   
         if (actor[xa].in_play == 0)
          continue;
         if (actor[xa].grace_period > 0)
          continue;
         if (hypot(enemy[e].attribute [8] - actor[xa].x, enemy[e].attribute [9] - actor[xa].y) < 30000)
         {
          zap_attack(e);
          enemy[e].attribute [6] = 40;
          break;
         }
        }  
       }
        else
         enemy[e].attribute [6] --;
     manage_turret(e, 0, xpart(enemy[e].angle, -30000), ypart(enemy[e].angle, -30000), 0);
      create_effect(EFFECT_ZAP_AIM, 
       enemy[e].attribute [8],
       enemy[e].attribute [9],
       enemy[e].attribute [8] - 30,
       enemy[e].attribute [9] - 30,
       enemy[e].attribute [8] + 30,
       enemy[e].attribute [9] + 30,
       enemy[e].attribute [7], enemy[e].angle,enemy[e].counter,0,0);
       enemy[e].recycle = 100; // attribute [6] used instead
       enemy[e].attribute [7] += xpart(enemy[e].counter * 4, 6);
     break;
    case ENEMY_OVERTRIANGLER:
     manage_turret(e, 0, xpart(enemy[e].angle, -30000), ypart(enemy[e].angle, -30000), 0);
     // fall-through...
   case ENEMY_TRIANGLER1:
   case ENEMY_TRIANGLER2:
   case ENEMY_TRIANGLER3:
    if (enemy[e].attribute [1] > 0)
    {
     enemy[e].attribute [1] += enemy[e].attribute [2];
     enemy[e].attribute [2] --;
     if (enemy[e].attribute [2] < -1)
      enemy[e].attribute [2] = -1;
    }

    if (enemy[e].attribute [1] < 0)
     enemy[e].attribute [1] = 0;
    /*    if (enemy[e].attribute [2] != 0)
    {
     if (enemy[e].attribute [1] < 24)
     {
      enemy[e].attribute [1] += enemy[e].attribute [2];
     }
     if (enemy[e].attribute [1] >= 24)
      enemy[e].attribute [2] = -6;
     if (enemy[e].attribute [1] <= 0)
      enemy[e].attribute [2] = 0;
    }*/
    break;
   case ENEMY_DEAD_TRI1:
   case ENEMY_DEAD_TRI2:
   case ENEMY_DEAD_TRI3:
//     drag_enemy(e, .01);
     if (enemy[e].attribute [0] % 2 == 0)
     {
       int passing_colours [5];
       int cangle = grand(ANGLE_FULL);
       int cx = enemy[e].x + xpart(cangle, grand(enemy[e].attribute [1]));
       int cy = enemy[e].y + ypart(cangle, grand(enemy[e].attribute [1]));
       passing_colours [0] = enemy[e].attribute [3];
       create_cloud(CLOUD_MED_TRANS_CIRCLE,
        cx, cy, 0, 0, enemy[e].x_speed, enemy[e].y_speed,
        200 + grand(350),0,2, 0, 0, 0, passing_colours);
       passing_colours [0] = enemy[e].attribute [4];
       create_cloud(CLOUD_MED_TRANS_CIRCLE,
        cx, cy, 0, 0, enemy[e].x_speed, enemy[e].y_speed,
        300 + grand(450),0,6, 0, 0, 0, passing_colours);
       simple_light(cx, cy, enemy[e].x_speed, enemy[e].y_speed, 600);
     }
     enemy[e].attribute [0] --;
     if (enemy[e].attribute [0] <= 0)
     {
      hurt_enemy(e, 99999, OWNER_ENEMY, 0);
      return 0;
     }   
     break; 
   case ENEMY_CIRCLER1:
    if (enemy[e].attribute [0] > 0)
     enemy[e].attribute [0]--;
     break;
   case ENEMY_WORMER1:
   case ENEMY_WORMER2:
   case ENEMY_WORMER3:
   case ENEMY_WORMER4:
    wormer_pulsates(e);
    break; 
    case ENEMY_HEAD1:
     manage_turret(e, 0, 25 * GRAIN, 27 * GRAIN, 0);
     manage_turret(e, 1, -25 * GRAIN, 27 * GRAIN, 0);
     break;
    case ENEMY_DEFENDER1:
     manage_turret(e, 0, 49 * GRAIN, 0, 0);
     manage_turret(e, 1, -49 * GRAIN, 0, 0);
     break;
    case ENEMY_DEFENDER2:
     manage_turret(e, 0, 0, 48 * GRAIN, 0);
     manage_turret(e, 1, 0, -48 * GRAIN, 0);
     break;
    case ENEMY_DEFENDER3:
     manage_turret(e, 0, 52 * GRAIN, 0, 0);
     manage_turret(e, 1, -52 * GRAIN, 0, 0);
     break;
    case ENEMY_OVERDISRUPTER:
     manage_turret(e, 0, 0, 50 * GRAIN, 0);
     manage_turret(e, 1, 0, -50 * GRAIN, 0);
     break;
    case ENEMY_BOSS1_1:
     manage_turret(e, 0, 50 * GRAIN, 65 * GRAIN, 0);
     manage_turret(e, 1, -50 * GRAIN, 65 * GRAIN, 0);
     manage_turret(e, 2, -50 * GRAIN, -65 * GRAIN, 0);
     manage_turret(e, 3, 50 * GRAIN, -65 * GRAIN, 0);
//     manage_turret(e, 0, 0, 60 * GRAIN);
//     manage_turret(e, 1, 62 * GRAIN, -13 * GRAIN);
//     manage_turret(e, 2, -62 * GRAIN, -13 * GRAIN);
     break;
    case ENEMY_BOSS1_3:
     manage_turret(e, 0, -83 * GRAIN, 59 * GRAIN, 0);
     manage_turret(e, 1, 83 * GRAIN, 59 * GRAIN, 0);
     manage_turret(e, 2, 0, -80 * GRAIN, 0);
     break;
    case ENEMY_BOSS2:
     enemy[e].angle += 5 + grand(10);
     enemy[e].angle &= ANGLE_FULL - 1;
     enemy[e].attribute [0] -= 5 + grand(10);
     enemy[e].attribute [0] &= ANGLE_FULL - 1;
     manage_turret(e, 0, - 62 * GRAIN, -62 * GRAIN, 0);
     manage_turret(e, 1, 62 * GRAIN, -62 * GRAIN, 0);
     manage_turret(e, 2, - 62 * GRAIN, 62 * GRAIN, 0);
     manage_turret(e, 3, 62 * GRAIN, 62 * GRAIN, 0);
     break;
    case ENEMY_BOSS2_2:
     manage_turret(e, 0, - 79 * GRAIN, 44 * GRAIN, 0);
     manage_turret(e, 1, 79 * GRAIN, 44 * GRAIN, 0);
     manage_turret(e, 2, 0 * GRAIN, -89 * GRAIN, 0);
     break;
    case ENEMY_BOSS2_3:
     manage_turret(e, 0, - 109 * GRAIN, 0 * GRAIN, 0);
     manage_turret(e, 1, 109 * GRAIN, 0 * GRAIN, 0);
     manage_turret(e, 2, 0 * GRAIN, 97 * GRAIN, 0);
     manage_turret(e, 3, 0 * GRAIN, -97 * GRAIN, 0);
     break;
    case ENEMY_BOSS3_1:
     manage_turret(e, 0, - 69 * GRAIN, -69 * GRAIN, 0);
     manage_turret(e, 1, 69 * GRAIN, -69 * GRAIN, 0);
     manage_turret(e, 2, -69 * GRAIN, 69 * GRAIN, 0);
     manage_turret(e, 3, 69 * GRAIN, 69 * GRAIN, 0);
     run_boss3_1(e);
     break;
    case ENEMY_BOSS3_2:
     manage_turret(e, 0, - 78 * GRAIN, 57 * GRAIN, 0);
     manage_turret(e, 1, 78 * GRAIN, 57 * GRAIN, 0);
     manage_turret(e, 2, 0 * GRAIN, -78 * GRAIN, 0);
     break;
    case ENEMY_BOSS3_3:
     manage_turret(e, 0, - 88 * GRAIN, -62 * GRAIN, 0);
     manage_turret(e, 1, 88 * GRAIN, -62 * GRAIN, 0);
     manage_turret(e, 2, -88 * GRAIN, 62 * GRAIN, 0);
     manage_turret(e, 3, 88 * GRAIN, 62 * GRAIN, 0);
     run_boss3_3(e);
     break;
    case ENEMY_BRACKET4:
     manage_turret(e, 0, 26 * GRAIN, 0, 0);
     manage_turret(e, 1, -26 * GRAIN, 0, 0);
     break;
    case ENEMY_MINER3:
     manage_turret(e, 0, 0, -30 * GRAIN, 0);
     break;
/*    case ENEMY_CRUISER1:
    case ENEMY_CRUISER2:
    case ENEMY_CRUISER3:
    case ENEMY_CRUISER4:
     manage_turret(e, 0, xpart(enemy[e].angle, 22000), ypart(enemy[e].angle, 22000));
     break;
see cruiser_ai     
     */
     
    case ENEMY_OVERBLATTER:
    case ENEMY_OVERBLATTER2:
/*     if (enemy[e].attribute [4] != 0)
     {
      enemy[e].attribute [3] += enemy[e].attribute [4];
      enemy[e].angle = enemy[e].attribute [3];
     } */
     xa = 5; // slew
     if (enemy[e].type == ENEMY_OVERBLATTER2)
      xa = 6;
     if (enemy[e].attribute [8] > 0)
      enemy[e].attribute [8] --;
     if (enemy[e].attribute [9] > 0)
      enemy[e].attribute [9] --;
     if (enemy[e].attacking != -1)
     {
      enemy[e].attribute [6] = 
          track_target_any_angle(enemy[e].x + xpart(enemy[e].angle + ANGLE_QUARTER, 26 * GRAIN), 
                                 enemy[e].y + ypart(enemy[e].angle + ANGLE_QUARTER, 26 * GRAIN),
                                 actor[enemy[e].attacking].x, actor[enemy[e].attacking].y, xa, 0, 
                                 enemy[e].attribute [6]);
      enemy[e].attribute [7] = 
          track_target_any_angle(enemy[e].x + xpart(enemy[e].angle - ANGLE_QUARTER, 26 * GRAIN), 
                                 enemy[e].y + ypart(enemy[e].angle - ANGLE_QUARTER, 26 * GRAIN),
                                 actor[enemy[e].attacking].x, actor[enemy[e].attacking].y, xa, 0, 
                                 enemy[e].attribute [7]);
     }
//     break; fall through
    case ENEMY_OVERSPINNER:
    case ENEMY_OVERSPIKEY:
    case ENEMY_UNDERSPIKEY:
    case ENEMY_UNDERSPIKEY2:
    case ENEMY_UNDERSPIKEY3:
    case ENEMY_OVERSPIKEY2:
     enemy[e].attribute [0] += enemy[e].attribute [1];
     enemy[e].attribute [0] &= ANGLE_FULL - 1;
     enemy[e].attribute [2] -= (enemy[e].attribute [1] / 3);
     enemy[e].attribute [2] &= ANGLE_FULL - 1;
     break;
    case ENEMY_SPINNER1:
    case ENEMY_SPINNER2:
    case ENEMY_SPINNER3:
    case ENEMY_SPINNER4:
    case ENEMY_SPINNER5:
    case ENEMY_SPIKEY1:
    case ENEMY_SPIKEY2:
    case ENEMY_SPIKEY3:
    case ENEMY_SPIKEY4:
    case ENEMY_SPIKEY5:
    case ENEMY_BLATTER1:
    case ENEMY_BLATTER2:
    case ENEMY_BLATTER3:
    case ENEMY_BLATTER4:
    case ENEMY_BLATTER5:
    case ENEMY_BLOATER1:
    case ENEMY_BLOATER2:
     enemy[e].attribute [0] += enemy[e].attribute [1];
     enemy[e].attribute [0] &= ANGLE_FULL - 1;
     break; 
//     if (enemy[e].attribute [0] > 0)
//      enemy[e].attribute [0] --;
//      break;
    case ENEMY_SHIELDER1: // mirrored for some turrets in move_turret
     if (enemy[e].attribute [0] > 0)
      enemy[e].attribute [0] --;
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
     break;
    case ENEMY_ORBITER1:
     xa = enemy[e].x - enemy[e].x_speed + xpart(enemy[e].attribute [1], 150000);
     xb = enemy[e].y - enemy[e].y_speed + ypart(enemy[e].attribute [1], 150000);
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
     bullet_colours [0] = TRANS_YELLOW;
     bullet_colours [1] = TRANS_ORANGE;
     bullet_colours [2] = TRANS_LRED;
     bullet_colours [3] = TRANS_DRED;
     for (i = 0; i < 3; i ++)
     {
      create_bullet(BULLET_ORBIT, 
       enemy[e].x + xpart(enemy[e].attribute [1] + (ANGLE_FULL / 3) * i, 150000), 
       enemy[e].y + ypart(enemy[e].attribute [1] + (ANGLE_FULL / 3) * i, 150000), 
       0, 0,
       OWNER_ENEMY, 100, 2, 100, 0,
       0, 5000, bullet_colours, 1,
       xa,
       xb,5,7,0);
     }  
     break;
    case ENEMY_ORBITER2:
     xa = enemy[e].x - enemy[e].x_speed + xpart(enemy[e].attribute [1], 150000);
     xb = enemy[e].y - enemy[e].y_speed + ypart(enemy[e].attribute [1], 150000);
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
     enemy[e].attribute [3] = (enemy[e].attribute [3] + enemy[e].attribute [4]) & 1023;    
     bullet_colours [0] = TRANS_YELLOW;
     bullet_colours [1] = TRANS_ORANGE;
     bullet_colours [2] = TRANS_LRED;
     bullet_colours [3] = TRANS_DRED;
     for (i = 0; i < 2; i ++)
     {
      create_bullet(BULLET_ORBIT, 
       enemy[e].x + xpart(enemy[e].attribute [1] + (ANGLE_HALF) * i, 150000), 
       enemy[e].y + ypart(enemy[e].attribute [1] + (ANGLE_HALF) * i, 150000), 
       0, 0,
       OWNER_ENEMY, 100, 2, 100, 0,
       0, 5000, bullet_colours, 1,
       xa,
       xb,5,7,0);
     }  
     for (i = 0; i < 2; i ++)
     {
      create_bullet(BULLET_ORBIT, 
       enemy[e].x + xpart(enemy[e].attribute [3] + (ANGLE_HALF) * i, 100000), 
       enemy[e].y + ypart(enemy[e].attribute [3] + (ANGLE_HALF) * i, 100000), 
       0, 0,
       OWNER_ENEMY, 100, 2, 100, 0,
       0, 5000, bullet_colours, 1,
       xa,
       xb,5,7,0);
     }  
     break;
    case ENEMY_ORBITER3:
//     xa = enemy[e].x - enemy[e].x_speed + xpart(enemy[e].attribute [1], 150000);
//     xb = enemy[e].y - enemy[e].y_speed + ypart(enemy[e].attribute [1], 150000);
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
//     bullet_colours [0] = TRANS_WHITE;
/*     bullet_colours [0] = TRANS_PURPLE;
     bullet_colours [1] = TRANS_LBLUE;
     bullet_colours [2] = TRANS_DBLUE;
     bullet_colours [3] = TRANS_DBLUE;*/
     bullet_colours [0] = TRANS_WHITE;
     bullet_colours [1] = TRANS_LGREY;
     bullet_colours [2] = TRANS_LGREY;
     bullet_colours [3] = TRANS_DGREY;
     enemy[e].attribute [3] = (enemy[e].attribute [3] + 1) & 0xff;
     for (i = 0; i < 4; i ++)
     {
      xc = pulsate(32, 32, (enemy[e].attribute [3] + i * 64) & 0xff);
      xd = pulsate(32, 32, (enemy[e].attribute [3] - 1 + i * 64) & 0xff);
      create_bullet(BULLET_ORBIT,
       enemy[e].x + xpart(enemy[e].attribute [1] + (ANGLE_QUARTER) * i, 150000 + xc * GRAIN), 
       enemy[e].y + ypart(enemy[e].attribute [1] + (ANGLE_QUARTER) * i, 150000 + xc * GRAIN),
       0, 0,
       OWNER_ENEMY, 150, 2, 100, 0,
       0, 4000, bullet_colours, 1,
       enemy[e].x - enemy[e].x_speed + xpart((enemy[e].attribute [1] + (ANGLE_QUARTER) * i - enemy[e].attribute [2]) & 1023, 150000 + xd * GRAIN),
       enemy[e].y - enemy[e].y_speed + ypart((enemy[e].attribute [1] + (ANGLE_QUARTER) * i - enemy[e].attribute [2]) & 1023, 150000 + xd * GRAIN),
       4,6,0);
     }  
     break;
    case ENEMY_ZAPPER1:
     enemy[e].recycle = 10;
     enemy[e].attribute [0] = 0;
     enemy[e].attribute [4] += enemy[e].attribute [5];
     enemy[e].attribute [4] &= 1023;
     if (enemy[e].attacking != ATTACK_NONE && actor[enemy[e].attacking].in_play == 1)
     {
      if ((distance = hypot(actor[enemy[e].attacking].x - enemy[e].x, actor[enemy[e].attacking].y - enemy[e].y)) < eclass[ec].range)
      {
       enemy[e].attribute [0] = 1;
       enemy[e].attribute [1] = actor[enemy[e].attacking].x;
       enemy[e].attribute [2] = actor[enemy[e].attacking].y;
       enemy[e].attribute [3] = distance / GRAIN;
       if (actor[enemy[e].attacking].grace_period == 0)
        hurt_actor(enemy[e].attacking, OWNER_ENEMY, 3);
      }
     }
     break;     
    case ENEMY_PULSER1:
    case ENEMY_PULSER2:
      enemy[e].attribute [0] ++;
      if (enemy[e].armour < 400)
       enemy[e].attribute [0] ++;
      if (enemy[e].armour < 200)
       enemy[e].attribute [0] ++;
      enemy[e].attribute [0] &= 255;
     break; 
    case ENEMY_SHADOW1:
     if (enemy[e].attribute [0] > 0)
      enemy[e].attribute [0] --;
       else
       {
        if (enemy[e].hurt_pulse > 0)
         enemy[e].attribute [0] = 9;
       }
     if (enemy[e].recycle == 10)
      enemy[e].attribute [0] = 78;
     break; 
    case ENEMY_SHADOW2:
     if (enemy[e].attribute [0] > 0)
      enemy[e].attribute [0] --;
       else
       {
        if (enemy[e].hurt_pulse > 0)
         enemy[e].attribute [0] = 9;
       }
     if (enemy[e].recycle == 10)
      enemy[e].attribute [0] = 58;
     break; 
 }



 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(eclass[ec].range, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)// && enemy[e].counter % 5 == 0)
 {
  if (enemy[e].counter % 8 == 0) 
   enemy[e].slew_dir = 0;  
  enemy[e].angle = track_target(e, enemy[e].attacking, slew_amount, enemy[e].slew_dir * -1);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

// ifnemy[e].type != ENEMY_DRIFTER3)
//   return 1;
/*
  int colours [4], ox, oy;

  enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] += enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL_DIR];
  if (enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] > ANGLE_FULL)
   enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] -= ANGLE_FULL;
  if (enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] < 0)
   enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] += ANGLE_FULL;

  colours [0] = GC_BLUE4;

  ox = enemy[e].x + cos(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL])) * 40000;
  oy = enemy[e].y + sin(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL])) * 40000;

  create_bullet(BULLET_ORBITAL, ox, oy,
   0, 0,
   OWNER_ENEMY, 50, 2, 100, 0,
   0, 0, colours, 1,0,0,0,0,0);


  ox = enemy[e].x + cos(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] + ANGLE_HALF)) * 40000;
  oy = enemy[e].y + sin(angle_to_radians(enemy[e].attribute [ATTRIB_DRIFTER3_ORBITAL] + ANGLE_HALF)) * 40000;

  create_bullet(BULLET_ORBITAL, ox, oy,
   0, 0,
   OWNER_ENEMY, 50, 2, 100, 0,
   0, 0, colours, 1,0,0,0,0,0);

 return 1;
*/
 return 1;
}



int move_auto_ai(int e)
{
 int ec = enemy[e].type;
 int bullet_colours [4];
 int i;
 
 drag_enemy(e, eclass[ec].drag_amount);

 int xa, xb, xc, xd;

 switch(enemy[e].type)
 {
    case ENEMY_ATTRACTOR:
     blast(enemy[e].x, enemy[e].y, 300000, 0, -90, OWNER_ENEMY);
     break; 
    case ENEMY_ORBITER1:
     xa = enemy[e].x - enemy[e].x_speed + xpart(enemy[e].attribute [1], 150000);
     xb = enemy[e].y - enemy[e].y_speed + ypart(enemy[e].attribute [1], 150000);
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
     bullet_colours [0] = TRANS_YELLOW;
     bullet_colours [1] = TRANS_ORANGE;
     bullet_colours [2] = TRANS_LRED;
     bullet_colours [3] = TRANS_DRED;
     for (i = 0; i < 3; i ++)
     {
      create_bullet(BULLET_ORBIT, 
       enemy[e].x + xpart(enemy[e].attribute [1] + (ANGLE_FULL / 3) * i, 150000), 
       enemy[e].y + ypart(enemy[e].attribute [1] + (ANGLE_FULL / 3) * i, 150000), 
       0, 0,
       OWNER_ENEMY, 100, 2, 100, 0,
       0, 5000, bullet_colours, 1,
       xa,
       xb,0,0,0);
     }  
     break;
    case ENEMY_ORBITER2:
     xa = enemy[e].x - enemy[e].x_speed + xpart(enemy[e].attribute [1], 150000);
     xb = enemy[e].y - enemy[e].y_speed + ypart(enemy[e].attribute [1], 150000);
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
     enemy[e].attribute [3] = (enemy[e].attribute [3] + enemy[e].attribute [4]) & 1023;    
     bullet_colours [0] = TRANS_YELLOW;
     bullet_colours [1] = TRANS_ORANGE;
     bullet_colours [2] = TRANS_LRED;
     bullet_colours [3] = TRANS_DRED;
     for (i = 0; i < 2; i ++)
     {
      create_bullet(BULLET_ORBIT, 
       enemy[e].x + xpart(enemy[e].attribute [1] + (ANGLE_HALF) * i, 150000), 
       enemy[e].y + ypart(enemy[e].attribute [1] + (ANGLE_HALF) * i, 150000), 
       0, 0,
       OWNER_ENEMY, 100, 2, 100, 0,
       0, 5000, bullet_colours, 1,
       xa,
       xb,0,0,0);
     }  
     for (i = 0; i < 2; i ++)
     {
      create_bullet(BULLET_ORBIT, 
       enemy[e].x + xpart(enemy[e].attribute [3] + (ANGLE_HALF) * i, 100000), 
       enemy[e].y + ypart(enemy[e].attribute [3] + (ANGLE_HALF) * i, 100000), 
       0, 0,
       OWNER_ENEMY, 100, 2, 100, 0,
       0, 5000, bullet_colours, 1,
       xa,
       xb,0,0,0);
     }  
     break;
    case ENEMY_ORBITER3:
//     xa = enemy[e].x - enemy[e].x_speed + xpart(enemy[e].attribute [1], 150000);
//     xb = enemy[e].y - enemy[e].y_speed + ypart(enemy[e].attribute [1], 150000);
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
//     bullet_colours [0] = TRANS_WHITE;
/*     bullet_colours [0] = TRANS_PURPLE;
     bullet_colours [1] = TRANS_LBLUE;
     bullet_colours [2] = TRANS_DBLUE;
     bullet_colours [3] = TRANS_DBLUE;*/
     bullet_colours [0] = TRANS_WHITE;
     bullet_colours [1] = TRANS_LGREY;
     bullet_colours [2] = TRANS_LGREY;
     bullet_colours [3] = TRANS_DGREY;
     enemy[e].attribute [3] = (enemy[e].attribute [3] + 1) & 0xff;
     for (i = 0; i < 4; i ++)
     {
      xc = pulsate(32, 32, (enemy[e].attribute [3] + i * 64) & 0xff);
      xd = pulsate(32, 32, (enemy[e].attribute [3] - 1 + i * 64) & 0xff);
      create_bullet(BULLET_ORBIT,
       enemy[e].x + xpart(enemy[e].attribute [1] + (ANGLE_QUARTER) * i, 150000 + xc * GRAIN), 
       enemy[e].y + ypart(enemy[e].attribute [1] + (ANGLE_QUARTER) * i, 150000 + xc * GRAIN),
       0, 0,
       OWNER_ENEMY, 150, 2, 100, 0,
       0, 5000, bullet_colours, 1,
       enemy[e].x - enemy[e].x_speed + xpart((enemy[e].attribute [1] + (ANGLE_QUARTER) * i - enemy[e].attribute [2]) & 1023, 150000 + xd * GRAIN),
       enemy[e].y - enemy[e].y_speed + ypart((enemy[e].attribute [1] + (ANGLE_QUARTER) * i - enemy[e].attribute [2]) & 1023, 150000 + xd * GRAIN),
       0,0,0);
     }  
     break;
    case ENEMY_WAVER1:
      create_effect(EFFECT_WAVE1, 
       enemy[e].x,
       enemy[e].y,
       enemy[e].x - 160 * GRAIN, enemy[e].y - 160 * GRAIN, 
       enemy[e].x + 160 * GRAIN,
       enemy[e].y + 160 * GRAIN,
       enemy[e].counter, 150,0,0,0);
      enemy[e].attribute [0] += enemy[e].attribute [1];
      enemy[e].attribute [0] &= ANGLE_FULL - 1;
      arena.waver_on_level = 1;
      break; 
    case ENEMY_WAVER2:
      create_effect(EFFECT_WAVE2, 
       enemy[e].x,
       enemy[e].y,
       enemy[e].x - 210 * GRAIN, enemy[e].y - 210 * GRAIN, 
       enemy[e].x + 210 * GRAIN,
       enemy[e].y + 210 * GRAIN,
       enemy[e].counter, 200,0,0,0);
      enemy[e].attribute [0] += enemy[e].attribute [1];
      enemy[e].attribute [0] &= ANGLE_FULL - 1;
      arena.waver_on_level = 1;
// if sizes are changed here, must also change them in bullet.c
      break; 
 }


  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 return 1;
}


int move_stinger_ai(int e)
{
 int ec = enemy[e].type;


 if (grand(eclass[ec].impulse_delay) == 0) 
 {
  accelerate_enemy(e, grand(ANGLE_FULL), eclass[ec].acceleration);
//  switch(enemy[e].type)
//  //{
//  }
 } 

 drag_enemy(e, eclass[ec].drag_amount);

 switch(enemy[e].type)
 {
    case ENEMY_BOSS1_2:
     manage_turret(e, 0, -80 * GRAIN, 0 * GRAIN, 0);
     manage_turret(e, 1, 80 * GRAIN, 0 * GRAIN, 0);
     manage_turret(e, 2, 0, 75 * GRAIN, 0);
     break;
 }

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(eclass[ec].range, enemy[e].x, enemy[e].y);
 }
 
 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].counter % 8 == 0) 
   enemy[e].slew_dir = 0;  
  enemy[e].angle = track_target(e, enemy[e].attacking, eclass[ec].slew, enemy[e].slew_dir * -1);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;
}



int move_periodic_stinger_ai(int e)
{
 int ec = enemy[e].type;


 if (enemy[e].next_impulse == 0) 
 {
  accelerate_enemy(e, grand(ANGLE_FULL), eclass[ec].acceleration);
  enemy[e].next_impulse = eclass[enemy[e].type].impulse_delay;
  switch(enemy[e].type)
  {
   case ENEMY_PUFFER1:
   case ENEMY_PUFFER2:
   case ENEMY_PUFFER3:
   case ENEMY_PUFFER4:
    if (enemy[e].attribute [1] == -1)
     enemy[e].attribute [1] = 1;
      else
       enemy[e].attribute [1] = -1;
    break;
  }
 } 
  else
   enemy[e].next_impulse --; 
  

 drag_enemy(e, eclass[ec].drag_amount);

 switch(enemy[e].type)
 {
  case ENEMY_PUFFER1:
  case ENEMY_PUFFER2:
  case ENEMY_PUFFER3:
  case ENEMY_PUFFER4:
   enemy[e].attribute [0] = (100 * (eclass[enemy[e].type].impulse_delay - enemy[e].next_impulse)) / 12;
      if (eclass[enemy[e].type].impulse_delay - enemy[e].next_impulse < 8)
       enemy[e].attribute [0] = ((100 * (eclass[enemy[e].type].impulse_delay)) / 12)
        / (eclass[enemy[e].type].impulse_delay - enemy[e].next_impulse + 1);
   enemy[e].attribute [2] += enemy[e].attribute [1];
   if (enemy[e].attribute [2] <= 0)
    enemy[e].attribute [2] += 1024;
   break;
 }

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(eclass[ec].range, enemy[e].x, enemy[e].y);
 }
 
 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].counter % 8 == 0) 
   enemy[e].slew_dir = 0;  
  enemy[e].angle = track_target(e, enemy[e].attacking, eclass[ec].slew, enemy[e].slew_dir * -1);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;
}


int move_turret_ai(int e)
{

// switch(enemy[e].type)
// {
// }
int xa, xb, i, bullet_colours [4];

 if (enemy[e].turret_main == -1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1; // should never happen!
 }

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(eclass[enemy[e].type].range, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  if (enemy[e].counter % 8 == 0) 
   enemy[e].slew_dir = 0;  
  enemy[e].angle = track_target(e, enemy[e].attacking, eclass[enemy[e].type].slew, enemy[e].slew_dir * -1);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 switch(enemy[e].type)
 {
  case ENEMY_CRUISER1_TURRET:
  case ENEMY_CRUISER2_TURRET:
  case ENEMY_DEFENDER2_TURRET3:
  case ENEMY_CRUISER3_TURRET:
  case ENEMY_CRUISER4_TURRET:
  case ENEMY_MINER3_TURRET:
  case ENEMY_BOSS2_TURRET4:
  case ENEMY_BOSS3_TURRET3:
     if (enemy[e].attribute [0] > 0)
      enemy[e].attribute [0] --;
   break;
  case ENEMY_DEFENDER1_TURRET3:
  case ENEMY_DEFENDER3_TURRET5:
   enemy[e].angle += enemy[e].attribute [0];
   break; 
  case ENEMY_DEFENDER3_TURRET1:
     if (enemy[e].attribute [0] > 0)
      enemy[e].attribute [0] --;
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
     break;
    case ENEMY_DEFENDER3_TURRET2:
     xa = enemy[e].x - enemy[e].x_speed + xpart(enemy[e].attribute [1], 100000);
     xb = enemy[e].y - enemy[e].y_speed + ypart(enemy[e].attribute [1], 100000);
     enemy[e].attribute [1] = (enemy[e].attribute [1] + enemy[e].attribute [2]) & 1023;    
     bullet_colours [0] = TRANS_YELLOW;
     bullet_colours [1] = TRANS_YELLOW;
     bullet_colours [2] = TRANS_LGREEN;
     bullet_colours [3] = TRANS_DGREEN;
     for (i = 0; i < 2; i ++)
     {
      create_bullet(BULLET_ORBIT, 
       enemy[e].x + xpart(enemy[e].attribute [1] + (ANGLE_HALF) * i, 100000), 
       enemy[e].y + ypart(enemy[e].attribute [1] + (ANGLE_HALF) * i, 100000), 
       0, 0,
       OWNER_ENEMY, 60, 2, 100, 0,
       0, 4000, bullet_colours, 1,
       xa,
       xb,4,6,0);
     }  
     break;
  case ENEMY_DEFENDER3_TURRET3:
   enemy[e].attribute [1] += enemy[e].attribute [2];
   enemy[e].attribute [1] &= 1023;
   break; 
 }   

 return 1;
 
}



void manage_turret(int e, int t, int x, int y, int rotate)
{
 if (enemy[e].turret [t] == -1)
  return;
  
 int te = enemy[e].turret [t];
 
 if (enemy[e].type == ENEMY_NONE)
  return;
 
 enemy[te].x = enemy[e].x + x;
 enemy[te].y = enemy[e].y + y;
 enemy[te].x_speed = enemy[e].x_speed;
 enemy[te].y_speed = enemy[e].y_speed;
 enemy[te].angle += rotate;


}




int move_fighter_ai(int e)
{

 if (enemy[e].attribute [0] == FIGHTER_MOVE)
 {
  if (enemy[e].x / 100000 == enemy[e].x_dest / 100000
     && enemy[e].y / 100000 == enemy[e].y_dest / 100000)
  {
//   enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
   enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
   if (enemy[e].attacking != ATTACK_NONE)
    enemy[e].attribute [0] = FIGHTER_ATTACK;
     else
      {
       enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
       enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
       enemy[e].attacking = ATTACK_NONE;
       enemy[e].attribute [0] = FIGHTER_MOVE;
      }
  }

     enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].x_dest, enemy[e].y_dest,
     enemy[e].angle, eclass[enemy[e].type].slew);
 }
  else
  {
   if (enemy[e].attacking == ATTACK_NONE
       || actor[enemy[e].attacking].in_play == 0)
       // assumes second condition not processed if first met.
   {
    enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
    enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
    enemy[e].attribute [0] = FIGHTER_MOVE;
    enemy[e].attacking = ATTACK_NONE;
    enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, enemy[e].x_dest, enemy[e].y_dest, enemy[e].angle, eclass[enemy[e].type].slew);
   }
  }
 
 drag_enemy(e, eclass[enemy[e].type].drag_amount);

 int ax, ay;
// int x1, y1;
// int passing_colour [5];
 
   ax = xpart(enemy[e].angle, eclass[enemy[e].type].acceleration);
   ay = ypart(enemy[e].angle, eclass[enemy[e].type].acceleration);
  
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

/*  if (enemy[e].type == ENEMY_FIGHTER3)
  {
    passing_colour [0] = TRANS_LGREY;
    passing_colour [1] = TRANS_DGREEN;
    passing_colour [2] = TRANS_LGREEN;
  }
   else*/

  if (enemy[e].attribute [0] == FIGHTER_ATTACK
      && enemy[e].attacking != ATTACK_NONE)
  {
    if (enemy[e].counter % 8 == 0) 
     enemy[e].slew_dir = 0;  
    enemy[e].angle = track_target(e, enemy[e].attacking, eclass[enemy[e].type].slew, enemy[e].slew_dir * -1);
//    enemy[e].angle = track_target(e, enemy[e].attacking, eclass[enemy[e].type].slew, 0);
    if (enemy[e].recycle == 0
            && hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 90000)
    {
     standard_attack(e, enemy[e].angle);
     if (enemy[e].burst_fire <= 1) // experimental!
     {
      enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
      enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
      enemy[e].attribute [0] = FIGHTER_MOVE;
      enemy[e].attacking = ATTACK_NONE;
     }
    }
  }
   else
    if (enemy[e].recycle == 0 && enemy[e].burst_fire > 0)
    {
     standard_attack(e, enemy[e].angle);
    }

  int base_colour = TRANS_YELLOW;
  int xa = 0;
  switch(enemy[e].type)
  {
    case ENEMY_FIGHTER3: 
     base_colour = TRANS_ORANGE;
     break;
    case ENEMY_FIGHTER4: 
     base_colour = TRANS_LGREEN;
     break;
    case ENEMY_MESSENGER:
    // att [0] = fighter ai
    // att [1] = fin angle
    // att [2] = motion dir
    // att [3] = fin slew_dir
     enemy[e].attribute [2] = radians_to_angle(atan2(enemy[e].y_speed, enemy[e].x_speed)) & 1023;
     xa = enemy_turn_towards_angle(enemy[e].attribute [1], enemy[e].attribute [2], 16, enemy[e].attribute [3]);
/*     if (xa < enemy[e].attribute [1] || (enemy[e].attribute [1] < 100 && xa > (ANGLE_FULL - 100)))
      enemy[e].attribute [3] = 1;
       else
        enemy[e].attribute [3] = -1; // wingfin slew_dir*/
   if (enemy[e].counter % 16 == 0)   
    enemy[e].attribute [3] = 0;
     enemy[e].attribute [1] = xa; 
     if (enemy[e].attribute [0] == FIGHTER_MOVE)
     {
      if (enemy[e].attribute [4] != -1 && enemy[enemy[e].attribute [4]].type == ENEMY_BOSS3_1 && enemy[e].counter % 97 == 0)
      {
       enemy[e].x_dest = enemy[enemy[e].attribute [4]].x;
       enemy[e].y_dest = enemy[enemy[e].attribute [4]].y;
      }
     }
     return 1; // no exhaust
  }
  int distance [4] = {10000, 10000, 0, 0};
  int angle [4] = {ANGLE_HALF - ANGLE_1_EIGHTH, ANGLE_HALF + ANGLE_1_EIGHTH, 0, 0};
  fighter_exhaust(e, ax, ay, distance, angle, base_colour);

 return 1;

}

int move_leaper_ai(int e)
{

 if (enemy[e].attribute [0] == FIGHTER_MOVE)
 {
  if (enemy[e].x / 100000 == enemy[e].x_dest / 100000
     && enemy[e].y / 100000 == enemy[e].y_dest / 100000)
  {
   enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
   if (enemy[e].attacking != ATTACK_NONE)
    enemy[e].attribute [0] = FIGHTER_ATTACK;
     else
      {
       enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
       enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
       enemy[e].attacking = ATTACK_NONE;
       enemy[e].attribute [0] = FIGHTER_MOVE;
      }
  }

     enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].x_dest, enemy[e].y_dest,
     enemy[e].angle, eclass[enemy[e].type].slew);
 }
  else
  {
   if (enemy[e].attacking == ATTACK_NONE
       || actor[enemy[e].attacking].in_play == 0)
       // assumes second condition not processed if first met.
   {
    enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
    enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
    enemy[e].attribute [0] = FIGHTER_MOVE;
    enemy[e].attacking = ATTACK_NONE;
    enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, enemy[e].x_dest, enemy[e].y_dest, enemy[e].angle, eclass[enemy[e].type].slew);
   }
  }
 
 drag_enemy(e, eclass[enemy[e].type].drag_amount);

// int ax, ay;

 if (enemy[e].next_impulse == 0) 
 {
  accelerate_enemy(e, enemy[e].angle, eclass[enemy[e].type].acceleration);
  enemy[e].next_impulse = eclass[enemy[e].type].impulse_delay;
     simple_cloud_trans(TRANS_YELLOW, enemy[e].x + xpart(enemy[e].angle + ANGLE_HALF, 12000), 
                                     enemy[e].y + ypart(enemy[e].angle + ANGLE_HALF, 12000),
                                     enemy[e].x_speed + xpart(enemy[e].angle + ANGLE_HALF, 4000),
                                     enemy[e].y_speed + ypart(enemy[e].angle + ANGLE_HALF, 4000),
                                     900);
     simple_cloud_trans(TRANS_ORANGE, enemy[e].x + xpart(enemy[e].angle + ANGLE_HALF, 12000), 
                                     enemy[e].y + ypart(enemy[e].angle + ANGLE_HALF, 12000),
                                     enemy[e].x_speed + xpart(enemy[e].angle + ANGLE_HALF, 4000),
                                     enemy[e].y_speed + ypart(enemy[e].angle + ANGLE_HALF, 4000),
                                     1200);
      simple_light(enemy[e].x + xpart(enemy[e].angle + ANGLE_HALF, 12000), 
                   enemy[e].y + ypart(enemy[e].angle + ANGLE_HALF, 12000),
                   enemy[e].x_speed + xpart(enemy[e].angle + ANGLE_HALF, 4000),
                   enemy[e].y_speed + ypart(enemy[e].angle + ANGLE_HALF, 4000),     
                   1400);
                   

 } 
  else
   enemy[e].next_impulse --; 
   

  if (enemy[e].attribute [0] == FIGHTER_ATTACK
      && enemy[e].attacking != ATTACK_NONE)
  {
    if (enemy[e].counter % 16 == 0) 
     enemy[e].slew_dir = 0;  
    enemy[e].angle = track_target(e, enemy[e].attacking, eclass[enemy[e].type].slew, enemy[e].slew_dir * -1);
    if (enemy[e].recycle == 0
            && hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 90000)
    {
     standard_attack(e, enemy[e].angle);
     if (enemy[e].burst_fire <= 1) // experimental!
     {
      enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
      enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
      enemy[e].attribute [0] = FIGHTER_MOVE;
      enemy[e].attacking = ATTACK_NONE;
     }
    }
  }
   else
    if (enemy[e].recycle == 0 && enemy[e].burst_fire > 0)
    {
     standard_attack(e, enemy[e].angle);
    }


 return 1;

}



int move_bomber_ai(int e)
{

 if (enemy[e].attribute [0] == FIGHTER_MOVE)
 {
  if (enemy[e].x / 100000 == enemy[e].x_dest / 100000
     && enemy[e].y / 100000 == enemy[e].y_dest / 100000)
  {
//   enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
   enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
   if (enemy[e].attacking != ATTACK_NONE)
    enemy[e].attribute [0] = FIGHTER_ATTACK;
     else
      {
       enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
       enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
       enemy[e].attacking = ATTACK_NONE;
       enemy[e].attribute [0] = FIGHTER_MOVE;
      }
  }

     enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].x_dest, enemy[e].y_dest,
     enemy[e].angle, eclass[enemy[e].type].slew);
 }
  else
  {
   if (enemy[e].attacking == ATTACK_NONE
       || actor[enemy[e].attacking].in_play == 0)
       // assumes second condition not processed if first met.
   {
    enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
    enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
    enemy[e].attribute [0] = FIGHTER_MOVE;
    enemy[e].attacking = ATTACK_NONE;
    enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y, enemy[e].x_dest, enemy[e].y_dest, enemy[e].angle, eclass[enemy[e].type].slew);
   }
  }
 
 drag_enemy(e, eclass[enemy[e].type].drag_amount);

 int ax, ay;
// int x1, y1;
// int passing_colour [5];
 
   ax = xpart(enemy[e].angle, eclass[enemy[e].type].acceleration);
   ay = ypart(enemy[e].angle, eclass[enemy[e].type].acceleration);
  
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

/*  if (enemy[e].type == ENEMY_FIGHTER3)
  {
    passing_colour [0] = TRANS_LGREY;
    passing_colour [1] = TRANS_DGREEN;
    passing_colour [2] = TRANS_LGREEN;
  }
   else*/
/*   switch(enemy[e].type)
   {
    default:
     passing_colour [0] = TRANS_DRED;
     passing_colour [1] = TRANS_LRED;
     passing_colour [2] = TRANS_YELLOW;
     break;
   }

  if (grand(3) == 0)
  {
   x1 = enemy[e].x + xpart(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_32, 12000);
   y1 = enemy[e].y + ypart(enemy[e].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_32, 12000);
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 160, passing_colour);
  }
  if (grand(3) == 0)
  {
   x1 = enemy[e].x + xpart(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_32, 12000);
   y1 = enemy[e].y + ypart(enemy[e].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_32, 12000);
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 160, passing_colour);
  }
  if (grand(3) == 0)
  {
   x1 = enemy[e].x + xpart(enemy[e].angle + ANGLE_HALF, 12000);
   y1 = enemy[e].y + ypart(enemy[e].angle + ANGLE_HALF, 12000);
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 160, passing_colour);
  }
*/

  if (enemy[e].attribute [0] == FIGHTER_ATTACK
      && enemy[e].attacking != ATTACK_NONE)
  {
    if (enemy[e].counter % 8 == 0) 
     enemy[e].slew_dir = 0;  
    enemy[e].angle = track_target(e, enemy[e].attacking, eclass[enemy[e].type].slew, enemy[e].slew_dir * -1);
//    enemy[e].angle = track_target(e, enemy[e].attacking, eclass[enemy[e].type].slew, 0);
    if (enemy[e].recycle == 0
            && hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 150000)
    {
     standard_attack(e, enemy[e].angle);
     if (enemy[e].burst_fire <= 1) // experimental!
     {
      enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
      enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
      enemy[e].attribute [0] = FIGHTER_MOVE;
      enemy[e].attacking = ATTACK_NONE;
     }
    }
  }
   else
    if (enemy[e].recycle == 0 && enemy[e].burst_fire > 0)
    {
     standard_attack(e, enemy[e].angle);
    }
  
  int base_colour = TRANS_YELLOW;
  switch(enemy[e].type)
  {
/*    case ENEMY_FIGHTER3: 
     base_colour = TRANS_ORANGE;
     break;
    case ENEMY_FIGHTER4: 
     base_colour = TRANS_LGREEN;
     break;*/
  }
  int distance [4] = {12000, 12000, 12000, 0};
  int angle [4] = {ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_32, ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_32, ANGLE_HALF, 0};
  fighter_exhaust(e, ax, ay, distance, angle, base_colour);


 return 1;

}

int move_cruiser_ai(int e)
{

  if (enemy[e].x / 100000 == enemy[e].x_dest / 100000
     && enemy[e].y / 100000 == enemy[e].y_dest / 100000)
  {
//   enemy[e].attacking = nearby_target(400000, enemy[e].x, enemy[e].y);
/*   enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);
   if (enemy[e].attacking != ATTACK_NONE)
    enemy[e].attribute [0] = FIGHTER_ATTACK;
     else
      {
       enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
       enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
       enemy[e].attacking = ATTACK_NONE;
       enemy[e].attribute [0] = FIGHTER_MOVE;
      }*/
      enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
      enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
  }
int angles = enemy[e].angle;

     enemy[e].angle = turn_towards_xy(enemy[e].x, enemy[e].y,
     enemy[e].x_dest, enemy[e].y_dest,
     enemy[e].angle, eclass[enemy[e].type].slew);
     
     angles = enemy[e].angle - angles;//angle_difference(enemy[e].angle, angles);
     if (angles >= ANGLE_HALF)
      angles -= ANGLE_FULL;

 drag_enemy(e, eclass[enemy[e].type].drag_amount);

 int ax, ay;
// int x1, y1;
// int passing_colour [5];
 
   ax = xpart(enemy[e].angle, eclass[enemy[e].type].acceleration);
   ay = ypart(enemy[e].angle, eclass[enemy[e].type].acceleration);
  
  enemy[e].x_speed += ax;
  enemy[e].y_speed += ay;

//  if (enemy[e].attribute [0] == FIGHTER_ATTACK
//      && enemy[e].attacking != ATTACK_NONE)
//  {
//    if (enemy[e].counter % 8 == 0) 
//     enemy[e].slew_dir = 0;  
//    enemy[e].angle = track_target(e, enemy[e].attacking, eclass[enemy[e].type].slew, enemy[e].slew_dir * -1);


 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(eclass[enemy[e].type].range, enemy[e].x, enemy[e].y);
 }

    if (enemy[e].recycle == 0
            && hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) < 150000)
    {
     standard_attack(e, enemy[e].angle);
/*     if (enemy[e].burst_fire <= 1) // experimental!
     {
      enemy[e].x_dest = grand(arena.max_x - 40000) + 20000;
      enemy[e].y_dest = grand(arena.max_y - 40000) + 20000;
      enemy[e].attribute [0] = FIGHTER_MOVE;
      enemy[e].attacking = ATTACK_NONE;
     }*/
    }
//  }
//   else

/*    if (enemy[e].recycle == 0)// && enemy[e].burst_fire > 0)
    {
     standard_attack(e, enemy[e].angle);
    }*/
  
  int base_colour = TRANS_YELLOW;
  int distance [4] = {25000, 25000, 0, 0};
//  int angle [4] = {ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_32, ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_32, 0, 0};
  int angle [4] = {ANGLE_HALF - ANGLE_1_EIGHTH, ANGLE_HALF + ANGLE_1_EIGHTH, 0, 0};
  switch(enemy[e].type)
  {
    case ENEMY_CRUISER4:
    case ENEMY_CRUISER3: 
     base_colour = TRANS_ORANGE;
     distance [2] = 25000;
     distance [3] = 25000;
     angle [2] = ANGLE_QUARTER;
     angle [3] = -ANGLE_QUARTER;
     break;
  }
  
  fighter_exhaust(e, ax, ay, distance, angle, base_colour);

 switch(enemy[e].type)
 {
    case ENEMY_CRUISER1:
    case ENEMY_CRUISER2:
    case ENEMY_CRUISER3:
    case ENEMY_CRUISER4:
     manage_turret(e, 0, xpart(enemy[e].angle + ANGLE_HALF, 18000), ypart(enemy[e].angle + ANGLE_HALF, 18000), angles);
     break;
 }


 return 1;

}


void fighter_exhaust(int e, int ax, int ay, int distance [4], int angle [4], int base_colour)
{
   int passing_colour [5];

   switch(base_colour)
   {
    case TRANS_YELLOW:
     passing_colour [0] = TRANS_DRED;
     passing_colour [1] = TRANS_LRED;
     passing_colour [2] = TRANS_YELLOW;
     break;
    case TRANS_ORANGE: 
     passing_colour [0] = TRANS_LRED;
     passing_colour [1] = TRANS_ORANGE;
     passing_colour [2] = TRANS_YELLOW;
     break;
    case TRANS_LGREEN: 
     passing_colour [0] = TRANS_DGREEN;
     passing_colour [1] = TRANS_LGREEN;
     passing_colour [2] = TRANS_YELLOW;
     break;
/*    case ENEMY_FIGHTER3: 
     passing_colour [0] = TRANS_LRED;
     passing_colour [1] = TRANS_ORANGE;
     passing_colour [2] = TRANS_YELLOW;
     break;
    case ENEMY_FIGHTER4: 
     passing_colour [0] = TRANS_DGREEN;
     passing_colour [1] = TRANS_LGREEN;
     passing_colour [2] = TRANS_YELLOW;
     break;*/
   }

  int i, x1, y1;
  
  for (i = 0; i < 4; i ++)
  {
   if (distance [i] == 0 || grand(2) != 0)
    continue;
   x1 = enemy[e].x + xpart(enemy[e].angle + angle [i], distance [i]);
   y1 = enemy[e].y + ypart(enemy[e].angle + angle [i], distance [i]);
   place_rocket_trail(x1 + grand(3001) - 1500, y1 + grand(3001) - 1500, enemy[e].x_speed - ax*10, enemy[e].y_speed - ay*10, 160, passing_colour);
  }


}


void run_boss3_1(int e)
{

 enemy[e].recycle = 10;
 if (enemy[e].counter % 32 != 0)
  return;
 
 if (enemy[e].attribute [0] < 10)
 {
  enemy[e].attribute [0] ++; 
  return;
 }  
    
 int i, mess = 0;
 
  for (i = 0; i < NO_ENEMIES; i ++)
  {
   if (enemy[i].type == ENEMY_MESSENGER)
    mess ++;
  }
  
  if (mess >= 4)
   return;

  int special [10]; 
   
  special [4] = e;
   
 create_enemy(ENEMY_MESSENGER, 0, enemy[e].x, enemy[e].y,
  enemy[e].x_speed, enemy[e].y_speed, grand(ANGLE_FULL), 0, special, TARGET_NO);

 int passing_colours [5];

     passing_colours[0] = TRANS_DBLUE;
     passing_colours[1] = TRANS_PURPLE;
     passing_colours[2] = TRANS_WHITE;
   place_explosion(enemy[e].x, enemy[e].y, 0,0,
    700 + crandom(250), passing_colours);
   place_burstlet_burst(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 4 + grand(3), 2, 6, 2000, 2500,
    4, passing_colours);


  enemy[e].attribute [0] = 0;

}


void run_boss3_3(int e)
{

 if (enemy[e].attribute [0] == 0)
  return;

 enemy[e].attribute [0] --;

 if (enemy[e].attribute [0] % 5 != 0)
  return;

 int i, b = -1, ang;
 
 for (i = 0; i < NO_BULLETS; i ++)
 {
  if (bullet[i].type == BULLET_SWIRL1)
  {
   b = i;
   break;
  } 
 }
 
 if (b == -1)
  return;
  
  int colours [4] = {COLOUR_RED6, COLOUR_ORANGE6, COLOUR_ORANGE8, COLOUR_YELLOW8};
  
 for (i = 0; i < 4; i ++)
 { 

  switch(i)
  {
   default:
   case 0: ang = - ANGLE_QUARTER + ANGLE_1_EIGHTH; break;
   case 1: ang = + ANGLE_QUARTER - ANGLE_1_EIGHTH; break;
   case 2: ang = ANGLE_QUARTER; break;
   case 3: ang = - ANGLE_QUARTER; break;
  }
  
      create_bullet(BULLET_SWIRL2, 
       enemy[e].x + xpart(enemy[e].angle + ang, 35000), 
       enemy[e].y + ypart(enemy[e].angle + ang, 35000), 
       enemy[e].x_speed + xpart(enemy[e].angle + ang, 3000), 
       enemy[e].y_speed + ypart(enemy[e].angle + ang, 3000),
       OWNER_ENEMY, 100, 100 + grand(30), 10, enemy[e].angle + ang + ANGLE_QUARTER,
       0, 1000, colours, 1,
       1290,0,b,36,3);

     simple_cloud_trans(TRANS_YELLOW, enemy[e].x + xpart(enemy[e].angle + ang, 35000), 
                                     enemy[e].y + ypart(enemy[e].angle + ang, 35000),
                                     0,0,
//                                     enemy[e].x_speed + xpart(enemy[e].angle + ang, 1000),
//                                     enemy[e].y_speed + ypart(enemy[e].angle + ang, 1000),
                                     400);
       
  }    

}

/*
int move_firebase(int e)
{

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 32 == 0)
  enemy[e].attacking = closest_target(enemy[e].x, enemy[e].y);

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}

int move_firebase_turret(int e)
{

 if (enemy[e].attribute [ATTRIB_FBTURRET_MAIN_DEAD] == 1)
 {
  hurt_enemy(e, 99999, OWNER_ENEMY, 0);
  return -1;
 }

 enemy[e].x_speed = 0;
 enemy[e].y_speed = 0;

 if (enemy[e].counter % 16 == 0)
 {
  enemy[e].attacking = nearby_target(200000, enemy[e].x, enemy[e].y);
 }

 if (enemy[e].attacking != ATTACK_NONE)
 {
  enemy[e].angle = track_target(e, enemy[e].attacking, 16);
  if (enemy[e].recycle == 0)
   standard_attack(e, enemy[e].angle);
 }

 return 1;

}
*/


void colourise_bullet(int arr [4], int tc)
{
 switch(tc)
 {
  case TRANS_YELLOW:
   arr [0] = TRANS_WHITE;
   arr [1] = TRANS_YELLOW;
   arr [2] = TRANS_ORANGE;
   arr [3] = TRANS_LRED;
   break; 
  case TRANS_LBLUE:
   arr [0] = TRANS_WHITE;
   arr [1] = TRANS_LBLUE;
   arr [2] = TRANS_DBLUE;
   arr [3] = TRANS_DGREY;
   break; 
  case TRANS_PURPLE:
   arr [0] = TRANS_WHITE;
   arr [1] = TRANS_PURPLE;
   arr [2] = TRANS_LBLUE;
   arr [3] = TRANS_DBLUE;
   break; 
  case TRANS_LGREEN:
   arr [0] = TRANS_YELLOW;
   arr [1] = TRANS_LGREEN;
   arr [2] = TRANS_DGREEN;
   arr [3] = TRANS_DGREY;
   break; 
  case TRANS_ORANGE:
   arr [0] = TRANS_YELLOW;
   arr [1] = TRANS_ORANGE;
   arr [2] = TRANS_LRED;
   arr [3] = TRANS_DRED;
   break;
  case COLOURISE_GREEN_SEEKER1:
   arr [0] = TRANS_YELLOW;
   arr [1] = TRANS_YELLOW;
   arr [2] = TRANS_LGREEN;
   arr [3] = TRANS_LGREEN;
   break;
  case COLOURISE_GREEN_SEEKER2:
   arr [0] = TRANS_LGREEN;
   arr [1] = TRANS_LGREEN;
   arr [2] = TRANS_LGREEN;
   arr [3] = TRANS_DGREEN;
   break;
  case COLOURISE_BLUE_SEEKER:
   arr [0] = TRANS_LBLUE;
   arr [1] = TRANS_LBLUE;
   arr [2] = TRANS_LBLUE;
   arr [3] = TRANS_DBLUE;
   break;
  case COLOURISE_YELLOW_SEEKER:
   arr [0] = TRANS_WHITE;
   arr [1] = TRANS_WHITE;
   arr [2] = TRANS_YELLOW;
   arr [3] = TRANS_ORANGE;
   break;

 }

}


void standard_attack(int e, int angle)
{

 int timer;
 int timer_rand = 0;
 int special_angle = -1;
 int special_angle2 = -1;

 int speed = 0;
 int speed_rand = 0;
 int speed_div = 5;
 int mass;
 int status;
 int displaced = 0;
 int colours [4] = {GC_GREEN8, GC_GREEN6, GC_GREEN4, GC_GREEN2};
 int colours_for_cloud [5] = {GC_GREEN8, GC_GREEN6, GC_GREEN4, GC_GREEN2, GC_GREEN1};
 int firing_distance = 0;
 int btype;
 int damage;
 int angle_rand = 0;
 int special1 = 0;
 int special2 = 0;
 int special3 = 0;
 int special4 = 0;
 int special5 = 0;
 int scatter = 0;

 int multiple = 1;
 int angle_increment = 0;

 int displace_series [4] = {0,0,0,0};

 int firing_angle;

 int recoil = 0;
 int size = 0;
// int t;

   if (enemy[e].burst_fire == 1)
    enemy[e].burst_fire = 0;

 switch(enemy[e].type)
 {
  case ENEMY_BEAMER1:
   enemy[e].attribute [0] = 150;
   enemy[e].recycle = 250;
   return;
// NOTE return! not break
  case ENEMY_BEAMER2:
   enemy[e].attribute [0] = 150;
   enemy[e].recycle = 230;
   return;
// NOTE return! not break
  case ENEMY_CIRCLER1:
   if (enemy[e].attacking != 0 && enemy[e].attacking != 1)
    return;
   if (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) > 350000)
    return;
//   create_cloud(
   create_bullet(BULLET_CIRCLER, actor[enemy[e].attacking].x + actor[enemy[e].attacking].x_speed * (30 + grand(30)) + grand(25001) - 12500, 
    actor[enemy[e].attacking].y + actor[enemy[e].attacking].y_speed * (30 + grand(30)) + grand(25001) - 12500, 
    0, 0, OWNER_ENEMY,
    0, 50, 0, 0, // 50 was 66
    0, 0, colours, 1, special1,special2,special3,special4,special5);
    enemy[e].recycle = 80;
    enemy[e].attribute [0] = 50; // was 66
   return;
// NOTE return! not break   
  case ENEMY_GUARDIAN1:
   btype = BULLET_BALL1;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
/*   colours [0] = GC_GREEN8;
   colours [1] = GC_GREEN6;
   colours [2] = GC_GREEN4;
   colours [3] = GC_GREEN2;*/
   firing_distance = 14000;
   enemy[e].recycle = 80;
   speed_div = 3;
   multiple = 1;
//   angle_increment = ANGLE_1_EIGHTH;
   recoil = 700;
   size = 3000;
   enemy_soundf(e, NWAV_BALL1, 500 + grand(30)); 
   break;
  case ENEMY_GUARDIAN2:
   btype = BULLET_TWIRLY1;
   size = 7000;
   damage = 200;
   timer = 40;
   timer_rand = 30;
   mass = 25;
   speed = 6000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 14000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 1;
//   angle_increment = ANGLE_1_EIGHTH;
   recoil = 700;
   special1 = grand(2);
   enemy_soundf(e, NWAV_BALL1, 400 + grand(30)); 
   break;
  case ENEMY_GUARDIAN3:
   btype = BULLET_TWISTY;
   size = 5000; // see bullet.c; this changes
   damage = 200;
   timer = 35;
   timer_rand = 15;
   mass = 40;
   speed = 7000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 14000;
   enemy[e].recycle = 50;
   speed_div = 3;
   multiple = 1;
   recoil = 700;
   special1 = 0;
   special2 = 1;
   if (grand(2) == 0)
    special2 = -1;
   enemy_soundf(e, NWAV_BALL1, 600 + grand(30)); 
   break;
  case ENEMY_GUARDIAN4:
   btype = BULLET_TWIRLY2;
   size = 5000;
   damage = 250;
   timer = 43;
   timer_rand = 30;
   mass = 40;
   speed = 7000;
   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_YELLOW;
   colours [2] = TRANS_ORANGE;
   colours [3] = TRANS_LRED;
   firing_distance = 14000;
   enemy[e].recycle = 55;
   speed_div = 3;
   multiple = 1;
   recoil = 700;
   enemy_soundf(e, NWAV_BALL1, 700 + grand(30)); 
   break;
  case ENEMY_GUARDIAN5:
   btype = BULLET_TWIRLY3;
   size = 12000;
   damage = 400;
   timer = 43;
   timer_rand = 30;
   mass = 50;
   speed = 6000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREEN;
   firing_distance = 14000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 1;
   recoil = 1200;
   special2 = 0;
   if (grand(2) == 0)
    special2 = 1;
   enemy_soundf(e, NWAV_BALL1, 300 + grand(10)); 
   break;
//  case ENEMY_HEAD1_EYE1: 
//case ENEMY_HEAD1: 
  case ENEMY_CURVE1:
   btype = BULLET_BLAST;
   damage = 200;
   timer = 150;
   timer_rand = 20;
   mass = 40;
   speed = 3000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 1000;
   enemy[e].recycle = 150;
   speed_div = 3;
   multiple = 2;
   displaced = 12;
   displace_series [0] = 1;
   displace_series [1] = -1;
   recoil = 1200;
   enemy_soundf(e, NWAV_LZAP, 900 + grand(100)); 
   size = 2000;
   break;
  case ENEMY_CURVE2:
   btype = BULLET_LINE_PULSE;
   damage = 250;
   timer = 150;
   timer_rand = 20;
   mass = 40;
   speed = 6000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_YELLOW;
   colours [2] = TRANS_ORANGE;
   colours [3] = TRANS_LRED;
/*   colours [0] = TRANS_LBLUE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE;
   colours [3] = TRANS_DGREY;*/
   firing_distance = 1000;
   enemy[e].recycle = 100;
   speed_div = 3;
   multiple = 2;
   displaced = 12;
   displace_series [0] = 1;
   displace_series [1] = -1;
   recoil = 1200;
   enemy_soundf(e, NWAV_LZAP, 400 + grand(50)); 
//   enemy_soundf(e, NWAV_MULTI, 400 + grand(10)); 
   size = 3000;
   break;
  case ENEMY_CURVE3:
   btype = BULLET_CURVEY;
   damage = 300;
   timer = 150;
   timer_rand = 20;
   mass = 40;
   speed = 9000;
   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_LGREY;
   colours [2] = TRANS_DGREY;
   colours [3] = TRANS_DGREY;
/*   colours [0] = TRANS_LBLUE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE;
   colours [3] = TRANS_DGREY;*/
   firing_distance = 1000;
   enemy[e].recycle = 100;
   speed_div = 3;
   multiple = 2;
   displaced = 12;
   displace_series [0] = 1;
   displace_series [1] = -1;
   recoil = 1200;
   enemy_soundf(e, NWAV_LZAP, 200 + grand(20)); 
//   enemy_soundf(e, NWAV_MULTI, 400 + grand(10)); 
   size = 3000;
   break;
  case ENEMY_MULTI1:
   btype = BULLET_BALL1;
   damage = 150;
   timer = 40;
   timer_rand = 10;
   mass = 10;
   speed = 4000;
/*   colours [0] = GC_GREEN8;
   colours [1] = GC_GREEN6;
   colours [2] = GC_GREEN4;
   colours [3] = GC_GREEN2;*/
   firing_distance = 10000;
   enemy[e].recycle = 110;
   speed_div = 3;
   multiple = 3;
   angle_increment = ANGLE_1_EIGHTH;
   angle -= ANGLE_1_EIGHTH;
   recoil = 900;
   size = 3000;
   enemy_soundf(e, NWAV_MULTI, 500 + grand(10)); 
   break;
  case ENEMY_MULTI2:
   btype = BULLET_BLAST;
   damage = 200;
   timer = 60;
   timer_rand = 20;
   mass = 40;
   speed = 5000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 8000;
   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 4;
   angle_increment = ANGLE_1_SIXTEENTH;
   angle -= (ANGLE_1_SIXTEENTH + ANGLE_1_32);
   recoil = 1200;
   enemy_soundf(e, NWAV_MULTI, 400 + grand(10)); 
   size = 2000;
   break;
  case ENEMY_MULTI3:
   btype = BULLET_BLAST;
   damage = 250;
   timer = 60;
   timer_rand = 20;
   mass = 40;
   speed = 6000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREY;
   firing_distance = 11000;
   enemy[e].recycle = 100;
   speed_div = 3;
   multiple = 5;
   angle_increment = ANGLE_1_SIXTEENTH;
   angle -= ANGLE_1_EIGHTH;//(ANGLE_1_SIXTEENTH + ANGLE_1_32);
   recoil = 1800;
   enemy_soundf(e, NWAV_MULTI, 300 + grand(10)); 
   size = 2000;
   break;
  case ENEMY_PUFFER1:
   btype = BULLET_BURNING_SPIRIT;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 6000;
   firing_distance = 11000;
   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DRED;
   colours_for_cloud [1] = TRANS_LRED;
   colours_for_cloud [2] = TRANS_YELLOW;
   recoil = 1200;
   enemy_soundf(e, NWAV_PUFF, 900 + grand(200)); 
   break;
  case ENEMY_PUFFER2:
   btype = BULLET_BURST;
   damage = 1;
   size = 3000;
   timer = 30;
   timer_rand = 35;
   mass = 10;
   speed = 7000;
   firing_distance = 14000;
   enemy[e].recycle = 110;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DRED;
   colours_for_cloud [1] = TRANS_LRED;
   colours_for_cloud [2] = TRANS_YELLOW;
   recoil = 1700;
   enemy_soundf(e, NWAV_PUFF, 700 + grand(150)); 
   break;
  case ENEMY_PUFFER3:
   btype = BULLET_PUFFY3;
   damage = 1;
   size = 3000;
   timer = 30;
   timer_rand = 35;
   mass = 10;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 110;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   colours [0] = TRANS_DGREEN;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_YELLOW;
   colours [3] = TRANS_YELLOW;
   recoil = 1700;
   enemy_soundf(e, NWAV_PUFF, 1300 + grand(200)); 
   break;
  case ENEMY_PUFFER4:
   btype = BULLET_PUFFY4;
   damage = 1;
   size = 3000;
   timer = 30;
   timer_rand = 35;
   mass = 10;
   speed = 8000;
   firing_distance = 20000;
   enemy[e].recycle = 110;
   speed_div = 3;
   multiple = 1;
/*   colours [0] = COLOUR_BLUE8;
   colours [1] = COLOUR_BLUE7;
   colours [2] = COLOUR_BLUE6;
   colours [3] = COLOUR_BLUE5;*/
   colours [0] = COLOUR_GREY6;
   colours [1] = COLOUR_BLUE8;
   colours [2] = COLOUR_BLUE7;
   colours [3] = COLOUR_BLUE5;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
//   colours [4] = COLOUR_BLUE4;
   recoil = 1700;
   enemy_soundf(e, NWAV_PUFF, 500 + grand(50)); 
   break;
  case ENEMY_SHADOW1:
   btype = BULLET_CRYSTAL1;
   damage = 1;
   size = 6000;
   timer = 30;
   timer_rand = 35;
   mass = 10;
   speed = 8000;
   firing_distance = 12000;
   speed_div = 3;
   multiple = 1;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   recoil = 1100;
   scatter = ANGLE_1_32;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 150 + grand(150);
     enemy[e].burst_fire = 3; // also in creation
    }
//   enemy[e].attribute [0] = 60;
   enemy_soundf(e, NWAV_SHADOW, 1500 + grand(50)); 
   break;
  case ENEMY_SHADOW2:
   btype = BULLET_CRYSTAL2;
   damage = 1;
   size = 6000;
   timer = 30;
   timer_rand = 35;
   mass = 10;
   speed = 9000;
   firing_distance = 12000;
   speed_div = 3;
   multiple = 1;
   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_LBLUE;
   colours [3] = TRANS_DBLUE;
   recoil = 1100;
   scatter = ANGLE_1_32;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 150 + grand(150);
     enemy[e].burst_fire = 3; // also in creation
    }
//   enemy[e].attribute [0] = 60;
   enemy_soundf(e, NWAV_SHADOW, 1300 + grand(50)); 
   break;
  case ENEMY_DISRUPTER1:
  case ENEMY_DISRUPTER2:
  case ENEMY_DISRUPTER3:
  case ENEMY_OVERDISRUPTER:
   firing_distance = 12000;
   if (enemy[e].type == ENEMY_OVERDISRUPTER)
    firing_distance = 26000;
  if (enemy[e].type == ENEMY_DISRUPTER1 || (enemy[e].type == ENEMY_OVERDISRUPTER && grand(3) == 0))
  {
   btype = BULLET_DISRUPT2;
   damage = 1;
   size = 3000;
   timer = 100;
   if (enemy[e].attacking == 0 || enemy[e].attacking == 1)
    timer = (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) / 4000);
   timer_rand = 5;
   mass = 10;
   speed = 4000;
   enemy[e].recycle = 120;
   colours_for_cloud [0] = TRANS_REVERSE;
   speed_div = 3;
   multiple = 1;
   recoil = -1200;
   enemy_soundf(e, NWAV_CHIME, 1000 + grand(50)); 
   break;
  } 
  if (enemy[e].type == ENEMY_DISRUPTER2 || (enemy[e].type == ENEMY_OVERDISRUPTER && grand(2) == 0))
  {
   btype = BULLET_DISRUPT1;
   damage = 1;
   size = 3000;
   timer = 100;
   if (enemy[e].attacking == 0 || enemy[e].attacking == 1)
    timer = (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) / 3000);
   timer_rand = 5;
   mass = 10;
   speed = 3000;
   enemy[e].recycle = 120;
//   colours_for_cloud [0] = TRANS_REVERSE;
   speed_div = 3;
   multiple = 1;
   recoil = 1200;
   enemy_soundf(e, NWAV_CYMBAL, 1000 + grand(50)); 
   break;
  } 
   btype = BULLET_DISRUPT3;
   damage = 1;
   size = 3000;
   timer = 100;
   if (enemy[e].attacking == 0 || enemy[e].attacking == 1)
    timer = (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) / 7000);
   timer_rand = 5;
   mass = 10;
   speed = 7000;
   colours_for_cloud [0] = TRANS_DARKEN;
   enemy[e].recycle = 120;
   speed_div = 3;
   multiple = 1;
   recoil = 1200;
   enemy_soundf(e, NWAV_CHIME, 400 + grand(50)); 
   break;
  case ENEMY_BRACKET1:
   recoil = 300;
  case ENEMY_DEFENDER2_TURRET1: 
   btype = BULLET_GREEN_BLAT;
   damage = DAM_GREEN_BLAT;
   timer = 30;
   timer_rand = 15;
   mass = 10;
   speed = 10000;
   firing_distance = 1000;
//   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 2;
   displaced = 6;
   displace_series [0] = 1;
   displace_series [1] = -1;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 160;
     enemy[e].burst_fire = 3 + grand(3); // also in creation
    }
   enemy_soundf(e, NWAV_GBLAT, 400); 
   break;
  case ENEMY_BRACKET2:
   recoil = 150;
  case ENEMY_DEFENDER2_TURRET2: 
   btype = BULLET_GREEN_BLAT;
   damage = DAM_GREEN_BLAT;
   timer = 30;
   timer_rand = 15;
   mass = 10;
   speed = 13000;
   firing_distance = 2000;
//   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   displaced = 5;
   displace_series [0] = 1;
   if (enemy[e].burst_fire % 2 == 0)
   {
    displace_series [0] = -1;
    enemy[e].angle -= grand(ANGLE_1_SIXTEENTH);
   } 
    else
     enemy[e].angle += grand(ANGLE_1_SIXTEENTH);
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 4;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 140;
     enemy[e].burst_fire = 6 + grand(6); // also in creation
    }
   enemy_soundf(e, NWAV_GBLAT, 500); 
   break;
  case ENEMY_BRACKET4: 
  case ENEMY_BRACKET3:
   recoil = 100;
//  case ENEMY_DEFENDER2_TURRET3: 
   btype = BULLET_GREEN_BLAT;
   damage = DAM_GREEN_BLAT;
   timer = 40;
   timer_rand = 15;
   mass = 10;
   speed = 12000;
   firing_distance = 2000;
   speed_div = 3;
   multiple = 2;
   displaced = 6;
   displace_series [0] = 1;
   displace_series [1] = -1;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   enemy[e].recycle = 15;
   enemy_soundf(e, NWAV_GBLAT, 400); 
   break;
  case ENEMY_BRACKET5:
   btype = BULLET_YELLOW_FLAK;
   damage = 100;
   timer = 4;
   timer_rand = 19;
   mass = 10;
   speed = 9000;
   firing_distance = 2000;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_ORANGE;
   colours_for_cloud [1] = TRANS_YELLOW;
   colours_for_cloud [2] = TRANS_WHITE;
   recoil = 300;
   enemy[e].recycle = 15;
//   enemy[e].attribute [0] = 7;
   speed_div = 3;
   multiple = 2;
   displaced = 6;
   displace_series [0] = 1;
   displace_series [1] = -1;
   enemy_soundf(e, NWAV_GBLAT, 300); 
   break;
  case ENEMY_TRIANGLER1:
   recoil = 100;
   btype = BULLET_TRI1;
   damage = 1;
   timer = 60;
   if (enemy[e].attacking == 0 || enemy[e].attacking == 1)
    timer = (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) / 6500) + 3;
   timer_rand = 15;
   mass = 10;
   speed = 2000;
   firing_distance = 15000;
   speed_div = 3;
   multiple = 1;
   displaced = 15;
   displace_series [0] = 1;
    enemy[e].attribute [1] = 13;
    enemy[e].attribute [2] = 7;
   if (enemy[e].attribute [0] == 1)
   {
    displace_series [0] = -1;
    enemy[e].attribute [0] = 0;
//    enemy[e].attribute [1] = 5;
//    enemy[e].attribute [2] = 5;
   }
    else
     {
      enemy[e].attribute [0] = 1;
//      enemy[e].attribute [1] = 5;
//      enemy[e].attribute [2] = 5;
     }
   colours_for_cloud [0] = TRANS_DRED;
   colours_for_cloud [1] = TRANS_ORANGE;
   colours_for_cloud [2] = TRANS_YELLOW;
   enemy[e].recycle = 65;
   size = 6000;
   enemy_soundf(e, NWAV_LAUNCH, 400); 
   break;
  case ENEMY_TRIANGLER2:
   recoil = 100;
   btype = BULLET_TRI2;
   damage = 1;
   timer = 60;
   if (enemy[e].attacking == 0 || enemy[e].attacking == 1)
    timer = (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) / 7000) + 3;
   timer_rand = 15;
   mass = 10;
   speed = 2000;
   firing_distance = 15000;
   speed_div = 3;
   multiple = 1;
   displaced = 17;
   displace_series [0] = 1;
    enemy[e].attribute [1] = 13;
    enemy[e].attribute [2] = 7;
   if (enemy[e].attribute [0] == 1)
   {
    displace_series [0] = -1;
    enemy[e].attribute [0] = 0;
//    enemy[e].attribute [1] = 5;
//    enemy[e].attribute [2] = 5;
   }
    else
     {
      enemy[e].attribute [0] = 1;
//      enemy[e].attribute [1] = 5;
//      enemy[e].attribute [2] = 5;
     }
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   enemy[e].recycle = 75;
   size = 6000;
   enemy_soundf(e, NWAV_LAUNCH, 300); 
   break;
  case ENEMY_TRIANGLER3:
   recoil = 100;
   btype = BULLET_TRI3;
   damage = 1;
   timer = 60;
   if (enemy[e].attacking == 0 || enemy[e].attacking == 1)
    timer = (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) / 8000) + 3;
   timer_rand = 15;
   mass = 10;
   speed = 2000;
   firing_distance = 15000;
   speed_div = 3;
   multiple = 1;
   displaced = 16;
   displace_series [0] = 1;
    enemy[e].attribute [1] = 13;
    enemy[e].attribute [2] = 7;
   if (enemy[e].attribute [0] == 1)
   {
    displace_series [0] = -1;
    enemy[e].attribute [0] = 0;
//    enemy[e].attribute [1] = 5;
//    enemy[e].attribute [2] = 5;
   }
    else
     {
      enemy[e].attribute [0] = 1;
//      enemy[e].attribute [1] = 5;
//      enemy[e].attribute [2] = 5;
     }
   colours_for_cloud [0] = TRANS_ORANGE;
   colours_for_cloud [1] = TRANS_YELLOW;
   colours_for_cloud [2] = TRANS_WHITE;
   enemy[e].recycle = 99;
   size = 6000;
   enemy_soundf(e, NWAV_LAUNCH, 200); 
   break;
  case ENEMY_OVERTRIANGLER:
   recoil = 100;
   btype = BULLET_OVERTRI;
   damage = 1;
   timer = 60;
   if (enemy[e].attacking == 0 || enemy[e].attacking == 1)
    timer = (hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x) / 5000) + 3;
   timer_rand = 15;
   mass = 10;
   speed = 2000;
   firing_distance = 15000;
   speed_div = 3;
   multiple = 1;
   displaced = 26;
   displace_series [0] = 1;
    enemy[e].attribute [1] = 13;
    enemy[e].attribute [2] = 7;
   if (enemy[e].attribute [0] == 1)
   {
    displace_series [0] = -1;
    enemy[e].attribute [0] = 0;
   }
    else
     {
      enemy[e].attribute [0] = 1;
     }
   colours_for_cloud [0] = TRANS_LGREEN;
   colours_for_cloud [1] = TRANS_YELLOW;
   colours_for_cloud [2] = TRANS_WHITE;
   enemy[e].recycle = 65;
   size = 9000;
   enemy_soundf(e, NWAV_LAUNCH, 150); 
   break;
  case ENEMY_ATTRACTOR:
   btype = BULLET_ATTRACTOR_LINE;
   damage = 0;//DAM_WORMS1;
   timer = 80;
   timer_rand = 30;
   mass = 60;
   speed = 0;//12000;
   firing_distance = 15000 + grand(60000);
   enemy[e].recycle = 1;
   speed_div = 3;
   multiple = 1;
   angle = grand(ANGLE_FULL);
//   displaced = grand(110);
//   displace_series [0] = 1;   
//   angle_increment = 102;//ANGLE_1_SIXTEENTH;
   colours [0] = TRANS_DBLUE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_PURPLE;
   colours [3] = TRANS_WHITE;
   recoil = 0;
   special2 = e;
   break;
  case ENEMY_WORMER1:
   btype = BULLET_EVIL_EEL;
   damage = DAM_WORMS1;
   timer = 80;
   timer_rand = 30;
   mass = 60;
   speed = 6000;
   firing_distance = 11000;
   enemy[e].recycle = 120;
   speed_div = 3;
   multiple = 10;
   angle_increment = 102;//ANGLE_1_SIXTEENTH;
   colours [0] = COLOUR_ORANGE4;
   colours [1] = COLOUR_ORANGE6;
   colours [2] = COLOUR_YELLOW6;
   colours [3] = COLOUR_YELLOW8;
   recoil = 0;
   special2 = 64;
   if (grand(2) == 0)
    special2 = -64;
   special3 = 3 + grand(3);
   special1 = 900;
   enemy_soundf(e, NWAV_WHINE, 1000 + grand(50));
   break;
  case ENEMY_WORMER2:
   btype = BULLET_SQUIRMY;
   damage = DAM_WORMS2;
   timer = 80;
   timer_rand = 30;
   mass = 80;
   speed = 6000;
   firing_distance = 11000;
   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 10;
   angle_increment = 102;//ANGLE_1_SIXTEENTH;
   colours [0] = COLOUR_GREEN4;
   colours [1] = COLOUR_GREEN6;
   colours [2] = COLOUR_YELLOW6;
   colours [3] = COLOUR_YELLOW8;
   recoil = 0;
   special2 = 94;
//   if (grand(2) == 0)
//    special2 = -74;
   special4 = 8; 
   special3 = special4;
   special1 = 1500;
   enemy_soundf(e, NWAV_WHINE, 700 + grand(50));
   break;
  case ENEMY_WORMER3:
   btype = BULLET_EVIL_WORM;
   damage = DAM_WORMS2;
   timer = 80;
   timer_rand = 0;
   mass = 80;
   speed = 6000;
   firing_distance = 11000;
   enemy[e].recycle = 120;
   speed_div = 3;
   multiple = 6;
   angle_increment = ANGLE_FULL / 6;
   colours [0] = COLOUR_RED5;
   colours [1] = COLOUR_ORANGE6;
   colours [2] = COLOUR_YELLOW6;
   colours [3] = COLOUR_YELLOW8;
/*   colours [0] = COLOUR_BLUE5;
   colours [1] = COLOUR_BLUE6;
   colours [2] = COLOUR_GREEN6;
   colours [3] = COLOUR_YELLOW8;*/
   recoil = 0;
   special1 = 390; // accel
   special2 = 60; // time of lock-on
   special3 = -1; // lock
   special4 = 6; // turn
   enemy_soundf(e, NWAV_WHINE, 500 + grand(50));
   break;
  case ENEMY_WORMER4:
   btype = BULLET_TURN_WORM;
   damage = DAM_WORMS2;
   timer = 100 + grand(40); // deliberately not timer_rand
   timer_rand = 0;
   mass = 80;
   speed = 6000;
   firing_distance = 11000;
   enemy[e].recycle = 120;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_FULL / 8;
   colours [0] = COLOUR_ORANGE5;
   colours [1] = COLOUR_YELLOW6;
   colours [2] = COLOUR_YELLOW7;
   colours [3] = COLOUR_WHITE;
   recoil = 0;
   special1 = 1100 + grand(300); // accel
   special2 = 5 + grand(9); // turn
   if (grand(2) == 0)
    special2 *= -1;
   enemy_soundf(e, NWAV_WHINE, 400 + grand(50));
   break;
/*  case ENEMY_HEAD1:
   btype = BULLET_YELLOW_PULSE;
   damage = 1;
   timer = 80;
   timer_rand = 30;
   mass = 60;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 150;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_1_EIGHTH;
   colours [0] = COLOUR_YELLOW8;
//   colours [1] = COLOUR_ORANGE6;
//   colours [2] = COLOUR_YELLOW6;
//   colours [3] = COLOUR_YELLOW8;
   recoil = 0;
   special1 = 250; // damage
   special2 = 200; // visible explosion size
   special3 = 50000; // blast size
   special4 = 500; // blast force
   break;
  case ENEMY_HEAD1_EYE1:
   btype = BULLET_CIRCLES;
   damage = 200;
   timer = 120;
   timer_rand = 30;
   mass = 120;
   speed = 5000;
   firing_distance = 4000;
   enemy[e].recycle = 90 + grand(70);
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_GREEN8;
   recoil = 500;
   size = 5000;
   break;*/
  case ENEMY_BRACKET4_TURRET: 
//  case ENEMY_HEAD1_EYE2:
   btype = BULLET_SHOCK;
   damage = 100;
   timer = 80;
   timer_rand = 30;
   mass = 20;
   speed = 15000;
   firing_distance = 4000;
   enemy[e].recycle = 60 + grand(50);
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_ORANGE4;
   colours [1] = COLOUR_ORANGE6;
   colours [2] = COLOUR_YELLOW6;
   colours [3] = COLOUR_YELLOW8;
   recoil = 500;
   enemy_soundf(e, NWAV_SZAP, 100 + grand(5));
   break;
/*  case ENEMY_HEAD1_EYE3:
   btype = BULLET_SQUIRMY;
   damage = DAM_WORMS2;
   timer = 80;
   timer_rand = 30;
   mass = 50;
   speed = 500;
   firing_distance = 4000;
   enemy[e].recycle = 50 + grand(30);
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_GREEN4;
   colours [1] = COLOUR_GREEN6;
   colours [2] = COLOUR_YELLOW6;
   colours [3] = COLOUR_YELLOW8;
   recoil = 500;
   special2 = 94;
//   if (grand(2) == 0)
//    special2 = -74;
   special4 = 8; 
   special3 = special4;
   special1 = 1500;
   break;*/
  case ENEMY_SPINNER1:
   btype = BULLET_SEEKER1;
   damage = DAM_SMALL_GREEN_SEEKER;
   timer = 220;
   timer_rand = 50;
   mass = 50;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 180;
   speed_div = 3;
   multiple = 1;
//   colours [0] = COLOUR_GREEN8;
   colourise_bullet(colours, COLOURISE_GREEN_SEEKER1);
   recoil = 50;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 30;
   special3 = 32;
   special4 = 100;
   special5 = 4001;
   enemy_soundf(e, NWAV_SEEKER, 700 + grand(150));
   break;
  case ENEMY_SPINNER2:
   btype = BULLET_SEEKER2;
   damage = DAM_LARGE_GREEN_SEEKER;
   timer = 220;
   timer_rand = 50;
   mass = 120;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 330;
   speed_div = 3;
   multiple = 1;
   colourise_bullet(colours, COLOURISE_GREEN_SEEKER2);
   recoil = 50;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 43;
   special3 = 48;
   special4 = 180;
   special5 = 7001;
   enemy_soundf(e, NWAV_SEEKER, 600 + grand(20));
   break;
  case ENEMY_SPINNER3:
   btype = BULLET_SEEKER3;
   damage = 1;
   timer = 220;
   timer_rand = 50;
   mass = 120;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 250;
   speed_div = 3;
   multiple = 1;
   colourise_bullet(colours, COLOURISE_BLUE_SEEKER);
   recoil = 50;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 45;
   special3 = 32;
   special4 = 120;
   special5 = 5001;
   enemy_soundf(e, NWAV_SEEKER, 800 + grand(20));
   break;
  case ENEMY_SPINNER4:
   btype = BULLET_SEEKER1;
   damage = DAM_SMALL_GREEN_SEEKER;
   timer = 220;
   timer_rand = 50;
   mass = 50;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 200;
   speed_div = 3;
   multiple = 1;
   colourise_bullet(colours, COLOURISE_GREEN_SEEKER1);
   recoil = 50;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 30;
   special3 = 32;
   special4 = 100;
   special5 = 4001;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
    enemy[e].angle = grand(ANGLE_FULL);
   }
    else
    {
     enemy[e].recycle = 210;
     enemy[e].burst_fire = 3; // also in creation
    }
   enemy_soundf(e, NWAV_SEEKER, 700 + grand(150));
   break;
//  case ENEMY_FIGHTER5: 
  case ENEMY_SPINNER5:
   btype = BULLET_SEEKER1;
   damage = DAM_YELLOW_SEEKER;
   timer = 220;
   timer_rand = 50;
   mass = 50;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 100;
   speed_div = 3;
   multiple = 1;
   colourise_bullet(colours, COLOURISE_YELLOW_SEEKER);
   recoil = 250;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 20;
   special3 = 6;
   special4 = 200;
   special5 = 4001;  
   enemy_soundf(e, NWAV_SEEKER, 1000 + grand(20));
   break;
  case ENEMY_OVERSPINNER:
   btype = BULLET_BIGSEEKER;
   damage = 1;
   timer = 330;
   timer_rand = 50;
   mass = 120;
   speed = 7000;
   size = 4000;
   firing_distance = 30000;
   enemy[e].recycle = 200;
   speed_div = 3;
   multiple = 1;
   colourise_bullet(colours, TRANS_PURPLE);//COLOURISE__SEEKER);
   recoil = 150;
   special1 = closest_target(enemy[e].x, enemy[e].y);
//   special2 = 45;
//   special3 = 32;
//   special4 = 120;
//   special5 = 5001;
   scatter = ANGLE_FULL;
   enemy_soundf(e, NWAV_SEEKER, 300 + grand(20));
   break;
  case ENEMY_OVERSPIKEY:
   btype = BULLET_BIGWINGS1;
   damage = 1;
   timer = 30;
   timer_rand = 0;
   mass = 20;
   speed = 1200;
   firing_distance = 30000;
   enemy[e].recycle = 50;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;//ANGLE_1_32;
   colourise_bullet(colours, TRANS_LGREEN);
   recoil = 1;
   size = 5000;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   angle_rand = ANGLE_FULL;
   break;
  case ENEMY_UNDERSPIKEY:
   btype = BULLET_BIGCIRCLES;
   damage = 250;
   timer = 120;
   timer_rand = 30;
   mass = 120;
   speed = 5500;
   firing_distance = 27000;
   enemy[e].recycle = 45;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_ORANGE8;
   recoil = 100;
   size = 9000;
   enemy_soundf(e, NWAV_CHIME2, 750 + grand(10)); 
   break;
  case ENEMY_UNDERSPIKEY2:
   btype = BULLET_OVERBLOCKS;
   damage = 250;
   timer = 120;
   timer_rand = 30;
   mass = 120;
   speed = 11000;
   firing_distance = 27000;
   enemy[e].recycle = 120;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = TRANS_ORANGE;
   colours [1] = TRANS_LRED;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   recoil = 100;
   size = 9000;
   special1 = 0;
//   enemy_soundf(e, NWAV_CHIME2, 750 + grand(10)); 
   enemy_soundf(e, NWAV_BLOCK, 700 + grand(100)); 
   break;
  case ENEMY_UNDERSPIKEY3:
   btype = BULLET_ZIGZAG2;
   damage = 250;
   timer = 120;
   timer_rand = 30;
   mass = 120;
   speed = 6000;
   firing_distance = 27000;
   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE;
   colours [3] = TRANS_DBLUE;
/* colours [0] = TRANS_WHITE;
   colours [1] = TRANS_YELLOW;
   colours [2] = TRANS_YELLOW;
   colours [3] = TRANS_ORANGE;*/
   recoil = 100;
   size = 9000;
   special1 = ANGLE_QUARTER;
   if (grand(2) == 0)
    special1 = -ANGLE_QUARTER;
   special2 = 12;
   special3 = 7000;
   special4 = enemy[e].angle;
   special5 = 0;
//   enemy_soundf(e, NWAV_CHIME2, 750 + grand(10)); 
//   special4 = enemy[e].x + xpart(enemy[e].angle, firing_distance);
//   special5 = enemy[e].y + ypart(enemy[e].angle, firing_distance);
   enemy_soundf(e, NWAV_BLOCK, 500 + grand(100)); 
   break;
  case ENEMY_OVERSPIKEY2:
   btype = BULLET_ZIGZAG2;
   damage = 350;
   timer = 120;
   timer_rand = 30;
   mass = 120;
   speed = 8000;
   firing_distance = 27000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
/* colours [0] = TRANS_WHITE;
   colours [1] = TRANS_YELLOW;
   colours [2] = TRANS_YELLOW;
   colours [3] = TRANS_ORANGE;*/
   recoil = 100;
   size = 9000;
   special1 = ANGLE_QUARTER;
   if (grand(2) == 0)
    special1 = -ANGLE_QUARTER;
   special2 = 12;
   special3 = 7000;
   special4 = enemy[e].angle;
   special5 = 1;
//   enemy_soundf(e, NWAV_CHIME2, 750 + grand(10)); 
//   special4 = enemy[e].x + xpart(enemy[e].angle, firing_distance);
//   special5 = enemy[e].y + ypart(enemy[e].angle, firing_distance);
   enemy_soundf(e, NWAV_BLOCK, 600 + grand(100)); 
   break;
  case ENEMY_BLATTER1:
   btype = BULLET_GREEN_BLAT;
   damage = DAM_GREEN_BLAT;
   timer = 30;
   timer_rand = 15;
   mass = 10;
   speed = 10000;
   firing_distance = 12000;
//   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   recoil = 100;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 60;
     enemy[e].burst_fire = 3 + grand(3); // also in creation
    }
   enemy_soundf(e, NWAV_GBLAT, 500); 
   break;
  case ENEMY_OVERBLATTER:
   btype = BULLET_BALL2;
   damage = 200;
   timer = 60;
   timer_rand = 10;
   mass = 10;
   speed = 5000;
//   speed_rand = 4000;
   firing_distance = 6000;
//   enemy[e].recycle = 50;
   colours_for_cloud [0] = TRANS_DRED;
   colours_for_cloud [1] = TRANS_LRED;
   colours_for_cloud [2] = TRANS_YELLOW;
   speed_div = 3;
   displaced = 26;
   if (enemy[e].attribute [4] == 1)
   {
    displace_series [0] = -1;
    enemy[e].attribute [4] = 0;
    special_angle = enemy[e].attribute [7];    
    enemy[e].attribute [9] = 7;
   }
    else
     {
      displace_series [0] = 1;
      enemy[e].attribute [4] = 1;
      special_angle = enemy[e].attribute [6];    
      enemy[e].attribute [8] = 7;
     }
   recoil = 100;
   size = 3000;
   enemy[e].recycle = 14;
   enemy_soundf(e, NWAV_MULTI, 1000 + grand(30)); 
   break;
  case ENEMY_BOSS2_TURRET3:
   btype = BULLET_BALL2;
   damage = 200;
   timer = 40;
   timer_rand = 13;
   mass = 10;
   speed = 4500;
//   speed_rand = 4000;
   firing_distance = 12000;
//   enemy[e].recycle = 50;
   colours_for_cloud [0] = TRANS_DRED;
   colours_for_cloud [1] = TRANS_LRED;
   colours_for_cloud [2] = TRANS_YELLOW;
   speed_div = 3;
   recoil = 0;
   size = 3000;
   multiple = 3;   
   angle_increment = ANGLE_1_SIXTEENTH;
   enemy[e].recycle = 80;
   enemy_soundf(e, NWAV_MULTI, 600 + grand(10)); 
   break;
  case ENEMY_OVERBLATTER2:
   btype = BULLET_BOLT;
   damage = 300;
   timer = 30;
   timer_rand = 10;
   mass = 20;
   speed = 7500;
   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE;
   colours [3] = TRANS_DGREY;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   firing_distance = 6000;
   speed_div = 3;
   displaced = 26;
   size = 3000;
   if (enemy[e].attribute [4] == 1)
   {
    displace_series [0] = -1;
    enemy[e].attribute [4] = 0;
    special_angle = enemy[e].attribute [7];    
    enemy[e].attribute [9] = 7;
   }
    else
     {
      displace_series [0] = 1;
      enemy[e].attribute [4] = 1;
      special_angle = enemy[e].attribute [6];    
      enemy[e].attribute [8] = 7;
     }
   recoil = 100;
   size = 3000;
   enemy[e].recycle = 20;
   enemy_soundf(e, NWAV_MULTI, 200 + grand(10)); 
   break;
//  case ENEMY_OVERTRIANGLER_TURRET: 
  case ENEMY_BLATTER2:
   btype = BULLET_BALL2;
   damage = 200;
   timer = 25;
   timer_rand = 10;
   mass = 10;
   speed = 5500;
   firing_distance = 14000;
   enemy[e].recycle = 25;
//   if (enemy[e].type == ENEMY_DEFENDER3_TURRET4)
//   {
//    firing_distance = 12000;
//   } 
//    enemy[e].recycle = 35;
   speed_div = 3;
   multiple = 1;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_BALL1, 800 + grand(10)); 
   break;
  case ENEMY_BLATTER3:
   btype = BULLET_BALL1;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   firing_distance = 14000;
   enemy[e].recycle = 20;
   speed_div = 3;
   multiple = 4;
   angle_increment = ANGLE_1_EIGHTH;
   angle -= ANGLE_QUARTER;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_MULTI, 700 + grand(10)); 
   break;
  case ENEMY_DEFENDER3_TURRET4:
   btype = BULLET_FLAME;
   damage = 0;
   timer = 30;
   timer_rand = 5;
   mass = 40;
   speed = 5000;
/*   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE;*/
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   firing_distance = 11000;
   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   recoil = 0;
   size = 2000;
   enemy_soundf(e, NWAV_SHORTBURST, 200 + grand(50)); 
   break;
  case ENEMY_BLATTER4:
   btype = BULLET_FLAME;
   damage = 0;
   timer = 25;
   timer_rand = 5;
   mass = 40;
   speed = 8000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 11000;
   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   recoil = 500;
   size = 2000;
   enemy_soundf(e, NWAV_SHORTBURST, 150 + grand(50)); 
//   enemy_soundf(e, NWAV_LZAP, 900 + grand(100)); 
/*   btype = BULLET_FORK3;
   damage = 100;
   timer = 15;
   timer_rand = 6;
   mass = 20;
   speed = 8000;
   firing_distance = 21000;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_RED5;
   colours [1] = COLOUR_RED6;
   colours [2] = COLOUR_ORANGE8;
   colours [3] = COLOUR_YELLOW8;
   recoil = 50;
//   size = 1000;
   special1 = COLOUR_YELLOW8;
   scatter = ANGLE_1_SIXTEENTH;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 4;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 90;
     enemy[e].burst_fire = 4 + grand(4); // also in creation
    }*/
   break;
  case ENEMY_BLATTER5:
   btype = BULLET_ZAP;
   damage = 100;
   timer = 19;
   timer_rand = 12;
   mass = 10;
   speed = 3000;
   speed_rand = 7000;
   firing_distance = 10000;
   enemy[e].recycle = 55;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_GREEN8;
   colours [2] = COLOUR_GREEN6;
   colours [3] = COLOUR_GREEN4;
   speed_div = 3;
   multiple = 10;
   scatter = ANGLE_1_EIGHTH;
   recoil = 500;
   enemy_soundf(e, NWAV_DNO, 1200 + grand(150)); 
   break;
  case ENEMY_PULSER1:
   btype = BULLET_PULSE1;
   damage = 150;
   timer = 220;
   timer_rand = 50;
   mass = 50;
   speed = 1000;
   firing_distance = 18000;
   enemy[e].recycle = 75;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_LRED;
   colours_for_cloud [1] = TRANS_ORANGE;
   colours_for_cloud [2] = TRANS_YELLOW;
   recoil = 50;
   special1 = closest_target(enemy[e].x, enemy[e].y);
//   special2 = 20;
//   special3 = 32;
//   special4 = 100;
//   special5 = 4001;
   enemy_soundf(e, NWAV_CHIRP, 1000 + grand(100)); 
   break;
  case ENEMY_PULSER2:
   btype = BULLET_PULSE2;
   damage = 250;
   timer = 220;
   timer_rand = 50;
   mass = 150;
   speed = 1000;
   firing_distance = 18000;
   enemy[e].recycle = 75;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   recoil = 50;
   special1 = closest_target(enemy[e].x, enemy[e].y);
//   special2 = 20;
//   special3 = 32;
//   special4 = 100;
//   special5 = 4001;
   enemy_soundf(e, NWAV_CHIRP, 700 + grand(100)); 
   break;
  case ENEMY_BOSS2_3:
   btype = BULLET_OVERPULSE;
   damage = 300;
   timer = 220;
   timer_rand = 50;
   mass = 300;
   speed = 1000;
   firing_distance = 36000;
   enemy[e].recycle = 80;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   recoil = 250;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = grand(2);
   enemy_soundf(e, NWAV_CHIRP, 200 + grand(30)); 
   break;
  case ENEMY_CRUISER1_TURRET:
   btype = BULLET_GREEN_BLAT;
   damage = DAM_GREEN_BLAT;
   timer = 30;
   timer_rand = 15;
   mass = 10;
   speed = 10000;
   firing_distance = 12000;
//   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   recoil = 300;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
    enemy[e].attribute [0] = 7;
   }
    else
    {
     enemy[e].recycle = 90;
     enemy[e].burst_fire = 3 + grand(3); // also in creation
     enemy[e].attribute [0] = 7;
    }
   enemy_soundf(e, NWAV_GBLAT, 500); 
   break;
  case ENEMY_CRUISER1: 
   btype = BULLET_BLAST;
   damage = 200;
   timer = 40;
   timer_rand = 20;
   mass = 40;
   speed = 4000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 14000;
   enemy[e].recycle = 66;
   speed_div = 3;
   multiple = 2;
   multiple = 2;
   displaced = 9;
   displace_series [0] = 1;
   displace_series [1] = -1;
   recoil = 100;
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 900 + grand(100)); 
   break;
  case ENEMY_CRUISER2_TURRET:
  case ENEMY_DEFENDER2_TURRET3:
   btype = BULLET_YELLOW_FLAK;
   damage = 100;
   timer = 8;
   timer_rand = 15;
   mass = 10;
   speed = 6000;
   firing_distance = 12000;
//   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_ORANGE;
   colours_for_cloud [1] = TRANS_YELLOW;
   colours_for_cloud [2] = TRANS_WHITE;
   recoil = 300;
   enemy[e].recycle = 34;
   enemy[e].attribute [0] = 7;
   enemy_soundf(e, NWAV_GBLAT, 300 + grand(50)); 
   break;
  case ENEMY_CRUISER2: 
   btype = BULLET_BLAST;
   damage = 300;
   timer = 40;
   timer_rand = 20;
   mass = 40;
   speed = 7000;
   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_LBLUE;
   colours [3] = TRANS_DBLUE;
   firing_distance = 14000;
   enemy[e].recycle = 45;
   speed_div = 3;
//   multiple = 2;
   multiple = 2;
   displaced = 9;
   displace_series [0] = 1;
   displace_series [1] = -1;
   recoil = 100;
//   scatter = ANGLE_1_EIGHTH;
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 1200 + grand(100)); 
   break;
  case ENEMY_CRUISER3_TURRET:
   btype = BULLET_ZAP_DRAG;
   damage = 50;
   timer = 15;
   timer_rand = 6;
   mass = 5;
   speed = 9000;
//   speed_rand = 7000;
   firing_distance = 9000;
   enemy[e].recycle = 6;
   colours [0] = COLOUR_ORANGE8;
   colours [1] = COLOUR_RED8;
   colours [2] = COLOUR_RED6;
   colours [3] = COLOUR_RED4;
   speed_div = 3;
//   multiple = 10;
   scatter = ANGLE_1_32;
   recoil = 50;
   enemy[e].attribute [0] = 7;
   special1 = -1;
   enemy_soundf(e, NWAV_SZAP, 1200 + grand(50)); 
   break;
  case ENEMY_CRUISER3:
   btype = BULLET_BLAST;
   damage = 200;
   timer = 40;
   timer_rand = 20;
   mass = 40;
   speed = 4000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 14000;
   enemy[e].recycle = 66;
   speed_div = 3;
   multiple = 4;
   displaced = 7;
   angle_increment = ANGLE_1_SIXTEENTH;
   angle -= ANGLE_1_32 + ANGLE_1_SIXTEENTH;
   displace_series [0] = -2;
   displace_series [1] = -1;
   displace_series [2] = 1;
   displace_series [3] = 2;
   recoil = 100;
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 600 + grand(100)); 
   break;
  case ENEMY_CRUISER4: // similar to guardian1
   btype = BULLET_BLAST;
   damage = 250;
   timer = 30;
   timer_rand = 10;
   mass = 50;
   speed = 7000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREEN;
   firing_distance = 14000;
   enemy[e].recycle = 66;
   speed_div = 3;
   multiple = 4;
   displaced = 7;
   angle_increment = ANGLE_1_SIXTEENTH;
   angle -= ANGLE_1_32 + ANGLE_1_SIXTEENTH;
   displace_series [0] = -2;
   displace_series [1] = -1;
   displace_series [2] = 1;
   displace_series [3] = 2;
   recoil = 150;
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 500 + grand(100)); 
   break;
  case ENEMY_CRUISER4_TURRET:
   btype = BULLET_SHOCK;
   damage = 100;
   timer = 10;
   timer_rand = 3;
   mass = 20;
   speed = 15000;
   firing_distance = 7000;
   enemy[e].recycle = 35;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_GREEN4;
   colours [1] = COLOUR_GREEN6;
   colours [2] = COLOUR_YELLOW6;
   colours [3] = COLOUR_YELLOW8;
   recoil = 100;
   enemy[e].attribute [0] = 7;
   enemy_soundf(e, NWAV_LZAP, 1200 + grand(100)); 
   break;
  case ENEMY_LEAPER1:
   btype = BULLET_BLAST;
   damage = 200;
   timer = 40;
   timer_rand = 20;
   mass = 40;
   speed = 4000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 9000;
   enemy[e].recycle = 40;
   speed_div = 3;
   multiple = 1;
   recoil = 500;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 20;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 100;
     enemy[e].burst_fire = 3; // also in creation
    }
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 1200 + grand(100)); 
   break;
  case ENEMY_LEAPER2:
   btype = BULLET_BLAST;
   damage = 250;
   timer = 40;
   timer_rand = 20;
   mass = 40;
   speed = 6000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREY;
   firing_distance = 9000;
   enemy[e].recycle = 40;
   speed_div = 3;
   multiple = 1;
   recoil = 500;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 20;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 100;
     enemy[e].burst_fire = 3; // also in creation
    }
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 1100 + grand(100)); 
   break;
  case ENEMY_FIGHTER1:
   btype = BULLET_BLAST;
   damage = 200;
   timer = 40;
   timer_rand = 20;
   mass = 40;
   speed = 4000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 10000;
   enemy[e].recycle = 40;
   speed_div = 3;
   multiple = 1;
   recoil = 500;
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 900 + grand(100)); 
   break;
  case ENEMY_FIGHTER2:
   btype = BULLET_BLUE_BLAT;
   damage = DAM_BLUE_BLAT;
   timer = 30;
   timer_rand = 15;
   mass = 10;
   speed = 10000;
   firing_distance = 1000;
//   enemy[e].recycle = 90;
   speed_div = 3;
   multiple = 2;
   displaced = 6;
   displace_series [0] = 1;
   displace_series [1] = -1;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   recoil = 300;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 160;
     enemy[e].burst_fire = 4 + grand(4); // also in creation
    }
   enemy_soundf(e, NWAV_GBLAT, 1800); 
   break;
  case ENEMY_FIGHTER3:
   btype = BULLET_SEEKER1;
   damage = DAM_SMALL_GREEN_SEEKER;
   timer = 220;
   timer_rand = 50;
   mass = 50;
   speed = 7000;
   firing_distance = 11000;
   enemy[e].recycle = 150;
   speed_div = 3;
   multiple = 2;
//   displaced = 6;
//   displace_series [0] = 1;
//   displace_series [1] = -1;
   angle_increment = ANGLE_1_SIXTEENTH;
   colourise_bullet(colours, COLOURISE_GREEN_SEEKER1);
   recoil = 50;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 30;
   special3 = 32;
   special4 = 100;
   special5 = 4001;
   enemy_soundf(e, NWAV_SEEKER, 700 + grand(150));
   break;
  case ENEMY_FIGHTER4:
   btype = BULLET_BLAST;
   damage = 250;
   timer = 20;
   timer_rand = 10;
   mass = 40;
   speed = 5000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREY;
   firing_distance = 14000;
   enemy[e].recycle = 80;
   speed_div = 3;
   multiple = 2;
   angle -= ANGLE_1_32;
//   angle_rand = 64;
   angle_increment = ANGLE_1_SIXTEENTH;
   recoil = 1200;
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 1000 + grand(100)); 
   break;
  case ENEMY_FIGHTER5:
   btype = BULLET_SEEKER1;
   damage = DAM_YELLOW_SEEKER;
   timer = 220;
   timer_rand = 50;
   mass = 50;
   speed = 7000;
   firing_distance = 11000;
//   enemy[e].recycle = 50;
   speed_div = 3;
   multiple = 1;
   colourise_bullet(colours, COLOURISE_YELLOW_SEEKER);
   recoil = 250;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 20;
   special3 = 6;
   special4 = 200;
   special5 = 4001;  
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 15;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 160;
     enemy[e].burst_fire = 3 + grand(3); // also in creation
    }
   enemy_soundf(e, NWAV_SEEKER, 1000 + grand(20));
   break;
  case ENEMY_MESSENGER:
   btype = BULLET_ZAP_DRAG;
   damage = 100;
   timer = 30;
   timer_rand = 10;
   mass = 10;
   speed = 8000;
   firing_distance = 7000;
   speed_div = 3;
   multiple = 2;
   displaced = 5;
   displace_series [0] = -1;
   displace_series [1] = 1;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW7;
   colours [2] = COLOUR_GREEN6;
   colours [3] = COLOUR_GREEN4;
   recoil = 0;
   scatter = 0;
//   angle = enemy[e].attribute [2];
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 5;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 90;
     enemy[e].burst_fire = 3 + grand(3); // also in creation
    }
   special1 = -1; 
   enemy_soundf(e, NWAV_SZAP, 700 + grand(50)); 
/*   btype = BULLET_MBOMB;
   damage = 1;
   timer = 50;
//   timer_rand = 20;
   mass = 40;
   speed = 1000;
   colours [0] = COLOUR_YELLOW8;
////   colours [1] = TRANS_ORANGE;
//   colours [2] = TRANS_LRED;
//   colours [3] = TRANS_DRED;
   firing_distance = 1000;
   enemy[e].recycle = 60;
   speed_div = 3;
   multiple = 1;
   recoil = 0;*/
   break;
  case ENEMY_BOMBER1:
   btype = BULLET_E_BOMB;
   damage = 1;
   timer = 50;
//   timer_rand = 20;
   mass = 40;
   speed = 1000;
   colours [0] = COLOUR_YELLOW8;
////   colours [1] = TRANS_ORANGE;
//   colours [2] = TRANS_LRED;
//   colours [3] = TRANS_DRED;
   firing_distance = 1000;
   enemy[e].recycle = 40;
   speed_div = 3;
   multiple = 1;
   recoil = 0;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 15;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 160;
     enemy[e].burst_fire = 3; // also in creation
    }
   enemy_soundf(e, NWAV_SPLERK, 1500 + grand(50)); 
   break;
  case ENEMY_BOMBER2:
   btype = BULLET_E_BOMB;
   damage = 1;
   timer = 15;
   timer_rand = 65;
   mass = 40;
   speed = 0;
   speed_rand = 5000;
   colours [0] = COLOUR_YELLOW8;
////   colours [1] = TRANS_ORANGE;
//   colours [2] = TRANS_LRED;
//   colours [3] = TRANS_DRED;
   firing_distance = 1000;
   enemy[e].recycle = 40;
   speed_div = 3;
   multiple = 4;
   angle_rand = 150;
   recoil = 0;
   enemy_soundf(e, NWAV_SPLERK, 1300 + grand(50)); 
/*   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 15;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 160;
     enemy[e].burst_fire = 3; // also in creation
    }*/
   break;
  case ENEMY_BOMBER3:
   btype = BULLET_E_BOMB2;
   damage = 1;
   timer = 50;
   timer_rand = 100;
   mass = 10;
   speed = 6000;
//   speed_rand = 8000;
//   colours [0] = COLOUR_YELLOW8;
   firing_distance = 5000;
//   enemy[e].recycle = 40;
   speed_div = 3;
   multiple = 1;
   angle_rand = ANGLE_FULL;
   recoil = 100;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 160;
     enemy[e].burst_fire = 6 + grand(6); // also in creation
    }
   enemy_soundf(e, NWAV_TEETH, 1200 + grand(50)); 
   break;
  case ENEMY_SPIKEY1:
   btype = BULLET_CIRCLES2;
   damage = 150;
   timer = 120;
   timer_rand = 30;
   mass = 50;
   speed = 5000;
   firing_distance = 4000;
   enemy[e].recycle = 150;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_YELLOW8;
   recoil = 100;
   size = 5000;
   enemy_soundf(e, NWAV_CHIME2, 1400 + grand(50)); 
   break;
  case ENEMY_SPIKEY3:
   btype = BULLET_CIRCLES2;
   damage = 150;
   timer = 120;
   timer_rand = 30;
   mass = 50;
   speed = 7000;
   firing_distance = 4000;
   enemy[e].recycle = 150;
   speed_div = 3;
   multiple = 3;
   angle_increment = ANGLE_1_32;
   colours [0] = COLOUR_YELLOW8;
   recoil = 150;
   size = 5000;
   enemy_soundf(e, NWAV_CHIME2, 1000 + grand(50)); 
   break;
  case ENEMY_SPIKEY2:
   btype = BULLET_BLOCKS;
   damage = 200;
   timer = 120;
   timer_rand = 30;
   mass = 25;
   speed = 7000 + grand(2000);
   firing_distance = 9000;
   speed_div = 3;
   multiple = 1;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREY;
//   angle_increment = ANGLE_1_32;
//   colours [0] = COLOUR_YELLOW8;
   recoil = 150;
   size = 4000;
/*   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 16;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 160;
     enemy[e].burst_fire = 3 + grand(3); // also in creation
    }*/
    enemy[e].recycle = 80;
   enemy_soundf(e, NWAV_BLOCK, 900 + grand(100)); 
   break;
  case ENEMY_SPIKEY5:
   btype = BULLET_THICK_SHOCK;
   damage = 300;
   timer = 120;
   timer_rand = 30;
   mass = 50;
   speed = 13000;
   firing_distance = 6000;
   enemy[e].recycle = 180;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;//ANGLE_1_32;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
/*   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW7;
   colours [2] = COLOUR_YELLOW5;
   colours [3] = COLOUR_YELLOW3;*/
   recoil = 350;
   size = 6000;
   enemy_soundf(e, NWAV_BLOCK, 200 + grand(50)); 
   break;
  case ENEMY_SPIKEY4:
   btype = BULLET_WINGS1;
   damage = 1;
   timer = 30;
   timer_rand = 0;
   mass = 20;
   speed = 1200;
   firing_distance = 6000;
   enemy[e].recycle = 40;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;//ANGLE_1_32;
   recoil = 1;
   size = 2000;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   angle_rand = ANGLE_FULL;
   break;
  case ENEMY_FORKER1:
   btype = BULLET_FORK2;
   damage = 150;
   timer = 20;
   timer_rand = 10;
   mass = 20;
   speed = 5000;
   firing_distance = 15000;
   enemy[e].recycle = 55;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_GREY3;
   colours [1] = COLOUR_GREY4;
   colours [2] = COLOUR_GREY6;
   colours [3] = COLOUR_WHITE;
   recoil = 500;
//   size = 1000;
   special1 = COLOUR_WHITE;
   enemy_soundf(e, NWAV_BLOCK, 500 + grand(50)); 
   break;
  case ENEMY_FORKER2:
   btype = BULLET_FORK1;
   damage = 150;
   timer = 10;
   timer_rand = 20;
   mass = 20;
   speed = 6000;
   firing_distance = 15000;
   enemy[e].recycle = 50;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_BLUE5;
   colours [1] = COLOUR_BLUE8;
   colours [2] = COLOUR_GREY6;
   colours [3] = COLOUR_WHITE;
   recoil = 500;
//   size = 1000;
   special1 = COLOUR_WHITE;
   enemy_soundf(e, NWAV_BLOCK, 500 + grand(50)); 
   break;
  case ENEMY_MINEFIELDER1:
   btype = BULLET_MINE1;
   damage = 1;
   timer = 500;
   timer_rand = 100;
   mass = 30;
   speed = 10000;
   speed_rand = 7000;
   firing_distance = 14000;
   enemy[e].recycle = 35;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   angle_rand = ANGLE_FULL;
//   colours [0] = COLOUR_YELLOW8;
   recoil = 50;
   size = MINE_SIZE;
   colours_for_cloud [0] = TRANS_LRED;
   colours_for_cloud [1] = TRANS_ORANGE;
   colours_for_cloud [2] = TRANS_YELLOW;
   enemy_soundf(e, NWAV_SPLERK, 900 + grand(100)); 
   break;
  case ENEMY_MINER1:
   btype = BULLET_MINE1;
   damage = 1;
   timer = 300;
   timer_rand = 100;
   mass = 30;
   speed = 7000;
   speed_rand = 5000;
   firing_distance = 11000;
   enemy[e].recycle = 35;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   angle_rand = ANGLE_FULL;
//   colours [0] = COLOUR_YELLOW8;
   recoil = 200;
   size = MINE_SIZE;
   colours_for_cloud [0] = TRANS_LRED;
   colours_for_cloud [1] = TRANS_ORANGE;
   colours_for_cloud [2] = TRANS_YELLOW;
   enemy_soundf(e, NWAV_SPLERK, 900 + grand(100)); 
   break;
  case ENEMY_MINER2:
   btype = BULLET_MINE2;
   damage = 1;
   timer = 300;
   timer_rand = 100;
   mass = 30;
   speed = 7000;
   speed_rand = 6000;
   firing_distance = 11000;
   enemy[e].recycle = 35;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   angle_rand = ANGLE_FULL;
//   colours [0] = COLOUR_YELLOW8;
   recoil = 200;
   size = MINE_SIZE;
   colours_for_cloud [0] = TRANS_DRED;
   colours_for_cloud [1] = TRANS_LRED;
   colours_for_cloud [2] = TRANS_ORANGE;
   enemy_soundf(e, NWAV_SPLERK, 500 + grand(50)); 
   break;
  case ENEMY_MINER3:
   btype = BULLET_MINE3;
   damage = 1;
   timer = 300;
   timer_rand = 100;
   mass = 30;
   speed = 7000;
   speed_rand = 6000;
   firing_distance = 11000;
   enemy[e].recycle = 35;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   angle_rand = ANGLE_FULL;
//   colours [0] = COLOUR_YELLOW8;
   recoil = 200;
   size = MINE_SIZE;
   colours_for_cloud [0] = TRANS_ORANGE;
   colours_for_cloud [1] = TRANS_YELLOW;
   colours_for_cloud [2] = TRANS_WHITE;
   enemy_soundf(e, NWAV_SPLERK, 1100 + grand(40)); 
   break;
  case ENEMY_MINER3_TURRET:
   btype = BULLET_BLUE_BLAT;
   damage = 50;
   timer = 30;
   timer_rand = 15;
   mass = 10;
   speed = 10000;
   firing_distance = 10000;
   speed_div = 3;
   multiple = 1;
   colours_for_cloud [0] = TRANS_DBLUE;
   colours_for_cloud [1] = TRANS_LBLUE;
   colours_for_cloud [2] = TRANS_WHITE;
   recoil = 300;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 7;
    enemy[e].burst_fire --;
    enemy[e].attribute [0] = 7;
   }
    else
    {
     enemy[e].recycle = 90;
     enemy[e].burst_fire = 4 + grand(4); // also in creation
     enemy[e].attribute [0] = 7;
    }
   scatter = 64; 
   enemy_soundf(e, NWAV_GBLAT, 1500); 
   break;
  case ENEMY_BOSS1_1:
   btype = BULLET_BALL1;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   firing_distance = 56000;
   enemy[e].recycle = 65;
   speed_div = 3;
   multiple = 8;
   angle_increment = ANGLE_1_EIGHTH;
   angle -= ANGLE_QUARTER;
   recoil = 0;
   size = 3000;
   enemy_soundf(e, NWAV_BALL1, 300 + grand(30)); 
   break;
  case ENEMY_BOSS1_2:
   btype = BULLET_SEEKER1;
   damage = DAM_SMALL_GREEN_SEEKER;
   timer = 100;
   timer_rand = 50;
   mass = 50;
   speed = 7000;
   firing_distance = 41000;
   enemy[e].recycle = 33;
   speed_div = 3;
   multiple = 1;
   colourise_bullet(colours, COLOURISE_GREEN_SEEKER1);
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   recoil = 5;
   special1 = closest_target(enemy[e].x, enemy[e].y);
   special2 = 30;
   special3 = 32;
   special4 = 100;
   special5 = 4001;
   angle = grand(ANGLE_FULL);
   enemy_soundf(e, NWAV_SEEKER, 700 + grand(150));
   break;
  case ENEMY_BOSS1_3:
   btype = BULLET_BOLT;
   damage = 150;
   timer = 60;
   timer_rand = 20;
   mass = 100;
   speed = 8000;
//   speed_rand = 3000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   colours_for_cloud [0] = TRANS_DRED;
   colours_for_cloud [1] = TRANS_LRED;
   colours_for_cloud [2] = TRANS_YELLOW;
   firing_distance = 35000;
//   enemy[e].recycle = 6 + grand(4);
   enemy[e].recycle = 55;
   speed_div = 3;
   multiple = 1;
//   angle_increment = enemy[e].attribute [0] - enemy[e].angle;
   size = 2000;
   recoil = 2000;
   enemy_sound_fire(e, NWAV_BURSTZ, 1200, 150, firing_distance);
   break;
  case ENEMY_BOSS1_TURRET1:
   btype = BULLET_BALL1;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   firing_distance = 9000;
   enemy[e].recycle = 100;
   speed_div = 3;
   multiple = 3;
   angle_increment = ANGLE_1_EIGHTH;
   angle -= ANGLE_1_EIGHTH;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_MULTI, 300 + grand(10)); 
   break;
  case ENEMY_BOSS1_TURRET2:
   btype = BULLET_CIRCLES2;
   damage = 150;
   timer = 120;
   timer_rand = 30;
   mass = 50;
   speed = 5000;
   firing_distance = 9000;
   enemy[e].recycle = 100;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_YELLOW8;
   recoil = 100;
   size = 5000;
   enemy_soundf(e, NWAV_CHIME2, 1200 + grand(50)); 
   break;
  case ENEMY_BOSS1_TURRET3:
   btype = BULLET_BALL2;
   damage = 200;
   timer = 45;
   timer_rand = 20;
   mass = 10;
   speed = 5500;
   firing_distance = 9000;
   enemy[e].recycle = 55;
   speed_div = 3;
   multiple = 1;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_BALL1, 800 + grand(30)); 
   break;
  case ENEMY_BOSS2:
   btype = BULLET_BOLT;
   damage = 250;
   timer = 60;
   timer_rand = 20;
   mass = 100;
   speed = 5000;
   speed_rand = 3000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_LGREEN;
   colours [3] = TRANS_DGREEN;
   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;
   firing_distance = 35000;
//   enemy[e].recycle = 6 + grand(4);
   enemy[e].recycle = 21 + grand(6);
   speed_div = 3;
   multiple = 2;
   angle_increment = enemy[e].attribute [0] - enemy[e].angle;
   size = 1000;
   recoil = 100;
   enemy_soundvf(e, NWAV_BURSTZ, 190, 1500);
   break;
  case ENEMY_BOSS2_2:
   enemy[e].attribute [0] ++;
   if (enemy[e].attribute [0] >= 60)
   {
    btype = BULLET_CHARGE;
    damage = 1;
    timer = 60;
    timer_rand = 20;
    mass = 100;
    speed = 21000;
//    speed_rand = 3000;
    colours [0] = TRANS_WHITE;
    colours [1] = TRANS_LBLUE;
    colours [2] = TRANS_DBLUE;
    colours [3] = TRANS_DGREY;
    colours_for_cloud [0] = TRANS_DBLUE;
    colours_for_cloud [1] = TRANS_LBLUE;
    colours_for_cloud [2] = TRANS_WHITE;
/*    colours [0] = TRANS_YELLOW;
    colours [1] = TRANS_LGREEN;
    colours [2] = TRANS_DGREEN;
    colours [3] = TRANS_DGREY;
    colours_for_cloud [0] = TRANS_DGREEN;
    colours_for_cloud [1] = TRANS_LGREEN;
    colours_for_cloud [2] = TRANS_YELLOW;*/
    firing_distance = 35000;
//   enemy[e].recycle = 6 + grand(4);
    enemy[e].recycle = 99;
    enemy[e].attribute [0] = 0;
    speed_div = 3;
    size = 13000;
    recoil = 2900;
    line_blast(enemy[e].x + xpart(enemy[e].angle, firing_distance), enemy[e].y + ypart(enemy[e].angle, firing_distance), BULLET_CHARGE_LINE, e);
    enemy_sound_fire(e, NWAV_BURSTZL, 500, 255, firing_distance);
   }
    else
    {
     if (enemy[e].attribute [0] % 5 == 0)
      enemy_sound_fire(e, NWAV_SZAP, 50 + enemy[e].attribute [0] * 1, 100, 35000);
     if (enemy[e].attribute [0] <= 32)// && grand(2) == 0)
     {
      btype = BULLET_CHARGE_LINE;
      damage = 0;//DAM_WORMS1;
      timer = 80;
      timer_rand = 30;
      mass = 1;
      speed = 0;//12000;
      firing_distance = 5000 + grand(120000);
      enemy[e].recycle = 1;
      speed_div = 3;
      multiple = 1;
//      angle = grand(ANGLE_FULL);
 //   displaced = grand(110);
 //   displace_series [0] = 1;   
 //   angle_increment = 102;//ANGLE_1_SIXTEENTH;
      colours [0] = TRANS_DBLUE;
      colours [1] = TRANS_LBLUE;
      colours [2] = TRANS_LBLUE;
      colours [3] = TRANS_WHITE;
/*      colours [0] = TRANS_DGREEN;
      colours [1] = TRANS_DGREEN;
      colours [2] = TRANS_LGREEN;
      colours [3] = TRANS_YELLOW;*/
      recoil = 0;
      enemy[e].recycle = 1;
      special2 = e;    
      scatter = ANGLE_HALF;
     } 
      else
       return;
    } 
   break;
  case ENEMY_BOSS3_2:
   btype = BULLET_NOVA;
   damage = 0; // uses blast for damage
   timer = 200;
//   timer_rand = 20;
   mass = 100;
   speed = 25000;
   speed_rand = 10000;
   colours [3] = TRANS_YELLOW; // used for hole lines
   colours [2] = TRANS_ORANGE;
   colours [1] = TRANS_LRED;
   colours [0] = TRANS_DRED;
/*   colours_for_cloud [0] = TRANS_DGREEN;
   colours_for_cloud [1] = TRANS_LGREEN;
   colours_for_cloud [2] = TRANS_YELLOW;*/
   firing_distance = 35000;
//   enemy[e].recycle = 6 + grand(4);
   enemy[e].recycle = 350;
   speed_div = 3;
//   multiple = 2;
// angle_increment = enemy[e].attribute [0] - enemy[e].angle;
   size = 1000;
   recoil = 3500;
   enemy_sound_fire(e, NWAV_BURSTZL, 300, 255, firing_distance);
   break;
  case ENEMY_BOSS3_3:
   btype = BULLET_SWIRL1;
   damage = 250;
   timer = 150;
   timer_rand = 0;
   mass = 120;
   speed = 4500 + grand(3000);
   size = 5000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = -35000;
   enemy[e].recycle = 152; // should only have 1 swirl1 at once
   speed_div = 3;
   recoil = 1200;
   enemy[e].attribute [0] = 16;
   enemy_sound_fire(e, NWAV_BURSTZL, 400, 255, firing_distance);
   break;
  case ENEMY_BOSS2_TURRET1:
   btype = BULLET_BIGBALL1;
   damage = 250;
   timer = 50;
   timer_rand = 30;
   mass = 80;
   speed = 5000;
   size = 4000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 13000;
   enemy[e].recycle = 60;
   speed_div = 3;
   recoil = 600;
   special1 = 5; // radius of circle   
   special2 = 7; // radius of outer circle   
   enemy_soundf(e, NWAV_BALL1, 100 + grand(10)); 
   break;
  case ENEMY_BOSS2_TURRET2:
   btype = BULLET_ZAP_DRAG;
   damage = 100;
   timer = 30;
   timer_rand = 10;
   mass = 10;
   speed = 9000;
   firing_distance = 12000;
   speed_div = 3;
   multiple = 1;
   colours [0] = COLOUR_BLUE8;
   colours [1] = COLOUR_BLUE7;
   colours [2] = COLOUR_BLUE6;
   colours [3] = COLOUR_BLUE4;
   recoil = 0;
   scatter = ANGLE_1_32;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 8;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 90;
     enemy[e].burst_fire = 6 + grand(5); // also in creation
    }
   special1 = -2; 
   enemy_soundf(e, NWAV_SZAP, 1000 + grand(50)); 
   break;
  case ENEMY_BOSS2_TURRET4:
   btype = BULLET_BFLAK;
   damage = 50;
   timer = 30;
   timer_rand = 3;
   if (enemy[e].attacking != ATTACK_NONE
       && actor[enemy[e].attacking].in_play == 1)
   {
    timer = hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x);
    timer /= 7000;
//    timer = t / 7;
    if (timer < 5)
     timer = 5;
    if (timer > 45)
     timer = 45;
   }
   mass = 10;
   speed = 12000;
   colours_for_cloud [0] = TRANS_WHITE;
   colours_for_cloud [1] = TRANS_PURPLE;
   colours_for_cloud [2] = TRANS_LBLUE;
   firing_distance = 14000;
   enemy[e].recycle = 85;
   speed_div = 3;
   enemy[e].attribute [0] = 56;
   enemy_soundf(e, NWAV_DNO, 300 + grand(100)); 
   break;
  case ENEMY_BOSS3_TURRET2:
   btype = BULLET_ZAP_DRAG;
   damage = 100;
   timer = 30;
   timer_rand = 10;
   mass = 10;
   speed = 12000;
   firing_distance = 6000;
   speed_div = 3;
   multiple = 1;
   colours [0] = COLOUR_BLUE8;
   colours [1] = COLOUR_BLUE7;
   colours [2] = COLOUR_BLUE6;
   colours [3] = COLOUR_BLUE4;
   recoil = 0;
   scatter = ANGLE_1_32;
   displaced = 7;
   displace_series [0] = -1;
   if (enemy[e].burst_fire % 2 == 0)
    displace_series [0] = 1;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 4;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 90;
     enemy[e].burst_fire = 6 + grand(5); // also in creation
    }
   special1 = -1;
   enemy_soundf(e, NWAV_SZAP, 700 + grand(50)); 
   break;
  case ENEMY_BOSS3_TURRET1:
   btype = BULLET_BLAST;
   damage = 250;
   timer = 60;
   timer_rand = 20;
   mass = 40;
   speed = 6000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREY;
   firing_distance = 11000;
   enemy[e].recycle = 100;
   speed_div = 3;
   multiple = 3;
   angle_increment = ANGLE_1_SIXTEENTH;
   angle -= ANGLE_1_SIXTEENTH;//(ANGLE_1_SIXTEENTH + ANGLE_1_32);
   enemy_soundf(e, NWAV_MULTI, 500 + grand(10)); 
//   recoil = 1800;
   size = 2000;
   enemy_soundf(e, NWAV_LZAP, 900 + grand(100)); 
   break;
  case ENEMY_BOSS3_TURRET3:
   btype = BULLET_BFLAK2;
   damage = 1;
   timer = 50;
   timer_rand = 25;
/*   if (enemy[e].attacking != ATTACK_NONE
       && actor[enemy[e].attacking].in_play == 1)
   {
    timer = hypot(enemy[e].y - actor[enemy[e].attacking].y, enemy[e].x - actor[enemy[e].attacking].x);
    timer /= 7000;
//    timer = t / 7;
    if (timer < 5)
     timer = 5;
    if (timer > 45)
     timer = 45;
   }*/
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW6;
   colours [2] = COLOUR_ORANGE6;
   colours [3] = COLOUR_RED5;
   mass = 10;
   speed = 12000;
   colours_for_cloud [0] = TRANS_WHITE;
   colours_for_cloud [1] = TRANS_YELLOW;
   colours_for_cloud [2] = TRANS_ORANGE;
   firing_distance = 16000;
   enemy[e].recycle = 60;
   speed_div = 3;
   enemy[e].attribute [0] = 56;
   enemy_soundf(e, NWAV_DNO, 150 + grand(50)); 
   break;
  case ENEMY_DEFENDER1:
   btype = BULLET_BIGBALL1;
   damage = 200;
   timer = 120;
   timer_rand = 20;
   mass = 150;
   speed = 2500;
   size = 6000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_LGREEN;
   colours [2] = TRANS_DGREEN;
   colours [3] = TRANS_DGREY;
   firing_distance = 23000;
   enemy[e].recycle = 43;
   speed_div = 3;
   recoil = 600;
   special1 = 6; // radius of circle   
   special2 = 8; // radius of outer circle   
//   enemy_soundf(e, NWAV_LZAP, 300 + grand(10)); 
   enemy_soundf(e, NWAV_BALL1, 100 + grand(10)); 
   break;
  case ENEMY_DEFENDER1_TURRET1:
   btype = BULLET_BALL1;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   firing_distance = 9000;
   enemy[e].recycle = 120;
   speed_div = 3;
   multiple = 3;
   angle_increment = ANGLE_1_EIGHTH;
   angle -= ANGLE_1_EIGHTH;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_MULTI, 300 + grand(10)); 
   break;
  case ENEMY_DEFENDER1_TURRET2:
   btype = BULLET_BALL2;
   damage = 200;
   timer = 45;
   timer_rand = 20;
   mass = 10;
   speed = 5500;
   firing_distance = 9000;
   enemy[e].recycle = 65;
   speed_div = 3;
   multiple = 1;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_BALL1, 800 + grand(30)); 
   break;
  case ENEMY_DEFENDER1_TURRET3:
   btype = BULLET_BALL1;
   damage = 150;
   timer = 40;
   timer_rand = 20;
   mass = 10;
   speed = 5000;
   firing_distance = 9000;
   enemy[e].recycle = 21;
   speed_div = 3;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_BALL1, 800 + grand(30)); 
   break;
  case ENEMY_DEFENDER2:
   btype = BULLET_BIGBALL1;
   damage = 300;
   timer = 120;
   timer_rand = 20;
   mass = 200;
   speed = 3500;
   size = 9000;
   colours [0] = TRANS_YELLOW;
   colours [1] = TRANS_ORANGE;
   colours [2] = TRANS_LRED;
   colours [3] = TRANS_DRED;
   firing_distance = 23000;
   enemy[e].recycle = 40;
   speed_div = 3;
   recoil = 600;
   special1 = 9; // radius of circle
   special2 = 11; // radius of outer circle
//   enemy_soundf(e, NWAV_LZAP, 200 + grand(10)); 
   enemy_soundf(e, NWAV_BALL1, 80 + grand(10)); 
   break;
  case ENEMY_DEFENDER3:
   btype = BULLET_BIGBALL1;
   damage = 400;
   timer = 120;
   timer_rand = 20;
   mass = 250;
   speed = 4000;
   size = 12000;
   colours [0] = TRANS_WHITE;
   colours [1] = TRANS_LBLUE;
   colours [2] = TRANS_DBLUE;
   colours [3] = TRANS_DGREY;
   firing_distance = 23000;
   enemy[e].recycle = 38;
   speed_div = 3;
   recoil = 600;
   special1 = 12; // radius of circle
   special2 = 15; // radius of outer circle
//   enemy_soundf(e, NWAV_LZAP, 150 + grand(10)); 
   enemy_soundf(e, NWAV_BALL1, 70 + grand(5)); 
   break;
  case ENEMY_DEFENDER3_TURRET3:
   btype = BULLET_SLIVER;
   damage = 100;
   timer = 30;
   timer_rand = 23;
   mass = 10;
   speed = 3500;
   speed_rand = 1000;
//   enemy[e].recycle = 50;
//   colours_for_cloud [0] = TRANS_DRED;
//   colours_for_cloud [1] = TRANS_LRED;
//   colours_for_cloud [2] = TRANS_YELLOW;
   colours [0] = TRANS_YELLOW;
   colours [1] = COLOUR_YELLOW8;
   speed_div = 3;
/*   switch(enemy[e].attribute [0])
   {
    default:
    case 0:
     firing_distance = 12000;
     break;
    case 1:
     displaced = 12;
     displace_series [0] = 1;
     break;
    case 2:
     firing_distance = -12000;
     break;
    case 3:
     displaced = 12;
     displace_series [0] = -1;
     break;
   }*/
   firing_distance = 12000;
   if (enemy[e].attribute [2] > 0)
   {
    enemy[e].attribute [0] --;
    if (enemy[e].attribute [0] == -1)
     enemy[e].attribute [0] = 3;
   }
    else
    {
     enemy[e].attribute [0] ++;
     if (enemy[e].attribute [0] >= 4)
      enemy[e].attribute [0] = 0;
    }
/*    special1 = grand(ANGLE_FULL);
    special2 = -100;
    if (grand(2) == 0)
     special2 = 100;*/
   special_angle2 = enemy[e].attribute [1] + enemy[e].attribute [0] * ANGLE_QUARTER;
   special_angle = angle;

   special2 = grand(2);
//   special_angle = angle;
//   special_angle2 = enemy[e].attribute [1];
//   angle = enemy[e].attribute [1];
   enemy[e].recycle = 5;
   enemy_sound(e, NWAV_CLICK);
   break;
  case ENEMY_DEFENDER3_TURRET5:
   btype = BULLET_BALL2;
   damage = 200;
   timer = 65;
   timer_rand = 20;
   mass = 10;
   speed = 3500;
   firing_distance = 12000;
   enemy[e].recycle = 22;
   speed_div = 3;
   multiple = 2;
   angle_increment = ANGLE_HALF;
   recoil = 100;
   size = 3000;
   enemy_soundf(e, NWAV_MULTI, 900 + grand(10)); 
   break;
  case ENEMY_DEFENDER3_TURRET6:
   btype = BULLET_ZIGZAG1;
   damage = 200;
   timer = 19;
   timer_rand = 5;
   mass = 20;
   speed = 12000;
   firing_distance = 12000;
   enemy[e].recycle = 35;
   speed_div = 3;
   multiple = 1;
   angle_increment = 0;
   colours [0] = COLOUR_ORANGE4;
   colours [1] = COLOUR_ORANGE6;
   colours [2] = COLOUR_YELLOW6;
   colours [3] = COLOUR_YELLOW8;
   recoil = 100;
   enemy[e].attribute [0] = 7;
   special1 = 4000;
   size = 2000;
   enemy_soundf(e, NWAV_BLOCK, 1900 + grand(100)); 
   break;
/*   btype = BULLET_ZAP_DRAG;
   damage = 100;
   timer = 27;
//   timer_rand = 12;
   mass = 10;
   speed = 13000;
//   speed_rand = 7000;
   firing_distance = 12000;
   enemy[e].recycle = 55;
   colours [0] = COLOUR_YELLOW8;
   colours [1] = COLOUR_YELLOW7;
   colours [2] = COLOUR_ORANGE7;
   colours [3] = COLOUR_RED6;
   speed_div = 3;
   multiple = 8;
//   scatter = ANGLE_1_SIXTEENTH;
   angle_increment = ANGLE_1_32;
   angle -= ANGLE_1_EIGHTH;
   recoil = 500;
   break;*/
   
/*  case ENEMY_STINGER2:
   btype = BULLET_STING;
   damage = 150;
   timer = 30;
   timer_rand = 20;
   mass = 10;
   speed = 3000;
   colours [0] = GC_YELLOW8;
   colours [1] = GC_YELLOW6;
   colours [2] = GC_YELLOW4;
   colours [3] = GC_YELLOW2;
   firing_distance = 4000;
   speed_div = 5;
   if (enemy[e].burst_fire > 0)
   {
    enemy[e].recycle = 15;
    enemy[e].burst_fire --;
   }
    else
    {
     enemy[e].recycle = 120;
     enemy[e].burst_fire = 4 + grand(4);
    }
   break;
  case ENEMY_STINGER3:
   btype = BULLET_STING;
   damage = 150;
   timer = 20;
   timer_rand = 15;
   mass = 10;
   speed = 4000;
//   colours [0] = GC_RED8;
//   colours [1] = GC_RED6;
//   colours [2] = GC_RED4;
//   colours [3] = GC_RED2;
   colours [0] = GC_WHITE;
   colours [1] = GC_RED8;
   colours [2] = GC_RED5;
   colours [3] = GC_RED3;
   firing_distance = 4000;
   speed_div = 4;
   enemy[e].recycle = 66;
   multiple = 3;
   angle_increment = ANGLE_1_SIXTEENTH;
   angle -= ANGLE_1_SIXTEENTH;
   enemy_soundf(e, ESOUND_STING, 900 + grand(400));
   break;*/
  default:
   return;
 }

//  float cos_angle;
//  float sin_angle;

  int xs;
  int ys;

  int x;
  int y;
  int i;

 for (i = 0; i < multiple; i ++)
 {
/*
  firing_angle = angle + i * angle_increment;

  cos_angle = cos(angle_to_radians(firing_angle));
  sin_angle = sin(angle_to_radians(firing_angle));

  xs = enemy[e].x_speed + cos_angle * speed;
  ys = enemy[e].y_speed + sin_angle * speed;

  x = enemy[e].x + cos_angle * firing_distance;
  y = enemy[e].y + sin_angle * firing_distance;

*/

  firing_angle = angle + i * angle_increment + grand(angle_rand * 2) - angle_rand + grand(scatter) - grand(scatter);

//  cos_angle = cos(angle_to_radians(firing_angle));
//  sin_angle = sin(angle_to_radians(firing_angle));

  if (special_angle != -1) // overblatter, some others
  {
   if (special_angle2 != -1)
    firing_angle = special_angle2;
   x = enemy[e].x + xpart(firing_angle, firing_distance);
   y = enemy[e].y + ypart(firing_angle, firing_distance);
   if (displaced != 0)
   {
    x += xpart(firing_angle + (ANGLE_QUARTER * displace_series [i]), GRAIN * displaced);
    y += ypart(firing_angle + (ANGLE_QUARTER * displace_series [i]), GRAIN * displaced);
   }
   firing_angle = special_angle;
   xs = enemy[e].x_speed + xpart(firing_angle, speed + grand(speed_rand));
   ys = enemy[e].y_speed + ypart(firing_angle, speed + grand(speed_rand));
   if (special_angle2 == -1)
   {
    x += xpart(firing_angle, firing_distance);
    y += ypart(firing_angle, firing_distance);
   } 
  }
   else
   {
    xs = enemy[e].x_speed + xpart(firing_angle, speed + grand(speed_rand));
    ys = enemy[e].y_speed + ypart(firing_angle, speed + grand(speed_rand));

    x = enemy[e].x + xpart(firing_angle, firing_distance);
    y = enemy[e].y + ypart(firing_angle, firing_distance);

    if (displaced != 0)
    {
//   x += cos(angle_to_radians(firing_angle + (ANGLE_QUARTER * displace_series [i]))) * GRAIN * displaced;
//   y += sin(angle_to_radians(firing_angle + (ANGLE_QUARTER * displace_series [i]))) * GRAIN * displaced;
     x += xpart(firing_angle + (ANGLE_QUARTER * displace_series [i]), GRAIN * displaced);
     y += ypart(firing_angle + (ANGLE_QUARTER * displace_series [i]), GRAIN * displaced);
    }
   } 


/*  // Note: recoil is added for every single bullet fired,
  //  so make it low for multiple bullets
  if (recoil != 0)
  {
   accelerate_enemy(e, firing_angle + ANGLE_HALF, recoil);
  }
*/

  switch(btype)
  {
   case BULLET_DISRUPT2:
    colours [0] = TRANS_REVERSE;
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       x, y, 0, 0, 0, 0, 1000, -9, 3, 0, 0, 0, colours);
    break;
   case BULLET_DISRUPT3:
    colours [0] = TRANS_DARKEN;
      create_cloud(CLOUD_MED_TRANS_CIRCLE,
       x, y, 0, 0, 0, 0, 1000, -9, 3, 0, 0, 0, colours);
       break;
   case BULLET_DISRUPT1:
      create_cloud(CLOUD_DISTORT,
       x, y, 0, 0, 0, 0, 1000, -9, 3, 0, 0, 0, colours);
       break;
  }

  switch(enemy[e].type)
  {
   case ENEMY_DEFENDER3:
      create_cloud(CLOUD_BLAST_CIRCLE,
       x, y, 0, 0, 
       enemy[e].x_speed + xpart(enemy[e].angle - ANGLE_HALF + ANGLE_1_EIGHTH, 13000), 
       enemy[e].y_speed + ypart(enemy[e].angle - ANGLE_HALF + ANGLE_1_EIGHTH, 13000),
       200 + grand(10), 10, 0, 0, 0, 0, colours);
      create_cloud(CLOUD_BLAST_CIRCLE,
       x, y, 0, 0, 
       enemy[e].x_speed + xpart(enemy[e].angle - ANGLE_HALF - ANGLE_1_EIGHTH, 13000), 
       enemy[e].y_speed + ypart(enemy[e].angle - ANGLE_HALF - ANGLE_1_EIGHTH, 13000),
       200 + grand(10), 10, 0, 0, 0, 0, colours);
   case ENEMY_DEFENDER1:
   case ENEMY_DEFENDER2:
   case ENEMY_BOSS2_TURRET1: 
      create_cloud(CLOUD_BLAST_CIRCLE,
       x, y, 0, 0, 
       enemy[e].x_speed + xpart(enemy[e].angle + ANGLE_QUARTER, 9000), 
       enemy[e].y_speed + ypart(enemy[e].angle + ANGLE_QUARTER, 9000),
       200 + grand(10), 10, 0, 0, 0, 0, colours);
      create_cloud(CLOUD_BLAST_CIRCLE,
       x, y, 0, 0, 
       enemy[e].x_speed + xpart(enemy[e].angle - ANGLE_QUARTER, 9000), 
       enemy[e].y_speed + ypart(enemy[e].angle - ANGLE_QUARTER, 9000),
       200 + grand(10), 10, 0, 0, 0, 0, colours);
      create_cloud(CLOUD_BLAST_CIRCLE,
       x, y, 0, 0, 
       enemy[e].x_speed + xpart(enemy[e].angle - ANGLE_HALF, 9000), 
       enemy[e].y_speed + ypart(enemy[e].angle - ANGLE_HALF, 9000),
       200 + grand(10), 10, 0, 0, 0, 0, colours);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 1200);
    break;
    case ENEMY_OVERSPINNER:
      create_cloud(CLOUD_BLAST_CIRCLE,
       x, y, 0, 0, 
       enemy[e].x_speed, 
       enemy[e].y_speed,
       400 + grand(10), 10, 2, 0, 0, 0, colours);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 800);
    break;
    case ENEMY_OVERSPIKEY:
      create_cloud(CLOUD_BLAST_CIRCLE,
       x, y, 0, 0, 
       enemy[e].x_speed, 
       enemy[e].y_speed,
       200 + grand(10), 10, 2, 0, 0, 0, colours);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 400);
    break;
   case ENEMY_WORMER1: 
   case ENEMY_WORMER2: 
   case ENEMY_WORMER3: 
   case ENEMY_WORMER4: 
     if (i == 0)
      simple_light(enemy[e].x, enemy[e].y, enemy[e].x_speed, enemy[e].y_speed, 900);
       // NOT x,y, because then it won't be centred
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 320);
//     if (grand(2) == 0)
//      special2 *= -1;
//     special3 = 3 + grand(3);
     break;
   case ENEMY_DISRUPTER2:
   case ENEMY_DISRUPTER3:
     simple_cloud_trans(colours_for_cloud [0], x, y, 0, 0, 520);
     break;
   case ENEMY_BOMBER2:
      if (i != 0) break;
   case ENEMY_BOMBER1:
      colours [0] = TRANS_YELLOW;
      colours [1] = TRANS_ORANGE;
      colours [2] = TRANS_LRED;
      colours [3] = TRANS_DRED;
      create_cloud(CLOUD_TRANS_FADING_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       50, -9, 3, 0, 0, 0, colours);
     break;
   case ENEMY_LEAPER1:
   case ENEMY_FIGHTER1:
   case ENEMY_SPIKEY4:
   case ENEMY_CRUISER1:
   case ENEMY_CRUISER3:
//   case ENEMY_CRUISER2_TURRET:
      colours [0] = TRANS_YELLOW;
      colours [1] = TRANS_ORANGE;
      colours [2] = TRANS_LRED;
      colours [3] = TRANS_DRED;
      create_cloud(CLOUD_TRANS_FADING_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       30, -9, 3, 0, 0, 0, colours);
     break;
   case ENEMY_CRUISER3_TURRET:
      create_cloud(CLOUD_SMALL_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       530, 150, 50, 0, 0, 0, colours);
     break;
   case ENEMY_BOMBER3:
   case ENEMY_BOSS2_TURRET2:
   case ENEMY_BOSS3_TURRET2:
      colours [0] = TRANS_WHITE;
      colours [1] = TRANS_LBLUE;
      colours [2] = TRANS_DBLUE;
      colours [3] = TRANS_DBLUE;
      create_cloud(CLOUD_TRANS_FADING_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       30, -9, 3, 0, 0, 0, colours);
     break;
   case ENEMY_CRUISER1_TURRET:
   case ENEMY_CRUISER2_TURRET:
   case ENEMY_DEFENDER2_TURRET3:
   case ENEMY_CRUISER4_TURRET:
      place_burstlet_burst(x, y, enemy[e].x_speed, enemy[e].y_speed, 2 + grand(2), 2, 2, 1000, 1500,
       1, colours_for_cloud);
      simple_light(x, y, 0, 0, 400);
      //simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 400);
    break; 
   case ENEMY_DEFENDER2_TURRET1:
   case ENEMY_DEFENDER2_TURRET2:
//   case ENEMY_DEFENDER2_TURRET3:
   case ENEMY_BRACKET1:
   case ENEMY_BRACKET2:
   case ENEMY_BRACKET3:
   case ENEMY_BRACKET4:
   case ENEMY_BRACKET5:
   case ENEMY_BLATTER1:
   case ENEMY_LEAPER2:
   case ENEMY_DEFENDER3_TURRET3:
      colours [0] = TRANS_YELLOW;
      colours [1] = TRANS_LGREEN;
      colours [2] = TRANS_DGREEN;
      colours [3] = TRANS_DGREEN;
      create_cloud(CLOUD_TRANS_FADING_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       30, -9, 3, 0, 0, 0, colours);
//       300, -100, 40, 0, 0, 0, colours);
     break;
   case ENEMY_BLATTER5:
    if (i > 0) break;
      colours [0] = TRANS_YELLOW;
      colours [1] = TRANS_LGREEN;
      colours [2] = TRANS_DGREEN;
      colours [3] = TRANS_DGREEN;
      create_cloud(CLOUD_TRANS_FADING_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       80, -9, 3, 0, 0, 0, colours);
       break;
   case ENEMY_FIGHTER2:
   case ENEMY_MINER3_TURRET:
      colours [0] = TRANS_WHITE;
      colours [1] = TRANS_LBLUE;
      colours [2] = TRANS_DBLUE;
      colours [3] = TRANS_DBLUE;
      create_cloud(CLOUD_TRANS_FADING_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       30, -9, 3, 0, 0, 0, colours);
     break;
   case ENEMY_HEAD1:
   case ENEMY_HEAD1_EYE1:
   case ENEMY_HEAD1_EYE2:
   case ENEMY_HEAD1_EYE3:
   case ENEMY_BRACKET4_TURRET:
   case ENEMY_SHADOW1:
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 320);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 320);
     break;
   case ENEMY_SPINNER1:
   case ENEMY_SPINNER2:
   case ENEMY_SPINNER4:
     simple_cloud(COLOUR_GREEN8, x, y, enemy[e].x_speed, enemy[e].y_speed, 520);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 520);
     break;
   case ENEMY_SPINNER3:
     simple_cloud(COLOUR_BLUE8, x, y, enemy[e].x_speed, enemy[e].y_speed, 720);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 720);
     break;
   case ENEMY_SPINNER5:
     simple_cloud(COLOUR_YELLOW8, x, y, enemy[e].x_speed, enemy[e].y_speed, 520);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 520);
     break;
   case ENEMY_FORKER1:
   case ENEMY_FORKER2:
     simple_cloud(COLOUR_WHITE, x, y, enemy[e].x_speed, enemy[e].y_speed, 520);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 520);
     break;
   case ENEMY_OVERBLATTER:
    if (i != 0) break;
   case ENEMY_BOSS1_1:
   case ENEMY_GUARDIAN1:
   case ENEMY_OVERTRIANGLER_TURRET:
   case ENEMY_DEFENDER3_TURRET4:
   case ENEMY_BLATTER2:
   case ENEMY_BLATTER3:
   case ENEMY_BOSS1_TURRET1:
   case ENEMY_BOSS1_TURRET3:
   case ENEMY_DEFENDER1_TURRET1:
   case ENEMY_DEFENDER1_TURRET2:
   case ENEMY_DEFENDER1_TURRET3:
   case ENEMY_DEFENDER3_TURRET5:
   case ENEMY_MULTI1:
   case ENEMY_CURVE1:
   case ENEMY_BOSS2_TURRET3:
   case ENEMY_DEFENDER3_TURRET6:
//    if (i == 0)
    {
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    }
    break;
   case ENEMY_BLATTER4:
     simple_cloud_trans(colours [1], x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
     simple_cloud_trans(colours [0], x, y, enemy[e].x_speed, enemy[e].y_speed, 350);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
     break;
   case ENEMY_GUARDIAN3: 
      create_cloud(CLOUD_TWISTY_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       50, 1, 0, enemy[e].angle, special2, 0, colours);
    break;
   case ENEMY_GUARDIAN5: 
      create_cloud(CLOUD_TWIRLY_CIRCLE,
       x, y,
       0, 0,
       enemy[e].x_speed, enemy[e].y_speed, 
       50, 1, 0, enemy[e].angle, special2, 0, colours);
    break;
   case ENEMY_GUARDIAN4: 
   case ENEMY_GUARDIAN2:
   case ENEMY_MULTI2:
   case ENEMY_MULTI3:
   case ENEMY_BOSS3_TURRET1:
//    if (i == 0)
    {
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 950);
     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
    }
    break;
   case ENEMY_BOSS1_3:
   case ENEMY_BOSS2:
   case ENEMY_TRIANGLER1:
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 1150);
//   case ENEMY_BOSS2_TURRET1:
   case ENEMY_BOSS1_2:
   case ENEMY_MINEFIELDER1: 
   case ENEMY_MINER1: 
   case ENEMY_MINER2: 
   case ENEMY_MINER3: 
   case ENEMY_PUFFER1:
   case ENEMY_PUFFER2:
   case ENEMY_PUFFER3:
   case ENEMY_PUFFER4:
   case ENEMY_PULSER1: 
   case ENEMY_PULSER2: 
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 1000, 1500,
       4, colours_for_cloud);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 900);
//    if (i == 0)
//    {
//     simple_cloud_trans(TRANS_ORANGE, x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
//     simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 550);
//    }
    break;
   case ENEMY_BOSS2_3: 
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 2000);
     simple_cloud_trans(TRANS_LGREEN, x, y, enemy[e].x_speed, enemy[e].y_speed, 2550);
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 3, 3000, 4500,
       1, colours_for_cloud);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 1700);
    break;
   case ENEMY_BOSS3_2: 
   case ENEMY_BOSS3_3: 
     simple_cloud_trans(TRANS_WHITE, x, y, enemy[e].x_speed, enemy[e].y_speed, 3000);
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 3550);
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 3, 3000, 5000,
       1, colours_for_cloud);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 1700);
    break;
   case ENEMY_TRIANGLER2:
   case ENEMY_OVERBLATTER2:
   case ENEMY_BOSS2_TURRET4:
   case ENEMY_SHADOW2:
     simple_cloud_trans(TRANS_WHITE, x, y, enemy[e].x_speed, enemy[e].y_speed, 900);
     simple_cloud_trans(TRANS_LBLUE, x, y, enemy[e].x_speed, enemy[e].y_speed, 1150);
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 1000, 1500,
       4, colours_for_cloud);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 900);
    break;
/*   case ENEMY_MESSENGER:
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 500);
     simple_cloud_trans(TRANS_LRED, x, y, enemy[e].x_speed, enemy[e].y_speed, 750);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 600);
    break;*/
   case ENEMY_TRIANGLER3:
   case ENEMY_BOSS3_TURRET3:
     simple_cloud_trans(TRANS_WHITE, x, y, enemy[e].x_speed, enemy[e].y_speed, 900);
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 1150);
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 1000, 1500,
       4, colours_for_cloud);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 1000);
    break;
   case ENEMY_OVERTRIANGLER:
     simple_cloud_trans(TRANS_WHITE, x, y, enemy[e].x_speed, enemy[e].y_speed, 1500);
     simple_cloud_trans(TRANS_YELLOW, x, y, enemy[e].x_speed, enemy[e].y_speed, 1950);
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 1000, 3400,
       4, colours_for_cloud);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 2000);
    break;
   case ENEMY_BOSS2_2:
    if (btype == BULLET_CHARGE)
    {
     simple_cloud_trans(TRANS_WHITE, x, y, enemy[e].x_speed, enemy[e].y_speed, 3000);
     simple_cloud_trans(TRANS_LBLUE, x, y, enemy[e].x_speed, enemy[e].y_speed, 3550);
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 3000, 5000,
       4, colours_for_cloud);
      simple_light(x, y, enemy[e].x_speed, enemy[e].y_speed, 2000);
    }  
    break;
    case ENEMY_SPIKEY5:
//    colours [0] = bullet[b].colours [0];
      create_cloud(CLOUD_LARGE_FADING_CIRCLE,
       x, y, 0, 0, 0, 0, 100 + grand(50), -3, 2, 0, 100 + grand(50), 0, colours);
       break;
    
/*   case ENEMY_JELLY:
      place_burstlet_burst(x, y, 0, 0, 3 + grand(3), 4, 2, 1000, 1500,
       4, colours_for_cloud);
   break;*/

  }

  create_bullet(btype, x, y,
   xs, ys, OWNER_ENEMY,
   damage, timer + grand(timer_rand), mass, firing_angle,
   status, size, colours, speed_div, special1,special2,special3,special4,special5);

 }

  if (recoil != 0)
  {
   if (enemy[e].turret_main != -1)
   {
     accelerate_enemy(enemy[e].turret_main, angle + ANGLE_HALF, recoil);
   }
    else
     accelerate_enemy(e, angle + ANGLE_HALF, recoil);
  }

}

void zap_attack(int e)
{
 
 int xs = enemy[e].attribute [8], ys = enemy[e].attribute [9];
 int i,j, xa, ya;
 
 int passing_colours [3] = {TRANS_ORANGE, TRANS_YELLOW, TRANS_WHITE};
 
 place_explosion(xs, ys, 0, 0, 400 + grand(300), passing_colours);
 blast(xs, ys, 50000, 400, 6000, OWNER_ENEMY);
// bullet_soundf(b, NWAV_BURSTZL, 300 + grand(100)); 
 play_wav_pos(NWAV_BURSTZL, 600, 255, xs, ys);
 
   for (j = 0; j < 5; j ++)
   {
    xa = xs + grand(40001) - 20000;
    ya = ys + grand(40001) - 20000;
    place_explosion_no_light(xa, ya, 0, 0, 100 + grand(150), passing_colours);
   }
   place_burstlet_burst(xs, ys, 0, 0, 6,
      6, 4, 1500, 1500, 7, passing_colours);

 int zigs = hypot(xs - enemy[e].x, ys - enemy[e].y) / 10000;
 int passing_colours2 [4] = {TRANS_DRED, TRANS_DRED, TRANS_LRED, TRANS_WHITE};
 int angle;
 int xprog, yprog, xb, yb, xc, yc;
 
 for (i = 0; i < 2; i ++)
 {
  if (i == 0)
  {
   xa = enemy[e].x + xpart(enemy[e].angle + ANGLE_QUARTER, 30000);
   ya = enemy[e].y + ypart(enemy[e].angle + ANGLE_QUARTER, 30000);
  }
   else
   {
    xa = enemy[e].x + xpart(enemy[e].angle - ANGLE_QUARTER, 30000);
    ya = enemy[e].y + ypart(enemy[e].angle - ANGLE_QUARTER, 30000);
   } 

   create_cloud(CLOUD_LIGHT,
       xa,
       ya,
       0, 0, 0, 0, 800 + crandom(105),20,0, 0, 0, 0, passing_colours2);


   angle = radians_to_angle(atan2(ys - ya, xs - xa));
   angle &= 1023;
   
   xprog = xpart(angle, 10000);
   yprog = ypart(angle, 10000);

   xc = xa;
   yc = ya;
   
  for (j = 0; j < zigs; j ++)
  {
   xa += xprog;
   ya += yprog;
   xb = xa + grand(9001) - 4500;
   yb = ya + grand(9001) - 4500;
       create_cloud(CLOUD_TRANS_FADING_LINE,
        xb,
        yb,
        xc,
        yc,
        0,
        0,
        21, 1,0, 0, 0, 0, passing_colours2);
   xc = xb;
   yc = yb;   
  }
 }



}


// no leading...
int track_target(int e, int attacking, int turning, int forbid)
{
 int retval = enemy_turn_towards_xy(enemy[e].x, enemy[e].y, actor[attacking].x, actor[attacking].y, enemy[e].angle, turning, forbid);

 if (retval < enemy[e].angle || (enemy[e].angle < 100 && retval > (ANGLE_FULL - 100)))
  enemy[e].slew_dir = -1;
   else
    enemy[e].slew_dir = 1;
    
  return retval;  
}

// this is used by overblatter
int track_target_any_angle(int x, int y, int target_x, int target_y, int turning, int forbid, int angle)
{
 int retval = enemy_turn_towards_xy(x, y, target_x, target_y, angle, turning, forbid);

// if (retval < angle || (angle < 100 && retval > (ANGLE_FULL - 100)))

  return retval;  
}

int near_angle(int angle1, int angle2)
{

 if (angle1 / 200 == angle2 / 200)
  return 1;

 if (angle1 < 100 && angle2 > ANGLE_FULL - 100)
  return 1;
 if (angle2 < 100 && angle1 > ANGLE_FULL - 100)
  return 1;

 return 0;
 
}



void accelerate_enemy(int e, int angle, int impulse)
{

  enemy[e].x_speed += cos(angle_to_radians(angle)) * impulse;
  enemy[e].y_speed += sin(angle_to_radians(angle)) * impulse;

}

void drag_enemy(int e, float drag_amount)
{
 enemy[e].x_speed *= (float) 1 - drag_amount;
 enemy[e].y_speed *= (float) 1 - drag_amount;
// enemy[e].x_speed *= (float) 1 - drag_amount;
// enemy[e].y_speed *= (float) 1 - drag_amount;
}



int spawn_enemy(int type, int subtype, int carrying_pickup, int target)
{

#ifdef SANITY_CHECK
if (type <= ENEMY_NONE || type >= NO_ENEMY_TYPES)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "%i", type);
     rest(1000);
//  exit(type);
    } 
#endif    
//if (type <= ENEMY_NONE || type >= NO_ENEMY_TYPES)
// return -1; // sanity check

// textprintf_ex(screen, small_font, 100,100, COLOUR_WHITE, -1, "type %i target %i Press I", type, target);

// do
// {
//   ticked = 0;
// } while (key [KEY_I] == 0);

 int special [10];
 int location [2];
 int passing_colours [5] = {COLOUR_GREEN8, COLOUR_GREEN8, COLOUR_GREEN8, COLOUR_GREEN8, COLOUR_GREEN8};
 int passing_colours2 [5] = {TRANS_DGREEN, TRANS_LGREEN, TRANS_YELLOW, TRANS_YELLOW, TRANS_YELLOW};

// char standard_sound = 1;
 int cloud_size = 200 + crandom(250) + eclass[type].radius / 10;

 switch(type)
 {
  default:
   find_target_start_point(location, type);
   break;
//  case (immobile enemies)
//   find_firebase_start_point(location);
//   break;
 }

/*     switch(type)
     {
      default:*/
      passing_colours2 [0] = TRANS_DGREEN;
      passing_colours2 [1] = TRANS_LGREEN;
      passing_colours2 [2] = TRANS_YELLOW;
      passing_colours [0] = COLOUR_GREEN8;
/*      break;
      case ENEMY_HEAD1:
      passing_colours2 [0] = TRANS_LRED;
      passing_colours2 [1] = TRANS_ORANGE;
      passing_colours2 [2] = TRANS_YELLOW;
      passing_colours [0] = COLOUR_YELLOW8;
      cloud_size = 2000;
      break;*/
/*      passing_colours [0] = GC_YELLOW8;
      passing_colours2 [0] = TRANS_ORANGE;
      passing_colours2 [1] = TRANS_YELLOW;
      passing_colours2 [2] = TRANS_WHITE;
      break;
      passing_colours [0] = GC_ORANGE8;
      passing_colours2 [0] = TRANS_ORANGE;
      passing_colours2 [1] = TRANS_ORANGE;
      passing_colours2 [2] = TRANS_YELLOW;
      break;
      passing_colours [0] = GC_RED8;
      passing_colours2 [0] = TRANS_DRED;
      passing_colours2 [1] = TRANS_LRED;
      passing_colours2 [2] = TRANS_YELLOW;
      break;
      passing_colours [0] = GC_BLUE8;
      passing_colours2 [0] = TRANS_DBLUE;
      passing_colours2 [1] = TRANS_LBLUE;
      passing_colours2 [2] = TRANS_WHITE;
      break;
      passing_colours [0] = GC_PURPLE8;
      passing_colours2 [0] = TRANS_LBLUE;
      passing_colours2 [1] = TRANS_PURPLE;
      passing_colours2 [2] = TRANS_WHITE;
      break;*/
//     }

  
      create_cloud(CLOUD_SHRINKING_CIRCLE,
       location [0],
       location [1],
       0, 0,
       0, 0,
       500, -150 - eclass[type].radius / 100, 10, 0, 0, 0, passing_colours);
       
      place_explosion(location [0], location [1], 0,0,
       cloud_size, passing_colours2);

//  break;

// }

// if (target == TARGET_CRAWLY)
//arena.crawlies_created ++;

 int e = create_enemy(type, subtype, location [0], location [1],
  0, 0, grand(ANGLE_FULL), carrying_pickup, special, target);

//  if (standard_sound == 1)
//     enemy_soundf(e, ESOUND_WARP_IN, 800 + grand(800) - eclass[type].mass);
/* if (eclass[enemy[e].type].role == ROLE_BOSS || eclass[enemy[e].type].role == ROLE_MINIBOSS)
 {
      enemy_soundf(e, NWAV_SPAWN, enemy_sound_pitch(enemy[e].type) / 2 + grand(200)); //800 + grand(200) - enemy_pitch(enemy[k].type));
 }
  else*/
      enemy_soundf(e, NWAV_PHASE, enemy_sound_pitch(enemy[e].type) / 2 + 200 + grand(200)); //800 + grand(200) - enemy_pitch(enemy[k].type));

//calculate_beat();
 
 return e;
// need to make sure: either that creatures made up of multiple

}

void find_target_start_point(int location [2], int et)
{
 int timeout = 0;
 
/* if (eclass[et].role == ROLE_BOSS)// || eclass[et].role == ROLE_MINIBOSS)
 {
  location [0] = arena.max_x / 2;
  location [1] = arena.max_y / 2;
  return;
 }*/

 do
 {
  location [0] = grand(arena.max_x - 20000 - eclass[et].edge_radius * 2) + 10000 + eclass[et].edge_radius;
  location [1] = grand(arena.max_y - 20000 - eclass[et].edge_radius * 2) + 10000 + eclass[et].edge_radius;
//  location [1] = grand(arena.max_y - 300000) + 150000;
  timeout ++;
  if (timeout >= 1000)
   break; // Oh well.
  if (actor[0].in_play == 1)
   if (location [0] > actor[0].x - 150000 && location [0] < actor[0].x + 150000
       && location [1] > actor[0].y - 150000 && location [1] < actor[0].y + 150000)
    continue;
  if (actor[1].in_play == 1)
   if (location [0] > actor[1].x - 150000 && location [0] < actor[1].x + 150000
       && location [1] > actor[1].y - 150000 && location [1] < actor[1].y + 150000)
    continue;

  break;
 }
  while (TRUE);

}

void find_crawly_start_point(int location [2])
{
 location [1] = 10000;
 int timeout = 0;

 do
 {
  location [0] = grand(arena.max_x - 100000) + 50000;
  location [0] /= GRID_WIDTH * GRAIN;
  location [0] *= GRID_WIDTH * GRAIN;
  location [0] -= 5000;
  timeout ++;
  if (timeout >= 1000)
   break; // Oh well.
  if (actor[0].in_play == 1 && actor[0].y < 100000)
   if (location [0] > actor[0].x - 50000 && location [0] < actor[0].x + 50000)
    continue;
  if (actor[1].in_play == 1 && actor[1].y < 100000)
   if (location [0] > actor[1].x - 50000 && location [0] < actor[1].x + 50000)
    continue;

  break;
 }
  while (TRUE);

}

void find_firebase_start_point(int location [2])
{
 int timeout = 0;

 do
 {
  location [0] = grand(arena.max_x - 200000) + 100000;
  location [0] /= GRID_WIDTH * GRAIN;
  location [0] *= GRID_WIDTH * GRAIN;
  location [0] -= 4000;
  location [1] = grand(arena.max_y - 200000) + 100000;
  location [1] /= GRID_WIDTH * GRAIN;
  location [1] *= GRID_WIDTH * GRAIN;
  location [1] -= 4000;
  timeout ++;
  if (timeout >= 1000)
   break; // Oh well.
  break;
 }
  while (TRUE);

}


void worm_new_target(int b)
{

 bullet[b].special3 = -1;
 
/*
enemy not yet actually destroyed, so this doesn't work:
          if (game.type == GAME_DUEL)
          {
             bullet[b].special3 = closest_enemy_or_actor(bullet[b].owner, bullet[b].x, bullet[b].y,
                     500000, 1);
          }
           else
             bullet[b].special3 = closest_enemy(bullet[b].x, bullet[b].y, 1000000, 1);
*/             
}



void register_destroyed(int e, int cause)
{
 int i, scoring = -1;

#ifdef SANITY_CHECK
if (e <= -1 || enemy[e].type <= ENEMY_NONE || enemy[e].type >= NO_ENEMY_TYPES)
{
     rectfill(screen, 370, 230, 430, 280, COLOUR_RED3);
     textprintf_centre_ex(screen, small_font, 400, 250, COLOUR_WHITE, -1, "register e %i, c %i", e, cause);
     if (e != -1)
      textprintf_centre_ex(screen, small_font, 400, 260, COLOUR_WHITE, -1, "register t %i", enemy[e].type);
     rest(2000);
     return;
//  exit(type);
    } 
#endif    

 if (enemy[e].type == ENEMY_NONE)
  return;



 if (cause >= 0)
  scoring = actor[cause].controller;
  
 for (i = 0; i < NO_BULLETS; i ++)
 {
  if (bullet[i].special3 == e)
  {
   if (bullet[i].type == BULLET_WORM_SORROW
      || bullet[i].type == BULLET_WORM_AGONY
      || bullet[i].type == BULLET_SPORE)
      {
       worm_new_target(i);
      }
  }    
   // clear up any locks that missiles have on the enemy
 }
/*
 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].lock == e)
   actor[i].lock = -1;
 }

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].turret_lock == e)
   actor[i].turret_lock = -1;
 }*/

 for (i = 0; i < MAX_TURRETS; i ++)
 {
  if (enemy[e].turret [i] != -1)
  {
   if (scoring >= 0)
    score_kill(enemy[enemy[e].turret [i]].type, SUBTYPE_NONE, scoring);
//   score_kill(i, enemy[e].turret [i], enemy[enemy[e].turret [i]].turret_main);
   enemy[enemy[e].turret [i]].turret_main = -1;
   if (cause == -2)
    enemy[enemy[e].turret [i]].ta_time_left = TA_TURRET_TIMEOUT;
  } 
 }

 if (enemy[e].turret_main != -1)
 {
   enemy[enemy[e].turret_main].turret [enemy[e].turret_index] = -1;
 }

 if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
 {
  if (cause == -2)
   return; // warped out
// more time...
  if (eclass[enemy[e].type].role == ROLE_TARGET)
   arena.time_left += 33.333 * 12;
  if (eclass[enemy[e].type].role == ROLE_MINIBOSS)
   arena.time_left += 33.333 * 36;
  if (eclass[enemy[e].type].role == ROLE_BOSS)
   arena.time_left += 33.333 * 60;
  return; 
 }
 

// Because time_attack can return before this, can't have anything important to all
//  game modes below.
 if (cause >= 0)
 {
  scoring = actor[cause].controller;
  score_kill(enemy[e].type, enemy[e].subtype, scoring);
 }
  else scoring = -1;

 if (enemy[e].target == TARGET_PRIMARY && arena.game_over == 0)//enemy[e].target == TARGET_PRIMARY && arena.game_over == 0)
 {
  if (eclass[enemy[e].type].role == ROLE_MINIBOSS)
   arena.targets_left_total -= 2; // ie 3 in total

  if (eclass[enemy[e].type].role == ROLE_BOSS)
   arena.targets_left_total -= 3; // ie 4 in total

  arena.targets_left_total --;

  if (arena.targets_left_total <= 0)
  {
   finish_level();
  }
 }

}


int check_for_targets(void)
{
// int i;

/* for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type != ENEMY_NONE && enemy[i].target == TARGET_PRIMARY)
   return 1;
 }*/

// if (arena.targets_left [0] > 0 || arena.targets_left [1] > 0 || arena.targets_left [2] > 0)
//  return 1;

 return 0;

}


void enemy_sound(int e, int sound)
{

 play_wav_pos(sound, 1000, 255, enemy[e].x, enemy[e].y);

}

void enemy_soundf(int e, int sound, int freq)
{

 play_wav_pos(sound, freq, 255, enemy[e].x, enemy[e].y);

}

void enemy_soundvf(int e, int sound, int vol, int freq)
{

 play_wav_pos(sound, freq, vol, enemy[e].x, enemy[e].y);

}

void enemy_sound_fire(int e, int sound, int freq, int vol, int dist)
{

 play_wav_pos(sound, freq, vol, enemy[e].x + xpart(enemy[e].angle, dist), enemy[e].y + ypart(enemy[e].angle, dist));

}


void score_kill(int etype, int subtype, int p)
{
//     textprintf_centre_ex(screen, small_font, 400, 200 + grand(50), COLOUR_WHITE, -1, "%i %i %i", etype, subtype, p);
//     rest(500);

// return;

 if (p <= -1)
  return;
  
 if (etype <= -1)
  return; 

 int score_gained = eclass[etype].level * 10;
//eclass[etype].level * eclass[etype].level * 10;


 switch(eclass[etype].role)
 {
  case ROLE_MINIBOSS: 
   score_gained *= 3; 
   break;
  case ROLE_BOSS: 
   score_gained *= 5; 
   break;
  case ROLE_TURRET: 
   score_gained /= 4; 
   break;
  case ROLE_EXTRA:
   score_gained /= 2; 
   break;
 }

  gain_score(p, score_gained);

/* if (subtype == SUBTYPE_NONE)
 {
  gain_score(p, eclass[etype].score);
 }
  else
   gain_score(p, eclass[etype].score * subtype * subtype);*/

}




int enemy_turn_towards_angle(int angle, int tangle, int turning, int forbid)
{

 if ((angle < tangle && tangle > angle + ANGLE_HALF)
     || (angle > tangle && tangle > angle - ANGLE_HALF))
 {
  if (forbid == -1)
   return angle;
  angle -= turning;
  if (angle < 0)
   angle += ANGLE_FULL;
 }
  else
  {
   if (forbid == 1)
    return angle;
   angle += turning;
   if (angle > ANGLE_FULL)
    angle -= ANGLE_FULL;
  }
  
 return angle;
 
}


int enemy_turn_towards_xy(int x1, int y1, int x2, int y2, int angle, int turning, int forbid)
{

  int tangle =
    radians_to_angle(atan2((y2 - y1), (x2 - x1)));
   if (tangle < 0)
    tangle += ANGLE_FULL;
   if (tangle > ANGLE_FULL)
    tangle -= ANGLE_FULL;

  return enemy_turn_towards_angle(angle, tangle, turning, forbid);

}
