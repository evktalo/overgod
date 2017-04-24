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
    
File: cmds.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - implementation of keyboard commands.
There's no reason for this stuff to be separate from input.c except that
I copied large parts of it from Captain Pork's World of Violence, which
needed a far more complex system because of its AI players. And I never
got around to changing it.

*/

#include "config.h"

#include "allegro.h"

#include <math.h>

#include "globvars.h"

#include "stuff.h"
#include "bullet.h"
#include "cloud.h"
#include "actor.h"
#include "display.h"
#include "sound.h"
#include "light.h"

#include "palette.h"

enum
{
FIRE_BOTH,
FIRE_CANNON_ONLY,
FIRE_MISSILE_ONLY
};

enum
{
ASOUND_CANNON,
ASOUND_BOMBS,
ASOUND_BLAT,
ASOUND_TUBE
};

#define BOMB_ANGLE_1 128
#define BOMB_ANGLE_2 256

#define BURST_RECYCLE_DELAY 7
#define BURST_RECYCLE_DELAY_CLAWS 2
#define BURST_RECYCLE_DELAY_WORMS 4

 
enum
{
PATTERN_TWO_PLUS_OTHERS, // two main secondaries plus other small ones
PATTERN_ONE_PLUS_OTHERS, // one main secondary plus other small ones
PATTERN_ALL_SAME, // all secondaries the same
PATTERN_SIDEWARDS // fire out to sides
};


/*
WEAPON_WOODEN_DARTS,
WEAPON_ICE_DARTS,
WEAPON_SILVER_TEETH,
WEAPON_SNOW_DARTS,
WEAPON_FROZEN_BREATH,
WEAPON_BRONZE_DARTS,
WEAPON_BRASS_TEETH,
WEAPON_CRYSTAL_TEETH,
WEAPON_GOLDEN_NEEDLES,
WEAPON_PARTICLE_SPITTER,
WEAPON_CRYSTAL_SPINES,
WEAPON_GOLDEN_TEETH,
WEAPON_NUMEROUS_DARTS,
WEAPON_NUMEROUS_BLADES,
WEAPON_FAR_SPITTER,
WEAPON_BURNING_SPIRIT
*/
int recycle_rates [6] =
{
/*21, downgraded 27/10/04
17,
14,
12,
10,
9*/
21,
18,
15,
13,
12,
11
/*, // Darts
{
32,
26,
21,
18,
16,
14
}, // Burst
{
10,
9,
8,
7,
6,
5
}, // Spines
{
32,
26,
21,
18,
16,
15
} // Teeth
*/
};

/*int recycle_rates [16] [6] =
{
{
21,
15,
13,
11,
10,
9
}, // Wooden darts
{
42,
36,
31,
27,
24,
21
}, // Ice darts
{
42,
36,
31,
27,
24,
21
//31,
//27,
//24,
//21,
//18,
//16
//24,
//21,
//18,
//16,
//14,
//13
//18,
//16,
//14,
//13,
//12,
//11
}, // Silver teeth
{
31,
27,
24,
21,
18,
16
}, // Snow darts
{
42,
36,
31,
27,
24,
21
}, // Frozen Breath
{
18,
15,
13,
11,
10,
9
}, // Brass Teeth
{
42,
36,
31,
27,
24,
21
}, // Shock Path
{
42,
36,
31,
27,
24,
21
}, // Crystal Teeth
{
18,
13,
11,
9,
8,
7
}, // Golden Needle
{
13,
11,
10,
8,
7,
6
}, // Particle Spitter
{
31,
27,
24,
21,
18,
16
}, // Crystal Spines
{
18,
18,
16,
14,
13,
12
}, // Golden Path
{
42,
36,
31,
27,
24,
21
}, // Numerous Darts
{
18,
18,
16,
14,
13,
12
}, // Numerous Blades
{
13,
17,
14,
12,
10,
9
}, // Far Spitter
{
18,
18,
16,
14,
13,
12
} // Burning Spirit

};
*/

void actor_thrust(int thractor, int angled);
void actor_slide(int thractor, int direction);
void actor_turn(int thractor, int direction);
void actor_brake(int thractor);
void actor_shoot(int sactor, int which_weapon);
void make_bcolours(int colours [4], int base_colour);
void fire_cannon(int sactor);
void fire_secondary(int sactor);
void cannon_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int size, int speed, int colours [4], int displaced, 
 int scatter, int speed_div);
void fire_missile(int sactor);
void secondary_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int size, int speed, int colours [4], int displaced, 
 int scatter, int speed_div, int special1, int special2, int special3, int special4, int special5,
 int flash_circle);
void missile_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced,
 int special5);
void fire_bomb(int sactor);
void bomb_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced);
void change_angle_arc(int angles [14], int width);
void change_displacement(int displace [7], int type);

void exhaust_colours(int level, int passing_colours [5]);

void fire_turret(int sactor);
void fire_heavy_cannon(int sactor);
void fire_backfire(int sactor);
void fire_sidekicks(int a);

void actor_drive(int a, int level, int distance_x, int distance_y, int angle, int thrust, int doubled);

void actor_sound(int a, int sound);
void make_drive_sound(int a, int drive);

void enact_commands(void)
{
 int i,j;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].in_play == 0)
   continue;
  actor[i].engine_demand = 1;
  actor[i].dragging = 0;
  
  if (actor[i].actor_cmd [CMD_THRUST] == 1)
   actor[i].engine_demand ++;
  if (actor[i].actor_cmd [CMD_BRAKE] == 1 && actor[i].upgraded_system [UPG_RETRO] > 0)
   actor[i].engine_demand ++;
  if (actor[i].upgraded_system [UPG_SLIDE] > 0)
  {
   if (actor[i].actor_cmd [CMD_LEFT1] == 1)
    actor[i].engine_demand ++;
   if (actor[i].actor_cmd [CMD_RIGHT1] == 1)
    actor[i].engine_demand ++;
  }
  // this makes sure available engine capacity is split between all active thrusters
   
  for (j = NO_CMDS - 1; j >= 0; j --) // reverse order to prevent drive + drag on at same time
  {
   if (actor[i].actor_cmd [j] == 1)
   {
    switch(j)
    {
     case CMD_THRUST:
     actor_thrust(i, 0);
     break;
     case CMD_LEFT:
     actor_turn(i, -1);
     break;
     case CMD_RIGHT:
     actor_turn(i, 1);
     break;
     case CMD_LEFT1:
     actor_slide(i, -1);
     break;
     case CMD_RIGHT1:
     actor_slide(i, 1);
     break;
     case CMD_BRAKE:
     actor_brake(i);
     break;
     case CMD_FIRE1:
     if (player[actor[i].controller].link_fire == 0)
      actor_shoot(i, FIRE_CANNON_ONLY);
       else
        actor_shoot(i, FIRE_BOTH);
     break;
     case CMD_FIRE2:
     actor_shoot(i, FIRE_MISSILE_ONLY);
     break;
//     case CMD_UPGRADE:
//     actor_upgrade(i);
//     break;
    }

/*
enum
{
CMD_THRUST,
CMD_LEFT,
CMD_RIGHT,
CMD_SYSTEM1,
CMD_SYSTEM2,
CMD_SYSTEM3,
CMD_BRAKE,
CMD_LEFT1,
CMD_RIGHT1,
CMD_LEFT2,
CMD_RIGHT2,
CMD_CHARGE_SHIELD
};
*/

   }
   actor[i].actor_cmd [j] = 0;
  }
 }

}

void actor_slide(int thractor, int direction)
{
// if (actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_CROSS] == 0)
//  return;
// int x_accel = cos(angle_to_radians(actor[thractor].angle) + (PI / 2) * direction) * actor[thractor].slide;
// int y_accel = sin(angle_to_radians(actor[thractor].angle) + (PI / 2) * direction) * actor[thractor].slide;
/* int x_accel = cos(angle_to_radians(actor[thractor].angle) + PI / 4 + (PI / 2) * direction) * actor[thractor].slide;
 int y_accel = sin(angle_to_radians(actor[thractor].angle) + PI / 4 + (PI / 2) * direction) * actor[thractor].slide;
 actor[thractor].x_speed += x_accel;
 actor[thractor].y_speed += y_accel;

 int passing_colours [5];

 exhaust_colours(actor[thractor].upgraded_system [UPG_SLIDE - 1], passing_colours);

 create_cloud(CLOUD_SPECK,
 actor[thractor].x,// - (actor[thractor].x_speed - x_accel * 10),
 actor[thractor].y,// - (actor[thractor].y_speed - y_accel * 10),
  0, 0,
  actor[thractor].x_speed - x_accel * 20 + (100 - grand(201)),
  actor[thractor].y_speed - y_accel * 20 + (100 - grand(201)),
  10 + grand(11),1,0, 0, 0, 0, passing_colours);

*/
// int passing_colours [5];
// int thrust_amount = 72;
// thrust_amount += actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_CROSS] * 13;
// thrust_amount /= actor[thractor].engine_demand;

// actor_drive(thractor, actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_CROSS] - 1, actor[thractor].slide_distance_x, actor[thractor].slide_distance_y, actor[thractor].angle + ANGLE_QUARTER + ANGLE_QUARTER * direction, thrust_amount, 0);
// actor_sound(thractor, 2);
// make_drive_sound(thractor, DRIVE_SLIDE);

/*
 float xcos = cos(angle_to_radians(actor[thractor].angle) + (PI / 2) + (PI/2) * direction);
 float ysin = sin(angle_to_radians(actor[thractor].angle) + (PI / 2) + (PI/2) * direction);
 int x_accel = xcos * thrust_amount / actor[thractor].engine_demand;
 int y_accel = ysin * thrust_amount / actor[thractor].engine_demand;
 actor[thractor].x_speed -= x_accel;
 actor[thractor].y_speed -= y_accel;

 exhaust_colours(actor[thractor].upgraded_system [UPG_SLIDE] - 1, passing_colours);
 
 create_cloud(CLOUD_SPECK,
 actor[thractor].x + xcos * 5 * GRAIN,// - (actor[thractor].x_speed - x_accel * 10),
 actor[thractor].y + ysin * 3 * GRAIN,// - (actor[thractor].y_speed - y_accel * 10),
  0, 0,
//  actor[thractor].x_speed - x_accel * 20 + (100 - grand(201)),
//  actor[thractor].y_speed - y_accel * 20 + (100 - grand(201)),
  actor[thractor].x_speed + x_accel * 20 + (300 - grand(601)),
  actor[thractor].y_speed + y_accel * 20 + (300 - grand(601)),
  10 + grand(11),1,0, 0, 0, 0, passing_colours);
*/

}

void actor_brake(int thractor)
{
// int passing_colours [5];

 if (actor[thractor].ship == SHIP_RETRO)
 {
//  int passing_colours [5];
/*  int thrust_amount = 72;
  thrust_amount += actor[thractor].upgraded_system [UPG_RETRO] * 13;
  thrust_amount /= actor[thractor].engine_demand;
  // level 1 retro gives same thrust as level 0 speed; l2->1, etc
  
  actor_drive(thractor, actor[thractor].upgraded_system [UPG_RETRO] - 1, actor[thractor].retro_distance_x, actor[thractor].retro_distance_y, actor[thractor].angle - ANGLE_QUARTER, thrust_amount, 0);

  make_drive_sound(thractor, DRIVE_RETRO);
   return;*/
   actor_thrust(thractor, ANGLE_HALF);
   return;
 }
 // else must be drag field...
 actor[thractor].dragging = 20;

}


void actor_thrust(int thractor, int angled)
{
 int thrust_amount = 170;

 thrust_amount += actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 4 + actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 4
  + actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 7
  + actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 2;

 if (actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] == 0)
  thrust_amount += 20;

 if (actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] == 1)
  thrust_amount -= 2;

/* thrust_amount += actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 5 + actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 5
  + actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 9
  + actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 2;
*/  
 thrust_amount /= 2; // actor[thractor].engine_demand;

 if (angled == 0) 
  actor_drive(thractor, actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE], actor[thractor].exhaust_distance_x, actor[thractor].exhaust_distance_y, actor[thractor].angle + ANGLE_QUARTER + angled, thrust_amount, angled);
   else
    actor_drive(thractor, actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE], actor[thractor].exhaust_distance_x + 2, actor[thractor].exhaust_distance_y + 2, actor[thractor].angle + ANGLE_QUARTER + angled, thrust_amount, angled);

 actor[thractor].dragging = actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 2; // 2
 make_drive_sound(thractor, DRIVE_THRUST);
}

/*
void actor_thrust(int thractor, int angled)
{
 int thrust_amount = 170; // 120
 thrust_amount += actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 16 // 22
  + actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 9; // 10 // 8
  
 thrust_amount /= 2; // actor[thractor].engine_demand;

 if (angled == 0) 
  actor_drive(thractor, actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE], actor[thractor].exhaust_distance_x, actor[thractor].exhaust_distance_y, actor[thractor].angle + ANGLE_QUARTER + angled, thrust_amount, angled);
   else
    actor_drive(thractor, actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE], actor[thractor].exhaust_distance_x + 2, actor[thractor].exhaust_distance_y + 2, actor[thractor].angle + ANGLE_QUARTER + angled, thrust_amount, angled);

 actor[thractor].dragging = actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 2;
 make_drive_sound(thractor, DRIVE_THRUST);
}
*/

// need to already have factored engine demand into thrust
void actor_drive(int a, int level, int distance_x, int distance_y, int angle, int thrust, int doubled)
{
// int passing_colours [5] = {TRANS_YELLOW,TRANS_ORANGE,TRANS_LRED,TRANS_DRED, TRANS_DGREY};
 int passing_colours [5] = {TRANS_ORANGE,TRANS_LRED,TRANS_LRED,TRANS_DRED, TRANS_DGREY};
 int passing_colours2 [5] = {TRANS_YELLOW,TRANS_YELLOW,TRANS_YELLOW,TRANS_YELLOW, TRANS_YELLOW};

 if (actor[a].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] > 1)
  passing_colours [1] = TRANS_ORANGE;
 if (actor[a].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] > 3)
  passing_colours2 [0] = TRANS_WHITE;

 int base_size = 23 + actor[a].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 2;
 int x_accel = xpart(angle, thrust);
 int y_accel = ypart(angle, thrust);
 int exhx, exhy;
 actor[a].x_speed -= x_accel;
 actor[a].y_speed -= y_accel;

// float xcos2, ysin2;
 int displace = 0;
 
 int i, j, k = 1, xs, ys, shrink = 2;
 
 actor[a].thrust = 7;

 if (doubled == 0)
 {
  switch(actor[a].ship)
  {
   case SHIP_ROUND:
    displace = ANGLE_1_SIXTEENTH;
    k = 2;
    shrink = 1;
    actor[a].thrust = 3;
    base_size = 15 + actor[a].ability [ABILITY_DRIVE] [SYMBOL_SQUARE];
    break;
   case SHIP_RETRO:
    displace = ANGLE_1_32 + 8;
    k = 2;
    shrink = 1;
    actor[a].thrust = 3;
    base_size = 15 + actor[a].ability [ABILITY_DRIVE] [SYMBOL_SQUARE];
    break;
  }
 }


 for (j = 0; j < k; j ++)
 {
   if (displace == 0)
   {
    exhx = actor[a].x + xpart(angle, distance_x * GRAIN) - 1000;
    exhy = actor[a].y + ypart(angle, distance_y * GRAIN) - 1000;
   }
    else
    {
     if (j == 0)
     {
      exhx = actor[a].x + xpart(angle + ANGLE_1_EIGHTH - displace, distance_x * GRAIN) - 1000;
      exhy = actor[a].y + ypart(angle + ANGLE_1_EIGHTH - displace, distance_y * GRAIN) - 1000;
     }
      else 
       {
        exhx = actor[a].x + xpart(angle - ANGLE_1_EIGHTH + displace, distance_x * GRAIN) - 1000;
        exhy = actor[a].y + ypart(angle - ANGLE_1_EIGHTH + displace, distance_y * GRAIN) - 1000;
       }
    } 
   add_light(LIGHT_NORMAL, 10 + grand(5), exhx + actor[a].x_speed, exhy + actor[a].y_speed);
 
   for (i = 0; i < 2; i ++)
   {
     xs = x_accel * (15 + grand(5)) + (600 - grand(1201));
     ys = y_accel * (15 + grand(5)) + (600 - grand(1201));
    create_cloud(CLOUD_DRIVE_CIRCLE,
     exhx + grand(3001) - 1500 - xs,
     exhy + grand(3001) - 1500 - ys,
     0, 0,
     xs + actor[a].x_speed, ys + actor[a].y_speed,
     base_size + grand(11),shrink + 2,1, 0, 0, 0, passing_colours2);
    create_cloud(CLOUD_DRIVE_CIRCLE, //TRANS_FADING_CIRCLE,
     exhx + grand(3001) - 1500 - xs,
     exhy + grand(3001) - 1500 - ys,
     0, 0,
     xs + actor[a].x_speed, ys + actor[a].y_speed,
     base_size + grand(11),shrink,0, 0, 0, 0, passing_colours);
   }
  } 
 
}
/*
void exhaust_colours(int level, int passing_colours [5])
{
 switch(level)
 {
  default:
  case 0:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_RED8;
   passing_colours [2] = COLOUR_RED6;
   passing_colours [3] = COLOUR_RED4;
   passing_colours [4] = COLOUR_RED3;
   break;
  case 1:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_ORANGE8;
   passing_colours [2] = COLOUR_ORANGE6;
   passing_colours [3] = COLOUR_ORANGE4;
   passing_colours [4] = COLOUR_ORANGE3;
   break;
  case 2:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_YELLOW6;
   passing_colours [2] = COLOUR_YELLOW4;
   passing_colours [3] = COLOUR_YELLOW3;
   passing_colours [4] = COLOUR_YELLOW2;
   break;
  case 3:
   passing_colours [0] = COLOUR_YELLOW8;
   passing_colours [1] = COLOUR_GREEN8;
   passing_colours [2] = COLOUR_GREEN6;
   passing_colours [3] = COLOUR_GREEN4;
   passing_colours [4] = COLOUR_GREEN2;
   break;
  case 4:
   passing_colours [0] = COLOUR_BLUE8;
   passing_colours [1] = COLOUR_BLUE6;
   passing_colours [2] = COLOUR_BLUE4;
   passing_colours [3] = COLOUR_BLUE3;
   passing_colours [4] = COLOUR_BLUE2;
   break;
  case 5:
   passing_colours [0] = COLOUR_WHITE;
   passing_colours [1] = COLOUR_BLUE8;
   passing_colours [2] = COLOUR_BLUE6;
   passing_colours [3] = COLOUR_BLUE4;
   passing_colours [4] = COLOUR_BLUE3;
   break;
 }

}

*/
void actor_turn(int thractor, int direction)
{
 int turned = 100; // 85
 turned += actor[thractor].ability [ABILITY_DRIVE] [SYMBOL_CIRCLE] * 15; // 12

 int target_angle = turned * direction;
 
/* if (actor[thractor].base_angle + target_angle < 0)
  actor[thractor].base_angle += 10240;
   else
   {
    if (actor[thractor].base_angle + target_angle > 10240)
     actor[thractor].base_angle -= 10240;
   }*/
  
 actor[thractor].base_angle += target_angle;
 
 if (actor[thractor].base_angle < 0)
  actor[thractor].base_angle += 10240;
   else
   {
    if (actor[thractor].base_angle > 10240)
     actor[thractor].base_angle -= 10240;
   }
   
 actor[thractor].angle = (actor[thractor].base_angle / 10) % 1024;
}


void actor_shoot(int sactor, int which_weapon)
{

 int recycled = 15;

 if (actor[sactor].recycle1 == 0 && (which_weapon == FIRE_BOTH || which_weapon == FIRE_CANNON_ONLY))
 {
  fire_cannon(sactor);
 }

 if (actor[sactor].secondary != SECOND_NONE && actor[sactor].recycle2 == 0 && (which_weapon == FIRE_BOTH || which_weapon == FIRE_MISSILE_ONLY))
 {
  fire_secondary(sactor);

  recycled = 140; // 66
  
  if (actor[sactor].ship == SHIP_HORSESHOE)
   recycled = 90; // 40

  actor[sactor].recycle2 += recycled;
 }
/*
 if (which_weapon == FIRE_BOTH || which_weapon == FIRE_CANNON_ONLY)
 {
 if (actor[sactor].upgraded_system [UPG_TURRET] > 0
     && actor[sactor].turret_recycle == 0)
 {
  fire_turret(sactor);

  recycled = 12;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 2)
    recycled = 11;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 3)
    recycled = 10;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 4)
    recycled = 9;
   if (actor[sactor].upgraded_system [UPG_TURRET] == 5)
    recycled = 8;

   actor[sactor].turret_recycle += recycled;
    
 }

 if (actor[sactor].upgraded_system [UPG_HEAVY] > 0
     && actor[sactor].heavy_recycle == 0)
 {
  fire_heavy_cannon(sactor);

  recycled = 25;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 2)
    recycled = 20;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 3)
    recycled = 17;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 4)
    recycled = 14;
   if (actor[sactor].upgraded_system [UPG_HEAVY] == 5)
    recycled = 12;

   actor[sactor].heavy_recycle += recycled;
    
 }

 if (actor[sactor].upgraded_system [UPG_BACKFIRE] > 0
     && actor[sactor].backfire_recycle == 0)
 {
  fire_backfire(sactor);

  recycled = 11;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 2)
    recycled = 10;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 3)
    recycled = 9;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 4)
    recycled = 8;
   if (actor[sactor].upgraded_system [UPG_BACKFIRE] == 5)
    recycled = 7;

   actor[sactor].backfire_recycle += recycled;
    
 }

 if (actor[sactor].upgraded_system [UPG_SIDEKICK] > 0
     && actor[sactor].sidekick_recycle == 0)
 {
  fire_sidekicks(sactor);

  recycled = 14;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 2)
    recycled = 13;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 3)
    recycled = 12;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 4)
    recycled = 11;
   if (actor[sactor].upgraded_system [UPG_SIDEKICK] == 5)
    recycled = 10;

   actor[sactor].sidekick_recycle += recycled;
    
 }
 
 }
*/
}

void continue_secondary_burst(int a)
{

 if (actor[a].secondary_burst_recycle == 0)
 {
  fire_secondary(a);
  actor[a].secondary_burst --;
  if (actor[a].secondary_burst == 1)
  {
   actor[a].secondary_burst = 0;  
   return;
  } 
  actor[a].secondary_burst_recycle = BURST_RECYCLE_DELAY;
  if (actor[a].secondary == SECOND_CLAWS)
   actor[a].secondary_burst_recycle = BURST_RECYCLE_DELAY_CLAWS;
  if (actor[a].secondary == SECOND_WORMS_AGONY)
   actor[a].secondary_burst_recycle = BURST_RECYCLE_DELAY_WORMS;
 }
  else
   actor[a].secondary_burst_recycle --;

}

void make_bcolours(int colours [4], int base_colour)
{
 switch(base_colour)
 {
  default:
   colours [0] = base_colour;
   colours [1] = base_colour - 1; // 2
   colours [2] = base_colour - 3; // 4
   colours [3] = base_colour - 5; // 5
   break;
  case TRANS_LBLUE:
   colours [0] = COLOUR_BLUE5;
   colours [1] = COLOUR_BLUE6;
   colours [2] = COLOUR_BLUE7;
   colours [3] = COLOUR_WHITE;
   break;
  case TRANS_DBLUE:
   colours [0] = COLOUR_BLUE4;
   colours [1] = COLOUR_BLUE5;
   colours [2] = COLOUR_BLUE6;
   colours [3] = COLOUR_BLUE7;
   break;
  case TRANS_YELLOW:
   colours [0] = COLOUR_YELLOW3;
   colours [1] = COLOUR_YELLOW5;
   colours [2] = COLOUR_YELLOW7;
   colours [3] = COLOUR_WHITE;
   break;
  case TRANS_ORANGE:
   colours [0] = COLOUR_YELLOW2;
   colours [1] = COLOUR_YELLOW3;
   colours [2] = COLOUR_YELLOW5;
   colours [3] = COLOUR_YELLOW7;
   break;
 } 
    
}

void fire_cannon(int sactor)
{

// int colours [4];
 int bcolours1 [4] = {COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE};
 int bcolours2 [4] = {COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE};
 int btype [2] = {BULLET_ZAP, BULLET_ZAP};
 int damage [2] = {0, 0};
 int timer [2] = {8, 8}; //= 20 - actor[sactor].upgraded_system [UPG_PROJECT] * 2;
 int mass [2] = {10, 10};
// int status = 0;
 int speed [2] = {10000, 10000};//= 16000;
 int rand_speed [2] = {0, 0};
 int speed_div [2] = {5, 5};
 int angles [7] = {0, 0, ANGLE_1_SIXTEENTH, -ANGLE_1_SIXTEENTH, ANGLE_1_EIGHTH, -ANGLE_1_EIGHTH, 0};
// int angles [7] = {0, 0, ANGLE_1_SIXTEENTH, -ANGLE_1_SIXTEENTH, ANGLE_1_EIGHTH, -ANGLE_1_EIGHTH, ANGLE_HALF};
// int angles [7] = {0, 0, 0, 0, 0, 0, ANGLE_HALF};
 int rand_angle = 0;
// unsigned char seed = counter;
// int displace [7] = {-4, 4, 1, -1, 7, -7, 0};
 int displace [7] = {-4, 4, 2, -2, 6, -6, 0};
// int displaced = 0;
 int number = 0;
 int rand_timer [2] = {0, 0};
 int scatter [2] = {0, 0};
 int size = 0;

// actor[sactor].recycle1 = recycle_rates [actor[sactor].primary] [actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE]];
// actor[sactor].recycle1 = recycle_rates [actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE]];
  switch(actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE])
  {
   default:
   case 0: actor[sactor].recycle1 = 21; break; // 21
   case 1: actor[sactor].recycle1 = 18; break; // 17
   case 2: actor[sactor].recycle1 = 15; break; // 14
   case 3: actor[sactor].recycle1 = 13; break; // 12
   case 4: actor[sactor].recycle1 = 12; break; // 10
   case 5: actor[sactor].recycle1 = 11; break; // 9
  }


 switch(actor[sactor].primary)
 {
/*   case WEAPON_SNOW_DARTS:
    btype [0] = BULLET_SNOW_DART;
    btype [1] = BULLET_SNOW_DART_SMALL;
    damage [0] = 80 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 30;
    damage [1] = 60 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 15;
    speed [0] = 25000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 4000;
    speed [1] = 20000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3200;
    timer [0] = 18 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    timer [1] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    rand_timer [0] = 2;
    rand_timer [1] = 2;
    mass [0] = 100;
    mass [1] = 100;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    break;
   case WEAPON_SILVER_TEETH:
    btype [0] = BULLET_SILVER_TOOTH;
    btype [1] = BULLET_SILVER_TOOTH;
    damage [0] = 70 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 20;
    damage [1] = 50 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 12;
    speed [0] = 23000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3100;
    speed [1] = 21000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2600;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, COLOUR_WHITE);
    make_bcolours(bcolours2, COLOUR_GREY6);
    timer [0] = 13 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 80;
    mass [1] = 80;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    break;
   case WEAPON_ICE_DARTS:
    btype [0] = BULLET_ICE_DART;
    btype [1] = BULLET_ICE_DART_SMALL;
    damage [0] = 70 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 20;
    damage [1] = 50 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 12;
    speed [0] = 25000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 4000;
    speed [1] = 20000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3200;
    timer [0] = 18 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    timer [1] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    rand_timer [0] = 2;
    rand_timer [1] = 2;
    mass [0] = 100;
    mass [1] = 100;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    break;
   case WEAPON_FROZEN_BREATH:
    btype [0] = BULLET_FROZEN_BREATH;
    btype [1] = BULLET_FROZEN_BREATH;
    damage [0] = 60 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 15;
    damage [1] = 60 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 15;
    speed [0] = 22000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3200;
    speed [1] = 22000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3200;
    timer [0] = 17 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    timer [1] = 17 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    rand_timer [0] = 4;
    rand_timer [1] = 4;
    mass [0] = 150;
    mass [1] = 150;
    scatter [0] = 5;
    scatter [1] = 5;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    change_angle_arc(angles, ANGLE_HALF);
    break;
   case WEAPON_CRYSTAL_TEETH:
    btype [0] = BULLET_CRYSTAL_TOOTH;
    btype [1] = BULLET_CRYSTAL_TOOTH;
    damage [0] = 20 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 7;
    damage [1] = 20 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 7;
    speed [0] = 17000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2800;
    speed [1] = 17000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2800;
    rand_speed [0] = 8000;
    rand_speed [1] = 8000;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, COLOUR_GREY6);
    make_bcolours(bcolours2, COLOUR_GREY6);
    timer [0] = 13 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 13 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 30;
    mass [1] = 30;
    scatter [0] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 3;
    scatter [1] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 3;
    number = 4 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 2;
    rand_angle = 1;
    break;
   case WEAPON_GOLDEN_NEEDLES:
    btype [0] = BULLET_GOLDEN_NEEDLE;
    btype [1] = BULLET_GOLDEN_NEEDLE_SMALL;
    damage [0] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    damage [1] = 20 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 5;
    speed [0] = 17000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2600;
    speed [1] = 15000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2300;
    speed_div [0] = 2 - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 3 - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
//    bcolours1 [0] = COLOUR_YELLOW8;
//    bcolours2 [0] = COLOUR_YELLOW6;
    make_bcolours(bcolours1, COLOUR_YELLOW8);
    make_bcolours(bcolours2, COLOUR_YELLOW6);
    timer [0] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 13 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 30;
    mass [1] = 30;
    scatter [0] = 3 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    scatter [1] = 5 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 2;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
//    rand_angle = 1;
    break;
   case WEAPON_PARTICLE_SPITTER:
    btype [0] = BULLET_PARTICLE_SPITTER;
    btype [1] = BULLET_PARTICLE_SPITTER;
    damage [0] = 10 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 5;
    damage [1] = 10 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 5;
    speed [0] = 9000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 1000;
    speed [1] = 9000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 1000;
    rand_speed [0] = 4000;
    rand_speed [1] = 4000;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    bcolours1 [0] = COLOUR_GREEN5;
    bcolours2 [1] = COLOUR_GREEN5;
    timer [0] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 6;
    rand_timer [1] = 6;
    mass [0] = 20;
    mass [1] = 20;
    scatter [0] = 70 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 10;
    scatter [1] = 70 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 10;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    rand_angle = 1;
    break;
   case WEAPON_CRYSTAL_SPINES:
    btype [0] = BULLET_CRYSTAL_SPINE;
    btype [1] = BULLET_CRYSTAL_SPINE;
    damage [0] = 50 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 15;
    damage [1] = 50 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 15;
    speed [0] = 21000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2600;
    speed [1] = 21000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2600;
    speed_div [0] = 1;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 1;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, COLOUR_GREY6);
    make_bcolours(bcolours2, COLOUR_GREY6);
    timer [0] = 13 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 13 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 80;
    mass [1] = 80;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    break;
   case WEAPON_FAR_SPITTER:
    btype [0] = BULLET_FAR_SPITTER;
    btype [1] = BULLET_FAR_SPITTER;
    damage [0] = 4 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    damage [1] = 4 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    speed [0] = 12000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 1500;
    speed [1] = 12000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 1500;
    rand_speed [0] = 9000;
    rand_speed [1] = 9000;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    bcolours1 [0] = COLOUR_YELLOW5;
    bcolours2 [1] = COLOUR_YELLOW5;
    timer [0] = 25 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    timer [1] = 25 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    rand_timer [0] = 6;
    rand_timer [1] = 6;
    mass [0] = 20;
    mass [1] = 20;
    scatter [0] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 5;
    scatter [1] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 5;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    rand_angle = 1;
    break;
   case WEAPON_NUMEROUS_BLADES:
    btype [0] = BULLET_NUMEROUS_BLADE;
    btype [1] = BULLET_NUMEROUS_BLADE;
    damage [0] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 15;
    damage [1] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 15;
    speed [0] = 15000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 1000;
    speed [1] = 15000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 1000;
    rand_speed [0] = 4000;
    rand_speed [1] = 4000;
//    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
//    speed_div [1] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    bcolours1 [0] = COLOUR_WHITE;
    bcolours2 [1] = COLOUR_WHITE;
    timer [0] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    timer [1] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3;
    rand_timer [0] = 6;
    rand_timer [1] = 6;
    mass [0] = 40;
    mass [1] = 40;
    scatter [0] = 6 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 2;
    scatter [1] = 6 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 2;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
//    rand_angle = 1;
    break;
  case WEAPON_NUMEROUS_DARTS:
    btype [0] = BULLET_NUMEROUS_DART;
    btype [1] = BULLET_NUMEROUS_DART;
    damage [0] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    damage [1] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    speed [0] = 16000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2500;
    speed [1] = 16000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2500;
    rand_speed [0] = 8000;
    rand_speed [1] = 8000;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, COLOUR_ORANGE8);
    make_bcolours(bcolours2, COLOUR_ORANGE8);
    timer [0] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 30;
    mass [1] = 30;
    scatter [0] = 75 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 11;
    scatter [1] = 75 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 11;
    number = 4 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 2;
    rand_angle = 1;
    break;
   case WEAPON_BRASS_TEETH:
    btype [0] = BULLET_BRASS_TOOTH;
    btype [1] = BULLET_BRASS_TOOTH_SMALL;
    damage [0] = 30 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    damage [1] = 20 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 5;
    speed [0] = 21000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2900;
    speed [1] = 18000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2500;
    speed_div [0] = 1;// - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 2 - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
//    bcolours1 [0] = COLOUR_YELLOW8;
//    bcolours2 [0] = COLOUR_YELLOW6;
    make_bcolours(bcolours1, COLOUR_YELLOW7);
    make_bcolours(bcolours2, COLOUR_YELLOW6);
    timer [0] = 16 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 14 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 30;
    mass [1] = 30;
    scatter [0] = 3 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    scatter [1] = 5 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 2;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
//    rand_angle = 1;
    change_angle_arc(angles, ANGLE_1_SIXTEENTH);
    break;
   case WEAPON_SHOCK_PATH:
    btype [0] = BULLET_SHOCK_PATH;
    btype [1] = BULLET_SHOCK_PATH_SMALL;
    damage [0] = 70 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 20;
    damage [1] = 50 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 12;
    speed [0] = 26000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3400;
    speed [1] = 23000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2900;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, TRANS_LBLUE);
    make_bcolours(bcolours2, TRANS_DBLUE);
    timer [0] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    timer [1] = 11 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 80;
    mass [1] = 80;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    change_angle_arc(angles, ANGLE_1_SIXTEENTH);
    break;
   case WEAPON_GOLDEN_PATH:
    btype [0] = BULLET_GOLDEN_PATH;
    btype [1] = BULLET_GOLDEN_PATH_SMALL;
    damage [0] = 70 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 20;
    damage [1] = 50 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 12;
    speed [0] = 26000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 3400;
    speed [1] = 23000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2900;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, TRANS_YELLOW);
    make_bcolours(bcolours2, TRANS_ORANGE);
    timer [0] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    timer [1] = 11 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 80;
    mass [1] = 80;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
//    change_angle_arc(angles, ANGLE_1_SIXTEENTH);
    break;
   case WEAPON_BURNING_SPIRIT:
    btype [0] = BULLET_BURNING_SPIRIT;
    btype [1] = BULLET_BURNING_SPIRIT;
    damage [0] = 60 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 20;
    damage [1] = 60 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 20;
    speed [0] = 13000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2000;
    speed [1] = 13000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2000;
    speed_div [0] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 3;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, TRANS_YELLOW);
    make_bcolours(bcolours2, TRANS_ORANGE);
    timer [0] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    timer [1] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2;
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 80;
    mass [1] = 80;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
//    change_angle_arc(angles, ANGLE_HALF);
    break;
*/

//Tr + Cr: Crystal teeth - long-range scattergun
//Ci: Golden Needles - fast, single shot, some scattering
//Ci + Cr: Particle Spitter

/*
None: Wooden darts

 Sq: Ice darts - slow, powerful, drag-affected
 Sq + Tr: Silver teeth - slow recycle, long-range, powerful, no drag
 Sq + Ci: Snow darts - like ice darts - trail blue sparks - faster recycle
 Sq + Cr: Frozen breath - short-range arc; reaches 180 degrees?

 Tr: Shock Path - fast-moving, slow recycle
Tr + Ci: Brass teeth - slightly scattered; like Lacewing (ship) cannon
 Tr + Cr: Crystal teeth - long-range scattergun

 Ci: Golden Needles - fast, single shot, some scattering
 Ci + Cr: Particle Spitter

 Cr: Crystal Spines - like Aether Squid

 Sq + Tr + Ci: Golden Path
 Sq + Tr + Cr: Numerous Darts
 Sq + Ci + Cr: Numerous Blades
 Tr + Ci + Cr: Far Spitter

All: Burning Spirit

*/

   default:
   case WPN_DARTS:
    btype [0] = BULLET_ZAP;
    btype [1] = BULLET_ZAP;
    damage [0] = 60 + (actor[sactor].ship == SHIP_POINTY) * 16;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 20;
    damage [1] = 30 + (actor[sactor].ship == SHIP_POINTY) * 8;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
//    speed [0] = 17000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2000;
//    speed [1] = 15000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 1900;
    speed [0] = 17000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2300;
    speed [1] = 15000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2200;
    speed_div [0] = 2; // smaller as triangle increases
    speed_div [1] = 2;
//   displace [0] = -1;
//   displace [1] = 1;
//   displace [2] = 0;
    if (actor[sactor].ship == SHIP_POINTY)
    {
     make_bcolours(bcolours1, COLOUR_WHITE);
     make_bcolours(bcolours2, COLOUR_GREY6);
    }
     else
     {
      make_bcolours(bcolours1, COLOUR_YELLOW8);
      make_bcolours(bcolours2, COLOUR_YELLOW7);
     } 
//    timer [0] = 15;
//    timer [1] = 14;
    timer [0] = 16;
    timer [1] = 15;
    rand_timer [0] = 3;
    rand_timer [1] = 2;
    mass [0] = 10;
    mass [1] = 10;
    scatter [0] = 0;
    scatter [1] = 0;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE];
    break;
/*

REMEMBER: Pointy ship does extra damage.

  case WPN_BURST:
    btype [0] = BULLET_CRYSTAL_SPINE;
    btype [1] = BULLET_CRYSTAL_SPINE;
    damage [0] = 50;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    damage [1] = 50;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    speed [0] = 16000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2500;
    speed [1] = 16000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2500;
    rand_speed [0] = 8000;
    rand_speed [1] = 8000;
    speed_div [0] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 2;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    make_bcolours(bcolours1, COLOUR_WHITE);
    make_bcolours(bcolours2, COLOUR_WHITE);
    timer [0] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 12 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 10;
    mass [1] = 10;
    scatter [0] = 75 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] * 11;
    scatter [1] = 75 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] * 11;
    number = 4 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] * 2;
    rand_angle = 1;
    break;
   case WPN_TEETH:
    btype [0] = BULLET_BRASS_TOOTH;
    btype [1] = BULLET_BRASS_TOOTH_SMALL;
    damage [0] = 60;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    damage [1] = 40;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 5;
    speed [0] = 21000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2900;
    speed [1] = 18000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2500;
    speed_div [0] = 1;// - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 2;// - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
//    bcolours1 [0] = COLOUR_YELLOW8;
//    bcolours2 [0] = COLOUR_YELLOW6;
    make_bcolours(bcolours1, COLOUR_YELLOW8);
    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 16 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 14 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 10;
    mass [1] = 10;
    scatter [0] = 3 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS];
    scatter [1] = 5 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CROSS] * 2;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE];
//    rand_angle = 1;
    change_angle_arc(angles, ANGLE_1_SIXTEENTH);
    change_displacement(displace, 0);
    break;
   case WPN_SPINES:
    btype [0] = BULLET_GOLDEN_NEEDLE;
    btype [1] = BULLET_GOLDEN_NEEDLE_SMALL;
    damage [0] = 60;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 10;
    damage [1] = 40;// + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 5;
    speed [0] = 17000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2600;
    speed [1] = 15000 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] * 2300;
    speed_div [0] = 2;// - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
    speed_div [1] = 2;// - actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE] / 3;
//    bcolours1 [0] = COLOUR_YELLOW8;
//    bcolours2 [0] = COLOUR_YELLOW6;
    make_bcolours(bcolours1, COLOUR_YELLOW8);
    make_bcolours(bcolours2, COLOUR_YELLOW6);
    timer [0] = 15 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    timer [1] = 13 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_TRIANGLE];
    rand_timer [0] = 3;
    rand_timer [1] = 3;
    mass [0] = 10;
    mass [1] = 10;
    scatter [0] = 3 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE];
    scatter [1] = 5 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE] * 2;
    number = 2 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_CIRCLE];
//    rand_angle = 1;
    change_angle_arc(angles, -1);
    change_displacement(displace, 0);
    break;*/
 }

 int i;
 int j = 0;
 int l = 0;
 int shot_angle;
 int k = 0;
 int firing_angle = actor[sactor].angle;

/*
    if (actor[sactor].cannon_status == 0)
    {
     actor[sactor].cannon_status ++;
     shot_angle = 1;
    } 
      else
      {
       actor[sactor].cannon_status = 0;
       shot_angle = -1;
      }
*/
    if ((actor[sactor].cannon_status % 2) == 0)
    {
     shot_angle = 1;
    } 
      else
      {
       shot_angle = -1;
      }

     actor[sactor].cannon_status ++;
     if (actor[sactor].cannon_status == 4)
      actor[sactor].cannon_status = 0;


 for (k = 0; k < 1 + (actor[sactor].ship == SHIP_FINS); k ++)
 {
  
  if (k == 1)
  {
/*    if (actor[sactor].cannon_status / 2 == 0)
     shot_angle = 1;
      else
       shot_angle = -1;
//    actor[sactor].cannon_status ++;
    if (actor[sactor].cannon_status == 4)
     actor[sactor].cannon_status = 0;  */
// duplicates spines code, so the two shouldn't be combined     
    firing_angle += ANGLE_HALF;
//    number /= 2;
//    j = 0;
    l = 0;
    j = 0;//1;
  }
/*  
  if (actor[sactor].primary == WPN_SPINES)
  {
    if (actor[sactor].cannon_status / 2 == 0)
     shot_angle = 1;
      else
       shot_angle = -1;
    actor[sactor].cannon_status ++;
    if (actor[sactor].cannon_status == 4)
     actor[sactor].cannon_status = 0;
  }
   else*/
//   {
//   }   
  
  for (i = 0; i < number; i ++)
  {
    if (i == 1)
     l = 1;
    if (i == 2)
    {
     j = 1;
     l = 2;
    } 
 //   colours [0] = bcolours [j];
//    if (k == 1) // ie backfire from finny ship
//    {
//     if (i % 2 != actor[sactor].cannon_status % 2)
//      continue;
//    }
    
    if (rand_angle == 1)
    {
       cannon_shot(sactor, btype [j], damage [j], timer [j] + grand(rand_timer [j]), mass [j],    
        firing_angle,
        0, size, speed [j] + grand(rand_speed [j]), bcolours1, displace [i], scatter [j], speed_div [j]); 
    }
     else
     {
      if (j == 0)
       cannon_shot(sactor, btype [j], damage [j], timer [j] + grand(rand_timer [j]), mass [j],    
        firing_angle + (angles [i] * shot_angle),
        0, size, speed [j] + grand(rand_speed [j]), bcolours1, displace [i] * shot_angle, scatter [j], speed_div [j]); 
         else
          cannon_shot(sactor, btype [j], damage [j], timer [j] + grand(rand_timer [j]), mass [j],    
           firing_angle + (angles [i] * shot_angle),
           0, size, speed [j] + grand(rand_speed [j]), bcolours2, displace [i] * shot_angle, scatter [j], speed_div [j]); 
     }
  } // end for i
 } // end for k
    

// actor_sound(sactor, ASOUND_CANNON);
  play_wavf(NWAV_DART, 800 + actor[sactor].ability [ABILITY_PRIMARY] [SYMBOL_SQUARE] * 80);


}


void change_angle_arc(int angles [14], int width)
{

 switch(width)
 {
  case ANGLE_HALF:
   angles [2] = ANGLE_1_EIGHTH; 
   angles [3] = -ANGLE_1_EIGHTH; 
   angles [4] = ANGLE_QUARTER; 
   angles [5] = -ANGLE_QUARTER; 
   angles [6] = ANGLE_HALF; 
   break;
  case ANGLE_1_SIXTEENTH:
   angles [2] = ANGLE_1_32; 
   angles [3] = -ANGLE_1_32; 
   angles [4] = ANGLE_1_SIXTEENTH; 
   angles [5] = -ANGLE_1_SIXTEENTH; 
   angles [6] = ANGLE_HALF; 
   break;
  case -1:
   angles [2] = ANGLE_1_SIXTEENTH; 
   angles [3] = ANGLE_1_EIGHTH; 
   angles [4] = -ANGLE_1_EIGHTH; 
   angles [5] = -ANGLE_1_SIXTEENTH;
   angles [6] = ANGLE_HALF; 
   break;
  case -2:
   angles [0] = ANGLE_HALF - ANGLE_1_SIXTEENTH; 
   angles [1] = ANGLE_HALF + ANGLE_1_SIXTEENTH; 
   angles [2] = ANGLE_HALF - ANGLE_1_EIGHTH; 
   angles [3] = ANGLE_HALF + ANGLE_1_EIGHTH; 
   angles [4] = ANGLE_QUARTER; 
   angles [5] = -ANGLE_QUARTER;
   angles [6] = ANGLE_HALF - ANGLE_1_32; 
   angles [7] = ANGLE_HALF + ANGLE_1_32; 
   angles [8] = ANGLE_HALF - ANGLE_1_SIXTEENTH - ANGLE_1_32; 
   angles [9] = ANGLE_HALF + ANGLE_1_SIXTEENTH + ANGLE_1_32; 
   angles [10] = ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_32; 
   angles [11] = ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_32; 
   angles [12] = ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH; 
   angles [13] = ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH; 
   break;
 }
}

void change_displacement(int displace [7], int type)
{
 switch(type)
 {
  case 0:
   displace [0] = -1;
   displace [1] = 1;
   displace [2] = 0;
   displace [3] = 0;
   displace [4] = 0;
   displace [5] = 0;
   displace [6] = 0;
   break;
 } 
}

void cannon_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int size, int speed, int colours [4], int displaced, 
 int scatter, int speed_div)
{
  angle += scatter;
  angle -= grand(scatter * 2);

//  speed += actor[sactor].upgraded_system [UPG_PROJECT] * 5000;
//  speed_div -= actor[sactor].upgraded_system [UPG_PROJECT];

 int xs = actor[sactor].x_speed + xpart(angle - ANGLE_QUARTER, speed / 3);
 int ys = actor[sactor].y_speed + ypart(angle - ANGLE_QUARTER, speed / 3);

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (displaced != 0)
 {
//  x += xpart(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced), GRAIN * displaced);
//  y += ypart(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced), GRAIN * displaced);
  x += xpart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN * displaced);
  y += ypart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN * displaced);
 }

 if (angle == 0)
  angle = 1;

 create_bullet(btype, x, y,
  xs, ys, sactor,
  damage, timer, mass, actor[sactor].angle,
  status, size, colours, speed_div, 0, 0, 0, 0, 0);

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

 x = actor[sactor].x + xpart(angle - ANGLE_QUARTER, actor[sactor].flash_distance);
 y = actor[sactor].y + ypart(angle - ANGLE_QUARTER, actor[sactor].flash_distance);

 if (displaced != 0)
 {
  x += xpart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN * displaced);
  y += ypart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN * displaced);
 }


    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
     0, 0, 0, 0, cloud_colours);

}





int adjust_damage(int base, int sactor)
{
 return base + (base * actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE]) / 3;
}


void fire_secondary(int sactor)
{
/*
    specials:
   1 = dam
   2 = explosion cloud size
   3 = blast radius
   4 = force
   */

// int colours [4];
 int bcolours1 [4] = {TRANS_DRED, TRANS_ORANGE, TRANS_YELLOW, TRANS_YELLOW};
 int bcolours2 [4] = {TRANS_DRED, TRANS_ORANGE, TRANS_YELLOW, TRANS_YELLOW};
 int btype [2] = {BULLET_ZAP, BULLET_ZAP};
 int damage [2] = {0, 0};
 int special1 [2] = {0, 0};
 int special2 [2] = {0, 0};
 int special3 [2] = {0, 0};
 int special4 [2] = {0, 0};
 int special5 [2] = {0, 0};
 int timer [2] = {10, 10}; //= 20 - actor[sactor].upgraded_system [UPG_PROJECT] * 2;
 int mass [2] = {100, 100};
// int status = 0;
 int speed [2] = {10000, 10000};//= 16000;
 int rand_speed [2] = {0, 0};
 int speed_div [2] = {5, 5};
 int angles [20] = {0, 0, ANGLE_1_SIXTEENTH, -ANGLE_1_SIXTEENTH, ANGLE_1_EIGHTH, -ANGLE_1_EIGHTH, 
  ANGLE_1_SIXTEENTH + ANGLE_1_32,
  - ANGLE_1_SIXTEENTH - ANGLE_1_32,
  ANGLE_1_32,
  - ANGLE_1_32,
  ANGLE_HALF,
  ANGLE_HALF,
  0,
  0,
/*  ANGLE_1_EIGHTH + ANGLE_1_32,
  - ANGLE_1_EIGHTH - ANGLE_1_32,
  ANGLE_1_EIGHTH - ANGLE_1_32,
  - ANGLE_1_EIGHTH + ANGLE_1_32,*/
//  ANGLE_QUARTER - ANGLE_1_32,
//   - ANGLE_QUARTER + ANGLE_1_32,
//  ANGLE_1_32,
//  - ANGLE_1_32
 };
 int rand_angle = 0;
// unsigned char seed = counter;
 int displace [3] = {-1, 1, 0};
 int number = 0;
 int rand_timer [2] = {0, 0};
 int scatter [2] = {0, 0};
 int pattern = PATTERN_TWO_PLUS_OTHERS;
 int size = 0;
 int base_speed;
 int flash_circle = TRANS_ORANGE;

// actor[sactor].recycle2 = 66;


 switch(actor[sactor].secondary)
 {
/*
Square - Damage
Circle - Number
Triangle - Size/Speed
*/
   default:
   case SECOND_FURIOUS_ORB:
    btype [0] = BULLET_FURIOUS_ORB;
    btype [1] = BULLET_FURIOUS_ORB_SMALL;
    damage [0] = 1;
    damage [1] = 1;
    // blast damage:
//    special1 [0] = 175 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 85;
    special1 [0] = adjust_damage(150, sactor);
    special1 [1] = special1 [0] / 2; //100 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 50;
    // visible explosion size:
    special2 [0] = 50 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 20;
    special2 [1] = 20 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10;
    // blast size:
    special3 [0] = 50000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10000;
    special3 [1] = 20000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 5000;
    // blast force
    special4 [0] = adjust_damage(300, sactor);
    special4 [1] = adjust_damage(100, sactor);
    speed [0] = 5000;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 1000;
    speed [1] = 2000;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 500;
    rand_speed [0] = 0;
    rand_speed [1] = 5000;
    speed_div [0] = 7; // smaller as triangle increases
    speed_div [1] = 8;
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
//    make_bcolours(bcolours1, COLOUR_YELLOW8);
//    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 50;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 5;
    timer [1] = 20;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 2;
    rand_timer [0] = 0;
    rand_timer [1] = 30;
    mass [0] = 100;
    mass [1] = 30;
    scatter [0] = 0;
    scatter [1] = 30;
    number = 1 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    pattern = PATTERN_ONE_PLUS_OTHERS;
    size = 3000;
//    play_wavf(NWAV_SPLERK, 1800);
    break;
   case SECOND_MANIFOLD_ORB:
    btype [0] = BULLET_MANIFOLD_ORB;
    damage [0] = 1;
    // blast damage
//    special1 [0] = 175 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 85;
    special1 [0] = adjust_damage(150, sactor);
    // visible explosion size
    special2 [0] = 50 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 20;
    // blast size
    special3 [0] = 50000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10000;
    // blast force
//    special4 [0] = 300 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 150;
    special4 [0] = adjust_damage(300, sactor);
    special5 [0] = actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    speed [0] = 5000;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
//    make_bcolours(bcolours1, COLOUR_YELLOW8);
//    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 45;
    rand_timer [0] = 0;
    mass [0] = 100;
    scatter [0] = 0;
    number = 1;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];
    pattern = PATTERN_ONE_PLUS_OTHERS;
    size = 3000;
//    play_wavf(NWAV_SPLERK, 1700);
    break;
   case SECOND_WORMS_SORROW:
/*
Square - Damage
Circle - Number
Triangle - Size/Speed
*/
    btype [0] = BULLET_WORM_SORROW;
//    damage [0] = 40 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 20;
    damage [0] = adjust_damage(13, sactor);
// REMEMBER - these are actually agony
    speed [0] = 10000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 100;
//    special1 [0] = 2500 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 900;
    special1 [0] = 2500 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 400;
    special3 [0] = -1;
//    special4 [0] = 8 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 3;
//    special4 [0] = 36 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 12;
    special4 [0] = 36 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 6;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
    bcolours1 [0] = COLOUR_YELLOW4;
    bcolours1 [1] = COLOUR_YELLOW5;
    bcolours1 [2] = COLOUR_YELLOW7;
    bcolours1 [3] = COLOUR_WHITE;
    timer [0] = 35 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 2;
    special5 [0] = actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 2;
    rand_timer [0] = 0; // so they all lock on at once. Randomised later
    mass [0] = 7;
    scatter [0] = 0;
    number = 4 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    pattern = PATTERN_ALL_SAME;
    change_angle_arc(angles, -2);
    flash_circle = TRANS_YELLOW;
    play_wavf(NWAV_WORMS, 1000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 50);
    break; 
   case SECOND_WORMS_AGONY:
    btype [0] = BULLET_WORM_AGONY;
// REMEMBER - these are actually sorrow    
//    damage [0] = 20 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 10;
    damage [0] = adjust_damage(10, sactor);
    speed [0] = 5000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 100;
    special1 [0] = 600 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 160;
//    special1 [0] = 300 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 120;
    special3 [0] = -1;
    special4 [0] = 8 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 3;
    special5 [0] = 0;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
    bcolours1 [0] = COLOUR_YELLOW4;
    bcolours1 [1] = COLOUR_YELLOW5;
    bcolours1 [2] = COLOUR_GREY6;
    bcolours1 [3] = COLOUR_WHITE;
    timer [0] = 150;
    rand_timer [0] = 0;
    mass [0] = 7;
    scatter [0] = 0;
    number = 1;
    pattern = PATTERN_ALL_SAME;
    if (actor[sactor].secondary_burst == 0)
    {
     actor[sactor].secondary_burst = 4 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
     actor[sactor].secondary_burst_recycle = BURST_RECYCLE_DELAY_WORMS;
    }
    play_wavf(NWAV_WORMS, 1200 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 50);
    angles [0] = ANGLE_HALF; 
    flash_circle = TRANS_YELLOW;
    break; 
   case SECOND_PANIC_EELS:
/*
Square - Damage
Circle - Number
Triangle - Size/Speed
*/
    btype [0] = BULLET_PANIC_EEL;
//    damage [0] = 50 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 25;
    damage [0] = adjust_damage(50, sactor);
    speed [0] = 5000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 100;
    special2 [0] = 64;
//    if (grand(2) == 0)
//     special2 [0] = -72;  in bullet.c
//    special3 [0] = 5 + grand(7);
//    special1 [0] = 900;
    special1 [0] = 900 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 220;
//    special3 [0] = -1;
    special4 [0] = actor[sactor].angle;
    special5 [0] = 9;// + grand(3);
//    special4 [0] = 8 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 3;
    rand_speed [0] = 8000;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
    bcolours1 [0] = COLOUR_GREEN4;
    bcolours1 [1] = COLOUR_GREEN5;
    bcolours1 [2] = COLOUR_GREEN6;
    bcolours1 [3] = COLOUR_GREEN8;
    timer [0] = 150;
    rand_timer [0] = 30;
    mass [0] = 30;
    scatter [0] = ANGLE_FULL;//1_32;
    number = 4 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    pattern = PATTERN_ALL_SAME;
//    change_angle_arc(angles, ANGLE_1_32);
    rand_angle = 1;
    flash_circle = TRANS_LGREEN;
    play_wavf(NWAV_TEETH, 100 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 5);
    break; 
   case SECOND_EYE_DESOLATION:
    btype [0] = BULLET_EYE_DESOLATION;
    damage [0] = 1;
    // blast damage
//    special1 [0] = 100 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 50; //150 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 75;
    special1 [0] = adjust_damage(100, sactor);
    // visible explosion size
    special2 [0] = 50 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 20;
    // blast size
    special3 [0] = 50000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10000;
    // acceleration
    special4 [0] = 200 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 100;
    // number of little things
    special5 [0] = actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    speed [0] = 2000;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
//    make_bcolours(bcolours1, COLOUR_YELLOW8);
//    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 55;
    rand_timer [0] = 0;
    mass [0] = 100;
    scatter [0] = 0;
    number = 1;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];
    pattern = PATTERN_ONE_PLUS_OTHERS;
    play_wavf(NWAV_JET, 500 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 50);
    break;
   case SECOND_BURNING_EYE:
    btype [0] = BULLET_BURNING_EYE;
    damage [0] = 1;
    // blast damage
//    special1 [0] = 80 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 40;
    special1 [0] = adjust_damage(80, sactor);
    // visible explosion size
    special2 [0] = 20 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 5;
    // blast size
    special3 [0] = 20000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 4000;
    // acceleration
    special4 [0] = 250 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 120;
    // number of little things
    special5 [0] = 1;//2 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    speed [0] = -6000;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
//    make_bcolours(bcolours1, COLOUR_YELLOW8);
//    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 55;
    rand_timer [0] = 20;
    mass [0] = 30;
    scatter [0] = 16;
    number = 1;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];
    pattern = PATTERN_ONE_PLUS_OTHERS;
    if (actor[sactor].secondary_burst == 0)
    {
     actor[sactor].secondary_burst = 2 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE];
     actor[sactor].secondary_burst_recycle = BURST_RECYCLE_DELAY;
    }
    play_wavf(NWAV_JET, 1200 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 200);
    break;
   case SECOND_FROZEN_TEETH:
    btype [0] = BULLET_FROZEN_TOOTH;
//    damage [0] = 100 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 50;
    damage [0] = adjust_damage(80, sactor);
    // blast damage
//    special1 [0] = 80 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 40;
//    special1 [0] = 60 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 30;
    // visible explosion size
    special2 [0] = 5 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 2;
    // blast size
//    special3 [0] = 20000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 4000;
    // acceleration
    special4 [0] = 120 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 60;
    // acceleration angle
    special5 [0] = actor[sactor].angle;
    speed [0] = 0;
    base_speed = 3000;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
    bcolours1 [0] = COLOUR_BLUE5;
    bcolours1 [1] = COLOUR_BLUE6;
    bcolours1 [2] = COLOUR_BLUE7;
    bcolours1 [3] = COLOUR_BLUE8;
//    make_bcolours(bcolours1, COLOUR_YELLOW8);
//    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 55;
    rand_timer [0] = 20;
    mass [0] = 30;
    scatter [0] = 16;
//    number = 2 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE];
    number = 4 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    pattern = PATTERN_SIDEWARDS;
    flash_circle = TRANS_LBLUE;
    play_wavf(NWAV_TEETH, 300 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 30);
    break;
   case SECOND_FROZEN_STARS:
    btype [0] = BULLET_FROZEN_STAR;
//    damage [0] = 50 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 25;
    damage [0] = adjust_damage(50, sactor);
    speed [0] = 4000; //8000;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 1900;
//    speed [0] = 5000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 1500;
    rand_speed [0] = 9000;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
    bcolours1 [0] = COLOUR_WHITE;
    bcolours1 [1] = COLOUR_BLUE8;
    bcolours1 [2] = COLOUR_BLUE6;
    bcolours1 [3] = COLOUR_BLUE8;
    timer [0] = 60 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 35;
    rand_timer [0] = 90;
    mass [0] = 30;
    scatter [0] = ANGLE_HALF;//FULL;
    number = 8 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 4;
    pattern = PATTERN_ALL_SAME;
//    change_angle_arc(angles, ANGLE_1_32);
    rand_angle = 1;
    flash_circle = TRANS_LBLUE;
    play_wavf(NWAV_TEETH, 1300);
    break; 
   case SECOND_TOXIC_SUN:
    btype [0] = BULLET_TOXIC_SUN;
    // does little damage itself
//    damage [0] = 30 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 15;
    damage [0] = adjust_damage(30, sactor);
    // flare damage
//    special1 [0] = 60 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 30;
    special1 [0] = adjust_damage(60, sactor);
    // next flare
    special2 [0] = 44;
    // flare visible explosion size
//    special2 [0] = 50 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 20;
    // blast size
//    special3 [0] = 50000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10000;
    // flare speed
    special4 [0] = 5000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 1000;
    // number of flares
//    special5 [0] = 4 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2;
    speed [0] = 2000;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
//    make_bcolours(bcolours1, COLOUR_YELLOW8);
//    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 44 + (4 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 2) * 3;
    rand_timer [0] = 0;
    mass [0] = 100;
    scatter [0] = 0;
    number = 1;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];
    pattern = PATTERN_ONE_PLUS_OTHERS;
//  play_wavf(NWAV_WORMS, 200);
    play_wavf(NWAV_SPLERK, 300);
    break;
   case SECOND_SPORES:
    btype [0] = BULLET_SPORE;
//    damage [0] = 80 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 40;
    damage [0] = adjust_damage(80, sactor);
    special1 [0] = 80 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 40;
    special3 [0] = -1;
    special4 [0] = 36 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 12;
    special5 [0] = 1;


    speed [0] = 2000;
    rand_speed [0] = 2000;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
//    make_bcolours(bcolours1, COLOUR_YELLOW8);
//    make_bcolours(bcolours2, COLOUR_YELLOW7);
    timer [0] = 100 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10;
//    rand_timer [0] = 20;
//    special1 [0] = timer [0] - 10;
    mass [0] = 50;
    scatter [0] = ANGLE_FULL;
    number = 1;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];
    pattern = PATTERN_ONE_PLUS_OTHERS;
    if (actor[sactor].secondary_burst == 0)
    {
     actor[sactor].secondary_burst = 2 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE];
     actor[sactor].secondary_burst_recycle = BURST_RECYCLE_DELAY;
    }
    play_wavf(NWAV_WORMS, 700);
    break;
   case SECOND_FLOWER:
    btype [0] = BULLET_FLOWER;
    damage [0] = 1;
    // blast damage
//    special1 [0] = 80 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 40; //150 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 75;
    special1 [0] = adjust_damage(80, sactor);
    // visible explosion size
    special2 [0] = 50 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 20;
    // blast size
    special3 [0] = 50000 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 20000;
    // acceleration
    special4 [0] = 200 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 100;
    // number of little things
    special5 [0] = 6 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 3;
    bcolours1 [0] = TRANS_YELLOW;
    bcolours1 [1] = TRANS_ORANGE;
    bcolours1 [2] = TRANS_LRED;
    bcolours1 [3] = TRANS_DRED;
    speed [0] = 0;
    rand_speed [0] = 0;
    speed_div [0] = 7; // smaller as triangle increases
    displace [0] = 0;
    displace [1] = 0;
    displace [2] = 0;
    timer [0] = 66;
    rand_timer [0] = 0;
    mass [0] = 100;
    scatter [0] = 0;
    number = 1;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];
    pattern = PATTERN_ONE_PLUS_OTHERS;
    play_wavf(NWAV_SPLERK, 250);
    break;
   case SECOND_CLAWS:
    btype [0] = BULLET_CLAW;
//    damage [0] = 25 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_SQUARE] * 12; // damage
    damage [0] = adjust_damage(25, sactor);
    special1 [0] = 500 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 150; // accel
    special2 [0] = actor[sactor].angle + grand(ANGLE_1_32) - grand(ANGLE_1_32);
    special3 [0] = 25;
    bcolours1 [0] = TRANS_YELLOW;
    bcolours1 [1] = TRANS_ORANGE;
    bcolours1 [2] = TRANS_LRED;
    bcolours1 [3] = TRANS_DRED;
    speed [0] = 12000;
    rand_speed [0] = 5000;
    speed_div [0] = 7; // smaller as triangle increases
    timer [0] = 100;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10;
    mass [0] = 20;
    scatter [0] = ANGLE_FULL;
    number = 1;// + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CROSS];
    pattern = PATTERN_ONE_PLUS_OTHERS;
    if (actor[sactor].secondary_burst == 0)
    {
     actor[sactor].secondary_burst = 6 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_CIRCLE] * 3;
     actor[sactor].secondary_burst_recycle = BURST_RECYCLE_DELAY_CLAWS;
    }
    flash_circle = 0;
    play_wav2(NWAV_TEETH, 1500 + actor[sactor].ability [ABILITY_SECONDARY] [SYMBOL_TRIANGLE] * 10, 80, 127);
    break;

 }
/*
 When adding seekers, remember to clear targets in enemy.c when target is destroyed!
*/

 int i;
 int j = 0;
 int l = 0;
 int shot_angle;
 int angle_fired = actor[sactor].angle;
 if (actor[sactor].bomb_status == 0)
 {
  actor[sactor].bomb_status = 1;
  shot_angle = 1;
 } 
   else
   {
    actor[sactor].bomb_status = 0;
    shot_angle = -1;
   }
 
 for (i = 0; i < number; i ++)
 {
  switch(pattern)
  {
   case PATTERN_TWO_PLUS_OTHERS:
    if (i == 1)
     l = 1;
    if (i == 2)
    {
     j = 1;
     l = 2;
    } 
    break;
   case PATTERN_ONE_PLUS_OTHERS:
    if (i == 1)
    {
     l = 2;
     j = 1;
    } 
    break;
   case PATTERN_SIDEWARDS:
//    special1 [j] = actor[sactor].angle;
    if (i % 2 == 0)
     angles [i] = ANGLE_QUARTER;
      else
       angles [i] = - ANGLE_QUARTER;
     l = 0;
     j = 0;
     speed [j] = (i / 2 + 1) * base_speed;
   break; 
// PATTERN_ALL_SAME - don't need to change   
  } 
//   colours [0] = bcolours [j];
   if (rand_angle == 1)
   {
      secondary_shot(sactor, btype [j], damage [j], timer [j] + grand(rand_timer [j]), mass [j],    
       angle_fired + grand(scatter [j]) - grand(scatter [j]),
       0, 0, speed [j] + grand(rand_speed [j]), bcolours1, displace [l], scatter [j], speed_div [j], special1 [j], special2 [j], special3 [j], special4 [j], special5 [j], flash_circle); 
   }
    else
    {
     if (j == 0)
      secondary_shot(sactor, btype [j], damage [j], timer [j] + grand(rand_timer [j]), mass [j],    
       angle_fired + (angles [i] * shot_angle),
       0, size, speed [j] + grand(rand_speed [j]), bcolours1, displace [l], scatter [j], speed_div [j], special1 [j], special2 [j], special3 [j], special4 [j], special5 [j], flash_circle); 
        else
         secondary_shot(sactor, btype [j], damage [j], timer [j] + grand(rand_timer [j]), mass [j],    
          angle_fired + (angles [i] * shot_angle),
          0, size, speed [j] + grand(rand_speed [j]), bcolours2, displace [l], scatter [j], speed_div [j], special1 [j], special2 [j], special3 [j], special4 [j], special5 [j], flash_circle); 
     }     
 }

// actor_sound(sactor, ASOUND_CANNON);

}


void secondary_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int size, int speed, int colours [4], int displaced, 
 int scatter, int speed_div, int special1, int special2, int special3, int special4, int special5,
 int flash_colour)
{
  angle += scatter;
  angle -= grand(scatter * 2);

 int xs = actor[sactor].x_speed + xpart(angle - ANGLE_QUARTER, speed / 3);
 int ys = actor[sactor].y_speed + ypart(angle - ANGLE_QUARTER, speed / 3);

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (displaced != 0)
 {
  x += xpart(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced), GRAIN * 3);
  y += ypart(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced), GRAIN * 3);
 }

 if (angle == 0)
  angle = 1;

 create_bullet(btype, x, y,
  xs, ys, sactor,
//  damage, timer, mass, actor[sactor].angle,
  damage, timer, mass, angle,
  status, size, colours, speed_div, special1, special2, special3, special4, special5);

 int cloud_colours [5];
 cloud_colours [0] = flash_colour;

 x = actor[sactor].x + xpart(angle - ANGLE_QUARTER, actor[sactor].flash_distance);
 y = actor[sactor].y + ypart(angle - ANGLE_QUARTER, actor[sactor].flash_distance);

 if (displaced != 0)
 {
  x += xpart(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced), GRAIN * 3);
  y += ypart(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced), GRAIN * 3);
 }

  if (flash_colour != 0)
    create_cloud(CLOUD_MED_TRANS_CIRCLE, x, y, 0, 0,
     actor[sactor].x_speed, actor[sactor].y_speed, 400, 50,
     0, 0, 0, 0, cloud_colours);

}










void fire_missile(int sactor)
{

 int colours [4] = {TRANS_LGREY, TRANS_DRED, TRANS_LRED, TRANS_YELLOW};

 int btype = BULLET_MISSILE;
 int damage = 30; // misleading; the explosion changes this
 if (actor[sactor].upgraded_system [UPG_WARHEAD] > 0)
 {
  damage += actor[sactor].upgraded_system [UPG_WARHEAD] * 8;
/*  switch(actor[sactor].upgraded_system [UPG_WARHEAD])
  {
   default:
   case 0:
    colours [3] = TRANS_YELLOW;
    break;
   case 1:
    colours [3] = TRANS_LRED;
    break;
   case 2:
    colours [3] = TRANS_LBLUE;
    break;
   case 3:
    colours [3] = TRANS_DBLUE;
    break;
   case 4:
    colours [3] = TRANS_LGREEN;
    break;
   case 5:
    colours [3] = TRANS_PURPLE;
    break;
  }*/
 }
/*
UPG_WARHEAD,
// increases missile damage
// special: cluster warhead
UPG_SEEKER,
// makes missiles track nearest target, then increases turn rate
// special:
//  1. Dauntless (seekers don't explode when target does; acquire new one)
//  2. Smart guidance - go for most important target
//  3. Target Analysis - send only enough missiles to destroy; prefer weaker
//      targets if they'll be destroyed
UPG_ROCKET,
// increases missile acceleration, but not range by much
// special: long range
UPG_TUBES,
// increases number of missiles
// special: extra mini-missiles, ?random directions, not seeking
*/
 int timer = 45;// - actor[sactor].upgraded_system [UPG_ROCKET];
 int mass = 10;
 int status = 0;
 int speed = 1000;
 unsigned char seed = counter;
 int size = 0;

 if (actor[sactor].angle == 0)
  actor[sactor].angle = 1; // I have no idea why this is necessary

 int specials [7] = {0,6,1,5,2,4,3};



 if (actor[sactor].bomb_status == 1)
 {
  specials [0] = 6;
  specials [1] = 0;
  specials [2] = 5;
  specials [3] = 1;
  specials [4] = 4;
  specials [5] = 2;
  specials [6] = 3;
  actor[sactor].bomb_status = 0;
 }
  else
   actor[sactor].bomb_status = 1;

 switch(actor[sactor].upgraded_system [UPG_TUBES])
 {
/*  case 1:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     break;*/
  case 0:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, size, speed, colours, 1, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, size, speed, colours, -1, specials [1]);
     break;
  case 1:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, size, speed, colours, 1, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, size, speed, colours, -1, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, size, speed, colours, 0, specials [2]);
     break;
  case 2:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 2, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -2, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [3]);
     break;
  case 3:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 2, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -2, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [4]);
     break;
/*  case 4:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + ANGLE_HALF,
      status, seed, speed, colours, 0, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 2, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -2, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [4]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [5]);
     break;*/
  case 4:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 3, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -3, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 2, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -2, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [4]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [5]);
     break;
  case 5:
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0, specials [0]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle + 16,
      status, seed, speed, colours, 3, specials [1]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle - 16,
      status, seed, speed, colours, -3, specials [2]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 2, specials [3]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -2, specials [4]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 1, specials [5]);
     missile_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, -1, specials [6]);
     break;
 }
//     cannon_shot(sactor, btype, damage, timer, mass, actor[sactor].angle,
//      status, seed, speed, colours, 1);

// actor_sound(sactor, ASOUND_TUBE);

}



void missile_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced,
 int special5)
{

 int special1;
 int special2;
 int special3;
 int special4;
 special5 *= 3;

 timer += grand(15);

 speed += actor[sactor].upgraded_system [UPG_ROCKET] * 1000;

 special1 = 200 + actor[sactor].upgraded_system [UPG_ROCKET] * 50;

 int xs = actor[sactor].x_speed + xpart(angle - ANGLE_QUARTER, speed / 3);
 int ys = actor[sactor].y_speed + ypart(angle - ANGLE_QUARTER, speed / 3);

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (displaced != 0)
 {
//  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
//  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
  x += xpart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN * 5 * displaced);
  y += ypart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN * 5 * displaced);
  xs += xpart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN / 8 * displaced);
  ys += ypart(angle - ANGLE_QUARTER + (ANGLE_QUARTER), GRAIN / 8 * displaced);
 }

 special3 = -1;

 if (actor[sactor].lock != -1)// && enemy[actor[sactor].lock].type != ENEMY_NONE)
 {
  if (actor[sactor].lock == LOCK_ACTOR0 && actor[0].in_play == 1)
   special3 = actor[sactor].lock;
  if (actor[sactor].lock == LOCK_ACTOR1 && actor[1].in_play == 1)
   special3 = actor[sactor].lock;
  if (actor[sactor].lock >= 0 && enemy[actor[sactor].lock].type != ENEMY_NONE)
   special3 = actor[sactor].lock;
 }
 
 special4 = 32 + 16 * actor[sactor].upgraded_system [UPG_SEEKER];
 
 if (angle == 0)
  angle = 1;

 create_bullet(btype, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, 1, special1, special2, special3, special4, special5);


}

void fire_bomb(int sactor)
{

 int colours [4] = {TRANS_LGREY, TRANS_DRED, TRANS_LRED, TRANS_YELLOW};


// int btype = BULLET_BOMB;
 int damage = 50; // misleading; the explosion changes this
 if (actor[sactor].upgraded_system [UPG_BOMB] > 0)
 {
  damage += actor[sactor].upgraded_system [UPG_BOMB] * 25;
 }
/*
UPG_BOMB,
// increases bomb damage
// special: dispersion (explosion size)
UPG_LAUNCHER,
// adds more bombs (side-bombs are smaller)
// special: cluster bombs (main one only?)
UPG_LOADER,
// increases bomb firing rate
// special: long range
*/
 int timer = 30;// + actor[sactor].upgraded_system [UPG_ROCKET];
 int mass = 10;
 int status = 0;
 int speed = 6500;
 unsigned char seed = counter;

 switch(actor[sactor].upgraded_system [UPG_LAUNCHER])
 {
  case 0:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     break;
  case 1:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     if (actor[sactor].bomb_status == 0)
     {
      bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
       status, seed, speed, colours, 0);
      actor[sactor].bomb_status = 1;
     } else
      {
       bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
        status, seed, speed, colours, 0);
       actor[sactor].bomb_status = 0;
      }
     break;
  case 2:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     break;
  case 3:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     if (actor[sactor].bomb_status == 0)
     {
      bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_2,
       status, seed, speed, colours, 0);
      actor[sactor].bomb_status = 1;
     } else
      {
       bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_2,
        status, seed, speed, colours, 0);
       actor[sactor].bomb_status = 0;
      }


     break;
  case 4:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     break;
  case 5:
     bomb_shot(sactor, BULLET_BOMB, damage, timer, mass, actor[sactor].angle,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_1,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle - BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + BOMB_ANGLE_2,
      status, seed, speed, colours, 0);
     bomb_shot(sactor, BULLET_BOMB_SIDE, damage, timer, mass, actor[sactor].angle + ANGLE_HALF,
      status, seed, speed, colours, 0);
     break;
 }

// actor_sound(sactor, ASOUND_BOMBS);


}



void bomb_shot(int sactor, int btype, int damage, int timer, int mass,
 int angle, int status, int seed, int speed, int colours [4], int displaced)
{
 int special1;
 int special2;
 int special3;
 int special4;
 int special5;

 timer += grand(4);

//  speed += actor[sactor].upgraded_system [UPG_ROCKET] * 1000;

/*
UPG_BOMB,
// increases bomb damage
// special: dispersion (explosion size)
UPG_LAUNCHER,
// adds more bombs (side-bombs are smaller)
// special: cluster bombs (main one only?)
UPG_LOADER,
// increases bomb firing rate
// special: long range
*/

 special1 = 30000 + actor[sactor].upgraded_system [UPG_BOMB] * 5000;
 // size of bomb's explosion
 if (btype == BULLET_BOMB_SIDE)
 {
  damage /= 2;
  special1 /= 2;
 }
     
 special2 = 0;
// int special1;
 // missiles: acceleration
 // bombs: explosion size
// int special2;
 // missiles: number of clusters
 // bombs: number of clusters


 int xs = actor[sactor].x_speed + xpart(angle - ANGLE_QUARTER, speed / 3);
 int ys = actor[sactor].y_speed + ypart(angle - ANGLE_QUARTER, speed / 3);

 int x = actor[sactor].x;
 int y = actor[sactor].y;

/* if (displaced != 0)
 {
//  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
//  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3 * displaced;
  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 5 * displaced;
  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 5 * displaced;
  xs += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN / 8 * displaced;
  ys += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN / 8 * displaced;
 }*/

 if (angle == 0)
  angle = 1;

 create_bullet(btype, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, 1, special1, special2, special3, special4, special5);


}


/*

void fire_turret(int sactor)
{

 int colours [4] = {COLOUR_RED8, COLOUR_RED6, COLOUR_RED4, COLOUR_RED2};

 int damage = 100;
 int timer = 11 + actor[sactor].upgraded_system [UPG_TURRET] * 2;
 int mass = 10;
 int status = 0;
 int speed = 22000;
// unsigned char seed = counter;

// int target = closest_enemy(actor[sactor].x, actor[sactor].y, 100000 + actor[sactor].upgraded_system [UPG_TURRET], 1);
 int target = actor[sactor].turret_lock;
 int at;

 int angle;
 int lead = 5;

 int target_x;
 int target_y;
 int source_x;
 int source_y;


 if (target == LOCK_ACTOR0 || target == LOCK_ACTOR1)
 {
  if (target == LOCK_ACTOR0)
   at = 0;
    else
     at = 1;

  lead = 5;

  target_x = actor[at].x + actor[at].x_speed * lead;
  target_y = actor[at].y + actor[at].y_speed * lead;
  source_x = actor[sactor].x + actor[sactor].x_speed * lead;
  source_y = actor[sactor].y + actor[sactor].y_speed * lead;

  angle = radians_to_angle(atan2(target_y - source_y,
          target_x - source_x));

 }
  else
  {

   if (target == -1 || enemy[target].type == ENEMY_NONE)
   {
    angle = actor[sactor].angle - ANGLE_QUARTER;
   }
     else
     {

      lead = 5;

      target_x = enemy[target].x + enemy[target].x_speed * lead;
      target_y = enemy[target].y + enemy[target].y_speed * lead;
      source_x = actor[sactor].x + actor[sactor].x_speed * lead;
      source_y = actor[sactor].y + actor[sactor].y_speed * lead;

// int angle = radians_to_angle(atan2(enemy[target].y - actor[sactor].y,
//               enemy[target].x - actor[sactor].x));
      angle = radians_to_angle(atan2(target_y - source_y,
               target_x - source_x));
     }
  }
  
// angle += (actor[sactor].upgraded_system [UPG_TURRET] * 3);
// angle -= grand(actor[sactor].upgraded_system [UPG_TURRET] * 6);
 angle += 20;
 angle -= grand(40);

 timer += grand(4);

 int speed_div = 4;

 speed += actor[sactor].upgraded_system [UPG_TURRET] * 800;
 speed_div -= actor[sactor].upgraded_system [UPG_TURRET] / 2;

 int xs = actor[sactor].x_speed + xpart(angle, speed / 3);
 int ys = actor[sactor].y_speed + ypart(angle, speed / 3);

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (angle == 0)
  angle = 1;

 create_bullet(BULLET_ZAP, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, 0, colours, speed_div, 0, 0, 0, 0, 0);

//  play_sound2(WAV_BLAT, 600, 155, 127);

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

 x = actor[sactor].x + xpart(angle, 2000);
 y = actor[sactor].y + ypart(angle, 2000);

// if (displaced != 0)
// {
//  x += cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
//  y += sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER * displaced))) * GRAIN * 3;
// }


    create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
     actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
     0, 0, 0, 0, cloud_colours);


}


void fire_heavy_cannon(int sactor)
{

 int colours [4] = {COLOUR_WHITE, COLOUR_GREY6, COLOUR_GREY4, COLOUR_GREY2};
  switch(actor[sactor].upgraded_system [UPG_HEAVY])
  {
   case 1:
    colours [3] = COLOUR_RED8;
    break;
   case 2:
    colours [3] = COLOUR_YELLOW8;
    break;
   case 3:
    colours [3] = COLOUR_GREEN8;
    break;
   case 4:
    colours [3] = COLOUR_BLUE8;
    break;
   case 5:
    colours [3] = COLOUR_WHITE;
    break;
  }


 int damage = 100 + 25 * actor[sactor].upgraded_system [UPG_HEAVY];
 int timer = 13;// + actor[sactor].upgraded_system [UPG_HEAVY];
 int mass = 10;
 int status = 0;
 int speed = 25000 + actor[sactor].upgraded_system [UPG_HEAVY] * 1000;
 unsigned char seed = counter;


 int angle = actor[sactor].angle - ANGLE_QUARTER;

 timer += grand(3);

 int speed_div = 4;

// speed += actor[sactor].upgraded_system [UPG_HEAVY] * 1000;
 speed_div -= actor[sactor].upgraded_system [UPG_HEAVY] / 2;

 int xs = actor[sactor].x_speed + xpart(angle, speed / 3);
 int ys = actor[sactor].y_speed + ypart(angle, speed / 3);

 int x, y;

 if (angle == 0)
  angle = 1;

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

// x = actor[sactor].x + cos(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 30;
// y = actor[sactor].y + sin(angle_to_radians(angle - ANGLE_QUARTER + (ANGLE_QUARTER))) * GRAIN * 30;
 x = actor[sactor].x + xpart(angle - ANGLE_QUARTER, GRAIN * 5);
 y = actor[sactor].y + ypart(angle - ANGLE_QUARTER, GRAIN * 5);

 create_bullet(BULLET_POWERED, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, speed_div, 0, 0, 0, 0, 0);

 x = actor[sactor].x + xpart(angle, 3000);
 y = actor[sactor].y + ypart(angle, 3000);
 x += xpart(angle - ANGLE_QUARTER, GRAIN * 3);
 y += ypart(angle - ANGLE_QUARTER, GRAIN * 3);
 create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
  actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
  0, 0, 0, 0, cloud_colours);

 x = actor[sactor].x + xpart(angle + ANGLE_QUARTER, GRAIN * 5); // * must be twice above because we need to counteract it
 y = actor[sactor].y + ypart(angle + ANGLE_QUARTER, GRAIN * 5);

 create_bullet(BULLET_POWERED, x, y,
  xs, ys, sactor,
  damage, timer, mass, angle,
  status, seed, colours, speed_div, 0, 0, 0, 0, 0);

// actor_sound(sactor, ASOUND_BLAT);

 x = actor[sactor].x + xpart(angle, 3000);
 y = actor[sactor].y + ypart(angle, 3000);
 x += xpart(angle + ANGLE_QUARTER, GRAIN * 3);
 y += ypart(angle + ANGLE_QUARTER, GRAIN * 3);
 create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
  actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
  0, 0, 0, 0, cloud_colours);


}

void fire_backfire(int sactor)
{

 int colours [4] = {COLOUR_GREEN8, COLOUR_GREEN6, COLOUR_GREEN4, COLOUR_GREEN2};

 int damage = 100;
 int timer = 14;
 int mass = 10;
 int status = 0;
 int speed = 29000;
 unsigned char seed = counter;

 int angle = actor[sactor].angle + ANGLE_QUARTER;

 timer += grand(4);

 int speed_div = 4;

 int xs;// = actor[sactor].x_speed + cos(angle_to_radians(angle)) * speed / 3;
 int ys;// = actor[sactor].y_speed + sin(angle_to_radians(angle)) * speed / 3;

 int x = actor[sactor].x;
 int y = actor[sactor].y;

 if (angle == 0)
  angle = 1;

// x = actor[sactor].x + cos(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 5;
// y = actor[sactor].y + sin(angle_to_radians(angle - ANGLE_QUARTER)) * GRAIN * 5;

 int i;
 int multiple = 1 + actor[sactor].upgraded_system [UPG_BACKFIRE];

 int angle_inc = ANGLE_1_SIXTEENTH;

 angle -= angle_inc * (multiple / 2) + 1;

 if (multiple % 2 == 0)
  angle += ANGLE_1_SIXTEENTH / 2;

 int cloud_colours [5];
 cloud_colours [0] = colours [0];

 int x2, y2;

 for (i = 0; i < multiple; i ++)
 {
//  angle = grand(1000);

 xs = actor[sactor].x_speed + xpart(angle, speed / 3);
 ys = actor[sactor].y_speed + ypart(angle, speed / 3);
 
  create_bullet(BULLET_ZAP, x, y,
   xs, ys, sactor,
   damage, timer, mass, angle,
   status, seed, colours, speed_div, 0, 0, 0, 0, 0);
   
  x2 = actor[sactor].x + xpart(angle, 2000);
  y2 = actor[sactor].y + ypart(angle, 2000);

//  x2 += cos(angle_to_radians(angle)) * GRAIN * 3;
//  y2 += sin(angle_to_radians(angle)) * GRAIN * 3;

  create_cloud(CLOUD_MED_CIRCLE, x2, y2, 0, 0,
   actor[sactor].x_speed, actor[sactor].y_speed, 300, 50,
   0, 0, 0, 0, cloud_colours);

  angle += angle_inc;

 }
  play_sound2(WAV_BLAT, 400, 155, 127);

}

void fire_sidekicks(int a)
{

 if (actor[a].sidekick_recycle > 0)
  return;

 int colours [4] = {COLOUR_RED8, COLOUR_RED6, COLOUR_RED4, COLOUR_RED2};
// int colours [4] = {COLOUR_YELLOW8, COLOUR_YELLOW6, COLOUR_YELLOW4, COLOUR_YELLOW2};
 int damage = 100;
 int timer = 14;
 int mass = 10;
 int status = 0;
 int speed = 29000;
 unsigned char seed = counter;
 int i;
 int angle;
 int speed_div = 2;
 int xs;// = actor[sactor].x_speed + cos(angle_to_radians(angle)) * speed / 3;
 int ys;// = actor[sactor].y_speed + sin(angle_to_radians(angle)) * speed / 3;
 int x;// = actor[a].sidekick_x [i];
 int y;// = actor[a].sidekick_y [i];
 int btype = BULLET_ZAP;
 timer += grand(4);

 switch(actor[a].upgraded_system [UPG_SIDEKICK])
 {
  case 2:
  case 3:
   damage = 125;
   btype = BULLET_POWERED;
   colours [3] = COLOUR_RED8;
   break;
  case 4:
   damage = 150;
   btype = BULLET_POWERED;
   colours [3] = COLOUR_YELLOW8;
   break;
  case 5:
   damage = 175;
   btype = BULLET_POWERED;
   colours [3] = COLOUR_GREEN8;
   break;
 }

 int cloud_colours [5];

 cloud_colours [0] = COLOUR_ORANGE8;

 for (i = 0; i < actor[a].sidekicks; i ++)
 {

 x = actor[a].sidekick_x [i];
 y = actor[a].sidekick_y [i];

 angle = actor[a].sidekick_angle [i];

 if (angle == 0)
  angle = 1;

  xs = actor[a].sidekick_x_speed [i] + xpart(angle, speed / 3);
  ys = actor[a].sidekick_y_speed [i] + ypart(angle, speed / 3);
 
  create_bullet(btype, x, y,
   xs, ys, a,
   damage, timer, mass, angle,
   status, seed, colours, speed_div, 0, 0, 0, 0, 0);
   
  x = x + xpart(angle, 4000);
  y = y + ypart(angle, 4000);

  create_cloud(CLOUD_MED_CIRCLE, x, y, 0, 0,
   actor[a].sidekick_x_speed [i], actor[a].sidekick_y_speed [i], 300, 50,
   0, 0, 0, 0, cloud_colours);

 }

  play_sound2(WAV_BLAT, 500, 155, 127);


}

*/
/*
void upgrade_sidekicks(int a)
{
 int old_number = actor[a].sidekicks;

 switch(actor[a].upgraded_system [UPG_SIDEKICK])
 {
  case 1:
  case 2:
   actor[a].sidekicks = 2;
   break;
  default:
   actor[a].sidekicks = 4;
   break;
 }

 int i;

 if (actor[a].sidekicks > old_number)
 {
  simple_cloud_trans(TRANS_LBLUE, actor[a].x, actor[a].y, 0, 0, 500);
  for (i = old_number; i < actor[a].sidekicks; i ++)
  {
   actor[a].sidekick_x [i] = actor[a].x;
   actor[a].sidekick_y [i] = actor[a].y;
   actor[a].sidekick_x_speed [i] = actor[a].x_speed;
   actor[a].sidekick_y_speed [i] = actor[a].y_speed;
   actor[a].sidekick_angle [i] = actor[a].angle;
  }
 }

}
*/
/*
void actor_sound(int a, int sound)
{

 switch(sound)
 {
  case ASOUND_CANNON:
  play_sound2(WAV_CANNON, 1000 + actor[a].upgraded_system [UPG_AUTOFIRE] * 200, 155 + actor[a].upgraded_system [UPG_POWER] * 20, 127);
  break;
  case ASOUND_BOMBS:
  play_sound2(WAV_BOMBS, 1000 - actor[a].upgraded_system [UPG_BOMB] * 70, 155 + actor[a].upgraded_system [UPG_LAUNCHER] * 20, 127);
  break;
  case ASOUND_BLAT:
  play_sound2(WAV_BLAT, 400 - actor[a].upgraded_system [UPG_HEAVY] * 20, 155, 127);
  break;
  case ASOUND_TUBE:
  play_sound2(WAV_TUBE, 1000 + actor[a].upgraded_system [UPG_ROCKET] * 100 + actor[a].upgraded_system [UPG_LOADER] * 50, 155, 127);
  break;
// play_sound(sample);
}

}

*/


void make_drive_sound(int a, int drive)
{

 if (actor[a].drive_sound [drive] == 0)
 {
//    play_sound2(WAV_ENGINE, 1000 + actor[a].upgraded_system [UPG_SPEED] * 50, 100, 127);
    play_wavf(NWAV_DRIVE, 1000 - (actor[a].ability [ABILITY_DRIVE] [SYMBOL_SQUARE] * 30) - (actor[a].ability [ABILITY_DRIVE] [SYMBOL_TRIANGLE] * 5));
    actor[a].drive_sound [DRIVE_THRUST] = 2;

/*  switch(drive)
  {
   case DRIVE_THRUST:
    play_sound2(WAV_ENGINE, 1000 + actor[a].upgraded_system [UPG_SPEED] * 50, 100, 127);
    actor[a].drive_sound [DRIVE_THRUST] = 10 - actor[a].upgraded_system [UPG_SPEED] - (actor[a].upgraded_system [UPG_SPEED] == 5);
    break;
   case DRIVE_SLIDE:
    play_sound2(WAV_ENGINE, 1000 + actor[a].upgraded_system [UPG_SLIDE] * 50, 100, 127);
    actor[a].drive_sound [DRIVE_SLIDE] = 10 - actor[a].upgraded_system [UPG_SLIDE];
    break;
   case DRIVE_RETRO:
    play_sound2(WAV_ENGINE, 1000 + actor[a].upgraded_system [UPG_RETRO] * 50, 100, 127);
    actor[a].drive_sound [DRIVE_RETRO] = 10 - actor[a].upgraded_system [UPG_RETRO];
    break;
  }*/
 }

}


