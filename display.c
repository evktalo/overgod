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
    
File: display.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - functions which put stuff onto the screen

*/
#include "config.h"

#include "allegro.h"

#include <math.h>
#include <string.h>


#include "globvars.h"
#include "palette.h"

#include "stuff.h"
#include "light.h"

#define COMPILING_DISPLAYC
#include "display.h"
#undef COMPILING_DISPLAYC

//#define DEBUG_DISPLAY

int debug_info;

extern struct lightstruct light [NO_LIGHTS];

int pline(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour);

//#define DEBUG_ENEMY_SIZE

BITMAP *player1;
BITMAP *player2;
#ifdef DEBUG_DISPLAY
#include <stdlib.h>
// for itoa, used in the screenshot code but not needed for ports etc
extern volatile int frames_per_second;
extern volatile int ticked;
extern int slacktime;
extern int long_slacktime_store;
extern int debug_sound [5];

int slack_graph [100];
int slack_graph_pos;
int fps_graph [100];
int fps_graph_pos;
#endif

extern struct optionstruct options;

//BITMAP *ship_bitmaps [2] [65]; // externed in displ_in.c
BITMAP *ship_bitmaps [NO_SHIP_TYPES] [17];

BITMAP *gb_ship_bitmaps [GB_SHIP_TYPES] [17];

BITMAP *enemy1_bmp [ENEMY1_BMPS];
RLE_SPRITE *enemy1_rle [ENEMY1_RLES];
RLE_SPRITE *small1_rle [SMALL1_RLES];
RLE_SPRITE *small3_rle [SMALL3_RLES];
RLE_SPRITE *enemy2_rle [ENEMY2_RLES];
RLE_SPRITE *enemy3_rle [ENEMY3_RLES];
RLE_SPRITE *enemy4_rle [ENEMY4_RLES];
RLE_SPRITE *enemy5_rle [ENEMY5_RLES];
RLE_SPRITE *enemy6_rle [ENEMY6_RLES];
RLE_SPRITE *enemy7_rle [ENEMY7_RLES];
RLE_SPRITE *enemy8_rle [ENEMY8_RLES];
RLE_SPRITE *enemy9_rle [ENEMY9_RLES];
BITMAP *small2_bmp [SMALL2_BMPS];
BITMAP *small4_bmp [SMALL4_BMPS];
BITMAP *enemy_bmps [ENEMY_BMPS];
//extern float cos_table [ANGLE_FULL];


BITMAP *pretile_bmp [NO_PRETILE_BMPS];
BITMAP *pretile_m_bmp [NO_MAZES] [NO_PRETILE_M_BMPS];
RLE_SPRITE *tile_rle [NO_TILE_RLES];
BITMAP *level_bmp;

BITMAP *redbang_bmp [50];
RLE_SPRITE *light_rle [100];

BITMAP *superjelly_bmp [2];
BITMAP *shield_bmp [SHIELD_BMPS];
RLE_SPRITE *waver1_circle;
RLE_SPRITE *waver2_circle;

RLE_SPRITE *large_ships [16] [5];

RLE_SPRITE *tile_background;

FONT *font2;
FONT *small_font;

struct effects_struct effect [MAX_EFFECTS];

int var1, var2, var3; // debug display vars, externed as necessary

int graphics_mode;
int scr_x;
int scr_y;
int grid_offset_x_1p;
int grid_offset_x_2p;
int grid_offset_x_2p_finetune;
int grid_offset_y;
int special_600_y;
int text_offset_x;
int text_offset_x_1p;
int text_offset_x_2p;
int text_offset_y;
int grid_finetune_x_1p;
int grid_finetune_x_2p;
int grid_finetune_y;
int visible_grids_y;

int tp_window_width;

void indicate_fps(BITMAP *bmp, int play);

void draw_effects(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_an_effect(BITMAP *bmp, int dr, int x, int y);
void beam_points(BITMAP *bmp, int x1, int y1, int x2, int y2, int angle, int out1, int out2, int colour);

//void draw_lock(BITMAP *bmp, int dr, int x, int y, int rad);
//void draw_turret_lock(BITMAP *bmp, int dr, int x, int y, int rad);
void draw_locks(BITMAP *bmp, int p);

float angle_to_radians(int angle);

int inflicteda, inflictede;

//void draw_grid(BITMAP *bmp, int xloc, int yloc, int max_x, int max_y, int colour);
void draw_stars(BITMAP *bmp, int max_x, int max_y, int player);
void draw_grid(BITMAP *bmp, int max_x, int max_y, int play, int colour, int edge_colour, int x_offset, int centre_x, int centre_y, int finetune_x, int finetune_y);
void draw_actors(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_hud(BITMAP *bmp, int x, int y, int i);
void draw_eyes(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_an_eye(BITMAP *bmp, int i, int x, int y);

void draw_an_actor(BITMAP *bmp, int dr, int x, int y, int play);
void draw_ship_bitmap(BITMAP *bmp, int x, int y, int angle, int which_ship, int width, int height);
void draw_triangle_ship(BITMAP *bmp, int dr, int size, int pointiness, int x, int y, unsigned char colour1, unsigned char colour2);
//void draw_scan(BITMAP *bmp, int max_x, int max_y, int play, char range);
//void draw_scan_dot(BITMAP *bmp, int x, int y, int type);
void draw_status2(BITMAP *bmp, int max_x, int max_y, int play);
void draw_bullets(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_a_bullet(BITMAP *bmp, int dr, int x, int y, int x2, int y2, int max_x, int max_y, int play);
void draw_enemies(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_an_enemy_inter(BITMAP *bmp, int e, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_an_enemy_inter2(BITMAP *bmp, int e, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_an_enemy(BITMAP *bmp, int dr, int x, int y);
void draw_clouds(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_a_cloud(BITMAP *bmp, int dr, int x, int y, int x2, int y2);
void draw_pickups(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_a_pickup(BITMAP *bmp, int dr, int x, int y);
void get_pickup_colour(int st, int cols [3]);
void write_colour_text(BITMAP *bmp, int x, int y, int which);
void draw_static_symbol(BITMAP *bmp, int x, int y, int col, int symb);
void draw_crosshair(BITMAP *bmp, int dr, int x, int y);
int get_circler_colour(int x1);
void draw_wave(BITMAP *bmp, int x, int y, int count, int size);

//void draw_sidekicks(BITMAP *bmp, int max_x, int max_y, int play, int a);

void draw_ship_status(BITMAP *bmp, int x, int y, int a, int style);
void draw_upgrades(BITMAP *bmp, int x, int y, int play);
char *upgrade_name(int i);
char *primary_name(int i);
void secondary_name(int i, char dstr [30]);

void draw_crawler_legs(BITMAP *bmp, int x, int y, int e, int col);
void draw_spinner(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2);
void draw_overspinner(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2);
void draw_spikey(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2, int in_correction, int out_correction);
void draw_blatter(BITMAP *bmp, int x, int y, int number, int distance, int rotation, int size, int col1, int col2);
void draw_squarey(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2);
void draw_orbiter(BITMAP *bmp, int x, int y, int attribute, int out1, int out2, int out3, int angle1, int angle2, int arms, int col1, int col2);
void draw_puffer(BITMAP *bmp, int x, int y, int angle, int number, int distance, int size, int incol, int outcol);
void draw_launchers(BITMAP *bmp, int x, int y, int angle, int d_f_centre, int diagonal, int interior_angle, int recoil1, int recoil2, int col1, int col2);
void draw_tri(BITMAP *bmp, int x, int y, int angle, int length1, int length2, int angle2, int col1, int col2);
void draw_waver(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2, RLE_SPRITE *waver_rle, int waver_rle_size);

void distortion(BITMAP *bmp, int x, int y, int wx, int wy, int dx, int dy);
BITMAP *distortion_mask;
BITMAP *crystal_bmp;


int set_lock_x [2];
int set_lock_y [2];
int set_lock_radius [2];
  
int set_turret_lock_x [2];
int set_turret_lock_y [2];
int set_turret_lock_radius [2];

#define NO_MESSAGES 10

char messages [2] [NO_MESSAGES] [70];
int message_style [2] [NO_MESSAGES];
int message_counter [2] [NO_MESSAGES];

char scroll_text [10] [50];
int scroll_position [10];
int scroll_colour [10];

void display_messages(BITMAP *bmp, int play, int max_x, int max_y);
void kill_message(int play, int i);

void draw_lights(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y);
void draw_a_light(BITMAP *bmp, int size, int x, int y);
void bordered_rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour1, int colour2);
void bordered_triangle(BITMAP *bmp, int x1, int y1, int x2, int y2, int x3, int y3, int col1, int col2);

void draw_spark_jump(BITMAP *bmp, int x1, int y1, int x2, int y2, int parts, int scatter, int col, int transy);

//char prim_name [4] [10]

/*
void display_scrollbar(BITMAP *bmp, int play, int max_x, int max_y)
{

}

void new_scroll(const char *msg, int colour)
{


}

void run_scrollbar(int play)
{


}
*/


void run_display(void)
{
 static int counter;

 counter++;

 if (counter > 40000)
  counter = 0;

// int i;
  int centre_x, centre_y;
// draw_grid(player1, 150 + counter, 150 + counter, scr_x, scr_y, 100, 2);
// draw_grid(player1, actor[0].x / GRAIN, actor[0].y / GRAIN, scr_x, scr_y, actor[0].angle, 2);
 if (game.users == 1)
 {
  centre_x = actor[player[game.single_player].actor_controlled].x + player[game.single_player].screen_shake_x;
  centre_y = actor[player[game.single_player].actor_controlled].y + player[game.single_player].screen_shake_y;
  set_lock_x [game.single_player] = 0;
  set_lock_y [game.single_player] = 0;
  set_lock_radius [game.single_player] = 0;
  set_turret_lock_x [game.single_player] = 0;
  set_turret_lock_y [game.single_player] = 0;
  set_turret_lock_radius [game.single_player] = 0;
//  draw_stars(player1, scr_x, scr_y, 0);
//  clear_to_color(player1, arena.colour3);
  draw_grid(player1, scr_x, scr_y, game.single_player, arena.colour1, arena.colour2, grid_offset_x_1p, centre_x, centre_y, grid_finetune_x_1p, grid_finetune_y);
  draw_pickups(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  draw_enemies(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  draw_actors(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  draw_bullets(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  draw_effects(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  draw_clouds(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  draw_eyes(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  draw_lights(player1, scr_x, scr_y, game.single_player, centre_x, centre_y);
  
// after here, it's just HUD and screen_shake isn't applied:  
  draw_upgrades(player1, scr_x / 2 - 120, scr_y - 45, game.single_player);
  draw_status2(player1, scr_x, scr_y, game.single_player);
  draw_locks(player1, game.single_player);
/*  if (actor[player[game.single_player].actor_controlled].lock != -1
      && enemy[actor[player[game.single_player].actor_controlled].lock].type != ENEMY_NONE
      && set_lock_radius [game.single_player] != 0)
   draw_lock(player1, player[game.single_player].actor_controlled, set_lock_x [game.single_player], set_lock_y [game.single_player], set_lock_radius [game.single_player]);
  if (actor[player[game.single_player].actor_controlled].turret_lock != -1
      && enemy[actor[player[game.single_player].actor_controlled].turret_lock].type != ENEMY_NONE)
   draw_turret_lock(player1, player[game.single_player].actor_controlled, set_turret_lock_x [game.single_player], set_turret_lock_y [game.single_player], set_turret_lock_radius [game.single_player]);*/
  indicate_fps(player1, game.single_player);
//  display_messages(player1, game.single_player, scr_x, scr_y);
/*  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  for (i = 195; i < 206; i ++)
  {
   rectfill(player1, 10 + (i-195) * 30, 100, 30 + (i-195) * 30, 150, i);
  }
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);*/
  if (options.run_vsync > 0)
   vsync();
   
//  textprintf_ex(player1, font, 100, 100, COLOUR_GREY5, COLOUR_YELLOW1, "%i", grand(500));

  blit(player1, screen, 0, 0, 0, 0, scr_x, scr_y);
 }
  else
  {
//   grid_offset_x_2p = 
   centre_x = actor[player[0].actor_controlled].x + player[0].screen_shake_x;
   centre_y = actor[player[0].actor_controlled].y + player[0].screen_shake_y;
   set_lock_x [0] = 0;
   set_lock_y [0] = 0;
   set_lock_radius [0] = 0;
   set_turret_lock_x [0] = 0;
   set_turret_lock_y [0] = 0;
   set_turret_lock_radius [0] = 0;
   set_lock_x [1] = 0;
   set_lock_y [1] = 0;
   set_lock_radius [1] = 0;
   set_turret_lock_x [1] = 0;
   set_turret_lock_y [1] = 0;
   set_turret_lock_radius [1] = 0;
//   draw_stars(player1, 310, scr_y, 0);
//   clear_to_color(player1, arena.colour3);
//   clear_to_color(player2, arena.colour3);
   draw_grid(player1, tp_window_width, scr_y, 0, arena.colour1, arena.colour2, grid_offset_x_2p, centre_x, centre_y, grid_offset_x_2p_finetune, grid_finetune_y);
   draw_pickups(player1, tp_window_width, scr_y, 0, centre_x, centre_y);
   draw_enemies(player1, tp_window_width, scr_y, 0, centre_x, centre_y);
   draw_actors(player1, tp_window_width, scr_y, 0, centre_x, centre_y);
   draw_bullets(player1, tp_window_width, scr_y, 0, centre_x, centre_y);
   draw_effects(player1, tp_window_width, scr_y, 0, centre_x, centre_y);
   draw_clouds(player1, tp_window_width, scr_y, 0, centre_x, centre_y);
   draw_eyes(player1, tp_window_width, scr_y, 0, centre_x, centre_y);
   draw_lights(player1, tp_window_width, scr_y, 0, centre_x, centre_y);

//   draw_scan(player1, 50, 50, actor[player[0].actor_controlled].angle, 0, 0);
//   draw_scan(player1, 260, 50, actor[player[0].actor_controlled].angle, 0, 1);
//   draw_hud(player1, 155, 360, player[0].actor_controlled);
//   draw_upgrades(player1, 40, 420, 0);
//   draw_upgrades(player1, 16, 460, 0);
   draw_upgrades(player1, tp_window_width / 2 - 120, scr_y - 20, 0);//game.single_player);
   draw_status2(player1, tp_window_width, scr_y, 0);
   indicate_fps(player1, 0);
//   display_messages(player1, 0, 310, scr_y);
   
   centre_x = actor[player[1].actor_controlled].x + player[1].screen_shake_x;
   centre_y = actor[player[1].actor_controlled].y + player[1].screen_shake_y;
//   draw_stars(player2, 310, scr_y, 1);
   draw_grid(player2, tp_window_width, scr_y, 1, arena.colour1, arena.colour2, grid_offset_x_2p, centre_x, centre_y, grid_offset_x_2p_finetune, grid_finetune_y);
   draw_eyes(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_pickups(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_enemies(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_actors(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_bullets(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_effects(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_clouds(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_eyes(player2, tp_window_width, scr_y, 1, centre_x, centre_y);
   draw_lights(player2, tp_window_width, scr_y, 1, centre_x, centre_y);

//   draw_scan(player2, 50, 50, actor[player[1].actor_controlled].angle, 1, 0);
//   draw_scan(player2, 260, 50, actor[player[1].actor_controlled].angle, 1, 1);
//   draw_hud(player2, 155, 360, player[1].actor_controlled);
   draw_upgrades(player2, tp_window_width / 2 - 120, scr_y - 20, 1);
   draw_status2(player2, tp_window_width, scr_y, 1);
//   draw_locks(player1, 0);
//   draw_locks(player2, 1);
   indicate_fps(player2, 1);
//   display_messages(player2, 1, tp_window_width, scr_y);
   if (options.run_vsync > 0)
    vsync();
   blit(player1, screen, 0, 0, 0, 0, tp_window_width, scr_y);
   blit(player2, screen, 0, 0, tp_window_width + 10, 0, tp_window_width, scr_y);
//   clear_bitmap(player1);
//   clear_bitmap(player2);
  }

}


void draw_locks(BITMAP *bmp, int p)
{
/* int a = player[p].actor_controlled;

  if (actor[a].lock == LOCK_ACTOR0)
  {
//   if (actor[0].in_play != 0)
//    draw_lock(bmp, a, set_lock_x [p], set_lock_y [p], set_lock_radius [p]);
  }
   else
   {
    if (actor[a].lock == LOCK_ACTOR1)
    {
//     if (actor[1].in_play != 0)
//      draw_lock(bmp, a, set_lock_x [p], set_lock_y [p], set_lock_radius [p]);
    }
     else
     {
//      if (actor[a].lock != -1
//       && enemy[actor[a].lock].type != ENEMY_NONE
//       && set_lock_radius [p] != 0)
//      draw_lock(bmp, a, set_lock_x [p], set_lock_y [p], set_lock_radius [p]);
     }
   }

  if (actor[a].turret_lock == LOCK_ACTOR0)
  {
   if (actor[0].in_play != 0)
    draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);
  }
   else
   {
    if (actor[a].turret_lock == LOCK_ACTOR1)
    {
     if (actor[1].in_play != 0)
      draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);
    }
     else
     {
      if (actor[a].turret_lock != -1
       && enemy[actor[a].turret_lock].type != ENEMY_NONE
       && set_turret_lock_radius [p] != 0)
      draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);
     }
   }
*/
//   if (actor[a].turret_lock != -1
//       && enemy[actor[a].turret_lock].type != ENEMY_NONE)
//    draw_turret_lock(bmp, a, set_turret_lock_x [p], set_turret_lock_y [p], set_turret_lock_radius [p]);

}


void draw_actors(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 int i, x, y;

 for (i = 0; i < NO_ACTORS; i ++)
 {
  if (actor[i].in_play == 0)
   continue;

//  if (actor[i].upgraded_system [UPG_SIDEKICK] > 0)
//   draw_sidekicks(bmp, max_x, max_y, play, i);

  if (i == player[play].actor_controlled)
  {
//   draw_an_actor(bmp, player[play].actor_controlled, max_x / 2, max_y / 2, play);
   draw_an_actor(bmp, player[play].actor_controlled, max_x / 2 - player[play].screen_shake_x / GRAIN, max_y / 2 - player[play].screen_shake_y / GRAIN, play);
   draw_crosshair(bmp, player[play].actor_controlled, max_x / 2, max_y / 2);
   continue;
  }

  if (actor[i].x < centre_x - (max_x / 2) * GRAIN - 7000
      || actor[i].x > centre_x + (max_x / 2) * GRAIN + 7000
      || actor[i].y < centre_y - (max_y / 2) * GRAIN - 7000
      || actor[i].y > centre_y + (max_y / 2) * GRAIN + 7000)
       continue;

  x = (actor[i].x / GRAIN) - (centre_x / GRAIN);
  y = (actor[i].y / GRAIN) - (centre_y / GRAIN);
  draw_an_actor(bmp, i, x + max_x / 2, y + max_y / 2, play);
  
 }




}


void draw_an_actor(BITMAP *bmp, int dr, int x, int y, int play)
{
 int i;

 int angle2 = 0;
 int angle = 0;
 if (angle < 0)
  angle += ANGLE_FULL;

 if (angle2 < 0)
  angle2 += ANGLE_FULL;

   draw_ship_bitmap(bmp, x, y, actor[dr].ship, actor[dr].angle, 12, 11);

 switch(actor[dr].ship)
 {
//  case SHIP_LACEWING:
//   draw_ship_bitmap(bmp, x, y, 0, actor[dr].angle, 12, 11);
//   break;
//  case SHIP_CAPYBARA:
//   draw_ship_bitmap(bmp, x, y, 1, actor[dr].angle, 10, 10);
//   break;
/*  case SHIP_RUMSFELD:
   draw_ship_bitmap(bmp, x, y, 1, actor[dr].angle, 10, 10);
   break;
  case SHIP_LENTIL:
   draw_ship_bitmap(bmp, x, y, 2, actor[dr].angle, 10, 10);
   break;
  case SHIP_CAPYBARA:
   draw_ship_bitmap(bmp, x, y, 3, actor[dr].angle, 10, 10);
   break;
  case SHIP_HOOKWORM:
   draw_ship_bitmap(bmp, x, y, 4, actor[dr].angle, 10, 10);
   break;
  case SHIP_PORKUPINE:
   draw_ship_bitmap(bmp, x, y, 5, actor[dr].angle, 10, 10);
   break;
  case SHIP_PRONG:
   draw_ship_bitmap(bmp, x, y, 6, actor[dr].angle, 10, 10);
   break;
  case SHIP_AETHER:
   draw_ship_bitmap(bmp, x, y, 7, actor[dr].angle, 10, 10);
   break;
  case SHIP_DESPOT:
   draw_ship_bitmap(bmp, x, y, 8, actor[dr].angle, 10, 10);
   break;
  case SHIP_LACEWING:
//   draw_ship_bitmap(bmp, x, y, 9, actor[dr].angle, 10, 10);
   draw_ship_bitmap(bmp, x, y, 0, actor[dr].angle, 12, 11);
//   circle(bmp, x, y, 8, COLOUR_YELLOW6);
   break;
  case SHIP_TORTFEASOR:
   draw_ship_bitmap(bmp, x, y, 10, actor[dr].angle, 10, 10);
   break;
  case SHIP_SCORPION:
   draw_ship_bitmap(bmp, x, y, 11, actor[dr].angle, 10, 10);
   break;
  case SHIP_GODBOTHERER:
   draw_ship_bitmap(bmp, x, y, 12, actor[dr].angle, 10, 10);
   break;
  case SHIP_BOTULUS:
   draw_ship_bitmap(bmp, x, y, 13, actor[dr].angle, 10, 10);
   break;
  case SHIP_SLIDEWINDER:
   draw_ship_bitmap(bmp, x, y, 14, actor[dr].angle, 10, 10);
   break;
  case SHIP_DOOM:
   draw_ship_bitmap(bmp, x, y, 15, actor[dr].angle, 10, 10);
   break;
*/
 }
 int x1, x2, y2;//, y1;

 if (actor[dr].shield_pulse > 0)
 {
/*  int scol = COLOUR_WHITE;
  if (actor[dr].shield_pulse == 9)
   scol = COLOUR_GREY6;
  if (actor[dr].shield_pulse == 8)
   scol = COLOUR_GREY5;
  if (actor[dr].shield_pulse < 8)
   scol = COLOUR_GREEN1 + actor[dr].shield_pulse;*/
  x1 = actor[dr].shield_pulse;
  if (actor[dr].shield_pulse > 7)
   x1 = 7;
  x1 = grand(x1 + 1);
  x2 = x + grand(3) - grand(3);
  y2 = y + grand(3) - grand(3);
  draw_trans_sprite(bmp, small4_bmp [BMP_SMALL4_SHIELD_1 + x1], x2 - 10, y2 - 10);
  draw_trans_sprite(bmp, shield_bmp [x1], x2, y2 - 10);
  draw_trans_sprite(bmp, shield_bmp [x1 + 8], x2, y2);
  draw_trans_sprite(bmp, shield_bmp [x1 + 16], x2 - 10, y2);
/*  draw_trans_sprite(bmp, small4_bmp [BMP_SMALL4_SHIELD_1 + grand(x1 + 1)], x - 10, y - 10);
  draw_trans_sprite(bmp, shield_bmp [grand(x1 + 1)], x, y - 10);
  draw_trans_sprite(bmp, shield_bmp [grand(x1 + 1) + 8], x, y);
  draw_trans_sprite(bmp, shield_bmp [grand(x1 + 1) + 16], x - 10, y);*/
 
//circle(bmp, x + grand(5) - 2, y + grand(5) - 2, 10, scol);
 }
 
 int gd;
 int y1, s1, s2, s3;

 if (actor[dr].grace_period > 0)
 {
 gd = actor[dr].grace_period / 3;
 if (gd > 15)
  gd = 15;
 y1 = 0;

//  x1 = gd + (actor[dr].grace_period / 3) % 4 + 3;
//  if (((actor[dr].grace_period / 3) / 4) % 2 == 0)
//   x1 = gd + 4 - (actor[dr].grace_period / 3) % 4 + 3;
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
//  x1 = gd + (actor[dr].grace_period / 3) % 4 + 3;
   x1 = (actor[dr].grace_period * 30) & 1023;
   y1 = gd / 4;
   if (arena.level % 2 == 0)
   {
    x1 = ANGLE_FULL - x1;
    s1 = y1 - 2;
    s2 = y1 - 1;
    s3 = y1;
   } 
    else
    {
     s3 = y1 - 2;
     s2 = y1 - 1;
     s1 = y1;
    }
// gd *= 3;
   for (i = 0; i < 3; i ++)
   {
    if (s1 > 0)
    circlefill(bmp, x + xpart(x1 + (ANGLE_FULL / 3) * i, gd), 
     y + ypart(x1 + (ANGLE_FULL / 3) * i, gd),
     s1, TRANS_WHITE);
     if (s2 > 0)
      circlefill(bmp, x + xpart(x1 + (ANGLE_FULL / 3) * i + ANGLE_1_SIXTEENTH, gd), 
       y + ypart(x1 + (ANGLE_FULL / 3) * i + ANGLE_1_SIXTEENTH, gd),
       s2, TRANS_WHITE);
     if (s3 > 0)
      circlefill(bmp, x + xpart(x1 + (ANGLE_FULL / 3) * i + ANGLE_1_EIGHTH, gd), 
       y + ypart(x1 + (ANGLE_FULL / 3) * i + ANGLE_1_EIGHTH, gd),
       s3, TRANS_WHITE);
   }
//   circle(bmp, x, y, x1, TRANS_YELLOW);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
//   circle(bmp, x, y, x1, COLOUR_YELLOW3 + gd);
  }
   else
    if (actor[dr].repairing > 0)
    {
 gd = actor[dr].repairing / 3;
 if (gd > 15)
  gd = 15;
 y1 = 0;

  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   x1 = actor[dr].repairing * 35;
   y1 = gd / 4;
   if (arena.level % 2 == 1)
   {
    x1 = ANGLE_FULL - x1;
    s1 = y1 - 2;
    s2 = y1 - 1;
    s3 = y1;
   } 
    else
    {
     s3 = y1 - 2;
     s2 = y1 - 1;
     s1 = y1;
    }
//   gd = 15; 
   for (i = 0; i < 4; i ++)
   {
    if (s1 > 0)
    circlefill(bmp, x + xpart(x1 + (ANGLE_FULL / 4) * i, gd + grand(2)), 
     y + ypart(x1 + (ANGLE_FULL / 4) * i, gd),
     s1, TRANS_ORANGE);
     if (s2 > 0)
      circlefill(bmp, x + xpart(x1 + (ANGLE_FULL / 4) * i + ANGLE_1_SIXTEENTH, gd), 
       y + ypart(x1 + (ANGLE_FULL / 4) * i + ANGLE_1_SIXTEENTH, gd),
       s2, TRANS_ORANGE);
     if (s3 > 0)
      circlefill(bmp, x + xpart(x1 + (ANGLE_FULL / 4) * i + ANGLE_1_EIGHTH, gd), 
       y + ypart(x1 + (ANGLE_FULL / 4) * i + ANGLE_1_EIGHTH, gd),
       s3, TRANS_ORANGE);
   }
//   circle(bmp, x, y, x1, TRANS_YELLOW);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
//   circle(bmp, x, y, x1, COLOUR_YELLOW3 + gd);
   
   
    }
//  circle(bmp, x + grand(3) - 1, y + grand(3) - 1, gd * 2 + grand(3), COLOUR_YELLOW1 + gd + grand(3));
/* if (gd > 10)
  gd = 10;

 if (actor[dr].grace_period > 0)
 {
  x1 = x + cos(angle_to_radians(counter * 8)) * gd;
  y1 = y + sin(angle_to_radians(counter * 8)) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
  x1 = x + cos(angle_to_radians(counter * 8) + PI / 2) * gd;
  y1 = y + sin(angle_to_radians(counter * 8) + PI / 2) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
  x1 = x + cos(angle_to_radians(counter * 8) + PI) * gd;
  y1 = y + sin(angle_to_radians(counter * 8) + PI) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
  x1 = x + cos(angle_to_radians(counter * 8) - PI / 2) * gd;
  y1 = y + sin(angle_to_radians(counter * 8) - PI / 2) * gd;
  putpixel(bmp, x1, y1, COLOUR_GREY2 + grand(5));
 }*/


  if (dr == 0 && actor[player[play].actor_controlled].lock == LOCK_ACTOR0)
  {
   set_lock_x [play] = x;// + (max_x / 2);
   set_lock_y [play] = y;// + (max_y / 2);
   set_lock_radius [play] = 12;
  }
  if (dr == 1 && actor[player[play].actor_controlled].lock == LOCK_ACTOR1)
  {
   set_lock_x [play] = x;// + (max_x / 2);
   set_lock_y [play] = y;// + (max_y / 2);
   set_lock_radius [play] = 12;
  }
  if (dr == 0 && actor[player[play].actor_controlled].turret_lock == LOCK_ACTOR0)
  {
   set_turret_lock_x [play] = x;// + (max_x / 2);
   set_turret_lock_y [play] = y;// + (max_y / 2);
   set_turret_lock_radius [play] = 12;
  }
  if (dr == 1 && actor[player[play].actor_controlled].turret_lock == LOCK_ACTOR1)
  {
   set_turret_lock_x [play] = x;// + (max_x / 2);
   set_turret_lock_y [play] = y;// + (max_y / 2);
   set_turret_lock_radius [play] = 12;
  }
  

}


void draw_crosshair(BITMAP *bmp, int dr, int x, int y)
{
 int xc, yc;
 int col = COLOUR_YELLOW8;
 if (actor[dr].recycle1 > 0)
  col = COLOUR_GREY6;

 xc = x + xpart(actor[dr].angle + 28 - ANGLE_QUARTER, 48);
 yc = y + ypart(actor[dr].angle + 28 - ANGLE_QUARTER, 48);
 putpixel(bmp, xc, yc, col);
 
 xc = x + xpart(actor[dr].angle - 28 - ANGLE_QUARTER, 48);
 yc = y + ypart(actor[dr].angle - 28 - ANGLE_QUARTER, 48);
 putpixel(bmp, xc, yc, col);
 
/* xc = x + xpart(actor[dr].angle + 32 - ANGLE_QUARTER, 32);
 yc = y + ypart(actor[dr].angle + 32 - ANGLE_QUARTER, 32);
 putpixel(bmp, xc, yc, col);
 
 xc = x + xpart(actor[dr].angle - 32 - ANGLE_QUARTER, 32);
 yc = y + ypart(actor[dr].angle - 32 - ANGLE_QUARTER, 32);
 putpixel(bmp, xc, yc, col);
 
 xc = x + xpart(actor[dr].angle - ANGLE_QUARTER, 30);
 yc = y + ypart(actor[dr].angle - ANGLE_QUARTER, 30);
 putpixel(bmp, xc, yc, col);
 
 xc = x + xpart(actor[dr].angle - ANGLE_QUARTER, 34);
 yc = y + ypart(actor[dr].angle - ANGLE_QUARTER, 34);
 putpixel(bmp, xc, yc, col);
*/
 if (actor[dr].secondary != SECOND_NONE)
 {
  col = COLOUR_RED8;
  if (actor[dr].recycle2 > 0)
   col = COLOUR_GREY3;
 
  xc = x + xpart(actor[dr].angle + 16 - ANGLE_QUARTER, 45);
  yc = y + ypart(actor[dr].angle + 16 - ANGLE_QUARTER, 45);
  putpixel(bmp, xc, yc, col);
 
  xc = x + xpart(actor[dr].angle - 16 - ANGLE_QUARTER, 45);
  yc = y + ypart(actor[dr].angle - 16 - ANGLE_QUARTER, 45);
  putpixel(bmp, xc, yc, col);

 }
}



void draw_ship_bitmap(BITMAP *bmp, int x, int y, int which_ship, int angle, int width, int height)
{
/*   int draw_angle = angle / 4;

   if (draw_angle < 64)
   {
    draw_sprite(bmp, ship_bitmaps [which_ship] [draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 128)
   {
    draw_sprite_v_flip(bmp, ship_bitmaps [which_ship] [128 - draw_angle], x - width, y - height - 1);
    return;
   }
   if (draw_angle < 192)
   {
    draw_sprite_vh_flip(bmp, ship_bitmaps [which_ship] [draw_angle - 128], x - width - 2, y - height - 1);
    return;
   }
   draw_sprite_h_flip(bmp, ship_bitmaps [which_ship] [256 - draw_angle], x - width - 2, y - height);
*/
   int draw_angle = angle / 17.0666;

//   draw_angle ++;
   if (draw_angle > 60)
    draw_angle = 0;

//   draw_angle = draw_angle / 2);

//   which_ship = 1;
// Adding a new ship? Remember GB_SHIP_TYPES!

   if (draw_angle < 15)
   {
    draw_sprite(bmp, gb_ship_bitmaps [which_ship] [draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 30)
   {
    draw_sprite_v_flip(bmp, gb_ship_bitmaps [which_ship] [30 - draw_angle], x - width, y - height - 1);
    return;
   }
   if (draw_angle < 45)
   {
    draw_sprite_vh_flip(bmp, gb_ship_bitmaps [which_ship] [draw_angle - 30], x - width, y - height - 1);
    return;
   }
   draw_sprite_h_flip(bmp, gb_ship_bitmaps [which_ship] [60 - draw_angle], x - width, y - height);

/*   int draw_angle = angle / 16;

//   draw_angle ++;
   if (draw_angle > 64)
    draw_angle = 0;

//   draw_angle = draw_angle / 2);

//   which_ship = 1;
// Adding a new ship? Remember GB_SHIP_TYPES!

   if (draw_angle < 16)
   {
    draw_sprite(bmp, gb_ship_bitmaps [which_ship] [draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 32)
   {
    draw_sprite_v_flip(bmp, gb_ship_bitmaps [which_ship] [32 - draw_angle], x - width, y - height - 1);
    return;
   }
   if (draw_angle < 48)
   {
    draw_sprite_vh_flip(bmp, gb_ship_bitmaps [which_ship] [draw_angle - 32], x - width, y - height - 1);
    return;
   }
   draw_sprite_h_flip(bmp, gb_ship_bitmaps [which_ship] [64 - draw_angle], x - width, y - height);
*/
/*   draw_angle ++;
   if (draw_angle > 64)
    draw_angle = 0;

   draw_angle = (draw_angle / 2);

   which_ship = 0;

   if (draw_angle < 16)
   {
    draw_sprite(bmp, gb_ship_bitmaps [which_ship] [draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 32)
   {
    draw_sprite_v_flip(bmp, gb_ship_bitmaps [which_ship] [32 - draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 48)
   {
    draw_sprite_vh_flip(bmp, gb_ship_bitmaps [which_ship] [draw_angle - 32], x - width, y - height);
    return;
   }
   draw_sprite_h_flip(bmp, gb_ship_bitmaps [which_ship] [64 - draw_angle], x - width, y - height);

*/
/*

   if (draw_angle < 16)
   {
    draw_sprite(bmp, ship_bitmaps [which_ship] [draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 32)
   {
    draw_sprite_v_flip(bmp, ship_bitmaps [which_ship] [32 - draw_angle], x - width, y - height);
    return;
   }
   if (draw_angle < 48)
   {
    draw_sprite_vh_flip(bmp, ship_bitmaps [which_ship] [draw_angle - 32], x - width, y - height);
    return;
   }
   draw_sprite_h_flip(bmp, ship_bitmaps [which_ship] [64 - draw_angle], x - width, y - height);
*/
}
/*
void draw_triangle_ship(BITMAP *bmp, int dr, int size, int pointiness, int x, int y, unsigned char colour1, unsigned char colour2)
{
 int x1,y1,x2,y2,x3,y3;

 float radangle = angle_to_radians(actor[dr].angle);
// float radangle = angle_to_radians(actor[dr].angle);

 x1 = x + cos(radangle) * (size + pointiness);
 y1 = y + sin(radangle) * (size + pointiness);
 x2 = x + cos(radangle + (PI * 2) / 3) * size;
 y2 = y + sin(radangle + (PI * 2) / 3) * size;
 x3 = x + cos(radangle - (PI * 2) / 3) * size;
 y3 = y + sin(radangle - (PI * 2) / 3) * size;

 triangle(bmp, x1, y1, x2, y2, x3, y3, colour2);
 line(bmp, x1, y1, x2, y2, colour1);
 line(bmp, x1, y1, x3, y3, colour1);
 line(bmp, x2, y2, x3, y3, colour1);

}
*/


void draw_status2(BITMAP *bmp, int max_x, int max_y, int play)
{

 int dcol1;
 int dcol2;
 int i, x, y;
 int a = player[play].actor_controlled;

 int minutes_left = arena.time_left / 2000;
 int seconds_left = (arena.time_left / 33.3333) - (minutes_left * 60);
 int centiseconds_left = (arena.time_left) - (minutes_left * 2000) - seconds_left * 33.333;
 centiseconds_left *= 3;
 int time_colour = COLOUR_GREY6;
// int time_colour2 = COLOUR_YELLOW3 + centiseconds_left / 25;
 int time_colour2 = COLOUR_YELLOW1 + (centiseconds_left % 50) / 7;
 int time_x = 80;
 int time_y = 20;
// char timestring [10] = "{";
// char istr [5];
 
 if (seconds_left < 0)
  seconds_left = 0;
 if (minutes_left < 0)
  minutes_left = 0;
 if (centiseconds_left < 0)
  centiseconds_left = 0;

 if (game.type != GAME_DUEL || game.duel_mode == DUEL_3_MINUTES || game.duel_mode == DUEL_10_MINUTES)
 {
  if (arena.level_finished == 1)
  {
   textprintf_centre_ex(bmp, font2, max_x / 2, 170, COLOUR_ORANGE2 + (counter / 4) % 4, -1, "{__l_e_v_e_l____c_l_e_a_r__}");
   textprintf_centre_ex(bmp, font, max_x / 2, 170, -1, -1, "{__l_e_v_e_l____c_l_e_a_r__}");
   time_colour = COLOUR_YELLOW8;
   time_colour2 = COLOUR_YELLOW4;
   if (arena.time_bonus > 0)
   {
    if (arena.seconds_left_when_finished == 1)
    {
     textprintf_centre_ex(bmp, font2, max_x / 2, 280 + text_offset_y, COLOUR_RED8 - (counter / 4) % 4, -1, "{_%i__second__left_}", arena.seconds_left_when_finished);
     textprintf_centre_ex(bmp, font, max_x / 2, 280 + text_offset_y, -1, -1, "{_%i__second__left_}", arena.seconds_left_when_finished);
    }
     else
     {
      textprintf_centre_ex(bmp, font2, max_x / 2, 280 + text_offset_y, COLOUR_RED8 - (counter / 4) % 4, -1, "{_%i__seconds__left_}", arena.seconds_left_when_finished);
      textprintf_centre_ex(bmp, font, max_x / 2, 280 + text_offset_y, -1, -1, "{_%i__seconds__left_}", arena.seconds_left_when_finished);
     }
    textprintf_centre_ex(bmp, font2, max_x / 2, 320 + text_offset_y, COLOUR_RED8 - (counter / 4) % 4, -1, "{_bonus:__%i_}", arena.time_bonus);
    textprintf_centre_ex(bmp, font, max_x / 2, 320 + text_offset_y, -1, -1, "{_bonus:__%i_}", arena.time_bonus);
   }
    else
    {
     textprintf_centre_ex(bmp, font2, max_x / 2, 320 + text_offset_y, COLOUR_GREEN8 - (counter / 4) % 4, -1, "{_no__bonus_}");
     textprintf_centre_ex(bmp, font, max_x / 2, 320 + text_offset_y, -1, -1, "{_no__bonus_}");
    }
  }
    else
    {
    if (minutes_left == 0 && seconds_left < 10)
    {
//      time_colour = COLOUR_RED8 - (counter / 4) % 4;
//      time_colour2 = COLOUR_RED4 + (counter / 4) % 4;
      time_colour2 = COLOUR_RED1 + (centiseconds_left % 50) / 7;
     }
    }

// textprintf_right(bmp, font, time_x, 20, time_colour, "%i : %i %i", minutes_left, seconds_left, centiseconds_left);

// if (minutes_left < 1)
// strcat(timestring, "0");
/* strcat(timestring, itoa(minutes_left, istr, 10));
 strcat(timestring, "_:_");
 if (seconds_left < 10)
  strcat(timestring, "0");
 strcat(timestring, itoa(seconds_left, istr, 10));
 strcat(timestring, "}");
 textprintf_right_ex(bmp, font2, time_x + 30, time_y, time_colour2, -1, timestring);
 textprintf_right_ex(bmp, font, time_x + 30, time_y, -1, -1, timestring);*/
 if (seconds_left < 10)
 {
  textprintf_right_ex(bmp, font2, time_x + 30, time_y, time_colour2, -1, "{%i_:_0%i}", minutes_left, seconds_left);
  textprintf_right_ex(bmp, font, time_x + 30, time_y, -1, -1, "{%i_:_0%i}", minutes_left, seconds_left);
 }
  else 
  {
   textprintf_right_ex(bmp, font2, time_x + 30, time_y, time_colour2, -1, "{%i_:_%i}", minutes_left, seconds_left);
   textprintf_right_ex(bmp, font, time_x + 30, time_y, -1, -1, "{%i_:_%i}", minutes_left, seconds_left);
  } 
  if (centiseconds_left < 10)
  {
   textprintf_right_ex(bmp, small_font, time_x + 55, time_y + 21, time_colour, -1, "0%i", centiseconds_left);
//   textprintf_right(bmp, font2, time_x + 80, 20, time_colour2, "0%i", centiseconds_left);
  }
    else
    {
     textprintf_right_ex(bmp, small_font, time_x + 55, time_y + 21, time_colour, -1, "%i", centiseconds_left);
//    textprintf_right(bmp, font2, time_x + 80, 20, time_colour2, "%i", centiseconds_left);
    }
 } // end if !duel || timed duel

// for (i = 0; i <
 if (game.type == GAME_SINGLE || game.type == GAME_COOP)
 {
  textprintf_right_ex(bmp, font2, max_x - 20, time_y, COLOUR_GREY5, -1, "{%i}", player[play].score);
  textprintf_right_ex(bmp, font, max_x - 20, time_y, -1, -1, "{%i}", player[play].score);
  textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "level %i", arena.level);
 }
 
 if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
 {
  centiseconds_left = game.ta_total_time / 120000;
  // cs is actually hours here. Probably unnecessary, but there are some crazy people out there.
  minutes_left = game.ta_total_time / 2000 - (centiseconds_left * 60);
  seconds_left = (game.ta_total_time / 33.3333) - (minutes_left * 60);
/*  strcpy(timestring, "");
  if (centiseconds_left > 0)
  {
   strcat(timestring, itoa(centiseconds_left, istr, 10));
   strcat(timestring, ":");
   if (minutes_left < 10)
    strcat(timestring, "0");
  } 
  strcat(timestring, itoa(minutes_left, istr, 10));
  strcat(timestring, ":");
  if (seconds_left < 10)
   strcat(timestring, "0");
  strcat(timestring, itoa(seconds_left, istr, 10));
  textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, timestring);*/
  if (centiseconds_left > 0)
  {
   if (minutes_left < 10)
   {
    if (seconds_left < 10)
    {
     textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:0%i:0%i", centiseconds_left, minutes_left, seconds_left);
    }
     else
      textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:0%i:%i", centiseconds_left, minutes_left, seconds_left);
   } 
     else
     {
      if (seconds_left < 10)
      {
       textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:%i:0%i", centiseconds_left, minutes_left, seconds_left);
      }
       else
        textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:%i:%i", centiseconds_left, minutes_left, seconds_left);
     }
  } 
   else
   {
    if (seconds_left < 10)
    {
     textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:0%i", minutes_left, seconds_left);
    }
     else
      textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:%i", minutes_left, seconds_left);
   }
  
/*  if (centiseconds_left > 0)
   textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:%i:%i", centiseconds_left, minutes_left, seconds_left);
    else
     textprintf_ex(bmp, small_font, 20, 56, COLOUR_GREY6, -1, "%i:%i", minutes_left, seconds_left);*/
//     textprintf_ex(bmp, small_font, 20, 66, COLOUR_GREY6, -1, "%i:%i", game.ta_enemy_index, game.ta_symbol_count);
//     textprintf_ex(bmp, small_font, 20, 66, COLOUR_GREY6, -1, "%i: %i %i %i %i %i %i %i %i %i %i", game.ta_enemy_index, game.ta_enemies [0], game.ta_enemies [1], game.ta_enemies [2], game.ta_enemies [3], game.ta_enemies [4], game.ta_enemies [5], game.ta_enemies [6], game.ta_enemies [7], game.ta_enemies [8], game.ta_enemies [9]);

 }
//     textprintf_ex(bmp, small_font, 20, 166, COLOUR_GREY6, -1, "%i: %i %i %i %i %i %i %i %i %i %i %i", arena.symbol_index, arena.symbol_list [0], arena.symbol_list [1], arena.symbol_list [2], arena.symbol_list [3], arena.symbol_list [4], arena.symbol_list [5], arena.symbol_list [6], arena.symbol_list [7], arena.symbol_list [8], arena.symbol_list [9], arena.symbol_list [10]);
 
 if (game.type == GAME_DUEL)
  {
   dcol1 = COLOUR_YELLOW4;
   dcol2 = COLOUR_BLUE8;
   if ((play == 0 && player[0].duel_score > player[1].duel_score)
      || (play == 1 && player[1].duel_score > player[0].duel_score))
   {
    dcol1 = COLOUR_YELLOW4;
    dcol2 = COLOUR_ORANGE8;
   }
   if (player[0].duel_score == player[1].duel_score)
   {
    dcol1 = COLOUR_BLUE4;
    dcol2 = COLOUR_BLUE8;
   }
   dcol1 += (counter / 4) % 4;
   dcol2 -= (counter / 4) % 4;
   textprintf_right_ex(bmp, font2, max_x - 20, time_y, dcol2, -1, "{%i}", player[play].duel_score);
   textprintf_right_ex(bmp, font, max_x - 20, time_y, -1, -1, "{%i}", player[play].duel_score);
   if (game.duel_mode == DUEL_10_POINTS)
    textprintf_right_ex(bmp, small_font, max_x - 20, 56, COLOUR_GREY6, -1, "First to 10");
   if (game.duel_mode == DUEL_30_POINTS)
    textprintf_right_ex(bmp, small_font, max_x - 20, 56, COLOUR_GREY6, -1, "First to 30");
  }


  
 if (actor[a].upgraded_system [UPG_TURRET] > 0)
 {
  int x2, y2;
  int col = COLOUR_YELLOW8 - (counter / 2) % 4;
  x = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  x2 = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  pline(bmp, x, y, x2, y2, col);
  x = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  x2 = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  pline(bmp, x, y, x2, y2, col);
  x = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  x2 = max_x / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  pline(bmp, x, y, x2, y2, col);
  x = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  y = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) - 5;
  x2 = max_x / 2 - (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  y2 = max_y / 2 + (50 + actor[a].upgraded_system [UPG_TURRET] * 17) + 5;
  pline(bmp, x, y, x2, y2, col);
 }

// Scanner:
// NOTE: Scanner assumes at most 2 players - 0 & 1!

// int dot_colour;

 if (game.mode_void == 0)
 {

 rectfill(bmp, max_x - 80, max_y - 105, max_x - 5, max_y - 35, COLOUR_BLUE1);


  if (play == 0)
  {
   if (game.users > 1)
   {
    i = player[1].actor_controlled;
    if (actor[i].in_play != 0)
    {
     x = (actor[i].x * 75) / arena.max_x + max_x - 80;
     y = (actor[i].y * 70) / arena.max_y + max_y - 105;
     hline(bmp, max_x - 80, y, max_x - 5, COLOUR_GREY3);
     vline(bmp, x, max_y - 105, max_y - 35, COLOUR_GREY3);
    }
   }
   i = player[0].actor_controlled;
   if (actor[i].in_play != 0)
   {
    x = (actor[i].x * 75) / arena.max_x + max_x - 80;
    y = (actor[i].y * 70) / arena.max_y + max_y - 105;
    hline(bmp, max_x - 80, y, max_x - 5, COLOUR_GREY5);
    vline(bmp, x, max_y - 105, max_y - 35, COLOUR_GREY5);
   }
  }
   else
   {
    if (game.users > 1)
    {
     i = player[0].actor_controlled;
     if (actor[i].in_play != 0)
     {
      x = (actor[i].x * 75) / arena.max_x + max_x - 80;
      y = (actor[i].y * 70) / arena.max_y + max_y - 105;
      hline(bmp, max_x - 80, y, max_x - 5, COLOUR_GREY3);
      vline(bmp, x, max_y - 105, max_y - 35, COLOUR_GREY3);
     }
    }
    i = player[1].actor_controlled;
    if (actor[i].in_play != 0)
    {
     x = (actor[i].x * 75) / arena.max_x + max_x - 80;
     y = (actor[i].y * 70) / arena.max_y + max_y - 105;
     hline(bmp, max_x - 80, y, max_x - 5, COLOUR_GREY5);
     vline(bmp, x, max_y - 105, max_y - 35, COLOUR_GREY5);
    }
   }



 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type == ENEMY_NONE
      || ((enemy[i].type == ENEMY_SHADOW1 || enemy[i].type == ENEMY_SHADOW2) && enemy[i].attribute [0] == 0))
   continue;
  x = (enemy[i].x * 75) / arena.max_x + max_x - 80;
  y = (enemy[i].y * 70) / arena.max_y + max_y - 105;
  if (enemy[i].target == TARGET_PRIMARY)
//   putpixel(bmp, x, y, COLOUR_YELLOW8);
  {
   circlefill(bmp, x, y, 1, COLOUR_YELLOW5);
   putpixel(bmp, x, y, COLOUR_YELLOW8);
  } 
    else
     putpixel(bmp, x, y, COLOUR_ORANGE8);
 }

 for (i = 0; i < NO_PICKUPS; i ++)
 {
  if (pickup[i].type == PICKUP_NONE)
   continue;
  x = (pickup[i].x * 75) / arena.max_x + max_x - 80;
  y = (pickup[i].y * 70) / arena.max_y + max_y - 105;
//  if (enemy[i].target == TARGET_PRIMARY)
//   putpixel(bmp, x, y, COLOUR_YELLOW8);
//    else
     putpixel(bmp, x, y, COLOUR_GREEN8 - (counter / 4) % 4);
 }

 rect(bmp, max_x - 80, max_y - 105, max_x - 5, max_y - 35, COLOUR_BLUE8);
 } // end if void mode

// draw_ship_status(bmp, 80, max_y - 150, player[play].actor_controlled, 0);
 draw_ship_status(bmp, 50, max_y - 80, player[play].actor_controlled, 0);

 if (arena.game_over != 0)
 {
  if (game.type == GAME_DUEL)
  {
   if (game.duel_winner == -1)
   {
    textprintf_centre_ex(bmp, font2, max_x / 2, 180 + text_offset_y, COLOUR_GREEN8 - (counter / 3) % 4, -1, "{draw}");
    textprintf_centre_ex(bmp, font, max_x / 2, 180 + text_offset_y, -1, -1, "{draw}");
   }
    else
    {
     if (game.duel_winner == play)
     {
      textprintf_centre_ex(bmp, font2, max_x / 2, 180 + text_offset_y, COLOUR_ORANGE8 - (counter / 3) % 4, -1, "{you__win}");
      textprintf_centre_ex(bmp, font, max_x / 2, 180 + text_offset_y, -1, -1, "{you__win}");
     }
      else
      {
       textprintf_centre_ex(bmp, font2, max_x / 2, 180 + text_offset_y, COLOUR_GREEN8 - (counter / 3) % 4, -1, "{you__lose}");
       textprintf_centre_ex(bmp, font, max_x / 2, 180 + text_offset_y, -1, -1, "{you__lose}");
      }
    }
  }
   else
   {
    textprintf_centre_ex(bmp, font2, max_x / 2, 180 + text_offset_y, COLOUR_GREEN8 - (counter / 3) % 4, -1, "{___g_a_m_e___o_v_e_r___}");
    textprintf_centre_ex(bmp, font, max_x / 2, 180 + text_offset_y, -1, -1, "{___g_a_m_e___o_v_e_r___}");
   }
 }
  else
  {
   if (arena.new_level_sign > 0)
   {
    dcol2 = COLOUR_YELLOW8;
    dcol2 -= (counter / 4) % 4;
    if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
    {
       textprintf_centre_ex(bmp, font2, max_x / 2, 160 + text_offset_y, dcol2, -1, "{___t_i_m_e__a_t_t_a_c_k___}");
       textprintf_centre_ex(bmp, font, max_x / 2,  160 + text_offset_y, -1, -1, "{___t_i_m_e__a_t_t_a_c_k___}");
    }
     else
     {
      if (game.type != GAME_DUEL)
      {
       textprintf_centre_ex(bmp, font2, max_x / 2, 160 + text_offset_y, dcol2, -1, "{____l_e_v_e_l__%i____}", arena.level);
       textprintf_centre_ex(bmp, font, max_x / 2, 160 + text_offset_y, -1, -1, "{____l_e_v_e_l__%i____}", arena.level);
      } 
     } 
    } 
   }
//  }

 if (game.type != GAME_DUEL && game.type != GAME_TIME_ATTACK && game.type != GAME_TIME_ATTACK_COOP)
 {

   rect(bmp, 19, 69, 26, 79, COLOUR_GREY2);
   rect(bmp, 28 + 63, 69, 34 + 63, 79, COLOUR_GREY2);
   rect(bmp, 26, 71, 28 + 63, 77, COLOUR_GREY2);
   rect(bmp, 27, 73, 27 + 63, 75, COLOUR_GREY2);

   rectfill(bmp, 20, 70, 25, 78, COLOUR_GREY5);
   rectfill(bmp, 29 + 63, 70, 33 + 63, 78, COLOUR_GREY5);

//   rectfill(bmp, 25, 72, 28 + 9 * 7, 76, COLOUR_GREY5);
  hline(bmp, 25, 72, 28 + 63, COLOUR_GREY5);
  hline(bmp, 25, 76, 28 + 63, COLOUR_GREY5);

  if (arena.targets_left_total > 0)
  {
//   char targets_left_string [30];
/*   rect(bmp, 19, 69, 26, 79, COLOUR_GREY2);
   rect(bmp, 21 + 7 + arena.targets_left_total * 7, 69, 27 + 7 + arena.targets_left_total * 7, 79, COLOUR_GREY2);
   rect(bmp, 26, 71, 25 + arena.targets_left_total * 7, 77, COLOUR_GREY2);

   rectfill(bmp, 20, 70, 25, 78, COLOUR_GREY5);
   rectfill(bmp, 22 + 7 + arena.targets_left_total * 7, 70, 26 + 7 + arena.targets_left_total * 7, 78, COLOUR_GREY5);
   
   for (i = 0; i < arena.targets_left_total; i ++)
   {
     rectfill(bmp, 29 + i * 7, 70, 32 + i * 7, 78, COLOUR_GREY5);
   }
   
   for (i = 0; i < arena.targets_left_total; i ++)
   {
     rect(bmp, 28 + i * 7, 70, 33 + i * 7, 78, COLOUR_GREY1);
   }

   rectfill(bmp, 25, 72, 28 + arena.targets_left_total * 7, 76, COLOUR_GREY5);
   */
   rectfill(bmp, 25, 73, 28 + arena.targets_left_total * 7, 75, COLOUR_YELLOW8 - (counter / 4) % 6);
 
//   targets_left_string [i] = '\0';

//   textprintf_ex(bmp, small_font, 20, 70, COLOUR_GREY4, -1, targets_left_string);
//   textprintf_ex(bmp, small_font, 19, 69, COLOUR_GREY6, -1, targets_left_string);
//  textprintf_centre_ex(bmp, small_font, max_x / 2, 60, COLOUR_GREY4, targets_left_string);
//  textprintf_centre_ex(bmp, small_font, max_x / 2 - 1, 59, COLOUR_GREY6, targets_left_string);
//    rectfill(bmp, x + i * 10 - 3, y - 30, x + i * 10 + 3, y - 24, COLOUR_RED8);
  }


// if (arena.time_left > 195 && arena.time_left < 330 && (arena.time_left / 15) % 3 != 0)
  if (arena.time_left > 240 && arena.time_left < 330 && (arena.time_left / 10) % 3 != 0)
  {
    textprintf_centre_ex(bmp, font2, max_x / 2, 250 + text_offset_y, COLOUR_RED8 - (counter / 2) % 4, -1, "{_hurry__up_}");
    textprintf_centre_ex(bmp, font, max_x / 2, 250 + text_offset_y, -1, -1, "{_hurry__up_}");
  }
 
// if (arena.hostile > 0 && arena.hostile < 150 && (arena.hostile / 15) % 2 == 0)
  if (arena.hostile > 0 && arena.hostile < 90 && (arena.hostile / 10) % 3 != 0)
  {
    textprintf_centre_ex(bmp, font2, max_x / 2, 250 + text_offset_y, COLOUR_RED8 - (counter / 2) % 4, -1, "{_watch__out_}");
    textprintf_centre_ex(bmp, font, max_x / 2, 250 + text_offset_y, -1, -1, "{_watch__out_}");
  }
 }
 
}



void draw_hud(BITMAP *bmp, int x, int y, int i)
{
 return;
// int viewing_angle = actor[i].moving_angle - actor[i].angle - ANGLE_QUARTER;

// draw_scan(bmp, 50, 50, 0, 0);
/*
 if (game[0].users == 2)
  draw_scan(bmp, 260, 50, 0, 1);
   else
    draw_scan(bmp, 590, 50, 0, 1);

 draw_ship_status(bmp, 50, 420, i, 1);

 if (actor[i].x_speed != 0 || actor[i].y_speed != 0)
 {
  // start off with the velocity indicator:
//  int distance1 = sqrt(hypot(actor[i].x_speed, actor[i].y_speed)) + 5 + actor[i].radius;
//  int distance2 = (sqrt(hypot(actor[i].x_speed, actor[i].y_speed)) * 4) / 5 + 5 + actor[i].radius;
/ *  int x_disp1 = cos(angle_to_radians(viewing_angle)) * distance1;
  int y_disp1 = sin(angle_to_radians(viewing_angle)) * distance1;
  int x_disp2 = cos(angle_to_radians(viewing_angle + 10)) * distance2;
  int y_disp2 = sin(angle_to_radians(viewing_angle + 10)) * distance2;
  int x_disp3 = cos(angle_to_radians(viewing_angle - 10)) * distance2;
  int y_disp3 = sin(angle_to_radians(viewing_angle - 10)) * distance2;

  line(bmp, x + x_disp1, y + y_disp1, x + x_disp2, y + y_disp2, COLOUR_GREEN4);
  line(bmp, x + x_disp1, y + y_disp1, x + x_disp3, y + y_disp3, COLOUR_GREEN4);
*/
// }
 
}

void draw_ship_status(BITMAP *bmp, int x, int y, int a, int style)
{

 int i;

 int shield_colour;

// char numbers [10];
// char nstring [10];

 if (game.type != GAME_DUEL && game.type != GAME_TIME_ATTACK && game.type != GAME_TIME_ATTACK_COOP)
 {
  for (i = 0; i < game.ships_left; i ++)
  {
     rectfill(bmp, x - 8, y - 70 - i * 9, x - 15, y - 77 - i * 9, COLOUR_YELLOW7);
     rect(bmp, x - 8, y - 70 - i * 9, x - 15, y - 77 - i * 9, COLOUR_YELLOW1);
  }
 }

 if (actor[a].in_play == 0)
  return;


 int armoured = (actor[a].armour * 100) / actor[a].max_armour;
 int armour_colour = RLE_LSHIP_BLUE;
// if (actor[a].in_play == 1)
// {
  if (armoured < 100)
   armour_colour = RLE_LSHIP_GREEN;
  if (armoured < 70)
   armour_colour = RLE_LSHIP_YELLOW;
  if (armoured < 30)
   armour_colour = RLE_LSHIP_RED;

  draw_rle_sprite(bmp, large_ships [actor[a].ship] [armour_colour], x - 24, y - 24);

  if (actor[a].hurt_pulse > 0)
   draw_rle_sprite(bmp, large_ships [actor[a].ship] [RLE_LSHIP_LINES], x - 24, y - 24);

  armour_colour = COLOUR_BLUE4;
  int armour_colour2 = COLOUR_BLUE1;

  if (armoured < 100)
  {
   armour_colour = COLOUR_GREEN4;
   armour_colour2 = COLOUR_GREEN1;
  }
  if (armoured < 70)
  {
   armour_colour = COLOUR_YELLOW4;
   armour_colour2 = COLOUR_YELLOW1;
  }
  if (armoured < 30)
  {
   armour_colour = COLOUR_RED4;
   armour_colour2 = COLOUR_RED1;
  }
  
//  armour_colour += 7 - (((arena.time_left) - ((arena.time_left / 2000) * 2000) - seconds_left * 33.333) % 50) / 7;
  armour_colour2 += 7 - ((arena.time_left % 105) % 35) / 5;
// int minutes_left = arena.time_left / 2000;
// int seconds_left = (arena.time_left / 33.3333) - (minutes_left * 60);

/*  itoa(actor[a].armour / 10, numbers, 10);
  strcpy(nstring, "{");
  strcat(nstring, numbers);
  strcat(nstring, "}");
  textprintf_ex(bmp, font2, x + 48, y + 15, armour_colour2, -1, nstring);
  textprintf_ex(bmp, font, x + 48, y + 15, -1, -1, nstring);*/

  textprintf_ex(bmp, font2, x + 48, y + 15, armour_colour2, -1, "{%i}", actor[a].armour / 10);
  textprintf_ex(bmp, font, x + 48, y + 15, -1, -1, "{%i}", actor[a].armour / 10);

  if (actor[a].ability [ABILITY_DEFENCE] [SYMBOL_CIRCLE] > 0)
  {
/*   itoa(actor[a].shield / 10, numbers, 10);
   strcpy(nstring, "{");
   strcat(nstring, numbers);
   strcat(nstring, "}");
   textprintf_ex(bmp, font2, x + 55, y - 20, COLOUR_GREEN4 + grand(5), -1, nstring);
   textprintf_ex(bmp, font, x + 55, y - 20, -1, -1, nstring);*/
   textprintf_ex(bmp, font2, x + 55, y - 20, COLOUR_GREEN4 + grand(5), -1, "{%i}", actor[a].shield / 10);
   textprintf_ex(bmp, font, x + 55, y - 20, -1, -1, "{%i}", actor[a].shield / 10);
  }

// }
 
// circlefill(bmp, x, y, 7, armour_colour);

 int layers = actor[a].shield / 70; // 100
 int last_layer = actor[a].shield % 70; // 100

 if (actor[a].in_play == 1
     && actor[a].ability [ABILITY_DEFENCE] [SYMBOL_CIRCLE] > 0 && actor[a].shield > 0)
 {
  for (i = 0; i < layers; i ++)
  {
   shield_colour = COLOUR_GREEN6;// + grand(6);
//   a2 = 64 + (actor[a].shield * 96) / actor[a].max_shield;
//   a1 = 64 - (actor[a].shield * 96) / actor[a].max_shield;
//   arc(bmp, x, y, itofix(-28), itofix(156), 35 + i * 3, shield_colour);
   rectfill(bmp, 0 + x - 20, y - 30 - (i * 6), 0 + x + 20, y - 35 - (i * 6), shield_colour);
   rect(bmp, 0 + x - 20, y - 30 - (i * 6), 0 + x + 20, y - 35 - (i * 6), COLOUR_GREEN1);

  }
  
   if (last_layer != 0)
   {
    shield_colour = COLOUR_GREEN6;// + grand(3);
    if (actor[a].shield_pulse > 0)
    {
/*     shield_colour = COLOUR_GREY4 + actor[a].shield_pulse / 3;
     if (shield_colour > COLOUR_WHITE)
      shield_colour = COLOUR_WHITE;*/
      shield_colour = actor[a].shield_pulse / 2;
      switch(shield_colour)
      {
       case 0: shield_colour = COLOUR_GREEN6; break;
       case 1: shield_colour = COLOUR_GREEN7; break;
       case 2: shield_colour = COLOUR_GREEN8; break;
       case 3: shield_colour = COLOUR_YELLOW8; break;
       default:
       case 4: shield_colour = COLOUR_WHITE; break;
       
      }
    }
//    a2 = 64 + (last_layer * 96) / 100;
//    a1 = 64 - (last_layer * 96) / 100;
//    a2 = 64 + (last_layer * 96) / 100;
//    a1 = 64 - (last_layer * 96) / 100;
//    arc(bmp, x, y, itofix(a1), itofix(a2), 35 + layers * 3, shield_colour);
   last_layer = 20 * last_layer / 70;
   rectfill(bmp, 0 + x - last_layer, y - 30 - (i * 6), 0 + x + last_layer, y - 35 - (i * 6), shield_colour);
   rect(bmp, 0 + x - last_layer, y - 30 - (i * 6), 0 + x + last_layer, y - 35 - (i * 6), COLOUR_GREEN1);
   }
/*
 if (actor[a].upgraded_system [UPG_SHIELD] > 0 && actor[a].shield > 0)
 {
  for (i = 0; i < actor[a].upgraded_system [UPG_SHIELD] + 1; i ++)
  {
   shield_colour = COLOUR_BLUE6 + grand(3);
   if (i == actor[a].upgraded_system [UPG_SHIELD]
       && actor[a].shield_pulse > 0)
    {
     shield_colour = COLOUR_GREY4 + actor[a].shield_pulse / 3;
     if (shield_colour > COLOUR_WHITE)
      shield_colour = COLOUR_WHITE;
    }
   a2 = 64 + (actor[a].shield * 96) / actor[a].max_shield;
   a1 = 64 - (actor[a].shield * 96) / actor[a].max_shield;
   arc(bmp, x, y, itofix(a1), itofix(a2), 25 + i * 3, shield_colour);

  }
*/
 }

// if (actor[a].ship != SHIP_SCORPION)
  if (player[actor[a].controller].link_fire == 1)
  {
    circlefill(bmp, x - 36, y + 43, 2, COLOUR_YELLOW3);
    textprintf_centre_ex(bmp, small_font, x - 35, y + 37, COLOUR_GREEN8, -1, "x");
  }
   else
   {
    textprintf_centre_ex(bmp, small_font, x - 36, y + 37, COLOUR_ORANGE8, -1, "|  |");
    circlefill(bmp, x - 36, y + 43, 2, COLOUR_RED8);
   }

 if (actor[a].ship != SHIP_RETRO)
 {
  if (actor[a].dragging == 20)
  {
    circlefill(bmp, x - 36, y + 33, 2, COLOUR_RED8);
  }
   else
   {
    circlefill(bmp, x - 36, y + 33, 2, COLOUR_YELLOW3);
   }
 }
 
 armour_colour = COLOUR_GREY4;
 if (actor[a].repairing > 0)
 {
  if (actor[a].repairing > 173)
   armour_colour = COLOUR_YELLOW8;
    else
     armour_colour = COLOUR_YELLOW1 + actor[a].repairing / 25;
  armour_colour2 = COLOUR_ORANGE1 + (actor[a].repairing / 4) % 8;

/*  itoa(actor[a].repairing / 2, numbers, 10);
  strcpy(nstring, "{");
  strcat(nstring, numbers);
  strcat(nstring, "}");
  textprintf_ex(bmp, font2, x + 48, y + 45, armour_colour2, -1, nstring);
  textprintf_ex(bmp, font, x + 48, y + 45, -1, -1, nstring);*/
  textprintf_ex(bmp, font2, x + 48, y + 45, armour_colour2, -1, "{%i}", actor[a].repairing / 2);
  textprintf_ex(bmp, font, x + 48, y + 45, -1, -1, "{%i}", actor[a].repairing / 2);

  armour_colour = COLOUR_YELLOW2 + (counter / 3) % 6;

 }
// armoured = (actor[a].energy * 50) / actor[a].max_energy;
 armoured /= 2;
 rectfill(bmp, x - 25, y + 30, x + armoured - 25, y + 45, armour_colour);
 rect(bmp, x - 25, y + 30, x + 25, y + 45, COLOUR_GREY6);
// add text

// for (i = 0; i < player[actor[a].controller].ships_left; i ++)
}


void draw_upgrades(BITMAP *bmp, int x, int y, int play)
{
// text_mode(-1);
 int i, j, k, x_offset, n, col, l;
 int a = player[play].actor_controlled;
 
// actor[a].ability [abil] [symb] ++;
/*
Primary - yellow
drive - green
armour - blue
secondary - red
*/
 col = COLOUR_RED8;
 for (i = 0; i < 4; i ++)
 {
  switch(i)
  {
   case ABILITY_PRIMARY:
    col = COLOUR_YELLOW5;
    break;
   case ABILITY_DRIVE:
    col = COLOUR_GREEN5;
    break;
   case ABILITY_DEFENCE:
    col = COLOUR_BLUE5;
    break;
   case ABILITY_SECONDARY:
    col = COLOUR_RED5;
    break;
  } 
  n = 0;
  for (j = 0; j < 4; j ++)
  {
   if (actor[a].ability [i] [j] > 0)
    n ++;
  } 
  if (n == 0)
   continue;
  x_offset = n * 6; 
  l = 0;
  for (j = 0; j < 3; j ++)
  {
   if (actor[a].ability [i] [j] > 0)
   {
    draw_static_symbol(bmp, x + i * 60 + l * 10 - x_offset, y + 8, col, j);
    for (k = 0; k < actor[a].ability [i] [j]; k ++)
    {
     hline(bmp, x + i * 60 + l * 10 - 3 - x_offset, y - k * 3, x + i * 60 + l * 10 + 3 - x_offset, col);
    }
    l ++;   
   }
  }
 
 }

// textprintf_ex(bmp, small_font, x - 20, y + 20, COLOUR_YELLOW6, -1, primary_name(actor[a].primary));
 char dstr [30];
 secondary_name(actor[a].secondary, dstr);
 if (game.users == 1)
  textprintf_ex(bmp, small_font, x + 250, y + 20, COLOUR_YELLOW6, -1, dstr);
   else 
    textprintf_ex(bmp, small_font, x + 220, y, COLOUR_YELLOW6, -1, dstr);
 
}

void draw_static_symbol(BITMAP *bmp, int x, int y, int col, int symb)
{
 switch(symb)
 {
  case SYMBOL_SQUARE:
   rectfill(bmp, x - 3, y - 3, x + 3, y + 3, col);
   break;
  case SYMBOL_CIRCLE:
   circlefill(bmp, x, y, 3, col);
   break;
/*  case SYMBOL_CROSS:
   rectfill(bmp, x - 1, y - 3, x + 1, y + 3, col);
   rectfill(bmp, x - 3, y - 1, x + 3, y + 1, col);
   break;*/
  case SYMBOL_TRIANGLE:
// triangle(bmp, x, y - 3, x + 3, y + 2, x - 3, y + 2, col);
   triangle(bmp, x, y + 5, x + 3, y - 2, x - 3, y - 2, col);
   break; 
 }
 
}
/*
char *upgrade_name(int i)
{
 switch (i)
 {
  case UPG_NONE:
   return "none";
  case UPG_SPEED:
   return "speed";
  case UPG_ARMOUR:
   return "armour";
  case UPG_POWER:
   return "power";
  case UPG_AUTOFIRE:
   return "autof";
  case UPG_MULTIFIRE:
   return "multif";
  case UPG_PROJECT:
   return "proj";
  case UPG_WARHEAD:
   return "whead";
  case UPG_ROCKET:
   return "rocket";
  case UPG_TUBES:
   return "tubes";
  case UPG_SLIDE:
   return "slide";
  case UPG_SEEKER:
   return "seek";
  case UPG_BOMB:
   return "bomb";
  case UPG_LAUNCHER:
   return "launch";
  case UPG_LOADER:
   return "load";
  case UPG_SPECIAL:
   return "special";
  case UPG_TURRET:
   return "turret";
  case UPG_SIDEKICK:
   return "sidek";
  case UPG_HEAVY:
   return "heavy";
  case UPG_ORBITAL:
   return "orbit";
  case UPG_BACKFIRE:
   return "backf";
  case UPG_SHIELD:
   return "shield";
  case UPG_RETRO:
   return "retro";
  case UPG_ROAMER:
   return "roam";
  case UPG_MINIMISSILES:
   return "minim";
  case UPG_DRIFT:
   return "drift";


   
 }

 return "ERROR!";
}

char *primary_name(int i)
{
 switch (i)
 {
  case WPN_DARTS:
   return "Darts";
  case WPN_SPINES:
   return "Spines";
  case WPN_BURST:
   return "Burst";
  case WPN_TEETH:
   return "Teeth";
 }
 
 return "ERROR!";

}
*/

void secondary_name(int i, char str [30])
{
 switch (i)
 {
  default:
  case SECOND_NONE: strcpy(str, "No Weapon"); break;
  case SECOND_FURIOUS_ORB: strcpy(str, "Furious Orb"); break;
  case SECOND_BURNING_EYE: strcpy(str, "Burning Eyes"); break;
  case SECOND_MANIFOLD_ORB: strcpy(str, "Manifold Orb"); break;
  case SECOND_EYE_DESOLATION: strcpy(str, "Eye of Desolation"); break;
  case SECOND_PANIC_EELS: strcpy(str, "Panic Eels"); break;
  case SECOND_WORMS_SORROW: strcpy(str, "Worms of Agony"); break;
  case SECOND_WORMS_AGONY: strcpy(str, "Worms of Sorrow"); break;
  case SECOND_FROZEN_STARS: strcpy(str, "Frozen Stars"); break;
  case SECOND_FROZEN_TEETH: strcpy(str, "Frozen Teeth"); break;
  case SECOND_TOXIC_SUN: strcpy(str, "Toxic Sun"); break;
  case SECOND_FLOWER: strcpy(str, "Toxic Flower"); break;
  case SECOND_SPORES: strcpy(str, "Golden Spores"); break;
  case SECOND_CLAWS: strcpy(str, "Claws of Hunger"); break;
 }

}



void draw_scan_dot(BITMAP *bmp, int x, int y, int type)
{

 switch(type)
 {
  case ACTORTYPE_SHIP:
   putpixel(bmp, x, y, COLOUR_BLUE4);
   break;
  case ACTORTYPE_REDSHIP:
   putpixel(bmp, x, y, COLOUR_RED4);
/*   putpixel(bmp, x + 1, y, 2);
   putpixel(bmp, x - 1, y, 2);
   putpixel(bmp, x, y + 1, 2);
   putpixel(bmp, x, y - 1, 2);*/
   break;

 }

}



void draw_bullets(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 int i, x, y, x2, y2;

 for (i = 0; i < NO_BULLETS; i ++)
 {
  if (bullet[i].type == BULLET_NONE)
   continue;
  if (bullet[i].left_owner == 0)
   continue;
  x = (bullet[i].x / GRAIN) - (centre_x / GRAIN);
  y = (bullet[i].y / GRAIN) - (centre_y / GRAIN);
/*  if ((bullet[i].x < centre_x - (max_x / 2) * GRAIN
      || bullet[i].x > centre_x + (max_x / 2) * GRAIN
      || bullet[i].y < centre_y - (max_y / 2) * GRAIN
      || bullet[i].y > centre_y + (max_y / 2) * GRAIN)
      &&
        (bullet[i].x2 < centre_x - (max_x / 2) * GRAIN
        || bullet[i].x2 > centre_x + (max_x / 2) * GRAIN
        || bullet[i].y2 < centre_y - (max_y / 2) * GRAIN
        || bullet[i].y2 > centre_y + (max_y / 2) * GRAIN))
      continue;*/
  if ((bullet[i].x + bullet[i].size < centre_x - (max_x / 2) * GRAIN
      || bullet[i].x - bullet[i].size > centre_x + (max_x / 2) * GRAIN
      || bullet[i].y + bullet[i].size < centre_y - (max_y / 2) * GRAIN
      || bullet[i].y - bullet[i].size > centre_y + (max_y / 2) * GRAIN)
      &&
        (bullet[i].x2 < centre_x - (max_x / 2) * GRAIN
        || bullet[i].x2 > centre_x + (max_x / 2) * GRAIN
        || bullet[i].y2 < centre_y - (max_y / 2) * GRAIN
        || bullet[i].y2 > centre_y + (max_y / 2) * GRAIN))
      continue;
  x2 = (bullet[i].x2 / GRAIN) - (centre_x / GRAIN);
  y2 = (bullet[i].y2 / GRAIN) - (centre_y / GRAIN);
  draw_a_bullet(bmp, i, x + (max_x / 2), y + (max_y / 2), x2 + (max_x / 2), y2 + (max_y / 2), max_x, max_y, play);
  
 }

}


// angle is the angle it's being viewed at, not its direction
void draw_a_bullet(BITMAP *bmp, int dr, int x, int y, int x2, int y2, int max_x, int max_y, int play)
{
 int xa, ya, xb, yb, xc, yc, cs;

#ifdef DEBUG_DISPLAY
if (debug_info == 1)
 textprintf_centre_ex(bmp, small_font, x, y, COLOUR_YELLOW4, COLOUR_GREY2, "%i", bullet[dr].type);
#endif

 switch(bullet[dr].type)
 {
   case BULLET_TRI1:
    draw_tri(bmp, x, y, bullet[dr].angle, 13, 8, ANGLE_QUARTER + ANGLE_1_32, GC_YELLOW1, GC_YELLOW6);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    xa = x - xpart(bullet[dr].angle, 4);
    ya = y - ypart(bullet[dr].angle, 4);
    circlefill(bmp, xa, ya, 4 + grand(3), TRANS_ORANGE);
    circlefill(bmp, xa, ya, 3 + grand(2), TRANS_YELLOW);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    break;
   case BULLET_TRI2:
    draw_tri(bmp, x, y, bullet[dr].angle, 13, 8, ANGLE_QUARTER + ANGLE_1_32, GC_RED1, GC_RED6);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    xa = x - xpart(bullet[dr].angle, 4);
    ya = y - ypart(bullet[dr].angle, 4);
    circlefill(bmp, xa, ya, 4 + grand(3), TRANS_LBLUE);
    circlefill(bmp, xa, ya, 3 + grand(2), TRANS_WHITE);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    break;
   case BULLET_TRI3:
    draw_tri(bmp, x, y, bullet[dr].angle, 14, 7, ANGLE_QUARTER + ANGLE_1_32, GC_BLUE1, GC_BLUE6);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    xa = x - xpart(bullet[dr].angle, 4);
    ya = y - ypart(bullet[dr].angle, 4);
    circlefill(bmp, xa, ya, 4 + grand(3), TRANS_YELLOW);
    circlefill(bmp, xa, ya, 3 + grand(2), TRANS_WHITE);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    break;
   case BULLET_OVERTRI:
    draw_tri(bmp, x, y, bullet[dr].angle, 18, 12, ANGLE_QUARTER + ANGLE_1_32, GC_ORANGE1, GC_ORANGE6);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    xa = x - xpart(bullet[dr].angle, 9);
    ya = y - ypart(bullet[dr].angle, 9);
    circlefill(bmp, xa, ya, 7 + grand(3), TRANS_LGREEN);
    circlefill(bmp, xa, ya, 6 + grand(2), TRANS_YELLOW);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    break;
/*    case BULLET_SLIVER:
     pline(bmp, x + xpart(bullet[dr].special1, 2), y + ypart(bullet[dr].special1, 2),
      x - xpart(bullet[dr].special1, 2), y - ypart(bullet[dr].special1, 2), COLOUR_YELLOW8);
     break;*/
   case BULLET_CIRCLER:
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    xa = bullet[dr].timeout;
/*    if (xa < 32)
     circle(bmp, x, y, xa, get_circler_colour(xa / 8));
    if (xa < 48 && xa > 16)
     circle(bmp, x, y, (xa - 16), get_circler_colour((xa - 16) / 8));
    if (xa < 64 && xa > 32)
     circle(bmp, x, y, (xa - 32), get_circler_colour((xa - 32) / 8));*/
    if (xa < 24)
     circle(bmp, x, y, xa * 2, get_circler_colour(xa / 6));
    if (xa < 36 && xa > 12)
     circle(bmp, x, y, (xa - 12) * 2, get_circler_colour((xa - 12) / 6));
    if (xa < 48 && xa > 24)
     circle(bmp, x, y, (xa - 24) * 2, get_circler_colour((xa - 24) / 6));
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
    break;
  case BULLET_TORPEDO2:
   xa = x + xpart(bullet[dr].angle, 3);
   ya = y + ypart(bullet[dr].angle, 3);
   xb = x + xpart(bullet[dr].angle + ANGLE_FULL / 3, 4);
   yb = y + ypart(bullet[dr].angle + ANGLE_FULL / 3, 4);
   xc = x + xpart(bullet[dr].angle - ANGLE_FULL / 3, 4);
   yc = y + ypart(bullet[dr].angle - ANGLE_FULL / 3, 4);
/*   cs = angle_to_radians(bullet[dr].angle);
   xa = (x) + ((float) cos(cs) * 3);
   ya = (y) + ((float) sin(cs) * 3);
   xb = (x) + ((float) cos(cs + (PI * 2) / 3) * 4);
   yb = (y) + ((float) sin(cs + (PI * 2) / 3) * 4);
   xc = (x) + ((float) cos(cs - (PI * 2) / 3) * 4);
   yc = (y) + ((float) sin(cs - (PI * 2) / 3) * 4);*/
/*   xa = (x) + ((float) cos(radangle + PI) * 3);
   ya = (y) + ((float) sin(radangle + PI) * 3);
   xb = (x) + ((float) cos(radangle + PI + (PI * 2) / 3) * 4);
   yb = (y) + ((float) sin(radangle + PI + (PI * 2) / 3) * 4);
   xc = (x) + ((float) cos(radangle + PI - (PI * 2) / 3) * 4);
   yc = (y) + ((float) sin(radangle + PI - (PI * 2) / 3) * 4);*/
   triangle(bmp, xa, ya, xb, yb, xc, yc, COLOUR_GREY5);
   line(bmp, xa, ya, xb, yb, COLOUR_BLUE7);
   line(bmp, xa, ya, xc, yc, COLOUR_BLUE7);
   line(bmp, xb, yb, xc, yc, COLOUR_BLUE7);
   break;
  case BULLET_HOSTILE:
   break;
  case BULLET_STING:
  case BULLET_STING2:
//   xa = x2 - x;
//   ya = y2 - y;
   x = (bullet[dr].x) - (actor[player[play].actor_controlled].x) + (max_x / 2) * GRAIN;
   y = (bullet[dr].y) - (actor[player[play].actor_controlled].y) + (max_y / 2) * GRAIN;
   x2 = (bullet[dr].x2) - (actor[player[play].actor_controlled].x) + (max_x / 2) * GRAIN;
   y2 = (bullet[dr].y2) - (actor[player[play].actor_controlled].y) + (max_y / 2) * GRAIN;
   xa = x2 - x;
   ya = y2 - y;
   pline(bmp, x / GRAIN, y / GRAIN, (x + 3 * xa) / GRAIN, (y + 3 * ya) / GRAIN, bullet[dr].colours [0]);
//   putpixel(bmp, x / GRAIN, y / GRAIN, bullet[dr].colours [0]);
   break;
  case BULLET_POWERED:
  case BULLET_ZAP:
  case BULLET_ZAP_DRAG:
  case BULLET_SILVER_TOOTH:
  case BULLET_CRYSTAL_TOOTH:
  case BULLET_CRYSTAL_SPINE:
  case BULLET_GOLDEN_NEEDLE:
  case BULLET_GOLDEN_NEEDLE_SMALL:
  case BULLET_NUMEROUS_DART:
  case BULLET_BRASS_TOOTH:
  case BULLET_BRASS_TOOTH_SMALL:
/*  xa = x2 - x;
  ya = y2 - y;
  pline(bmp, x, y, x + xa, y + ya, bullet[dr].colours [0]);
  pline(bmp, x + xa, y + ya, x + 2 * xa, y + 2 * ya, bullet[dr].colours [1]);
  pline(bmp, x + 2 * xa, y + 2 * ya, x + 3 * xa, y + 3 * ya, bullet[dr].colours [2]);*/
// we need the high granularity for zaps
  x = (bullet[dr].x) - (actor[player[play].actor_controlled].x);
  y = (bullet[dr].y) - (actor[player[play].actor_controlled].y);
  x2 = (bullet[dr].x2) - (actor[player[play].actor_controlled].x);
  y2 = (bullet[dr].y2) - (actor[player[play].actor_controlled].y);
  xa = x2 - x;
  ya = y2 - y;
  pline(bmp, (x) / GRAIN + (max_x / 2), y / GRAIN + (max_y / 2), (x + xa) / GRAIN + (max_x / 2), (y + ya) / GRAIN + (max_y / 2), bullet[dr].colours [0]);
  pline(bmp, (x + xa) / GRAIN + (max_x / 2), (y + ya) / GRAIN + (max_y / 2), (x + 2 * xa) / GRAIN + (max_x / 2), (y + 2 * ya) / GRAIN + (max_y / 2), bullet[dr].colours [1]);
  pline(bmp, (x + 2 * xa) / GRAIN + (max_x / 2), (y + 2 * ya) / GRAIN + (max_y / 2), (x + 3 * xa) / GRAIN + (max_x / 2), (y + 3 * ya) / GRAIN + (max_y / 2), bullet[dr].colours [2]);
   break;
  case BULLET_E_BOMB2:
   x2 = GC_WHITE - (bullet[dr].timeout / 4) % 4;
   xa = x + xpart((bullet[dr].timeout % 32) * 32, 5);
   ya = y + ypart((bullet[dr].timeout % 32) * 32, 5);
   putpixel(bmp, xa, ya, x2);
   xa = x + xpart((bullet[dr].timeout % 32) * 32 + ANGLE_HALF, 5);
   ya = y + ypart((bullet[dr].timeout % 32) * 32 + ANGLE_HALF, 5);
   putpixel(bmp, xa, ya, x2);
   x2 = GC_GREY4 + (bullet[dr].timeout / 4) % 4;
   circlefill(bmp, x, y, 2, x2);
   x2 = COLOUR_WHITE - (bullet[dr].timeout / 4) % 4;
   circlefill(bmp, x, y, 1, x2);
   break;
  case BULLET_DISRUPT1:
    clear_bitmap(distortion_mask);
    circlefill(distortion_mask, 100, 100, 9, 1);
    distortion(bmp, x, y, 19, 19, grand(31) - 15, grand(31) - 15);  
    break; 
  case BULLET_DISRUPT1_DROP:
    clear_bitmap(distortion_mask);
    xa = bullet[dr].special1;
    if (xa > 95)
     xa = 95;
    circlefill(distortion_mask, 100, 100, xa, 1);
    ya = bullet[dr].timeout / 2;
    distortion(bmp, x, y, xa, xa, grand(ya) - grand(ya), grand(ya) - grand(ya));  
    break; 
  case BULLET_DISRUPT2:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, 6 + pulsate(64, 3, bullet[dr].timeout), TRANS_REVERSE);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break; 
  case BULLET_DISRUPT3:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, 6 + pulsate(64, 3, bullet[dr].timeout), TRANS_DARKEN);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break; 
  case BULLET_MINE1:
/*   x2 = COLOUR_WHITE - (bullet[dr].timeout / 4) % 4;
   xa = x + xpart((bullet[dr].timeout % 32) * 32, 5);
   ya = y + ypart((bullet[dr].timeout % 32) * 32, 5);
   putpixel(bmp, xa, ya, x2);
   xa = x + xpart((bullet[dr].timeout % 32) * 32 + ANGLE_HALF, 5);
   ya = y + ypart((bullet[dr].timeout % 32) * 32 + ANGLE_HALF, 5);
   putpixel(bmp, xa, ya, x2);*/
//   x2 = COLOUR_GREY4 + (bullet[dr].timeout / 4) % 4;
   circlefill(bmp, x, y, 3, GC_GREEN2);
   circlefill(bmp, x, y, 2, GC_GREEN8);
   x2 = COLOUR_ORANGE2 + (bullet[dr].timeout / 4) % 7;
   putpixel(bmp, x, y, x2);
//   circlefill(bmp, x, y, 1, x2);
   break;
  case BULLET_MINE2:
   circlefill(bmp, x, y, 3, GC_RED2);
   circlefill(bmp, x, y, 2, GC_RED8);
   x2 = COLOUR_YELLOW2 + (bullet[dr].timeout / 4) % 7;
   putpixel(bmp, x, y, x2);
   break;
  case BULLET_MINE3:
   circlefill(bmp, x, y, 3, GC_YELLOW2);
   circlefill(bmp, x, y, 2, GC_YELLOW8);
   x2 = COLOUR_BLUE2 + (bullet[dr].timeout / 2) % 7;
   putpixel(bmp, x, y, x2);
   break;
  case BULLET_MBOMB: // was going to be the messengers' weapon, but not any more
  case BULLET_E_BOMB:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, 2 + grand(2), TRANS_WHITE);
   circlefill(bmp, x, y, 3 + grand(3), TRANS_YELLOW);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   draw_a_light(bmp, 7 + grand(3), x, y);
   break; 
  case BULLET_TWIRLY1:
   x2 = (bullet[dr].timeout % 32) * 32;
   if (bullet[dr].special1 == 1)
    x2 = ANGLE_FULL - x2;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   xa = x + xpart(x2, 10);
   ya = y + ypart(x2, 10);
   circlefill(bmp, xa, ya, 2 + grand(2), bullet[dr].colours [0]);
   circlefill(bmp, xa, ya, 3 + grand(3), bullet[dr].colours [1]);
   draw_a_light(bmp, 8 + grand(3), xa, ya);
   xa = x + xpart(x2 + ANGLE_HALF, 10);
   ya = y + ypart(x2 + ANGLE_HALF, 10);
   circlefill(bmp, xa, ya, 2 + grand(2), bullet[dr].colours [0]);
   circlefill(bmp, xa, ya, 3 + grand(3), bullet[dr].colours [1]);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   draw_a_light(bmp, 8 + grand(3), xa, ya);
   break;
  case BULLET_TWIRLY2:
   x2 = (bullet[dr].timeout % 32) * 32;
   if (bullet[dr].special1 == 1)
    x2 = ANGLE_FULL - x2;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   xa = x + xpart(x2, 6);
   ya = y + ypart(x2, 6);
   circlefill(bmp, xa, ya, 2 + grand(2), bullet[dr].colours [0]);
   draw_a_light(bmp, 8 + grand(3), xa, ya);
   xa = x + xpart(x2 + ANGLE_HALF, 6);
   ya = y + ypart(x2 + ANGLE_HALF, 6);
   circlefill(bmp, xa, ya, 2 + grand(2), bullet[dr].colours [0]);
   circlefill(bmp, x, y, 3 + grand(2), bullet[dr].colours [0]);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   draw_a_light(bmp, 8 + grand(3), xa, ya);
   break;
  case BULLET_TWIRLY3:
   x2 = (bullet[dr].timeout % 32) * 32;
   if (bullet[dr].special2 == 1)
    x2 = ANGLE_FULL - x2;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   for (cs = 0; cs < 3; cs ++)
   {
//    xa = x + xpart(x2 + ANGLE_QUARTER * cs, 12);
//    ya = y + ypart(x2 + ANGLE_QUARTER * cs, 12);
    xa = x + xpart(x2 + (ANGLE_FULL / 3) * cs, 12);
    ya = y + ypart(x2 + (ANGLE_FULL / 3) * cs, 12);
    circlefill(bmp, xa, ya, 2 + grand(2), TRANS_YELLOW);
    circlefill(bmp, xa, ya, 3 + grand(3), TRANS_LRED);
    draw_a_light(bmp, 8 + grand(3), xa, ya);
   } 
   x2 = ANGLE_FULL - x2;
   for (cs = 0; cs < 3; cs ++)
   {
    xa = x + xpart(x2 + (ANGLE_FULL / 3) * cs, 4);
    ya = y + ypart(x2 + (ANGLE_FULL / 3) * cs, 4);
    circlefill(bmp, xa, ya, 2 + grand(2), TRANS_YELLOW);
    circlefill(bmp, xa, ya, 3 + grand(3), TRANS_LGREEN);
   } 
   draw_a_light(bmp, 12 + grand(4), x, y);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case BULLET_TWISTY:
   x2 = (bullet[dr].special1 % 32) * 32;
   y2 = xpart(x2, 13);
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   xa = x + xpart(bullet[dr].angle + ANGLE_QUARTER, y2);
   ya = y + ypart(bullet[dr].angle + ANGLE_QUARTER, y2);
   circlefill(bmp, xa, ya, 3 + grand(2), bullet[dr].colours [0]);
   circlefill(bmp, xa, ya, 4 + grand(3), bullet[dr].colours [1]);
   draw_a_light(bmp, 8 + grand(3), xa, ya);
   xa = x + xpart(bullet[dr].angle - ANGLE_QUARTER, y2);
   ya = y + ypart(bullet[dr].angle - ANGLE_QUARTER, y2);
   circlefill(bmp, xa, ya, 3 + grand(2), bullet[dr].colours [0]);
   circlefill(bmp, xa, ya, 4 + grand(3), bullet[dr].colours [1]);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   draw_a_light(bmp, 8 + grand(3), xa, ya);
   break;
  case BULLET_OVERPULSE:
   x2 = (bullet[dr].timeout % 64) * 16;
   if (bullet[dr].special2 == 1)
    x2 = ANGLE_FULL - x2;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   for (cs = 0; cs < 4; cs ++)
   {
    xa = x + xpart(x2 + ANGLE_QUARTER * cs, 12 + xpart(x2, 5));
    ya = y + ypart(x2 + ANGLE_QUARTER * cs, 12 + xpart(x2, 5)); // yes, xpart
    circlefill(bmp, xa, ya, 6 + grand(2) + xpart(x2, 3), TRANS_YELLOW);
    circlefill(bmp, xa, ya, 7 + grand(4) + xpart(x2, 3), TRANS_LGREEN);
    draw_a_light(bmp, 8 + grand(3), xa, ya);
   }
   circlefill(bmp, x, y, 12 + grand(2) + xpart(x2, 3), TRANS_YELLOW);
   circlefill(bmp, x, y, 13 + grand(5) + xpart(x2, 3), TRANS_LGREEN);
    /*   for (cs = 0; cs < 3; cs ++)
   {
    xa = x + xpart(x2 + (ANGLE_FULL / 3) * cs, 12 + xpart((x2 * 2), 5));
    ya = y + ypart(x2 + (ANGLE_FULL / 3) * cs, 12 + xpart((x2 * 2), 5)); // yes, xpart
    circlefill(bmp, xa, ya, 2 + grand(2), TRANS_YELLOW);
    circlefill(bmp, xa, ya, 3 + grand(3), TRANS_LGREEN);
    draw_a_light(bmp, 8 + grand(3), xa, ya);
   } 
   y2 = ANGLE_FULL - x2;
   for (cs = 0; cs < 3; cs ++)
   {
    xa = x + xpart(y2 + (ANGLE_FULL / 3) * cs + ANGLE_FULL / 6, 12 - xpart((y2 * 2), 5));
    ya = y + ypart(y2 + (ANGLE_FULL / 3) * cs + ANGLE_FULL / 6, 12 - xpart((y2 * 2), 5)); // yes, xpart
    circlefill(bmp, xa, ya, 2 + grand(2), TRANS_YELLOW);
    circlefill(bmp, xa, ya, 3 + grand(3), TRANS_LGREEN);
    draw_a_light(bmp, 8 + grand(3), xa, ya);
   }*/
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
/*  case BULLET_GOLDEN_NEEDLE:
  case BULLET_GOLDEN_NEEDLE_SMALL:
   x = (bullet[dr].x) - (actor[player[play].actor_controlled].x) + (max_x / 2) * GRAIN;
   y = (bullet[dr].y) - (actor[player[play].actor_controlled].y) + (max_y / 2) * GRAIN;
   x2 = (bullet[dr].x2) - (actor[player[play].actor_controlled].x) + (max_x / 2) * GRAIN;
   y2 = (bullet[dr].y2) - (actor[player[play].actor_controlled].y) + (max_y / 2) * GRAIN;
   xa = x2 - x;
   ya = y2 - y;
   pline(bmp, x / GRAIN, y / GRAIN, (x + 3 * xa) / GRAIN, (y + 3 * ya) / GRAIN, bullet[dr].colours [0]);
   break;
  case BULLET_FROZEN_BREATH:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_3 + counter % 2], (x) - 3, (y) - 3);
   break;   
  case BULLET_SNOW_DART: 
  case BULLET_ICE_DART:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_3 + counter % 2], (x) - 2, (y) - 2);
   break;   */
  case BULLET_BURNING_SPIRIT:
   draw_a_light(bmp, 6 + grand(3), x - grand(2), y - grand(2));
  case BULLET_BURNING_DRAGGED: // fall-through
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_ORANGE_BLOB_5 + counter % 2], (x) - 3 + counter % 2, (y) - 3 + counter % 2);
   break;   
  case BULLET_BURST:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_ORANGE_BLOB_2 + counter % 4], (x) - 4 + counter % 2, (y) - 4 + counter % 2);
   draw_a_light(bmp, 6 + grand(3), x - grand(2), y - grand(2));
   break;   
  case BULLET_PUFFY3:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_GREEN_BLOB_2 + counter % 4], (x) - 4 + counter % 2, (y) - 4 + counter % 2);
   draw_a_light(bmp, 6 + grand(3), x - grand(2), y - grand(2));
   break;   
  case BULLET_PUFFY4:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_2 + counter % 4], (x) - 4 + counter % 2, (y) - 4 + counter % 2);
   draw_a_light(bmp, 6 + grand(3), x - grand(2), y - grand(2));
   break;   
  case BULLET_GREEN_BLAT:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_GREEN_BLOB_5 + bullet[dr].timeout % 2], (x) - 3 + bullet[dr].timeout % 2, (y) - 3 + bullet[dr].timeout % 2);
//   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_GREEN_BLOB_5 + counter % 2], (x) - 3, (y) - 3);
   break;   
  case BULLET_BLUE_BLAT:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_5 + bullet[dr].timeout % 2], (x) - 3 + bullet[dr].timeout % 2, (y) - 3 + bullet[dr].timeout % 2);
   break;   
  case BULLET_YELLOW_FLAK:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_YELLOW_BLOB_5 + bullet[dr].timeout % 2], (x) - 3 + bullet[dr].timeout % 2, (y) - 3 + bullet[dr].timeout % 2);
   break;   
/*  case BULLET_SNOW_DART_SMALL: 
  case BULLET_ICE_DART_SMALL:
   draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_5], (x) - 1, (y) - 1);
   break;   
  case BULLET_PARTICLE_SPITTER:
  case BULLET_FAR_SPITTER:
   cs = (bullet[dr].timeout / 2) % 6;
   if (cs > 3)
    cs = 6 - cs;
   putpixel(bmp, x, y, bullet[dr].colours [0] + cs);
   break; */
  case BULLET_FROZEN_STAR:
  case BULLET_EVIL_STAR:
//   cs = (bullet[dr].timeout / 3) % 4;
//   xa = bullet[dr].colours [1];
//   if (dr % 2 == 0)
//    cs = 4 - cs;
   circlefill(bmp, x, y, 1, bullet[dr].colours [1] - 4 + (bullet[dr].timeout / 5) % 4);
   putpixel(bmp, x, y, bullet[dr].colours [0] - 4 + (bullet[dr].timeout / 3) % 4);
//   putpixel(bmp, x - 1, y, xa);
//   putpixel(bmp, x + 1, y, xa);
   break;
//  case BULLET_NUMEROUS_BLADE:
//  case BULLET_FROZEN_STAR:
  case BULLET_SLIVER:
   cs = (bullet[dr].timeout / 3) % 4;
   xa = bullet[dr].colours [1] - 2;
   if (bullet[dr].special2 == 0) // terrible
    cs = 4 - cs;
   putpixel(bmp, x, y, bullet[dr].colours [1]);
   switch(cs)
   {
    case 0: 
     putpixel(bmp, x - 1, y, xa);
     putpixel(bmp, x + 1, y, xa);
     break;
    case 1: 
     putpixel(bmp, x - 1, y - 1, xa);
     putpixel(bmp, x + 1, y + 1, xa);
     break;
    case 2: 
     putpixel(bmp, x, y - 1, xa);
     putpixel(bmp, x, y + 1, xa);
     break;
    case 3: 
     putpixel(bmp, x + 1, y - 1, xa);
     putpixel(bmp, x - 1, y + 1, xa);
     break;
   }
   break; 
   case BULLET_WINGS1:
    circlefill(bmp, x, y, 2, GC_GREY3);
    circlefill(bmp, x, y, 1, GC_GREY5);
    putpixel(bmp, x, y, GC_GREY6);
    pline(bmp, x, y, 
     x + xpart(bullet[dr].angle + ANGLE_HALF, 8 - (bullet[dr].timeout / 4)),
     y + ypart(bullet[dr].angle + ANGLE_HALF, 8 - (bullet[dr].timeout / 4)),
     GC_GREY5);
    break; 
   case BULLET_WINGS2:
    circlefill(bmp, x, y, 2, GC_GREY3);
    circlefill(bmp, x, y, 1, GC_GREY5);
    putpixel(bmp, x, y, GC_GREY6);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle + (30 - bullet[dr].timeout) * 5, 7),
     y - ypart(bullet[dr].angle + (30 - bullet[dr].timeout) * 5, 7),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle - (30 - bullet[dr].timeout) * 5, 7),
     y - ypart(bullet[dr].angle - (30 - bullet[dr].timeout) * 5, 7),
     GC_GREY5);
    break; 
   case BULLET_WINGS3:
    circlefill(bmp, x, y, 2, GC_GREY3);
    circlefill(bmp, x, y, 1, GC_GREY5);
    putpixel(bmp, x, y, GC_GREY6);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle + 150, 7),
     y - ypart(bullet[dr].angle + 150, 7),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle - 150, 7),
     y - ypart(bullet[dr].angle - 150, 7),
     GC_GREY5);
    break;
   case BULLET_BIGWINGS1:
    circlefill(bmp, x, y, 4, GC_GREY2);
    circlefill(bmp, x, y, 3, GC_GREY5);
//    putpixel(bmp, x, y, GC_GREY6);
    pline(bmp, x, y, 
     x + xpart(bullet[dr].angle + ANGLE_HALF, 12 - (bullet[dr].timeout / 3)),
     y + ypart(bullet[dr].angle + ANGLE_HALF, 12 - (bullet[dr].timeout / 3)),
     GC_GREY5);
    break; 
   case BULLET_BIGWINGS2:
    circlefill(bmp, x, y, 4, GC_GREY2);
    circlefill(bmp, x, y, 3, GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle + (30 - bullet[dr].timeout) * 7, 11),
     y - ypart(bullet[dr].angle + (30 - bullet[dr].timeout) * 7, 11),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle - (30 - bullet[dr].timeout) * 7, 11),
     y - ypart(bullet[dr].angle - (30 - bullet[dr].timeout) * 7, 11),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle + (30 - bullet[dr].timeout) * 3, 11),
     y - ypart(bullet[dr].angle + (30 - bullet[dr].timeout) * 3, 11),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle - (30 - bullet[dr].timeout) * 3, 11),
     y - ypart(bullet[dr].angle - (30 - bullet[dr].timeout) * 3, 11),
     GC_GREY5);
    break; 
   case BULLET_BIGWINGS3:
    circlefill(bmp, x, y, 4, GC_GREY2);
    circlefill(bmp, x, y, 3, GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle + 210, 11),
     y - ypart(bullet[dr].angle + 210, 11),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle - 210, 11),
     y - ypart(bullet[dr].angle - 210, 11),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle + 90, 11),
     y - ypart(bullet[dr].angle + 90, 11),
     GC_GREY5);
    pline(bmp, x, y, 
     x - xpart(bullet[dr].angle - 90, 11),
     y - ypart(bullet[dr].angle - 90, 11),
     GC_GREY5);
    break;
//  case BULLET_POWERED:
//  circlefill(bmp, (x), y, 1, bullet[dr].colours [random() % 4]);
//  if (crandom(3) == 0)
//   circlefill(bmp, (x), y, 1, bullet[dr].colours [2]);
//    else
//     circlefill(bmp, (x), y, 1, bullet[dr].colours [0]);
//  circlefill(bmp, (x), y, 1 + random() % 2, bullet[dr].colours [0]);
//   break;
/*  case BULLET_PLASMA:
   switch(bullet[dr].colours [0])
   {
    case TRANS_DRED:
     xa = RLE_SMALL1_RED_BLOB_L;
     break;
    default:
    case TRANS_DBLUE:
     xa = RLE_SMALL1_BLUE_BLOB_L;
     break;
    case TRANS_DGREEN:
     xa = RLE_SMALL1_GREEN_BLOB_L;
     break;
    case TRANS_ORANGE:
     xa = RLE_SMALL1_ORANGE_BLOB_L;
     break;
    case TRANS_YELLOW:
     xa = RLE_SMALL1_YELLOW_BLOB_L;
     break;
   }
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [xa], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [xa + 1], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [xa + 2], (x) - 5, (y) - 5);
//  circlefill(bmp, (x), y, 1 + random() % 2, bullet[dr].colours [0]);
   break;*/
//  case BULLET_E_BOMB:
//   circlefill(bmp, x, y, 2, bullet[dr].colours [0] - (bullet[dr].timeout / 8) % 4);
//     break;
  case BULLET_BALL1: 
     if ((counter / 3) % 2 == 1)
      draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_ORANGE_BLOB_1], (x) - 4, (y) - 4);
       else
        draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_ORANGE_BLOB_2], (x) - 4, (y) - 4);
   break;   
  case BULLET_BALL2: 
     if ((counter / 3) % 2 == 1)
      draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_YELLOW_BLOB_1], (x) - 4, (y) - 4);
       else
        draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_YELLOW_BLOB_2], (x) - 4, (y) - 4);
   break;   
  case BULLET_PULSE1: 
   if ((bullet[dr].timeout / 8) % 2 == 1)
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_PULSE1 + (bullet[dr].timeout / 2) % 4], (x) - 6, (y) - 6);
     else
      draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_PULSE4 - (bullet[dr].timeout / 2) % 4], (x) - 6, (y) - 6);
   draw_a_light(bmp, 8 + grand(3), x - grand(2), y - grand(2));
   break;   
  case BULLET_PULSE2: 
   if ((bullet[dr].timeout / 8) % 2 == 1)
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_PULSE2_1 + (bullet[dr].timeout / 2) % 4], (x) - 6, (y) - 6);
     else
      draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_PULSE2_4 - (bullet[dr].timeout / 2) % 4], (x) - 6, (y) - 6);
   draw_a_light(bmp, 8 + grand(3), x - grand(2), y - grand(2));
   break;   
  case BULLET_BFLAK: 
/*     if ((counter / 3) % 2 == 1)
      draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_YELLOW_BLOB_1], (x) - 4, (y) - 4);
       else
        draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_YELLOW_BLOB_2], (x) - 4, (y) - 4);
     xa = grand(3);
     ya = 3 - xa;
     x2 = xpart(bullet[dr].timeout * 29, 9);
     y2 = ypart(bullet[dr].timeout * 29, 9);
     draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_YELLOW_BLOB_4 + xa], x - ya + x2, y - ya + y2);
     draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_YELLOW_BLOB_4 + xa], x - ya - x2, y - ya - y2);*/
     xa = bullet[dr].timeout * 18;
     x2 = 4 + grand(3);
     y2 = 6 + grand(4);
/*     ya = TRANS_WHITE;
     if (grand(3) == 0)
      ya = TRANS_LBLUE;
     if (grand(3) == 0)
      ya = TRANS_PURPLE;
     xb = TRANS_LBLUE;
     if (ya == TRANS_LBLUE || grand(3) == 0)
      xb = TRANS_DBLUE; 
     if (grand(5) == 0)
      xb = TRANS_DGREY; */
     ya = TRANS_WHITE;
     xb = TRANS_LBLUE; 
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     triangle(bmp, x + xpart(xa, x2), y + ypart(xa, x2),
                   x + xpart(xa + ANGLE_FULL / 3, x2), y + ypart(xa + ANGLE_FULL / 3, x2),
                   x + xpart(xa - ANGLE_FULL / 3, x2), y + ypart(xa - ANGLE_FULL / 3, x2),
                   ya);
     triangle(bmp, x + xpart(xa, y2), y + ypart(xa, y2),
                   x + xpart(xa + ANGLE_FULL / 3, y2), y + ypart(xa + ANGLE_FULL / 3, y2),
                   x + xpart(xa - ANGLE_FULL / 3, y2), y + ypart(xa - ANGLE_FULL / 3, y2),
                   xb);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;   
   case BULLET_ZIGZAG2:
    cs = bullet[dr].colours [0];
    if (bullet[dr].special2 <= 3)
     cs = bullet[dr].colours [1];
    if (bullet[dr].special2 <= 2)
     cs = bullet[dr].colours [2];
    if (bullet[dr].special2 <= 1)
     cs = bullet[dr].colours [3];
    int points2 [8];
    int c2 = bullet[dr].special4 + ANGLE_QUARTER;
    int xd = 4 + bullet[dr].special2 / 2;
    x2 = ((bullet[dr].x2) - (actor[player[play].actor_controlled].x)) / GRAIN + (max_x / 2);
    y2 = ((bullet[dr].y2) - (actor[player[play].actor_controlled].y)) / GRAIN + (max_y / 2);
    points2 [0] = x + xpart(c2, xd);
    points2 [1] = y + ypart(c2, xd);
    points2 [2] = x - xpart(c2, xd);    
    points2 [3] = y - ypart(c2, xd);    
    points2 [4] = x2 - xpart(c2, xd);    
    points2 [5] = y2 - ypart(c2, xd);    
    points2 [6] = x2 + xpart(c2, xd);    
    points2 [7] = y2 + ypart(c2, xd);    
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    polygon(bmp, 4, points2, cs);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    break; 
  case BULLET_MANIFOLD_ORB: 
  case BULLET_FURIOUS_ORB:
   circlefill(bmp, x, y, 3, GC_GREY2);
   circlefill(bmp, x, y, 2, GC_GREY6);
//  case BULLET_BOMB:
//     draw_sprite(bmp, small2_bmp [BMP_SMALL2_BOMB], (x) - 3, (y) - 3);
     break;
  case BULLET_MANIFOLD_ORB_SMALL: 
  case BULLET_FURIOUS_ORB_SMALL:
   circlefill(bmp, x, y, 2, GC_GREY2);
   circlefill(bmp, x, y, 1, GC_GREY6);
//   circlefill(bmp, x, y, 1, GC_GREY2);
//   putpixel(bmp, x, y, GC_GREY6);
   break;
  case BULLET_EYE_DESOLATION:
     switch((bullet[dr].angle % ANGLE_FULL) / ANGLE_QUARTER)
     { // divisor was 42
       case 0:
       xa = (bullet[dr].angle % ANGLE_FULL) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite(bmp, small4_bmp [BMP_SMALL4_EYE_7 - xa], (x) - 4, (y) - 5);
       break;
       case 1:
       xa = (bullet[dr].angle - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_v_flip(bmp, small4_bmp [BMP_SMALL4_EYE_1 + xa], (x) - 4, (y) - 5);
       break;
       case 2:
       xa = (bullet[dr].angle - ANGLE_HALF) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_vh_flip(bmp, small4_bmp [BMP_SMALL4_EYE_7 - xa], (x) - 4, (y) - 5);
       break;
       case 3:
       xa = (bullet[dr].angle - ANGLE_HALF - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_h_flip(bmp, small4_bmp [BMP_SMALL4_EYE_1 + xa], (x) - 4, (y) - 5);
       break;

     }
     break;
  case BULLET_BURNING_EYE:
     switch((bullet[dr].angle % ANGLE_FULL) / ANGLE_QUARTER)
     { // divisor was 42
       case 0:
       xa = (bullet[dr].angle % ANGLE_FULL) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite(bmp, small4_bmp [BMP_SMALL4_BEYE_7 - xa], (x) - 4, (y) - 5);
       break;
       case 1:
       xa = (bullet[dr].angle - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_v_flip(bmp, small4_bmp [BMP_SMALL4_BEYE_1 + xa], (x) - 4, (y) - 5);
       break;
       case 2:
       xa = (bullet[dr].angle - ANGLE_HALF) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_vh_flip(bmp, small4_bmp [BMP_SMALL4_BEYE_7 - xa], (x) - 4, (y) - 5);
       break;
       case 3:
       xa = (bullet[dr].angle - ANGLE_HALF - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_h_flip(bmp, small4_bmp [BMP_SMALL4_BEYE_1 + xa], (x) - 4, (y) - 5);
       break;
     }
     break;
/*  case BULLET_BOMBLET:
  case BULLET_BOMB_SIDE:
     draw_sprite(bmp, small2_bmp [BMP_SMALL2_SIDE_BOMB], (x) - 3, (y) - 3);
     break;
  case BULLET_FROZEN_TOOTH:
//     circlefill(bmp, x, y, 1, COLOUR_WHITE);  
     break;   */
/*  case BULLET_MISSILE:
     switch((bullet[dr].angle % ANGLE_FULL) / ANGLE_QUARTER)
     {
       case 0:
       xa = (bullet[dr].angle % ANGLE_FULL) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite(bmp, small2_bmp [BMP_SMALL2_MISSILE_7 - xa], (x) - 3, (y) - 3);
       break;
       case 1:
       xa = (bullet[dr].angle - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_v_flip(bmp, small2_bmp [BMP_SMALL2_MISSILE_1 + xa], (x) - 3, (y) - 3);
       break;
       case 2:
       xa = (bullet[dr].angle - ANGLE_HALF) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_vh_flip(bmp, small2_bmp [BMP_SMALL2_MISSILE_7 - xa], (x) - 3, (y) - 3);
       break;
       case 3:
       xa = (bullet[dr].angle - ANGLE_HALF - ANGLE_QUARTER) / 42;
       if (xa > 6) xa = 6;
       if (xa < 0) xa = 0;
       draw_sprite_h_flip(bmp, small2_bmp [BMP_SMALL2_MISSILE_1 + xa], (x) - 3, (y) - 3);
       break;

     }
//     circlefill(bmp, (x), y, 2, COLOUR_GREY6);
     break;
  case BULLET_PREMINE:
     draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PREMINE], (x) - 3, (y) - 3);
     break;
  case BULLET_MINE:
//     if (dr % 2 == 0) // as it's purely for display, this horrible thing is okay
//      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_MINE1 + (bullet[dr].timeout / 16) % 3], (x) - 5, (y) - 5);
//       else
        draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_MINE3 - (bullet[dr].timeout / (16)) % 3], (x) - 3, (y) - 3);
     break;
  case BULLET_PRESEEKMINE:
     draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PRESEEKMINE], (x) - 3, (y) - 3);
     break;
  case BULLET_SEEKMINE:
     draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_SEEKMINE3 - (bullet[dr].timeout / (12)) % 3], (x) - 3, (y) - 3);
     break;
  case BULLET_MISSILE_MINI:
     circlefill(bmp, (x), y, 1, COLOUR_GREY5);
     break;
  case BULLET_NICE_ORBITAL:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, bullet[dr].special1 / 2 + 1, bullet[dr].colours [0]);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 2 + grand(3) + bullet[dr].special1, x, y);
     break;*/
  case BULLET_TOXIC_SUN:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, 5 + grand(3), TRANS_YELLOW); //bullet[dr].colours [0]);
     circlefill(bmp, x, y, 6 + grand(3), TRANS_LGREEN); //bullet[dr].colours [1]);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 21 + grand(5), x, y);
     break;
  case BULLET_TOXIC_FLARE:
//     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
//     circlefill(bmp, x, y, 2 + grand(2), //bullet[dr].colours [0]);
//     circlefill(bmp, x, y, 3 + grand(2), //bullet[dr].colours [1]);
//     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 7 + grand(4), x, y);
     break;
//  case BULLET_PETAL1:
  //case BULLET_PETAL2:
    // draw_a_light(bmp, 11 + grand(4), x, y);
     break;
  case BULLET_BLAST:
  case BULLET_BOLT:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, 3 + grand(2), bullet[dr].colours [0]);
     circlefill(bmp, x, y, 4 + grand(2), bullet[dr].colours [1]);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 8 + grand(4), x, y);
     break;
  case BULLET_NOVA:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     if (bullet[dr].timeout > 140)
     {
      xa = bullet[dr].timeout - 139;
      if (xa > 15)
       xa = 14;
      circlefill(bmp, x, y, xa + grand(4), TRANS_WHITE);
     }
     if (bullet[dr].timeout > 100)
     {
      xa = bullet[dr].timeout - 99;
      if (xa > 18)
       xa = 18;
      circlefill(bmp, x, y, xa + grand(4), TRANS_YELLOW);
     }
     if (bullet[dr].timeout > 50 && bullet[dr].timeout < 140)
     {
      xa = bullet[dr].timeout - 49;
      if (xa > 20)
       xa = 20;
      circlefill(bmp, x, y, xa + grand(4), TRANS_ORANGE);
     }
     if (bullet[dr].timeout > 20 && bullet[dr].timeout < 70)
     {
      xa = bullet[dr].timeout - 20;
      if (xa > 20)
       xa = 20;
      circlefill(bmp, x, y, xa + grand(4), TRANS_LRED);
     }
     if (bullet[dr].timeout < 40)
     {
      xa = xpart(bullet[dr].timeout * 11 - ANGLE_QUARTER, 50);
//      if (xa > 20)
//       xa = 20;
      circlefill(bmp, x, y, xa + grand(4), TRANS_DRED);
     }
//     circlefill(bmp, x, y, bullet[dr].special1 + grand(3), bullet[dr].colours [0]);
//     circlefill(bmp, x, y, bullet[dr].special2 + grand(3), bullet[dr].colours [1]);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, bullet[dr].special1 + 6 + grand(4), x, y);
     break;
  case BULLET_HOLE:
      xa = x + grand(3) - grand(3);
      ya = y + grand(3) - grand(3);
      drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
      circlefill(bmp, xa, ya, 4 + grand(4), TRANS_YELLOW);
      circlefill(bmp, xa, ya, 4 + grand(4), TRANS_LRED);
      drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     circlefill(bmp, xa, ya, 5 + grand(4), GC_BLACK);
     draw_a_light(bmp, 8 + grand(4), xa, ya);
   break;   
  case BULLET_BIGBALL1:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, bullet[dr].special1 + grand(3), bullet[dr].colours [0]);
     circlefill(bmp, x, y, bullet[dr].special2 + grand(3), bullet[dr].colours [1]);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, bullet[dr].special1 + 6 + grand(4), x, y);
     break;
  case BULLET_SWIRL1:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, 4 + grand(3), bullet[dr].colours [0]);
     circlefill(bmp, x, y, 5 + grand(3), bullet[dr].colours [1]);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 11 + grand(4), x, y);
     break;
  case BULLET_BFLAK2:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, 2 + grand(3), TRANS_WHITE);
     circlefill(bmp, x, y, 3 + grand(3), TRANS_YELLOW);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 6 + grand(4), x, y);
     break;
  case BULLET_SPORE:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, 3 + pulsate(64, 2, bullet[dr].timeout), TRANS_YELLOW);
     circlefill(bmp, x, y, 4 - pulsate(64, 2, bullet[dr].timeout), TRANS_ORANGE);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 8 + pulsate(64, 2, bullet[dr].timeout), x, y);
     break;
  case BULLET_FLOWER:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, 3 + pulsate(64, 2, bullet[dr].timeout), TRANS_LGREEN);
     circlefill(bmp, x, y, 4 - pulsate(64, 2, bullet[dr].timeout), TRANS_DGREEN);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 8 + pulsate(64, 2, bullet[dr].timeout), x, y);
     break;
  case BULLET_FLOWER2:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circlefill(bmp, x, y, 3 + pulsate(64, 2, bullet[dr].timeout), TRANS_LRED);
     circlefill(bmp, x, y, 4 - pulsate(64, 2, bullet[dr].timeout), TRANS_ORANGE);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 8 + pulsate(64, 2, bullet[dr].timeout), x, y);
     break;
  case BULLET_BLUE_BLOB:
     circlefill(bmp, (x), y, 3, COLOUR_BLUE8);
     break;
//     circlefill(bmp, (x), y, 5, bullet[dr].colours [0]);
  case BULLET_FLAK:
  case BULLET_RED_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_RED_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_RED_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_RED_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_YELLOW_BLOB:
  case BULLET_TORPEDO:
//     circlefill(bmp, (x), y, 3, COLOUR_YELLOW8);
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_YELLOW_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_YELLOW_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_YELLOW_BLOB_L], (x) - 5, (y) - 5);
     break;
//  case BULLET_ORBIT:
//   circle(bmp, x,y,7, COLOUR_WHITE);
//   break;
  case BULLET_CURVE:
  case BULLET_PURPLE_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PURPLE_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PURPLE_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_PURPLE_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_ORANGE_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_ORANGE_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_ORANGE_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_ORANGE_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_SEEKER_BLOB3:
  case BULLET_SEEKER_BLOB:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_BLUE_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_BLUE_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_BLUE_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_SEEKER_BLOB2:
     if ((counter / 3) % 4 == 1)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_GREEN_BLOB_S], (x) - 5, (y) - 5);
     if ((counter / 3) % 2 == 0)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_GREEN_BLOB_M], (x) - 5, (y) - 5);
     if ((counter / 3) % 4 == 3)
      draw_rle_sprite(bmp, small1_rle [RLE_SMALL1_GREEN_BLOB_L], (x) - 5, (y) - 5);
     break;
  case BULLET_PRONG:
   cs = bullet[dr].angle;
   if (bullet[dr].special1 == 1)
    cs = ANGLE_FULL - bullet[dr].angle;
   xa = x + xpart(cs, 2); 
   ya = y + ypart(cs, 2); 
   xb = x - xpart(cs, 2); 
   yb = y - ypart(cs, 2); 
   pline(bmp, xa, ya, xb, yb, bullet[dr].colours [0]);
   break;
  case BULLET_CRYSTAL1:
  case BULLET_CRYSTAL2:
    clear_bitmap(crystal_bmp);
//    circlefill(crystal_bmp, 10 + grand(2) - grand(2), 10 + grand(2) - grand(2), 9 + grand(2) - grand(2), TRANS_LRED);
//    circlefill(crystal_bmp, 10, 10, 7 + grand(2) - grand(2), bullet[dr].colours [0 + grand(4)]);
    circlefill(crystal_bmp, 10, 10, 7 + grand(2) - grand(2), bullet[dr].colours [0 + abs(xpart(bullet[dr].timeout * 22, 4))]);
    circlefill(crystal_bmp, 10 + grand(2) - grand(2), 10 + grand(2) - grand(2), 3 + grand(2) - grand(2), 0);
    draw_trans_sprite(bmp, crystal_bmp, x - 10 + grand(2) - grand(2), y - 10 + grand(2) - grand(2));
    break; 
 }


}

/*
void draw_sidekicks(BITMAP *bmp, int max_x, int max_y, int play, int a)
{
// NOTE: not xpart/ypart optimised at all

 int i, x, y, x1, x2,x3, y1, y2, y3;
 float radangle;

 for (i = 0; i < actor[a].sidekicks; i ++)
 {
  if (actor[a].sidekick_x [i] + 2000 < actor[player[play].actor_controlled].x - (max_x / 2) * GRAIN
      || actor[a].sidekick_x [i] - 2000 > actor[player[play].actor_controlled].x + (max_x / 2) * GRAIN
      || actor[a].sidekick_y [i] + 2000 < actor[player[play].actor_controlled].y - (max_y / 2) * GRAIN
      || actor[a].sidekick_y [i] - 2000 > actor[player[play].actor_controlled].y + (max_y / 2) * GRAIN)
      continue;
      
  x = ((actor[a].sidekick_x [i] / GRAIN) - (actor[player[play].actor_controlled].x / GRAIN)) + max_x / 2;
  y = ((actor[a].sidekick_y [i] / GRAIN) - (actor[player[play].actor_controlled].y / GRAIN)) + max_y / 2;

//  circle(bmp, x + max_x / 2, y + max_y / 2, 3, COLOUR_GREEN7);
   radangle = angle_to_radians(actor[a].sidekick_angle [i]);
   x1 = (x) + ((float) cos(radangle + PI) * 3);
   y1 = (y) + ((float) sin(radangle + PI) * 3);
   x2 = (x) + ((float) cos(radangle + PI + (PI * 2) / 3) * 4);
   y2 = (y) + ((float) sin(radangle + PI + (PI * 2) / 3) * 4);
   x3 = (x) + ((float) cos(radangle + PI - (PI * 2) / 3) * 4);
   y3 = (y) + ((float) sin(radangle + PI - (PI * 2) / 3) * 4);
   triangle(bmp, x1, y1, x2, y2, x3, y3, COLOUR_GREY5);
   line(bmp, x1, y1, x2, y2, COLOUR_GREY4);
   line(bmp, x1, y1, x3, y3, COLOUR_GREY4);
   line(bmp, x2, y2, x3, y3, COLOUR_GREY4);

 }


}
*/

void draw_enemies(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 int e, et;

// we need to do this a few times so the bigger enemies are always drawn at the back.
//  also, draw_an_enemy_inter2 makes sure turrets are drawn along with their owner.
 for (e = 0; e < NO_ENEMIES; e ++)
 {
  et = enemy[e].type;
  if (et == ENEMY_NONE)
   continue;
  if (eclass[et].role == ROLE_BOSS)
   draw_an_enemy_inter(bmp, e, max_x, max_y, play, centre_x, centre_y);
 }

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  et = enemy[e].type;
  if (et == ENEMY_NONE)
   continue;
  if (eclass[et].role == ROLE_MINIBOSS)
   draw_an_enemy_inter(bmp, e, max_x, max_y, play, centre_x, centre_y);
 }

 for (e = 0; e < NO_ENEMIES; e ++)
 {
  et = enemy[e].type;
  if (et == ENEMY_NONE)
   continue;
  if (eclass[et].role != ROLE_TURRET
      && eclass[et].role != ROLE_BOSS
      && eclass[et].role != ROLE_MINIBOSS)
   draw_an_enemy_inter(bmp, e, max_x, max_y, play, centre_x, centre_y);
 }

}


void draw_an_enemy_inter(BITMAP *bmp, int e, int max_x, int max_y, int play, int centre_x, int centre_y)
{

  int i;
  
  draw_an_enemy_inter2(bmp, e, max_x, max_y, play, centre_x, centre_y);
  
  if (eclass[enemy[e].type].turrets > 0)
  {
   for (i = 0; i < MAX_TURRETS; i ++)
   {
    if (enemy[e].turret [i] != -1)
     draw_an_enemy_inter2(bmp, enemy[e].turret [i], max_x, max_y, play, centre_x, centre_y);    
   }
  }

}

void draw_an_enemy_inter2(BITMAP *bmp, int e, int max_x, int max_y, int play, int centre_x, int centre_y)
{

  if (enemy[e].x + enemy[e].edge_radius + 9000 < centre_x - (max_x / 2) * GRAIN
      || enemy[e].x - enemy[e].edge_radius - 9000 > centre_x + (max_x / 2) * GRAIN
      || enemy[e].y + enemy[e].edge_radius + 9000 < centre_y - (max_y / 2) * GRAIN
      || enemy[e].y - enemy[e].edge_radius - 9000 > centre_y + (max_y / 2) * GRAIN)
      return;
   
      
  int x = ((enemy[e].x / GRAIN) - (centre_x / GRAIN));// / GRAIN;
  int y = ((enemy[e].y / GRAIN) - (centre_y / GRAIN));// / GRAIN;

  draw_an_enemy(bmp, e, x + max_x / 2, y + max_y / 2);

}

// assumes that distortion_mask bitmap set up
// x,y is the centre of the mask, wx,wy is width, dx,dy is degree of distortion
void distortion(BITMAP *bmp, int x, int y, int wx, int wy, int dx, int dy)
{
 
 int i, j;
 
 int xp = 1, yp = 1;
 
 if (dx < 0)
 {
  xp = -1;
  wx *= -1;
 }
 if (dy < 0)
 {
  yp = -1; 
  wy *= -1;
 }
 
 for (i = wx * -1; i != wx; i += xp)
 {
  for (j = wy * -1; j != wy; j += yp)
  {
   if (x + i + dx >= bmp->w
      || x + i + dx < 0
      || y + j + dy >= bmp->h 
      || y + j + dy < 0)
      {
       putpixel(bmp, x + i, y + j, 0);
       continue;
      }
   if (getpixel(distortion_mask, 100 + i, 100 + j) != 0)
    putpixel(bmp, x + i, y + j, getpixel(bmp, x + i + dx, y + j + dy));
  } 
 }

}

/*

void draw_lock(BITMAP *bmp, int dr, int x, int y, int rad)
{
// if (enemy[dr].type == ENEMY_NONE)
//  return;
 int lock_colour = COLOUR_YELLOW8 - (counter / 2) % 4;

 int angle = (counter * 8) % 256;
 arc(bmp, x, y, itofix(angle), itofix(angle + 64), rad, lock_colour);
 arc(bmp, x, y, itofix(angle + 128), itofix(angle + 192), rad, lock_colour);
 int fangle = ANGLE_FULL - (( - counter) * 8) % ANGLE_FULL;
 int x1 = x + xpart(fangle, rad + 1);
 int x2 = x + xpart(fangle, rad + 7);
 int y1 = y + ypart(fangle, rad + 1);
 int y2 = y + ypart(fangle, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + xpart(fangle + ANGLE_QUARTER, rad + 1);
 x2 = x + xpart(fangle + ANGLE_QUARTER, rad + 7);
 y1 = y + ypart(fangle + ANGLE_QUARTER, rad + 1);
 y2 = y + ypart(fangle + ANGLE_QUARTER, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + xpart(fangle + ANGLE_HALF, rad + 1);
 x2 = x + xpart(fangle + ANGLE_HALF, rad + 7);
 y1 = y + ypart(fangle + ANGLE_HALF, rad + 1);
 y2 = y + ypart(fangle + ANGLE_HALF, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + xpart(fangle - ANGLE_QUARTER, rad + 1);
 x2 = x + xpart(fangle - ANGLE_QUARTER, rad + 7);
 y1 = y + ypart(fangle - ANGLE_QUARTER, rad + 1);
 y2 = y + ypart(fangle - ANGLE_QUARTER, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + xpart(fangle + ANGLE_1_EIGHTH, rad + 1);
 x2 = x + xpart(fangle + ANGLE_1_EIGHTH, rad + 7);
 y1 = y + ypart(fangle + ANGLE_1_EIGHTH, rad + 1);
 y2 = y + ypart(fangle + ANGLE_1_EIGHTH, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + xpart(fangle - ANGLE_1_EIGHTH, rad + 1);
 x2 = x + xpart(fangle - ANGLE_1_EIGHTH, rad + 7);
 y1 = y + ypart(fangle - ANGLE_1_EIGHTH, rad + 1);
 y2 = y + ypart(fangle - ANGLE_1_EIGHTH, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + xpart(fangle + ANGLE_3_EIGHTHS, rad + 1);
 x2 = x + xpart(fangle + ANGLE_3_EIGHTHS, rad + 7);
 y1 = y + ypart(fangle + ANGLE_3_EIGHTHS, rad + 1);
 y2 = y + ypart(fangle + ANGLE_3_EIGHTHS, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
 x1 = x + xpart(fangle - ANGLE_3_EIGHTHS, rad + 1);
 x2 = x + xpart(fangle - ANGLE_3_EIGHTHS, rad + 7);
 y1 = y + ypart(fangle - ANGLE_3_EIGHTHS, rad + 1);
 y2 = y + ypart(fangle - ANGLE_3_EIGHTHS, rad + 7);
 pline(bmp, x1, y1, x2, y2, lock_colour);
}


void draw_turret_lock(BITMAP *bmp, int dr, int x, int y, int rad)
{
// float fangle = angle_to_radians(ANGLE_FULL - (( - counter) * 8) % ANGLE_FULL);
 int col = COLOUR_YELLOW8 - (counter / 2) % 4;
 float fangle = angle_to_radians((counter * 6) % ANGLE_FULL);
 int x1 = x + cos(fangle + PI / 8) * (rad + 1);
 int y1 = y + sin(fangle + PI / 8) * (rad + 1);
 int x2 = x + cos(fangle - PI / 8) * (rad + 1);
 int y2 = y + sin(fangle - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);
 x1 = x + cos(fangle + PI / 2 + PI / 8) * (rad + 1);
 y1 = y + sin(fangle + PI / 2 + PI / 8) * (rad + 1);
 x2 = x + cos(fangle + PI / 2 - PI / 8) * (rad + 1);
 y2 = y + sin(fangle + PI / 2 - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);
 x1 = x + cos(fangle + PI + PI / 8) * (rad + 1);
 y1 = y + sin(fangle + PI + PI / 8) * (rad + 1);
 x2 = x + cos(fangle + PI - PI / 8) * (rad + 1);
 y2 = y + sin(fangle + PI - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);
 x1 = x + cos(fangle - PI / 2 + PI / 8) * (rad + 1);
 y1 = y + sin(fangle - PI / 2 + PI / 8) * (rad + 1);
 x2 = x + cos(fangle - PI / 2 - PI / 8) * (rad + 1);
 y2 = y + sin(fangle - PI / 2 - PI / 8) * (rad + 1);
 pline(bmp, x1, y1, x2, y2, col);

}

*/

void draw_an_enemy(BITMAP *bmp, int dr, int x, int y)
{
// x += 500;
// y += 500;

 int xa;//, ya;
 int col1, col2, col3, col4;

 int x1,y1,x2,y2,x3,y3;

// float radangle;

 col1 = enemy[dr].colours [0];
 col2 = enemy[dr].colours [1];
 if (enemy[dr].hurt_pulse > 0)
 {
//  col1 = enemy[dr].hurt_pulse_colour2 + enemy[dr].hurt_pulse;
//  col2 = enemy[dr].hurt_pulse_colour1 + enemy[dr].hurt_pulse / 2;
  switch(enemy[dr].hurt_pulse / 2)
  {
   default:
   case 3:
    col1 = COLOUR_WHITE;
    col2 = COLOUR_WHITE;
    break;
   case 2:
    col1 += 2;
    col2 += 2;
    break;
    case 1:
    case 0:
     col1 += 1;
     col2 += 1;
     break;
  }
 }
/*#ifdef DEBUG_ENEMY_SIZE
 int store_x = x;
 int store_y = y;
#endif*/

#ifdef DEBUG_DISPLAY
 int store_x = x;
 int store_y = y;
#endif


 switch(enemy[dr].type)
 {
  default:
   textprintf_ex(bmp, font, x,y, COLOUR_WHITE, -1, "ERROR: %i %i", dr, enemy[dr].type);
   break;
  case ENEMY_GUARDIAN1:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_GUARDIAN1], (x) - 29, (y) - 24);
   circlefill(bmp, (x), y, 19, col1);
   circle(bmp, (x), y, 19, col2);
   x = x + xpart(enemy[dr].angle, 12);
   y = y + ypart(enemy[dr].angle, 12);
//   col1 = GC_GREEN6;
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_GUARDIAN2:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_GUARDIAN2], (x) - 25, (y) - 34);
   circlefill(bmp, (x), y, 19, col1);
   circle(bmp, (x), y, 19, col2);
   x = x + xpart(enemy[dr].angle, 12);
   y = y + ypart(enemy[dr].angle, 12);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
//   textprintf_ex(bmp, small_font, x,y, COLOUR_WHITE, -1, "%i %i", enemy[dr].angle, xpart(enemy[dr].angle, 12));
   break;
  case ENEMY_GUARDIAN3:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_GUARDIAN3], (x) - 29, (y) - 24);
   circlefill(bmp, (x), y, 19, col1);
   circle(bmp, (x), y, 19, col2);
   x = x + xpart(enemy[dr].angle, 12);
   y = y + ypart(enemy[dr].angle, 12);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
//   textprintf_ex(bmp, small_font, x,y, COLOUR_WHITE, -1, "%i %i", enemy[dr].angle, xpart(enemy[dr].angle, 12));
   break;
  case ENEMY_GUARDIAN4:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_GUARDIAN4], (x) - 34, (y) - 32);
   circlefill(bmp, (x), y, 19, col1);
   circle(bmp, (x), y, 19, col2);
   x = x + xpart(enemy[dr].angle, 12);
   y = y + ypart(enemy[dr].angle, 12);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
//   textprintf_ex(bmp, small_font, x,y, COLOUR_WHITE, -1, "%i %i", enemy[dr].angle, xpart(enemy[dr].angle, 12));
   break;
  case ENEMY_GUARDIAN5:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_GUARDIAN5], (x) - 34, (y) - 29);
   circlefill(bmp, (x), y, 19, col1);
   circle(bmp, (x), y, 19, col2);
   x = x + xpart(enemy[dr].angle, 12);
   y = y + ypart(enemy[dr].angle, 12);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
//   textprintf_ex(bmp, small_font, x,y, COLOUR_WHITE, -1, "%i %i", enemy[dr].angle, xpart(enemy[dr].angle, 12));
   break;
  case ENEMY_MULTI1:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_MULTI1], (x) - 34, (y) - 34);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x1 = x + xpart(enemy[dr].angle, 8);
   y1 = y + ypart(enemy[dr].angle, 8);
   circlefill(bmp, x1, y1, 5, col1);
   circle(bmp, x1, y1, 5, col2);
/*   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH, 10);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH, 10);
   circlefill(bmp, x1, y1, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH, 10);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH, 10);
   circlefill(bmp, x1, y1, 2, col1);
   circle(bmp, x1, y1, 2, col2);*/
   break;
  case ENEMY_MULTI2:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_MULTI2], (x) - 33, (y) - 33);
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   x1 = x + xpart(enemy[dr].angle, 8);
   y1 = y + ypart(enemy[dr].angle, 8);
   circlefill(bmp, x1, y1, 4, col1);
   circle(bmp, x1, y1, 4, col2);
   break;
  case ENEMY_MULTI3:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_MULTI3], (x) - 32, (y) - 32);
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   x1 = x + xpart(enemy[dr].angle, 10);
   y1 = y + ypart(enemy[dr].angle, 10);
   circlefill(bmp, x1, y1, 4, col1);
   circle(bmp, x1, y1, 4, col2);
   break;
  case ENEMY_PULSER1:
   x1 = (enemy[dr].attribute [0] / 4) % 8;
   if (x1 >= 4)
    x1 = 8 - x1;
   draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER1_V], (x) - 11, (y) - 35 - x1);
   draw_sprite_v_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER1_V], (x) - 11, (y) + 8 + x1);
   draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER1_H], (x) + 8 + x1, (y) - 11);
   draw_sprite_h_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER1_H], (x) - 35 - x1, (y) - 11);
   circlefill(bmp, (x), y, 17, col1);
   circle(bmp, (x), y, 17, col2);
//   x = x + xpart(enemy[dr].angle, 12);
//   y = y + ypart(enemy[dr].angle, 12);
//   col1 = GC_GREEN6;
//   circlefill(bmp, (x), y, 3, col1);
//   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_PULSER2:
   x1 = (enemy[dr].attribute [0] / 4) % 8;
   if (x1 >= 4)
    x1 = 8 - x1;
   draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER2_V], (x) - 12, (y) - 35 - x1);
   draw_sprite_v_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER2_V], (x) - 12, (y) + 8 + x1);
   draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER2_H], (x) + 8 + x1, (y) - 12);
   draw_sprite_h_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER2_H], (x) - 35 - x1, (y) - 12);
   circlefill(bmp, (x), y, 17, col1);
   circle(bmp, (x), y, 17, col2);
//   x = x + xpart(enemy[dr].angle, 12);
//   y = y + ypart(enemy[dr].angle, 12);
//   col1 = GC_GREEN6;
//   circlefill(bmp, (x), y, 3, col1);
//   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_FORKER1:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_FORKER1], (x) - 26, (y) - 26);
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   x1 = x + xpart(enemy[dr].angle, 7);
   y1 = y + ypart(enemy[dr].angle, 7);
   x2 = x + xpart(enemy[dr].angle, 12);
   y2 = y + ypart(enemy[dr].angle, 12);
//   col1 = GC_GREEN6;
   pline(bmp, x1, y1, x2, y2, col2);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_FORKER2:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_FORKER2], (x) - 26, (y) - 26);
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   x1 = x + xpart(enemy[dr].angle, 7);
   y1 = y + ypart(enemy[dr].angle, 7);
   x2 = x + xpart(enemy[dr].angle, 12);
   y2 = y + ypart(enemy[dr].angle, 12);
//   col1 = GC_GREEN6;
   pline(bmp, x1, y1, x2, y2, col2);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_MINER1:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_MINER1], (x) - 19, (y) - 18);
   circlefill(bmp, (x), y, 14, col1);
   circle(bmp, (x), y, 14, col2);
   break;
  case ENEMY_MINER2:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_MINER2], (x) - 24, (y) - 24);
   circlefill(bmp, (x), y, 14, col1);
   circle(bmp, (x), y, 14, col2);
   break;
  case ENEMY_MINER3:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_MINER3], (x) - 24, (y) - 42);
   circlefill(bmp, (x), y, 14, col1);
   circle(bmp, (x), y, 14, col2);
   break;
  case ENEMY_MINER3_TURRET:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   xa = 7;
   if (enemy[dr].attribute [0] > 0)
    xa = 7 - enemy[dr].attribute [0];
   if (enemy[dr].attribute [0] >= 6)
    xa = 6; 
   x1 = x + xpart(enemy[dr].angle, xa);
   y1 = y + ypart(enemy[dr].angle, xa);
   circlefill(bmp, (x1), y1, 2, col1);
   circle(bmp, (x1), y1, 2, col2);
   break;
  case ENEMY_CIRCLER1:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_CIRCLER1], (x) - 34, (y) - 24);
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   x1 = ((enemy[dr].counter + 224) / 16) % 9 + 1;
   x1 *= 8;
   rectfill(bmp, x - 32, y - 9, x - 26, y + 7, x1);
   rectfill(bmp, x + 26, y - 9, x + 32, y + 7, x1);
   x1 = ((enemy[dr].counter + 240) / 16) % 9 + 1;
   x1 *= 8;
   rectfill(bmp, x - 24, y - 9, x - 18, y + 7, x1);
   rectfill(bmp, x + 18, y - 9, x + 24, y + 7, x1);
   x1 = ((enemy[dr].counter + 256) / 16) % 9 + 1;
   x1 *= 8;
   rectfill(bmp, x - 16, y - 9, x - 10, y + 7, x1);
   rectfill(bmp, x + 10, y - 9, x + 16, y + 7, x1);
//   60, 15
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   if (enemy[dr].attribute [0] > 0)
   {
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    x1 = 50 - enemy[dr].attribute [0];
    if (x1 < 24)
     circle(bmp, x, y, x1 * 2, get_circler_colour(x1 / 6));
    if (x1 < 32 && x1 > 12)
     circle(bmp, x, y, (x1 - 12) * 2, get_circler_colour((x1 - 12) / 6));
    if (x1 < 48 && x1 > 24)
     circle(bmp, x, y, (x1 - 24) * 2, get_circler_colour((x1 - 24) / 6));
/*
    x1 = 66 - enemy[dr].attribute [0];
    if (x1 < 32)
     circle(bmp, x, y, x1, get_circler_colour(x1 / 8));
    if (x1 < 48 && x1 > 16)
     circle(bmp, x, y, (x1 - 16), get_circler_colour((x1 - 16) / 8));
    if (x1 < 64 && x1 > 32)
     circle(bmp, x, y, (x1 - 32), get_circler_colour((x1 - 32) / 8));
*/
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   }
   break;
  case ENEMY_PUFFER1:
  draw_puffer(bmp, x, y, enemy[dr].attribute [2], 3, 16, enemy[dr].attribute [0] / 105 + 6, GC_GREEN5, GC_GREEN2);
  
/*   xa = enemy[dr].attribute [0] / 105 + 6;
   x1 = x + xpart((enemy[dr].attribute [2] * 8) % 1024, 16);
   y1 = y + ypart((enemy[dr].attribute [2] * 8) % 1024, 16);
   circlefill(bmp, (x1), y1, xa, GC_GREEN5);
   circle(bmp, (x1), y1, xa, GC_GREEN2);
   x1 = x + xpart(((enemy[dr].attribute [2] + 85) * 8) % 1024, 16);
   y1 = y + ypart(((enemy[dr].attribute [2] + 85) * 8) % 1024, 16);
   circlefill(bmp, (x1), y1, xa, GC_GREEN5);
   circle(bmp, (x1), y1, xa, GC_GREEN2);
   x1 = x + xpart(((enemy[dr].attribute [2] - 85) * 8) % 1024, 16);
   y1 = y + ypart(((enemy[dr].attribute [2] - 85) * 8) % 1024, 16);
   circlefill(bmp, (x1), y1, xa, GC_GREEN5);
   circle(bmp, (x1), y1, xa, GC_GREEN2);*/
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x = x + xpart(enemy[dr].angle, 10);
   y = y + ypart(enemy[dr].angle, 10);
   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_PUFFER2:
  draw_puffer(bmp, x, y, enemy[dr].attribute [2], 4, 18, enemy[dr].attribute [0] / 105 + 6, GC_RED5, GC_RED2);
/*   xa = enemy[dr].attribute [0] / 105 + 6;
   x1 = x + xpart((enemy[dr].attribute [2] * 8) % 1024, 18);
   y1 = y + ypart((enemy[dr].attribute [2] * 8) % 1024, 18);
   circlefill(bmp, (x1), y1, xa, GC_RED5);
   circle(bmp, (x1), y1, xa, GC_RED2);
   x1 = x + xpart(((enemy[dr].attribute [2] + 32) * 8) % 1024, 18);
   y1 = y + ypart(((enemy[dr].attribute [2] + 32) * 8) % 1024, 18);
   circlefill(bmp, (x1), y1, xa, GC_RED5);
   circle(bmp, (x1), y1, xa, GC_RED2);
   x1 = x + xpart(((enemy[dr].attribute [2] - 32) * 8) % 1024, 18);
   y1 = y + ypart(((enemy[dr].attribute [2] - 32) * 8) % 1024, 18);
   circlefill(bmp, (x1), y1, xa, GC_RED5);
   circle(bmp, (x1), y1, xa, GC_RED2);
   x1 = x + xpart(((enemy[dr].attribute [2] + 64) * 8) % 1024, 18);
   y1 = y + ypart(((enemy[dr].attribute [2] + 64) * 8) % 1024, 18);
   circlefill(bmp, (x1), y1, xa, GC_RED5);
   circle(bmp, (x1), y1, xa, GC_RED2);*/
   circlefill(bmp, (x), y, 18, col1);
   circle(bmp, (x), y, 18, col2);
   x = x + xpart(enemy[dr].angle, 13);
   y = y + ypart(enemy[dr].angle, 13);
   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_PUFFER3:
   draw_puffer(bmp, x, y, enemy[dr].attribute [2], 3, 16, enemy[dr].attribute [0] / 105 + 6, GC_YELLOW5, GC_YELLOW2);
   circlefill(bmp, (x), y, 14, col1);
   circle(bmp, (x), y, 14, col2);
   x = x + xpart(enemy[dr].angle, 11);
   y = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_PUFFER4:
   draw_puffer(bmp, x, y, enemy[dr].attribute [2], 5, 23, enemy[dr].attribute [0] / 105 + 7, GC_GREEN5, GC_GREEN2);
   circlefill(bmp, (x), y, 21, col1);
   circle(bmp, (x), y, 21, col2);
   x = x + xpart(enemy[dr].angle, 16);
   y = y + ypart(enemy[dr].angle, 16);
   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;

  case ENEMY_BLATTER1:
   draw_blatter(bmp, x, y, 5, 19, enemy[dr].attribute [0], 9, GC_GREEN2, GC_GREEN5);
   circlefill(bmp, (x), y, 16, col1);
   circle(bmp, (x), y, 16, col2);
   x = x + xpart(enemy[dr].angle, 12);
   y = y + ypart(enemy[dr].angle, 12);
   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_BLATTER2:
   draw_blatter(bmp, x, y, 3, 16, enemy[dr].attribute [0], 12, GC_ORANGE2, GC_ORANGE5);
   circlefill(bmp, (x), y, 16, col1);
   circle(bmp, (x), y, 16, col2);
   x = x + xpart(enemy[dr].angle, 14);
   y = y + ypart(enemy[dr].angle, 14);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BLATTER3:
   draw_blatter(bmp, x, y, 4, 16, enemy[dr].attribute [0], 9, GC_YELLOW2, GC_YELLOW5);
   circlefill(bmp, x, y, 16, col1);
   circle(bmp, x, y, 16, col2);
   x1 = x + xpart(enemy[dr].angle, 14);
   y1 = y + ypart(enemy[dr].angle, 14);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_QUARTER, 14);
   y1 = y + ypart(enemy[dr].angle - ANGLE_QUARTER, 14);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH, 14);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH, 14);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH, 14);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH, 14);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_BLATTER4:
   draw_blatter(bmp, x, y, 5, 19, enemy[dr].attribute [0], 9, GC_BLUE2, GC_BLUE6);
   circlefill(bmp, (x), y, 16, col1);
   circle(bmp, (x), y, 16, col2);
//   x1 = x + xpart(enemy[dr].angle, 12);
//   y1 = y + ypart(enemy[dr].angle, 12);
//   x2 = x + xpart(enemy[dr].angle, 16);
//   y2 = y + ypart(enemy[dr].angle, 16);
//   col1 = GC_GREEN6;
//   pline(bmp, x1, y1, x2, y2, col2);
   x1 = x + xpart(enemy[dr].angle, 11);
   y1 = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_BLATTER5:
   draw_blatter(bmp, x, y, 4, 12, enemy[dr].attribute [0], 9, GC_RED2, GC_RED5);
   circlefill(bmp, (x), y, 11, col1);
   circle(bmp, (x), y, 11, col2);
   x = x + xpart(enemy[dr].angle, 11);
   y = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BLOATER1:
//   draw_blatter(bmp, x, y, 3, 15 + pulsate(16, 6, enemy[dr].counter), enemy[dr].attribute [0], 10 + pulsate(32, 5, enemy[dr].counter), GC_GREEN2, GC_GREEN5);
   draw_blatter(bmp, x, y, 3, 15 + pulsate(32, 6, enemy[dr].counter), enemy[dr].attribute [0], 10 + pulsate(16, 3, enemy[dr].counter), GC_GREEN2, GC_GREEN5);
   x1 = 17 - pulsate(32, 3, enemy[dr].counter);  
   circlefill(bmp, (x), y, x1, col1);
   circle(bmp, (x), y, x1, col2);
   break;
  case ENEMY_BLOATER2:
   draw_blatter(bmp, x, y, 5, 15 + pulsate(32, 6, enemy[dr].counter), enemy[dr].attribute [0], 8 + pulsate(16, 3, enemy[dr].counter), GC_ORANGE2, GC_ORANGE5);
   x1 = 17 - pulsate(32, 3, enemy[dr].counter);   
   circlefill(bmp, (x), y, x1, col1);
   circle(bmp, (x), y, x1, col2);
   break;
  case ENEMY_MINEFIELDER1:
   x1 = pulsate(32, 4, enemy[dr].counter);
   y1 = y - 5 - x1;
   y2 = y - 25 - x1;
   rectfill(bmp, x - 20, y1, x + 20, y2, GC_RED5);
   rect(bmp, x - 20, y1, x + 20, y2, GC_RED2);
   y1 = y + 5 + x1;
   y2 = y + 25 + x1;
   rectfill(bmp, x - 20, y1, x + 20, y2, GC_RED5);
   rect(bmp, x - 20, y1, x + 20, y2, GC_RED2);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_SPINNER1:
   draw_spinner(bmp, x, y, enemy[dr].attribute [0], 32, 10, ANGLE_1_32, 1024 / 3, 3, GC_YELLOW2, GC_YELLOW6);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   break;
  case ENEMY_SPINNER2:
   draw_spinner(bmp, x, y, enemy[dr].attribute [0], 38, 10, ANGLE_1_SIXTEENTH, 1024 / 3, 3, GC_ORANGE2, GC_ORANGE7);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   break;
  case ENEMY_SPINNER3:
   draw_spinner(bmp, x, y, enemy[dr].attribute [0], 42, 10, ANGLE_1_SIXTEENTH, 1024 / 4, 4, GC_BLUE2, GC_BLUE8);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_SPINNER4:
   draw_spinner(bmp, x, y, enemy[dr].attribute [0], 42, 10, ANGLE_1_32, 1024 / 3, 3, GC_YELLOW2, GC_YELLOW7);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   break;
  case ENEMY_SPINNER5:
   draw_spinner(bmp, x, y, enemy[dr].attribute [0], 45, 5, ANGLE_1_32, 1024 / 4, 4, GC_BLUE2, GC_BLUE8);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   break;
  case ENEMY_OVERSPINNER:
   draw_overspinner(bmp, x, y, enemy[dr].attribute [2], 70, 10, ANGLE_1_32, 1024 / 6, 6, GC_BLUE2, GC_BLUE6);
   draw_overspinner(bmp, x, y, enemy[dr].attribute [0], 110, 10, 48, 1024 / 5, 5, GC_ORANGE2, GC_ORANGE6);
   circlefill(bmp, (x), y, 30, col1);
   circle(bmp, (x), y, 30, col2);
   break;
  case ENEMY_OVERSPIKEY:
   draw_spikey(bmp, x, y, enemy[dr].attribute [2], 70, 20, ANGLE_QUARTER, 1024 / 6, 6, GC_RED2, GC_RED6, -2, -3);
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 110, 20, ANGLE_QUARTER, 1024 / 5, 5, GC_BLUE2, GC_BLUE6, -2, -3);
   circlefill(bmp, (x), y, 30, col1);
   circle(bmp, (x), y, 30, col2);
   break;
  case ENEMY_UNDERSPIKEY:
   draw_spikey(bmp, x, y, enemy[dr].attribute [2], 70, 25, ANGLE_QUARTER, 1024 / 6, 6, GC_YELLOW2, GC_YELLOW6, -2, -3);
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 110, 15, ANGLE_QUARTER, 1024 / 5, 5, GC_GREEN2, GC_GREEN6, -2, -3);
   circlefill(bmp, (x), y, 26, col1);
   circle(bmp, (x), y, 26, col2);
   x = x + xpart(enemy[dr].angle, 23);
   y = y + ypart(enemy[dr].angle, 23);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_UNDERSPIKEY2:
   draw_spikey(bmp, x, y, enemy[dr].attribute [2], 80, 25, ANGLE_QUARTER, 1024 / 3, 3, GC_ORANGE2, GC_ORANGE6, -2, -3);
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 100, 25, ANGLE_QUARTER, 1024 / 4, 4, GC_RED2, GC_RED6, -2, -3);
   circlefill(bmp, (x), y, 26, col1);
   circle(bmp, (x), y, 26, col2);
   x = x + xpart(enemy[dr].angle, 21);
   y = y + ypart(enemy[dr].angle, 21);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_UNDERSPIKEY3:
   draw_overspinner(bmp, x, y, enemy[dr].attribute [2], 70, 5, ANGLE_1_SIXTEENTH, 1024 / 4, 4, GC_RED2, GC_RED6);
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 90, 26, ANGLE_QUARTER, 1024 / 4, 4, GC_YELLOW2, GC_YELLOW6, -2, -3);
   circlefill(bmp, (x), y, 26, col1);
   circle(bmp, (x), y, 26, col2);
   x = x + xpart(enemy[dr].angle, 21);
   y = y + ypart(enemy[dr].angle, 21);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_OVERSPIKEY2:
   draw_overspinner(bmp, x, y, enemy[dr].attribute [2], 80, 5, ANGLE_1_32 + 4, 1024 / 6, 6, GC_BLUE2, GC_BLUE6);
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 110, 26, ANGLE_1_EIGHTH + ANGLE_1_32, 1024 / 5, 5, GC_RED2, GC_RED6, -2, -3);
   circlefill(bmp, (x), y, 27, col1);
   circle(bmp, (x), y, 27, col2);
   x = x + xpart(enemy[dr].angle, 22);
   y = y + ypart(enemy[dr].angle, 22);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_OVERBLATTER:
   draw_blatter(bmp, x, y, 4, 50, enemy[dr].attribute [0], 13, GC_RED2, GC_RED5);
//   draw_overspinner(bmp, x, y, ANGLE_FULL - enemy[dr].angle, 52, 10, ANGLE_1_32, 1024 / 6, 4, GC_RED2, GC_RED6);
   draw_blatter(bmp, x, y, 5, 50, enemy[dr].attribute [2], 15, GC_YELLOW2, GC_YELLOW6);
  case ENEMY_OVERBLATTER2:
   if (enemy[dr].type == ENEMY_OVERBLATTER2)
   {
    draw_blatter(bmp, x, y, 6, 52, enemy[dr].attribute [0], 13, GC_BLUE2, GC_BLUE6);
    draw_blatter(bmp, x, y, 4, 49, enemy[dr].attribute [2], 15, GC_GREY2, GC_GREY6);
   }
   circlefill(bmp, (x), y, 26, col1);
   circle(bmp, (x), y, 26, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_QUARTER, 26);
   y1 = y + ypart(enemy[dr].angle + ANGLE_QUARTER, 26);
   circlefill(bmp, x1, y1, 7, col1);
   circle(bmp, x1, y1, 7, col2);
   xa = 7;
   if (enemy[dr].attribute [8] > 0)
    xa = 7 - enemy[dr].attribute [8];
   if (enemy[dr].attribute [8] >= 6)
    xa = 3; 
   x1 = x1 + xpart(enemy[dr].attribute [6], xa);
   y1 = y1 + ypart(enemy[dr].attribute [6], xa);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_QUARTER, 26);
   y1 = y + ypart(enemy[dr].angle - ANGLE_QUARTER, 26);
   circlefill(bmp, x1, y1, 7, col1);
   circle(bmp, x1, y1, 7, col2);
   xa = 7;
   if (enemy[dr].attribute [9] > 0)
    xa = 7 - enemy[dr].attribute [9];
   if (enemy[dr].attribute [9] >= 6)
    xa = 3; 
   x1 = x1 + xpart(enemy[dr].attribute [7], xa);
   y1 = y1 + ypart(enemy[dr].attribute [7], xa);;
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_WAVER1:
//   draw_spinner(bmp, x, y, enemy[dr].attribute [0], 32, 10, ANGLE_1_32, 1024 / 3, 3, GC_YELLOW2, GC_YELLOW6);
   draw_waver(bmp, x, y, enemy[dr].attribute [0], 34, 15, ANGLE_1_32, 1024 / 4, 4, GC_RED1, GC_RED5, waver1_circle, 20);
//   int outward, int inward, int angle1, int angle2, int arms, int col1, int col2, RLE_SPRITE *waver_rle, int waver_rle_size)
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_WAVER2:
//   draw_spinner(bmp, x, y, enemy[dr].attribute [0], 32, 10, ANGLE_1_32, 1024 / 3, 3, GC_YELLOW2, GC_YELLOW6);
   draw_waver(bmp, x, y, enemy[dr].attribute [0], 41, 15, ANGLE_1_32, 1024 / 3, 3, GC_GREEN1, GC_GREEN5, waver2_circle, 20);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_SPIKEY1:
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 32, 10, ANGLE_1_EIGHTH, 1024 / 3, 3, GC_GREEN2, GC_GREEN6, -2, -3);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_SPIKEY2:
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 36, 15, ANGLE_1_EIGHTH, 1024 / 4, 4, GC_YELLOW2, GC_YELLOW6, -2, -3);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_SPIKEY3:
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 32, 10, ANGLE_1_EIGHTH, 1024 / 3, 3, GC_YELLOW2, GC_YELLOW6, -2, -3);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
/*   x1 = x + xpart(enemy[dr].angle - ANGLE_1_32, 13);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_32, 13);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_32, 13);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_32, 13);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);*/
   x1 = x + xpart(enemy[dr].angle, 13);
   y1 = y + ypart(enemy[dr].angle, 13);
   circlefill(bmp, x1, y1, 4, col1);
   circle(bmp, x1, y1, 4, col2);
   break;
  case ENEMY_SPIKEY4:
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 39, 10, ANGLE_1_EIGHTH, 1024 / 3, 3, GC_YELLOW2, GC_YELLOW6, -2, -3);
   draw_spikey(bmp, x, y, ANGLE_FULL - enemy[dr].attribute [0], 28, 5, ANGLE_1_EIGHTH, ANGLE_HALF, 2, GC_YELLOW2, GC_YELLOW6, -2, -3);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   break;
  case ENEMY_SPIKEY5:
   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 45, 10, ANGLE_1_SIXTEENTH + ANGLE_1_32, 1024 / 4, 4, GC_GREY1, GC_GREY6, -2, -3);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
/*  case ENEMY_SPINNER1:
   void draw_spinner(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2)
   draw_spinner(bmp, x, y, enemy[dr].attribute [0], size1, size2, radius, col1, col2, angle1, angle2
   xa = enemy[dr].attribute [0];
   col3 = GC_YELLOW2;
   col4 = GC_YELLOW6;
   bordered_triangle(bmp, 
    x + xpart(enemy[dr].attribute [0] + ANGLE_1_32, 32), 
    y + ypart(enemy[dr].attribute [0] + ANGLE_1_32, 32), 
    x + xpart(enemy[dr].attribute [0] - ANGLE_1_32, 32), 
    y + ypart(enemy[dr].attribute [0] - ANGLE_1_32, 32), 
    x - xpart(enemy[dr].attribute [0], 10), 
    y - ypart(enemy[dr].attribute [0], 10), 
    col3, col4);
   bordered_triangle(bmp, 
    x + xpart(enemy[dr].attribute [0] + ANGLE_1_32 + (1024 / 3), 32), 
    y + ypart(enemy[dr].attribute [0] + ANGLE_1_32 + (1024 / 3), 32), 
    x + xpart(enemy[dr].attribute [0] - ANGLE_1_32 + (1024 / 3), 32), 
    y + ypart(enemy[dr].attribute [0] - ANGLE_1_32 + (1024 / 3), 32), 
    x - xpart(enemy[dr].attribute [0] + (1024 / 3), 10), 
    y - ypart(enemy[dr].attribute [0] + (1024 / 3), 10), 
    col3, col4);
   bordered_triangle(bmp, 
    x + xpart(enemy[dr].attribute [0] + ANGLE_1_32 - (1024 / 3), 32), 
    y + ypart(enemy[dr].attribute [0] + ANGLE_1_32 - (1024 / 3), 32), 
    x + xpart(enemy[dr].attribute [0] - ANGLE_1_32 - (1024 / 3), 32), 
    y + ypart(enemy[dr].attribute [0] - ANGLE_1_32 - (1024 / 3), 32), 
    x - xpart(enemy[dr].attribute [0] - (1024 / 3), 10), 
    y - ypart(enemy[dr].attribute [0] - (1024 / 3), 10), 
    col3, col4);
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
//   x = x + xpart(enemy[dr].angle, 10);
//   y = y + ypart(enemy[dr].angle, 10);
//   circlefill(bmp, (x), y, 2, col1);
//   circle(bmp, (x), y, 2, col2);
   break;*/
  case ENEMY_DEFENDER3_TURRET1:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x1 = x + xpart(enemy[dr].attribute [1], 12);
   y1 = y + ypart(enemy[dr].attribute [1], 12);
   circlefill(bmp, (x1), y1, 4, col1);
   circle(bmp, (x1), y1, 4, col2);
   x2 = x - xpart(enemy[dr].attribute [1], 12);
   y2 = y - ypart(enemy[dr].attribute [1], 12);
   circlefill(bmp, x2, y2, 4, col1);
   circle(bmp, x2, y2, 4, col2);
   if (enemy[dr].attribute [0] > 0)
   {
     col2 = enemy[dr].attribute [0] / 3;
     switch(col2)
     {
      default: col1 = TRANS_WHITE; break;
      case 2: col1 = TRANS_LBLUE; break;
      case 1: col1 = TRANS_DBLUE; break;
      case 0: col1 = TRANS_DGREY; break;
     }
     draw_spark_jump(bmp, x1, y1, x2, y2, 4, col2 + 6, col1, 1);
     draw_a_light(bmp, col2 * 2 + 3 + grand(7), x1, y1);
     draw_a_light(bmp, col2 * 2 + 3 + grand(7), x2, y2);
   }   
   break;
  case ENEMY_DEFENDER3_TURRET2:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   draw_orbiter(bmp, x, y, enemy[dr].attribute [1], 8, 14, 20, ANGLE_1_SIXTEENTH, ANGLE_HALF, 2, GC_RED1, GC_RED6);
   break;
  case ENEMY_DEFENDER3_TURRET3:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x1 = xpart(enemy[dr].attribute [1], 12);
   y1 = ypart(enemy[dr].attribute [1], 12);
   circlefill(bmp, x + x1, y + y1, 3, col1);
   circle(bmp, x + x1, y + y1, 3, col2);
   circlefill(bmp, x - x1, y - y1, 3, col1);
   circle(bmp, x - x1, y - y1, 3, col2);
   x1 = xpart(enemy[dr].attribute [1] + ANGLE_QUARTER, 12);
   y1 = ypart(enemy[dr].attribute [1] + ANGLE_QUARTER, 12);
   circlefill(bmp, x - x1, y - y1, 3, col1);
   circle(bmp, x - x1, y - y1, 3, col2);
   circlefill(bmp, x + x1, y + y1, 3, col1);
   circle(bmp, x + x1, y + y1, 3, col2);
   break;
  case ENEMY_DEFENDER3_TURRET4:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 11);
   y = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_DEFENDER3_TURRET5:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x1 = xpart(enemy[dr].angle, 11);
   y1 = ypart(enemy[dr].angle, 11);
   circlefill(bmp, x + x1, y + y1, 3, col1);
   circle(bmp, x + x1, y + y1, 3, col2);
   circlefill(bmp, x - x1, y - y1, 3, col1);
   circle(bmp, x - x1, y - y1, 3, col2);
   break;
  case ENEMY_DEFENDER3_TURRET6:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 11);
   y = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x), y, 4, col1);
   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_SHIELDER1:
//   draw_spikey(bmp, x, y, enemy[dr].attribute [0], 45, 10, ANGLE_1_SIXTEENTH + ANGLE_1_32, 1024 / 4, 4, GC_GREY1, GC_GREY6);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);

   x1 = x + xpart(enemy[dr].attribute [1], 15);
   y1 = y + ypart(enemy[dr].attribute [1], 15);
   circlefill(bmp, (x1), y1, 4, col1);
   circle(bmp, (x1), y1, 4, col2);
   x2 = x - xpart(enemy[dr].attribute [1], 15);
   y2 = y - ypart(enemy[dr].attribute [1], 15);
   circlefill(bmp, x2, y2, 4, col1);
   circle(bmp, x2, y2, 4, col2);
   draw_squarey(bmp, x, y, ANGLE_FULL - enemy[dr].attribute [1], 40, 22, ANGLE_1_SIXTEENTH, ANGLE_FULL / 3, 3, GC_BLUE1, GC_BLUE6);
   if (enemy[dr].attribute [0] > 0)
   {
     col2 = enemy[dr].attribute [0] / 3;
     switch(col2)
     {
      default: col1 = TRANS_WHITE; break;
      case 2: col1 = TRANS_LBLUE; break;
      case 1: col1 = TRANS_DBLUE; break;
      case 0: col1 = TRANS_DGREY; break;
     }
     draw_spark_jump(bmp, x1, y1, x2, y2, 4, col2 + 6, col1, 1);
     draw_a_light(bmp, col2 * 2 + 3 + grand(7), x1, y1);
     draw_a_light(bmp, col2 * 2 + 3 + grand(7), x2, y2);
   }   
   break;
  case ENEMY_ZAPPER1:
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_QUARTER, 13);
   y1 = y + ypart(enemy[dr].angle + ANGLE_QUARTER, 13);
   circlefill(bmp, (x1), y1, 4, col1);
   circle(bmp, (x1), y1, 4, col2);
   x2 = x + xpart(enemy[dr].angle - ANGLE_QUARTER, 13);
   y2 = y + ypart(enemy[dr].angle - ANGLE_QUARTER, 13);
   circlefill(bmp, x2, y2, 4, col1);
   circle(bmp, x2, y2, 4, col2);
   for (xa = 0; xa < 8; xa ++)
   {
    x3 = x + xpart(enemy[dr].attribute [4] + (xa * ANGLE_1_EIGHTH), 35);
    y3 = y + ypart(enemy[dr].attribute [4] + (xa * ANGLE_1_EIGHTH), 35);
    col1 = COLOUR_GREY1;
    col3 = 0;
    col4 = ((ANGLE_FULL - (enemy[dr].attribute [4] * 2)) & 1023);
    if (col4 / ANGLE_1_EIGHTH == (xa + 2) % 8
     || col4 / ANGLE_1_EIGHTH == (xa + 6) % 8)
     {
      if (enemy[dr].attribute [5] < 0)
      {
        col3 = 18 + grand(3) - (col4 % ANGLE_1_EIGHTH) / 10;
        col1 = COLOUR_YELLOW8 - (col4 % ANGLE_1_EIGHTH) / 16;
      } 
       else
       {
        col3 = 11 + grand(3) + (col4 % ANGLE_1_EIGHTH) / 10;
        col1 = COLOUR_YELLOW1 + (col4 % ANGLE_1_EIGHTH) / 16;
       }
     }  
    circlefill(bmp, x3, y3, 7, col1);
    circle(bmp, x3, y3, 7, GC_GREY3);
    if (col3 > 0)
     draw_a_light(bmp, col3, x3, y3);
   }
//   draw_squarey(bmp, x, y, ANGLE_FULL - enemy[dr].attribute [1], 40, 22, ANGLE_1_SIXTEENTH, ANGLE_FULL / 3, 3, GC_BLUE1, GC_BLUE6);
   if (enemy[dr].attribute [0] == 1)
   {
     col2 = 3 - (counter / 4) % 4;//grand(4); //enemy[dr].attribute [1];
     switch(col2)
     {
      default: col1 = TRANS_YELLOW; break;
      case 2: col1 = TRANS_LGREEN; break;
      case 1: col1 = TRANS_DGREEN; break;
      case 0: col1 = TRANS_DGREEN; break;
/*      default: col1 = TRANS_WHITE; break;
      case 2: col1 = TRANS_LBLUE; break;
      case 1: col1 = TRANS_DBLUE; break;
      case 0: col1 = TRANS_DGREY; break;*/
     }
     x3 = (enemy[dr].attribute [1] / GRAIN) - (enemy[dr].x / GRAIN) + x;
     y3 = (enemy[dr].attribute [2] / GRAIN) - (enemy[dr].y / GRAIN) + y;
     draw_spark_jump(bmp, x1, y1, x3, y3, 7 + enemy[dr].attribute [3] / 15, col2 + 5 + enemy[dr].attribute [3] / 50, col1, 1);
     draw_spark_jump(bmp, x2, y2, x3, y3, 7 + enemy[dr].attribute [3] / 15, col2 + 5 + enemy[dr].attribute [3] / 50, col1, 1);
     draw_a_light(bmp, col2 * 2 + 3 + grand(7), x1, y1);
     draw_a_light(bmp, col2 * 2 + 3 + grand(7), x2, y2);
     draw_a_light(bmp, col2 * 2 + 3 + grand(7), x3, y3);
   }   
   break;
  case ENEMY_OVERZAPPER:
   circlefill(bmp, (x), y, 30, col1);
   circle(bmp, (x), y, 30, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_QUARTER, 30);
   y1 = y + ypart(enemy[dr].angle + ANGLE_QUARTER, 30);
   circlefill(bmp, (x1), y1, 5, col1);
   circle(bmp, (x1), y1, 5, col2);
   x2 = x + xpart(enemy[dr].angle - ANGLE_QUARTER, 30);
   y2 = y + ypart(enemy[dr].angle - ANGLE_QUARTER, 30);
   circlefill(bmp, x2, y2, 5, col1);
   circle(bmp, x2, y2, 5, col2);
   for (xa = 0; xa < 8; xa ++)
   {
    x3 = x + xpart(enemy[dr].attribute [4] + (xa * ANGLE_1_EIGHTH), 60);
    y3 = y + ypart(enemy[dr].attribute [4] + (xa * ANGLE_1_EIGHTH), 60);
    col1 = COLOUR_GREY1;
    col3 = 0;
    col4 = ((ANGLE_FULL - (enemy[dr].attribute [4] * 2)) & 1023);
    if (col4 / ANGLE_1_EIGHTH == (xa + 2) % 8
     || col4 / ANGLE_1_EIGHTH == (xa + 6) % 8)
     {
      if (enemy[dr].attribute [5] < 0)
      {
        col3 = 18 + grand(3) - (col4 % ANGLE_1_EIGHTH) / 10;
        col1 = COLOUR_YELLOW8 - (col4 % ANGLE_1_EIGHTH) / 16;
      } 
       else
       {
        col3 = 11 + grand(3) + (col4 % ANGLE_1_EIGHTH) / 10;
        col1 = COLOUR_YELLOW1 + (col4 % ANGLE_1_EIGHTH) / 16;
       }
     }  
    circlefill(bmp, x3, y3, 9, col1);
    circle(bmp, x3, y3, 9, GC_GREY3);
    if (col3 > 0)
     draw_a_light(bmp, col3, x3, y3);
   }
  break; 
  case ENEMY_ATTRACTOR:
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_ORBITER1:
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   draw_orbiter(bmp, x, y, enemy[dr].attribute [1], 17, 22, 40, ANGLE_1_SIXTEENTH, ANGLE_FULL / 3, 3, GC_RED1, GC_RED6);
   break;
  case ENEMY_ORBITER2:
   circlefill(bmp, (x), y, 11, col1);
   circle(bmp, (x), y, 11, col2);
   draw_orbiter(bmp, x, y, enemy[dr].attribute [1], 16, 21, 44, ANGLE_1_SIXTEENTH, ANGLE_HALF, 2, GC_ORANGE1, GC_ORANGE6);
   draw_orbiter(bmp, x, y, enemy[dr].attribute [3], 15, 21, 41, ANGLE_1_SIXTEENTH, ANGLE_HALF, 2, GC_ORANGE1, GC_ORANGE6);
   break;
  case ENEMY_ORBITER3:
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   draw_orbiter(bmp, x, y, enemy[dr].attribute [1], 17, 25, 56, ANGLE_1_SIXTEENTH, ANGLE_QUARTER, 4, GC_BLUE1, GC_BLUE5);
   break;
  case ENEMY_BRACKET1:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_BRACKET1], (x) - 25, (y) - 19);
   circlefill(bmp, (x), y, 11, col1);
   circle(bmp, (x), y, 11, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   break;
  case ENEMY_BRACKET2:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_BRACKET2], (x) - 27, (y) - 16);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 5);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 5);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 5);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 5);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   break;
  case ENEMY_BRACKET3:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_BRACKET3], (x) - 28, (y) - 19);
   circlefill(bmp, (x), y, 11, col1);
   circle(bmp, (x), y, 11, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   break;
  case ENEMY_BRACKET4:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_BRACKET4], (x) - 34, (y) - 24);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
   circle(bmp, x1, y1, 2, col2);
   break;
  case ENEMY_BRACKET4_TURRET:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   x = x + xpart(enemy[dr].angle, 7);
   y = y + ypart(enemy[dr].angle, 7);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BRACKET5:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_BRACKET5], (x) - 31, (y) - 45);
   circlefill(bmp, (x), y, 11, col1);
   circle(bmp, (x), y, 11, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 6);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 6);
   circle(bmp, x1, y1, 2, col2);
   break;
  case ENEMY_CURVE1:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_CURVE1], (x) - 34, (y) - 26);
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_CURVE2:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_CURVE2], (x) - 34, (y) - 26);
   if (enemy[dr].armour <= 500 && grand(500) > enemy[dr].armour)
   {
    x1 = 3;//2 + (300 - enemy[dr].armour) / 70;
    x += grand(x1) - grand(x1);
    y += grand(x1) - grand(x1);
   }
   circlefill(bmp, x, y, 13, col1);
   circle(bmp, x, y, 13, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_CURVE3:
   draw_rle_sprite(bmp, enemy6_rle [RLE_ENEMY6_CURVE3], (x) - 34, (y) - 26);
   if (enemy[dr].armour <= 500 && grand(500) > enemy[dr].armour)
   {
    x1 = 3;//2 + (300 - enemy[dr].armour) / 70;
    x += grand(x1) - grand(x1);
    y += grand(x1) - grand(x1);
   }
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_DEAD_TRI1:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER1], x - 36, y - 36);
   break;
  case ENEMY_DEAD_TRI2:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER2], x - 35, y - 35);
   break;
  case ENEMY_DEAD_TRI3:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER3], x - 35, y - 35);
   break;
  case ENEMY_TRIANGLER1:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER1], (x) - 36, (y) - 36);
   circlefill(bmp, (x), y, 16, col1);
   circle(bmp, (x), y, 16, col2);
    x1 = 3;
    y1 = 3;
    if (enemy[dr].attribute [0] == 1)
     x1 -= enemy[dr].attribute [1] / 3;
      else
       y1 -= enemy[dr].attribute [1] / 3;
   draw_launchers(bmp, x, y, enemy[dr].angle, 15, 14, 96, x1, y1, COLOUR_YELLOW1, COLOUR_YELLOW6);
//   x = x + xpart(enemy[dr].angle, 18);
//   y = y + ypart(enemy[dr].angle, 18);
//   circlefill(bmp, (x), y, 4, col1);
//   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_TRIANGLER2:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER2], (x) - 35, (y) - 35);
   circlefill(bmp, (x), y, 18, col1);
   circle(bmp, (x), y, 18, col2);
    x1 = 3;
    y1 = 3;
    if (enemy[dr].attribute [0] == 1)
     x1 -= enemy[dr].attribute [1] / 3;
      else
       y1 -= enemy[dr].attribute [1] / 3;
   draw_launchers(bmp, x, y, enemy[dr].angle, 17, 13, 105, x1, y1, COLOUR_ORANGE1, COLOUR_ORANGE6);
   break;
  case ENEMY_TRIANGLER3:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER3], (x) - 35, (y) - 35);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
    x1 = 3;
    y1 = 3;
    if (enemy[dr].attribute [0] == 1)
     x1 -= enemy[dr].attribute [1] / 3;
      else
       y1 -= enemy[dr].attribute [1] / 3;
   draw_launchers(bmp, x, y, enemy[dr].angle, 16, 12, 112, x1, y1, COLOUR_BLUE1, COLOUR_BLUE6);
   break;
  case ENEMY_OVERTRIANGLER:
   draw_rle_sprite(bmp, enemy5_rle [RLE_ENEMY5_OVERTRIANGLER], (x) - 52, (y) - 52);
   circlefill(bmp, (x), y, 33, col1);
   circle(bmp, (x), y, 33, col2);
    x1 = 3;
    y1 = 3;
    if (enemy[dr].attribute [0] == 1)
     x1 -= enemy[dr].attribute [1];
      else
       y1 -= enemy[dr].attribute [1];
   draw_launchers(bmp, x, y, enemy[dr].angle, 29, 25, 96, x1, y1, COLOUR_RED1, COLOUR_RED6);
   break;
  case ENEMY_OVERTRIANGLER_TURRET:
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_DISRUPTER1:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_DISRUPTER1], (x) - 44, (y) - 12);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x = x + xpart(enemy[dr].angle, 15);
   y = y + ypart(enemy[dr].angle, 15);
   circlefill(bmp, (x), y, 4, col1);
   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_DISRUPTER2:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_DISRUPTER2], (x) - 14, (y) - 46);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x = x + xpart(enemy[dr].angle, 15);
   y = y + ypart(enemy[dr].angle, 15);
   circlefill(bmp, (x), y, 4, col1);
   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_DISRUPTER3:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_DISRUPTER3], (x) - 44, (y) - 16);
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x = x + xpart(enemy[dr].angle, 15);
   y = y + ypart(enemy[dr].angle, 15);
   circlefill(bmp, (x), y, 4, col1);
   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_WORMER1:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_WORMER1], (x) - 31, (y) - 29);
   circlefill(bmp, (x), y, enemy[dr].radius / GRAIN, col1);
   circle(bmp, (x), y, enemy[dr].radius / GRAIN, col2);
   break;
  case ENEMY_WORMER2:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_WORMER2], (x) - 30, (y) - 21);
   circlefill(bmp, (x), y, enemy[dr].radius / GRAIN, col1);
   circle(bmp, (x), y, enemy[dr].radius / GRAIN, col2);
   break;
  case ENEMY_WORMER3:
   draw_rle_sprite(bmp, enemy4_rle [RLE_ENEMY4_WORMER3], (x) - 32, (y) - 32);
   circlefill(bmp, (x), y, enemy[dr].radius / GRAIN, col1);
   circle(bmp, (x), y, enemy[dr].radius / GRAIN, col2);
   break;
  case ENEMY_WORMER4:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_WORMER4], (x) - 41, (y) - 41);
   circlefill(bmp, (x), y, enemy[dr].radius / GRAIN, col1);
   circle(bmp, (x), y, enemy[dr].radius / GRAIN, col2);
   break;
  case ENEMY_HEAD1:
   draw_rle_sprite(bmp, enemy5_rle [RLE_ENEMY5_HEAD1], (x) - 40, (y) - 20);
   circlefill(bmp, (x), y, 13, col1);
   circle(bmp, (x), y, 13, col2);
   break;
  case ENEMY_HEAD1_EYE1:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   x = x + xpart(enemy[dr].angle, 4);
   y = y + ypart(enemy[dr].angle, 4);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 2, col2);
   break;
  case ENEMY_HEAD1_EYE2:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   x = x + xpart(enemy[dr].angle, 4);
   y = y + ypart(enemy[dr].angle, 4);
//   circlefill(bmp, (x), y, 2, col1);
   circlefill(bmp, (x), y, 2, COLOUR_BLUE6);
   circle(bmp, (x), y, 2, COLOUR_BLUE1);
   break;
  case ENEMY_HEAD1_EYE3:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   x = x + xpart(enemy[dr].angle, 4);
   y = y + ypart(enemy[dr].angle, 4);
//   circlefill(bmp, (x), y, 2, col1);
   circlefill(bmp, (x), y, 3, COLOUR_RED6);
   circle(bmp, (x), y, 3, COLOUR_RED1);
   break;
  case ENEMY_DEFENDER1:
   draw_rle_sprite(bmp, enemy5_rle [RLE_ENEMY5_DEFENDER1], (x) - 65, (y) - 33);
   circlefill(bmp, (x), y, 25, col1);
   circle(bmp, (x), y, 25, col2);
   x = x + xpart(enemy[dr].angle, 21);
   y = y + ypart(enemy[dr].angle, 21);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_DEFENDER1_TURRET1:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
   circlefill(bmp, (x), y, 5, col1);
   circle(bmp, (x), y, 5, col2);
   break;
  case ENEMY_DEFENDER1_TURRET2:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_DEFENDER1_TURRET3:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_DEFENDER2:
   draw_rle_sprite(bmp, enemy5_rle [RLE_ENEMY5_DEFENDER2], (x) - 33, (y) - 60);
   circlefill(bmp, (x), y, 25, col1);
   circle(bmp, (x), y, 25, col2);
   x = x + xpart(enemy[dr].angle, 21);
   y = y + ypart(enemy[dr].angle, 21);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_DEFENDER2_TURRET1:
  case ENEMY_DEFENDER2_TURRET2:
//  case ENEMY_DEFENDER2_TURRET3:
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
//   x = x + xpart(enemy[dr].angle, 9);
//   y = y + ypart(enemy[dr].angle, 9);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 5);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 5);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 5);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 5);
   circle(bmp, x1, y1, 2, col2);
   break;
  case ENEMY_DEFENDER3:
   draw_rle_sprite(bmp, enemy5_rle [RLE_ENEMY5_DEFENDER3], (x) - 65, (y) - 33);
   circlefill(bmp, (x), y, 25, col1);
   circle(bmp, (x), y, 25, col2);
   x = x + xpart(enemy[dr].angle, 21);
   y = y + ypart(enemy[dr].angle, 21);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_OVERDISRUPTER:
   draw_rle_sprite(bmp, enemy5_rle [RLE_ENEMY5_OVERDISRUPTER], (x) - 20, (y) - 69);
   circlefill(bmp, (x), y, 25, col1);
   circle(bmp, (x), y, 25, col2);
   x = x + xpart(enemy[dr].angle, 23);
   y = y + ypart(enemy[dr].angle, 23);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_BEAMER1:
   draw_rle_sprite(bmp, enemy7_rle [RLE_ENEMY7_BEAMER1], (x) - 92, (y) - 69);
   circlefill(bmp, (x), y, 35, col1);
   circle(bmp, (x), y, 35, col2);
   x1 = x + xpart(enemy[dr].angle, 31);
   y1 = y + ypart(enemy[dr].angle, 31);
   circlefill(bmp, x1, y1, 8, COLOUR_ORANGE5);
   circle(bmp, x1, y1, 8, COLOUR_ORANGE1);
   x1 = x + xpart(enemy[dr].angle, 32);
   y1 = y + ypart(enemy[dr].angle, 32);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   xa = abs(xpart(enemy[dr].counter * 8, 7));
   if (xa == 0)
    xa = 1;
   col3 = COLOUR_BLUE1 + xa;
   circlefill(bmp, x, y - 59, 7, col3);
   circle(bmp, x, y - 59, 7, GC_GREY1);
   draw_a_light(bmp, abs(xpart(enemy[dr].counter * 8, 21)) + 4, x, y - 59);
   col3 = COLOUR_BLUE8 - xa;
   circlefill(bmp, x, y + 59, 7, col3);
   circle(bmp, x, y + 59, 7, GC_GREY1);
   draw_a_light(bmp, abs(xpart(enemy[dr].counter * 8 + ANGLE_QUARTER, 21)) + 4, x, y + 59);
   break;
  case ENEMY_BEAMER2:
   draw_rle_sprite(bmp, enemy7_rle [RLE_ENEMY7_BEAMER2], (x) - 92, (y) - 69);
   circlefill(bmp, (x), y, 35, col1);
   circle(bmp, (x), y, 35, col2);
   x1 = x + xpart(enemy[dr].angle, 31);
   y1 = y + ypart(enemy[dr].angle, 31);
   circlefill(bmp, x1, y1, 8, COLOUR_GREY5);
   circle(bmp, x1, y1, 8, COLOUR_GREY1);
   x1 = x + xpart(enemy[dr].angle, 32);
   y1 = y + ypart(enemy[dr].angle, 32);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   xa = abs(xpart(enemy[dr].counter * 8, 7));
   if (xa == 0)
    xa = 1;
   col3 = COLOUR_YELLOW1 + xa;
   circlefill(bmp, x, y - 59, 7, col3);
   circle(bmp, x, y - 59, 7, GC_GREY1);
   draw_a_light(bmp, abs(xpart(enemy[dr].counter * 8, 21)) + 4, x, y - 59);
   col3 = COLOUR_YELLOW8 - xa;
   circlefill(bmp, x, y + 59, 7, col3);
   circle(bmp, x, y + 59, 7, GC_GREY1);
   draw_a_light(bmp, abs(xpart(enemy[dr].counter * 8 + ANGLE_QUARTER, 21)) + 4, x, y + 59);
   break;
  case ENEMY_BOSS1_1:
   draw_rle_sprite(bmp, enemy7_rle [RLE_ENEMY7_BOSS1_1], (x) - 94, (y) - 111);
   circlefill(bmp, (x), y, 55, col1);
   circle(bmp, (x), y, 55, col2);
   break;
  case ENEMY_BOSS1_2:
   draw_rle_sprite(bmp, enemy7_rle [RLE_ENEMY7_BOSS1_2], (x) - 114, (y) - 30);
   circlefill(bmp, (x), y, 40, col1);
   circle(bmp, (x), y, 40, col2);
   break;
  case ENEMY_BOSS1_3:
   draw_rle_sprite(bmp, enemy7_rle [RLE_ENEMY7_BOSS1_3], (x) - 114, (y) - 115);
   circlefill(bmp, (x), y, 40, col1);
   circle(bmp, (x), y, 40, col2);
   x = x + xpart(enemy[dr].angle, 35);
   y = y + ypart(enemy[dr].angle, 35);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_BOSS1_TURRET1:
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x1 = x + xpart(enemy[dr].angle, 9);
   y1 = y + ypart(enemy[dr].angle, 9);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH, 9);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH, 9);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH, 9);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH, 9);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_BOSS1_TURRET2:
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BOSS1_TURRET3:
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   x = x + xpart(enemy[dr].angle, 9);
   y = y + ypart(enemy[dr].angle, 9);
//   circlefill(bmp, (x), y, 2, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BOSS3_1:
   draw_rle_sprite(bmp, enemy8_rle [RLE_ENEMY8_BOSS3], (x) - 119, (y) - 119);
   circlefill(bmp, (x), y, 35, col1);
   circle(bmp, (x), y, 35, col2);
   break;
  case ENEMY_BOSS3_3:
   draw_rle_sprite(bmp, enemy8_rle [RLE_ENEMY8_BOSS3_3], (x) - 140, (y) - 111);
   circlefill(bmp, x, y, 35, col1);
   circle(bmp, x, y, 35, col2);
   x1 = x - xpart(enemy[dr].angle, 33);
   y1 = y - ypart(enemy[dr].angle, 33);
   circlefill(bmp, x1, y1, 7, col1);
   circle(bmp, x1, y1, 7, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_QUARTER, 35);
   y1 = y + ypart(enemy[dr].angle + ANGLE_QUARTER, 35);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_QUARTER, 35);
   y1 = y + ypart(enemy[dr].angle - ANGLE_QUARTER, 35);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_QUARTER + ANGLE_1_EIGHTH, 35);
   y1 = y + ypart(enemy[dr].angle - ANGLE_QUARTER + ANGLE_1_EIGHTH, 35);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_QUARTER - ANGLE_1_EIGHTH, 35);
   y1 = y + ypart(enemy[dr].angle + ANGLE_QUARTER - ANGLE_1_EIGHTH, 35);
   circlefill(bmp, x1, y1, 3, col1);
   circle(bmp, x1, y1, 3, col2);
   break;
  case ENEMY_BOSS2:
   draw_rle_sprite(bmp, enemy8_rle [RLE_ENEMY8_BOSS2], (x) - 124, (y) - 124);
   circlefill(bmp, (x), y, 35, col1);
   circle(bmp, (x), y, 35, col2);
   break;
  case ENEMY_BOSS2_2:
   draw_rle_sprite(bmp, enemy7_rle [RLE_ENEMY7_BOSS2_2], (x) - 115, (y) - 127);
   if (enemy[dr].attribute [0] > 20 && grand(40) < enemy[dr].attribute [0])
   {
    x += grand(enemy[dr].attribute [0] / 10) - grand(enemy[dr].attribute [0] / 10);
    y += grand(enemy[dr].attribute [0] / 10) - grand(enemy[dr].attribute [0] / 10);
   }
   circlefill(bmp, (x), y, 35, col1);
   circle(bmp, (x), y, 35, col2);
   x = x + xpart(enemy[dr].angle, 32);
   y = y + ypart(enemy[dr].angle, 32);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   if (enemy[dr].recycle <= 1)
   {
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
    circlefill(bmp, x, y, 6 + enemy[dr].attribute [0] / 3 + grand(8), TRANS_LBLUE);
    circlefill(bmp, x, y, 3 + enemy[dr].attribute [0] / 3 + grand(4), TRANS_WHITE);
    draw_a_light(bmp, 6 + enemy[dr].attribute [0] / 3 + grand(8), x, y);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);    
   }
   break;
  case ENEMY_BOSS2_3:
   draw_rle_sprite(bmp, enemy8_rle [RLE_ENEMY8_BOSS2_3], (x) - 132, (y) - 120);
   circlefill(bmp, (x), y, 35, col1);
   circle(bmp, (x), y, 35, col2);
   break;
  case ENEMY_BOSS2_TURRET1:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 11);
   y = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x), y, 4, col1);
   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_BOSS2_TURRET3:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 10);
   y = y + ypart(enemy[dr].angle, 10);
   circlefill(bmp, (x), y, 6, col1);
   circle(bmp, (x), y, 6, col2);
   break;
  case ENEMY_BOSS2_TURRET4:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   xa = 11;
   if (enemy[dr].attribute [0] > 0)
    xa = 11 - enemy[dr].attribute [0] / 8;
   if (enemy[dr].attribute [0] >= 48)
    xa = 5; 
   x = x + xpart(enemy[dr].angle, xa);
   y = y + ypart(enemy[dr].angle, xa);
   circlefill(bmp, (x), y, 4, col1);
   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_BOSS2_TURRET2:
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x = x + xpart(enemy[dr].angle, 11);
   y = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_BOSS3_2:
   draw_rle_sprite(bmp, enemy7_rle [RLE_ENEMY7_BOSS3_2], (x) - 115, (y) - 120);
   circlefill(bmp, (x), y, 35, col1);
   circle(bmp, (x), y, 35, col2);
   x = x + xpart(enemy[dr].angle, 31);
   y = y + ypart(enemy[dr].angle, 31);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_BOSS3_TURRET1:
   circlefill(bmp, (x), y, 14, col1);
   circle(bmp, (x), y, 14, col2);
   x = x + xpart(enemy[dr].angle, 13);
   y = y + ypart(enemy[dr].angle, 13);
   circlefill(bmp, (x), y, 5, col1);
   circle(bmp, (x), y, 5, col2);
   break;
  case ENEMY_BOSS3_TURRET2:
   circlefill(bmp, (x), y, 14, col1);
   circle(bmp, (x), y, 14, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 7);
   y1 = y + ypart(enemy[dr].angle + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 7);
   circlefill(bmp, x1, y1, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   x1 = x + xpart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 7);
   y1 = y + ypart(enemy[dr].angle - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 7);
   circlefill(bmp, x1, y1, 2, col1);
   circle(bmp, x1, y1, 2, col2);
   break;
  case ENEMY_BOSS3_TURRET3:
   circlefill(bmp, (x), y, 14, col1);
   circle(bmp, (x), y, 14, col2);
   xa = 13;
   if (enemy[dr].attribute [0] > 0)
    xa = 11 - enemy[dr].attribute [0] / 8;
   if (enemy[dr].attribute [0] >= 48)
    xa = 5; 
   x = x + xpart(enemy[dr].angle, xa);
   y = y + ypart(enemy[dr].angle, xa);
   circlefill(bmp, (x), y, 4, col1);
   circle(bmp, (x), y, 4, col2);
   break;
  case ENEMY_FIGHTER5: 
  case ENEMY_FIGHTER4: 
  case ENEMY_FIGHTER3: 
  case ENEMY_FIGHTER2: 
  case ENEMY_FIGHTER1:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
//   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 10);
//   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 10);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 10);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 10);
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH, 10);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH, 10);
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   break;
  case ENEMY_LEAPER2:
  case ENEMY_LEAPER1:
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER, 25);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER, 25);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER + ANGLE_1_EIGHTH, 19);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER + ANGLE_1_EIGHTH, 19);
   triangle(bmp, x, y, x1, y1, x2, y2, eclass[enemy[dr].type].colour1 - 1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER , 26);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER, 26);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER + ANGLE_1_EIGHTH + 16, 20);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER + ANGLE_1_EIGHTH + 16, 20);
   line(bmp, x, y, x1, y1, GC_GREY1); // was col2
   line(bmp, x2, y2, x1, y1, GC_GREY1);
   line(bmp, x, y, x2, y2, GC_GREY1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER, 25);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER, 25);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER - ANGLE_1_EIGHTH, 19);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER - ANGLE_1_EIGHTH, 19);
   triangle(bmp, x, y, x1, y1, x2, y2, eclass[enemy[dr].type].colour1 - 1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER, 26);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER, 26);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER - ANGLE_1_EIGHTH - 16, 20);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER - ANGLE_1_EIGHTH - 16, 20);
   line(bmp, x, y, x1, y1, GC_GREY1);
   line(bmp, x2, y2, x1, y1, GC_GREY1);
   line(bmp, x, y, x2, y2, GC_GREY1);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 11);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 11);
   circlefill(bmp, (x1), y1, 4, col1);
   circle(bmp, (x1), y1, 4, col2);
   break;
  case ENEMY_MESSENGER:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   x1 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 25);
   y1 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 25);
   x2 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_SIXTEENTH, 23);
   y2 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_SIXTEENTH, 23);
   triangle(bmp, x, y, x1, y1, x2, y2, TRANS_DBLUE); //, eclass[enemy[dr].type].colour1 - 2);
   x1 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 26);
   y1 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 26);
   x2 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_SIXTEENTH + 16, 23);
   y2 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF - ANGLE_1_SIXTEENTH + 16, 23);
//   line(bmp, x, y, x1, y1, GC_GREY1); // was col2
//   line(bmp, x2, y2, x1, y1, GC_GREY1);
//   line(bmp, x, y, x2, y2, GC_GREY1);
   x1 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 25);
   y1 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 25);
   x2 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_SIXTEENTH, 23);
   y2 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_SIXTEENTH, 23);
   triangle(bmp, x, y, x1, y1, x2, y2, TRANS_DBLUE);//eclass[enemy[dr].type].colour1 - 2);
   x1 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 26);
   y1 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 26);
   x2 = x + xpart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_SIXTEENTH - 16, 23);
   y2 = y + ypart(enemy[dr].attribute [1] + ANGLE_HALF + ANGLE_1_SIXTEENTH - 16, 23);
//   line(bmp, x, y, x1, y1, GC_GREY1); // was col2
//   line(bmp, x2, y2, x1, y1, GC_GREY1);
//   line(bmp, x, y, x2, y2, GC_GREY1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH, 22);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH, 22);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH, 22);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH, 22);
   triangle(bmp, x, y, x1, y1, x2, y2, TRANS_LBLUE);//, eclass[enemy[dr].type].colour1 - 1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH, 23);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_SIXTEENTH, 23);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH - 12, 23);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_SIXTEENTH - 12, 23);
//   line(bmp, x, y, x1, y1, GC_GREY1); // was col2
//   line(bmp, x2, y2, x1, y1, GC_GREY1);
//   line(bmp, x, y, x2, y2, GC_GREY1);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
/*   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 11);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 11);
   circlefill(bmp, (x1), y1, 4, col1);
   circle(bmp, (x1), y1, 4, col2);*/
   break;
  case ENEMY_BOMBER1:
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, (x1), y1, 3, COLOUR_ORANGE5);
   circle(bmp, (x1), y1, 3, COLOUR_ORANGE1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, (x1), y1, 3, COLOUR_ORANGE5);
   circle(bmp, (x1), y1, 3, COLOUR_ORANGE1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 11);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 11);
   circlefill(bmp, (x1), y1, 3, COLOUR_ORANGE5);
   circle(bmp, (x1), y1, 3, COLOUR_ORANGE1);
   break;
  case ENEMY_BOMBER2:
   col3 = COLOUR_ORANGE5;//enemy[dr].colours [1];
   col4 = COLOUR_ORANGE2;//enemy[dr].colours [0] - 2;
//   radangle = angle_to_radians(enemy[dr].angle);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 16);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 16);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3) + 5, 23);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3) + 5, 23);
   x3 = x + xpart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3) - 5, 23);
   y3 = y + ypart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3) - 5, 23);
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 17);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 17);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3), 25);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3), 25);
   x3 = x + xpart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3), 25);
   y3 = y + ypart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3), 25);
   pline(bmp, x1, y1, x2, y2, col4);
   pline(bmp, x1, y1, x3, y3, col4);
   pline(bmp, x2, y2, x3, y3, col4);
//   circlefill(bmp, (x1), y1, 3, col1);
//   circle(bmp, (x1), y1, 3, col2);
//   circlefill(bmp, (x), y, 7, col1);
//   circle(bmp, (x), y, 7, col2);
//   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 6;
//   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 6;
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, (x1), y1, 3, COLOUR_BLUE5);
   circle(bmp, (x1), y1, 3, COLOUR_BLUE1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, (x1), y1, 3, COLOUR_BLUE5);
   circle(bmp, (x1), y1, 3, COLOUR_BLUE1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 11);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 11);
   circlefill(bmp, (x1), y1, 3, COLOUR_BLUE5);
   circle(bmp, (x1), y1, 3, COLOUR_BLUE1);
   break;
  case ENEMY_BOMBER3:
   col3 = COLOUR_ORANGE5;//enemy[dr].colours [1];
   col4 = COLOUR_ORANGE2;//enemy[dr].colours [0] - 2;
//   radangle = angle_to_radians(enemy[dr].angle);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 16);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 16);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3), 23);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3), 23);
   x3 = x + xpart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3), 23);
   y3 = y + ypart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3), 23);
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 17);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 17);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3), 25);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF + (ANGLE_FULL / 3), 25);
   x3 = x + xpart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3), 25);
   y3 = y + ypart(enemy[dr].angle + ANGLE_HALF - (ANGLE_FULL / 3), 25);
   pline(bmp, x1, y1, x2, y2, col4);
   pline(bmp, x1, y1, x3, y3, col4);
   pline(bmp, x2, y2, x3, y3, col4);
   circlefill(bmp, (x), y, 10, col1);
   circle(bmp, (x), y, 10, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH - ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, (x1), y1, 3, COLOUR_GREY5);
   circle(bmp, (x1), y1, 3, COLOUR_GREY1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH + ANGLE_1_SIXTEENTH, 12);
   circlefill(bmp, (x1), y1, 3, COLOUR_GREY5);
   circle(bmp, (x1), y1, 3, COLOUR_GREY1);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF, 11);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF, 11);
   circlefill(bmp, (x1), y1, 3, COLOUR_GREY5);
   circle(bmp, (x1), y1, 3, COLOUR_GREY1);
   break;
  case ENEMY_CRUISER4:
  case ENEMY_CRUISER3:
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER, 21);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_QUARTER, 21);
   circlefill(bmp, (x1), y1, 6, col1 - 1);
   circle(bmp, (x1), y1, 6, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER, 21);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_QUARTER, 21);
   circlefill(bmp, (x1), y1, 6, col1 - 1);
   circle(bmp, (x1), y1, 6, col2);
  case ENEMY_CRUISER1:
  case ENEMY_CRUISER2:
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 21);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 21);
   circlefill(bmp, (x1), y1, 6, col1 - 1);
   circle(bmp, (x1), y1, 6, col2);
   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH, 21);
   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH, 21);
   circlefill(bmp, (x1), y1, 6, col1 - 1);
   circle(bmp, (x1), y1, 6, col2);
   circlefill(bmp, (x), y, 19, col1);
   circle(bmp, (x), y, 19, col2);
//   x1 = x + xpart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 10);
//   y1 = y + ypart(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH, 10);
//   textprintf_ex(bmp, small_font, x, y, COLOUR_GREY6, -1, "%i: %i %i %i %i", dr, enemy[dr].turret [0], enemy[dr].turret [1], enemy[dr].turret [2], enemy[dr].turret [3]);
   break;
  case ENEMY_DEFENDER2_TURRET3:
  case ENEMY_CRUISER2_TURRET:
  case ENEMY_CRUISER3_TURRET:
  case ENEMY_CRUISER4_TURRET:
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   xa = 9;
   if (enemy[dr].attribute [0] > 0)
    xa = 9 - enemy[dr].attribute [0];
   if (enemy[dr].attribute [0] >= 6)
    xa = 6; 
   x1 = x + xpart(enemy[dr].angle, xa);
   y1 = y + ypart(enemy[dr].angle, xa);
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
//   textprintf_ex(bmp, small_font, x, y, COLOUR_GREY6, -1, "m%i i%i %i %i %i %i", enemy[dr].turret_main, enemy[dr].turret_index, enemy[dr].turret [0], enemy[dr].turret [1], enemy[dr].turret [2], enemy[dr].turret [3]);
   break;
  case ENEMY_CRUISER1_TURRET:
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   xa = 9;
   if (enemy[dr].attribute [0] > 0)
    xa = 9 - enemy[dr].attribute [0];
   if (enemy[dr].attribute [0] >= 6)
    xa = 6; 
   x1 = x + xpart(enemy[dr].angle, xa);
   y1 = y + ypart(enemy[dr].angle, xa);
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
//   textprintf_ex(bmp, small_font, x, y, COLOUR_GREY6, -1, "m%i i%i %i %i %i %i", enemy[dr].turret_main, enemy[dr].turret_index, enemy[dr].turret [0], enemy[dr].turret [1], enemy[dr].turret [2], enemy[dr].turret [3]);
   break;
  case ENEMY_SHADOW1:
   if (enemy[dr].attribute [0] == 0)
    break; // invisible! this is nasty
   if (enemy[dr].attribute [0] > 9)
   {
    col3 = GC_BLUE4;
    col4 = GC_BLUE1; 
    if (enemy[dr].attribute [0] > 70)
    {
      drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
      col1 = TRANS_WHITE;
      col2 = TRANS_WHITE;
      col3 = TRANS_WHITE;
      col4 = TRANS_WHITE;
     if (enemy[dr].attribute [0] > 72)
     {
      col1 = TRANS_LGREY;
      col2 = TRANS_LGREY;
      col3 = TRANS_LGREY;
      col4 = TRANS_LGREY;
     }
     if (enemy[dr].attribute [0] > 75)
     {
      col1 = TRANS_DGREY;
      col2 = TRANS_DGREY;
      col3 = TRANS_DGREY;
      col4 = TRANS_DGREY;
     }
    }
   }
    else
    {
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
      col1 = TRANS_DBLUE;
      col2 = TRANS_DBLUE;
      col3 = TRANS_DBLUE;
      col4 = TRANS_DBLUE;
     if (enemy[dr].attribute [0] > 3)
     {
      col1 = TRANS_LBLUE;
      col2 = TRANS_DBLUE;
      col3 = TRANS_LBLUE;
      col4 = TRANS_DBLUE;
     }
     if (enemy[dr].attribute [0] > 6)
     {
      col1 = TRANS_WHITE;
      col2 = TRANS_WHITE;
      col3 = TRANS_WHITE;
      col4 = TRANS_DGREY;
     }
     if (enemy[dr].hurt_pulse > 0)
      col1 = TRANS_WHITE;
    }
   triangle(bmp, x - 4, y - 4, x - 50, y - 4, x - 4, y - 27, col3);
   pline(bmp, x - 4, y - 4, x - 50, y - 4, col4);
   pline(bmp, x - 4, y - 4, x - 4, y - 27, col4);
   pline(bmp, x - 4, y - 27, x - 50, y - 4, col4);
   triangle(bmp, x + 4, y - 4, x + 50, y - 4, x + 4, y - 27, col3);
   pline(bmp, x + 4, y - 4, x + 50, y - 4, col4);
   pline(bmp, x + 4, y - 4, x + 4, y - 27, col4);
   pline(bmp, x + 4, y - 27, x + 50, y - 4, col4);
   triangle(bmp, x + 4, y + 4, x + 50, y + 4, x + 4, y + 27, col3);
   pline(bmp, x + 4, y + 4, x + 50, y + 4, col4);
   pline(bmp, x + 4, y + 4, x + 4, y + 27, col4);
   pline(bmp, x + 4, y + 27, x + 50, y + 4, col4);
   triangle(bmp, x - 4, y + 4, x - 50, y + 4, x - 4, y + 27, col3);
   pline(bmp, x - 4, y + 4, x - 50, y + 4, col4);
   pline(bmp, x - 4, y + 4, x - 4, y + 27, col4);
   pline(bmp, x - 4, y + 27, x - 50, y + 4, col4);
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x1 = x + xpart(enemy[dr].angle, 11);
   y1 = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
//   if (enemy[dr].attribute [0] <= 15)
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case ENEMY_SHADOW2:
   if (enemy[dr].attribute [0] == 0)
    break; // invisible! this is nasty
   if (enemy[dr].attribute [0] > 9)
   {
    col3 = GC_GREY4;
    col4 = GC_GREY1; 
    if (enemy[dr].attribute [0] > 50)
    {
      drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
      col1 = TRANS_WHITE;
      col2 = TRANS_WHITE;
      col3 = TRANS_WHITE;
      col4 = TRANS_WHITE;
     if (enemy[dr].attribute [0] > 52)
     {
      col1 = TRANS_LGREY;
      col2 = TRANS_LGREY;
      col3 = TRANS_LGREY;
      col4 = TRANS_LGREY;
     }
     if (enemy[dr].attribute [0] > 55)
     {
      col1 = TRANS_DGREY;
      col2 = TRANS_DGREY;
      col3 = TRANS_DGREY;
      col4 = TRANS_DGREY;
     }
    }
   }
    else
    {
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
      col1 = TRANS_DGREY;
      col2 = TRANS_DGREY;
      col3 = TRANS_DGREY;
      col4 = TRANS_DGREY;
     if (enemy[dr].attribute [0] > 3)
     {
      col1 = TRANS_LGREY;
      col2 = TRANS_DGREY;
      col3 = TRANS_LGREY;
      col4 = TRANS_DGREY;
     }
     if (enemy[dr].attribute [0] > 6)
     {
      col1 = TRANS_WHITE;
      col2 = TRANS_WHITE;
      col3 = TRANS_WHITE;
      col4 = TRANS_DGREY;
     }
     if (enemy[dr].hurt_pulse > 0)
      col1 = TRANS_WHITE;
    }
   triangle(bmp, x - 4, y - 4, x - 30, y - 4, x - 4, y - 52, col3);
   pline(bmp, x - 4, y - 4, x - 30, y - 4, col4);
   pline(bmp, x - 4, y - 4, x - 4, y - 52, col4);
   pline(bmp, x - 4, y - 52, x - 30, y - 4, col4);
   triangle(bmp, x + 4, y - 4, x + 30, y - 4, x + 4, y - 52, col3);
   pline(bmp, x + 4, y - 4, x + 30, y - 4, col4);
   pline(bmp, x + 4, y - 4, x + 4, y - 52, col4);
   pline(bmp, x + 4, y - 52, x + 30, y - 4, col4);
   triangle(bmp, x + 4, y + 4, x + 30, y + 4, x + 4, y + 52, col3);
   pline(bmp, x + 4, y + 4, x + 30, y + 4, col4);
   pline(bmp, x + 4, y + 4, x + 4, y + 52, col4);
   pline(bmp, x + 4, y + 52, x + 30, y + 4, col4);
   triangle(bmp, x - 4, y + 4, x - 30, y + 4, x - 4, y + 52, col3);
   pline(bmp, x - 4, y + 4, x - 30, y + 4, col4);
   pline(bmp, x - 4, y + 4, x - 4, y + 52, col4);
   pline(bmp, x - 4, y + 52, x - 30, y + 4, col4);
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   x1 = x + xpart(enemy[dr].angle, 11);
   y1 = y + ypart(enemy[dr].angle, 11);
   circlefill(bmp, (x1), y1, 4, col1);
   circle(bmp, (x1), y1, 4, col2);
//   if (enemy[dr].attribute [0] <= 15)
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;


/*  case ENEMY_BOUNCER:
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_BOUNCER2:
   col4 = enemy[dr].colours [0] - 2;
   x1 = x + xpart(enemy[dr].angle, 12);
   y1 = y + ypart(enemy[dr].angle, 12);
   x2 = x + xpart(enemy[dr].angle + ANGLE_HALF, 12);
   y2 = y + ypart(enemy[dr].angle + ANGLE_HALF, 12);
   pline(bmp, x1, y1, x2, y2, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_FIGHTER:
   circlefill(bmp, (x), y, 8, col1);
   circle(bmp, (x), y, 8, col2);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 6;
   y1 = y + sin(angle_to_radisans(enemy[dr].angle + ANGLE_HALF - ANGLE_1_EIGHTH)) * 6;
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 6;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF + ANGLE_1_EIGHTH)) * 6;
   circlefill(bmp, (x1), y1, 3, col1);
   circle(bmp, (x1), y1, 3, col2);
   break;
  case ENEMY_BOSS2:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   circlefill(bmp, (x), y, 90, col3);
   circle(bmp, (x), y, 90, col4);
   x1 = cos(angle_to_radians(enemy[dr].angle)) * (90);
   y1 = sin(angle_to_radians(enemy[dr].angle)) * (90);
   line(bmp, x + x1, y + y1, x - x1, y - y1, col4);
   x1 = cos(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * (90);
   y1 = sin(angle_to_radians(enemy[dr].angle + ANGLE_QUARTER)) * (90);
   line(bmp, x + x1, y + y1, x - x1, y - y1, col4);
   circlefill(bmp, (x), y, 27, col1);
   circle(bmp, (x), y, 27, col2);
   break;
  case ENEMY_BOSS2_TURRET:
   if (enemy[dr].recycle < 20 && enemy[dr].recycle % 4 < 2)// && enemy[dr].burst_fire <= 0)
    col1 += 3;
   circlefill(bmp, (x), y, 15, col1);
   circle(bmp, (x), y, 15, col2);
   break;
  case ENEMY_SWERVER:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = (x) + cos(radangle) * (13);
   y1 = (y) + sin(radangle) * (13);
   x2 = (x) + cos(radangle + (PI * 2) / 3) * 11;
   y2 = (y) + sin(radangle + (PI * 2) / 3) * 11;
   x3 = (x) + cos(radangle - (PI * 2) / 3) * 11;
   y3 = (y) + sin(radangle - (PI * 2) / 3) * 11;
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   line(bmp, x1, y1, x2, y2, col4);
   line(bmp, x1, y1, x3, y3, col4);
   line(bmp, x2, y2, x3, y3, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_SWERVER2:
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = (x) + cos(radangle + PI) * (11);
   y1 = (y) + sin(radangle + PI) * (11);
   x2 = (x) + cos(radangle + PI + (PI * 2) / 3) * 14;
   y2 = (y) + sin(radangle + PI + (PI * 2) / 3) * 14;
   x3 = (x) + cos(radangle + PI - (PI * 2) / 3) * 14;
   y3 = (y) + sin(radangle + PI - (PI * 2) / 3) * 14;
   triangle(bmp, x1, y1, x2, y2, x3, y3, col3);
   line(bmp, x1, y1, x2, y2, col4);
   line(bmp, x1, y1, x3, y3, col4);
   line(bmp, x2, y2, x3, y3, col4);
   circlefill(bmp, (x), y, 7, col1);
   circle(bmp, (x), y, 7, col2);
   break;
  case ENEMY_CLOAKER:
   col1 = COLOUR_BLACK;
   col2 = COLOUR_BLACK;
   col3 = COLOUR_BLACK;
   col4 = COLOUR_BLACK;
   if (enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] > 0)
   {
    col1 = enemy[dr].colours [0] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 12;
    col2 = enemy[dr].colours [1] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 6;
    if (col1 > enemy[dr].colours [0] + 7)
     col1 = enemy[dr].colours [0] + 7;
    if (col2 > enemy[dr].colours [1] + 7)
     col2 = enemy[dr].colours [1] + 7;
    col3 = enemy[dr].colours [0] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 6;
    col4 = enemy[dr].colours [1] + enemy[dr].attribute [ATTRIB_CLOAKER_PULSE] / 3;
    if (col3 > enemy[dr].colours [0] + 7)
     col3 = enemy[dr].colours [0] + 7;
    if (col4 > enemy[dr].colours [1] + 7)
     col4 = enemy[dr].colours [1] + 7;
   }
   triangle(bmp, x - 2, y - 2, x - 30, y - 2, x - 2, y - 15, col1);
   pline(bmp, x - 2, y - 2, x - 30, y - 2, col2);
   pline(bmp, x - 2, y - 2, x - 2, y - 15, col2);
   pline(bmp, x - 2, y - 15, x - 30, y - 2, col2);
   triangle(bmp, x + 2, y - 2, x + 30, y - 2, x + 2, y - 15, col1);
   pline(bmp, x + 2, y - 2, x + 30, y - 2, col2);
   pline(bmp, x + 2, y - 2, x + 2, y - 15, col2);
   pline(bmp, x + 2, y - 15, x + 30, y - 2, col2);
   triangle(bmp, x + 2, y + 2, x + 30, y + 2, x + 2, y + 15, col1);
   pline(bmp, x + 2, y + 2, x + 30, y + 2, col2);
   pline(bmp, x + 2, y + 2, x + 2, y + 15, col2);
   pline(bmp, x + 2, y + 15, x + 30, y + 2, col2);
   triangle(bmp, x - 2, y + 2, x - 30, y + 2, x - 2, y + 15, col1);
   pline(bmp, x - 2, y + 2, x - 30, y + 2, col2);
   pline(bmp, x - 2, y + 2, x - 2, y + 15, col2);
   pline(bmp, x - 2, y + 15, x - 30, y + 2, col2);
   circlefill(bmp, (x), y, 9, col3);
   circle(bmp, (x), y, 9, col4);
   break;
  case ENEMY_SUPERJELLY:
   x1 = (enemy[dr].attribute [ATTRIB_JELLY_PULSE] / 4) % 8;
   if (x1 >= 4)
    x1 = 8 - x1;
   draw_sprite(bmp, superjelly_bmp [0], (x) - 10, (y) - 24 - x1);
   draw_sprite_v_flip(bmp, superjelly_bmp [0], (x) - 10, (y) + 4 + x1);
   draw_sprite(bmp, superjelly_bmp [1], (x) + 4 + x1, (y) - 10);
   draw_sprite_h_flip(bmp, superjelly_bmp [1], (x) - 24 - x1, (y) - 10);
   circlefill(bmp, (x), y, 9, col1);
   circle(bmp, (x), y, 9, col2);
   break;
  case ENEMY_BEE:
   thing = grand(ANGLE_QUARTER);
   col4 = COLOUR_GREY4;
   radangle = angle_to_radians(enemy[dr].angle);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF - thing)) * 9;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF - thing)) * 9;
   line(bmp, x, y, x1, y1, col4);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF + thing)) * 9;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF + thing)) * 9;
   line(bmp, x, y, x1, y1, col4);
   x1 = x + cos(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 4;
   y1 = y + sin(angle_to_radians(enemy[dr].angle + ANGLE_HALF)) * 4;
   col3 = enemy[dr].colours [1];
   col4 = enemy[dr].colours [0] - 2;
   circlefill(bmp, (x1), y1, 3, col3);
   circle(bmp, (x1), y1, 3, col4);
   circlefill(bmp, (x), y, 3, col1);
   circle(bmp, (x), y, 3, col2);
   break;
  case ENEMY_JELLY:
   draw_rle_sprite(bmp, enemy1_rle [RLE_ENEMY1_JELLY1 + (enemy[dr].counter / 3) % 6], (x) - 22, (y) - 8); // 11
   circlefill(bmp, (x), y, 12, col1);
   circle(bmp, (x), y, 12, col2);
   break;
*/   
 }
#ifdef DEBUG_ENEMY_SIZE
 circle(bmp, store_x, store_y, eclass[enemy[dr].type].radius / GRAIN, COLOUR_YELLOW2 + counter % 6);
#endif

#ifdef DEBUG_DISPLAY
if (debug_info == 1)
 textprintf_centre_ex(bmp, small_font, store_x, store_y, COLOUR_GREEN8, COLOUR_GREY2, "%i", enemy[dr].type);
#endif


}

void draw_spark_jump(BITMAP *bmp, int x1, int y1, int x2, int y2, int parts, int scatter, int col, int transy)
{

    if (transy) 
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

//    int sangle = radians_to_angle(atan2(y1 - y2, x1 - x2));
    int xstep = (x2 - x1) / parts;
    int ystep = (y2 - y1) / parts;    
    
    int i, sx1 = x1, sy1 = y1, sx2, sy2;
    
    for (i = 0; i < parts; i ++)
    {
     sx2 = x1 + xstep * i;
     sy2 = y1 + ystep * i;
     
     sx2 += grand(scatter) - grand(scatter);
     sy2 += grand(scatter) - grand(scatter);
     
     line(bmp, sx1, sy1, sx2, sy2, col); 
     
     sx1 = sx2;
     sy1 = sy2;
    }
    
    line(bmp, sx1, sy1, x2, y2, col); 

    if (transy) 
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);


}

void draw_tri(BITMAP *bmp, int x, int y, int angle, int length1, int length2, int angle2, int col1, int col2)
{
 int points [6];

 points [0] = x + xpart(angle, length1); 
 points [1] = y + ypart(angle, length1); 
 points [2] = x + xpart(angle + angle2, length2); 
 points [3] = y + ypart(angle + angle2, length2); 
 points [4] = x + xpart(angle - angle2, length2); 
 points [5] = y + ypart(angle - angle2, length2); 

 triangle(bmp, points [0], points [1], points [2], points [3], points [4], points [5], col2);

 points [0] = x + xpart(angle, length1 + 1); 
 points [1] = y + ypart(angle, length1 + 1); 
 points [2] = x + xpart(angle + angle2, length2 + 1); 
 points [3] = y + ypart(angle + angle2, length2 + 1); 
 points [4] = x + xpart(angle - angle2, length2 + 1); 
 points [5] = y + ypart(angle - angle2, length2 + 1); 
 line(bmp, points [0], points [1], points [2], points [3], col1);
 line(bmp, points [0], points [1], points [4], points [5], col1);
 line(bmp, points [2], points [3], points [4], points [5], col1);

}



void draw_launchers(BITMAP *bmp, int x, int y, int angle, int d_f_centre, int diagonal, int interior_angle, int recoil1, int recoil2, int col1, int col2)
{

 int i;
 int points [8];
 int p1_x, p1_y;
 
 for (i = 0; i < 2; i ++)
 {
    if (i == 0)
    {
     p1_x = x + xpart(angle + ANGLE_QUARTER, d_f_centre);
     p1_y = y + ypart(angle + ANGLE_QUARTER, d_f_centre);
     p1_x += xpart(angle, recoil1);
     p1_y += ypart(angle, recoil1);
    }
     else 
     {
      p1_x = x + xpart(angle - ANGLE_QUARTER, d_f_centre);
      p1_y = y + ypart(angle - ANGLE_QUARTER, d_f_centre);
      p1_x += xpart(angle, recoil2);
      p1_y += ypart(angle, recoil2);
     }
    
    
    points [0] = p1_x + xpart(angle + interior_angle, diagonal);
    points [1] = p1_y + ypart(angle + interior_angle, diagonal);
    points [2] = p1_x + xpart(angle - interior_angle, diagonal);
    points [3] = p1_y + ypart(angle - interior_angle, diagonal);
    points [4] = p1_x - xpart(angle + interior_angle, diagonal);
    points [5] = p1_y - ypart(angle + interior_angle, diagonal);
    points [6] = p1_x - xpart(angle - interior_angle, diagonal);
    points [7] = p1_y - ypart(angle - interior_angle, diagonal);
    polygon(bmp, 4, points, col2);

    points [0] = p1_x + xpart(angle + interior_angle, diagonal + 1);
    points [1] = p1_y + ypart(angle + interior_angle, diagonal + 1);
    points [2] = p1_x + xpart(angle - interior_angle, diagonal + 1);
    points [3] = p1_y + ypart(angle - interior_angle, diagonal + 1);
    points [4] = p1_x - xpart(angle + interior_angle, diagonal + 1);
    points [5] = p1_y - ypart(angle + interior_angle, diagonal + 1);
    points [6] = p1_x - xpart(angle - interior_angle, diagonal + 1);
    points [7] = p1_y - ypart(angle - interior_angle, diagonal + 1);
/*    points [0] = x + xpart(attribute + angle1 + (i * angle2), outward);
    points [1] = y + ypart(attribute + angle1 + (i * angle2), outward);
    points [2] = x + xpart(attribute + angle1 + (i * angle2), inward - 2);
    points [3] = y + ypart(attribute + angle1 + (i * angle2), inward - 2);
    points [4] = x + xpart(attribute - angle1 + (i * angle2), inward - 2);
    points [5] = y + ypart(attribute - angle1 + (i * angle2), inward - 2);
    points [6] = x + xpart(attribute - angle1 + (i * angle2), outward);
    points [7] = y + ypart(attribute - angle1 + (i * angle2), outward);*/


 pline(bmp, points [0], points [1], points [2], points [3], col1);
 pline(bmp, points [2], points [3], points [4], points [5], col1);
 pline(bmp, points [4], points [5], points [6], points [7], col1);
 pline(bmp, points [0], points [1], points [6], points [7], col1);

  } 
}


void draw_squarey(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2)
{

 int i;
 int points [8];
 
 for (i = 0; i < arms; i ++)
 {

    points [0] = x + xpart(attribute + angle1 + (i * angle2) - 2, outward - 1);
    points [1] = y + ypart(attribute + angle1 + (i * angle2) - 2, outward - 1);
    points [2] = x + xpart(attribute + angle1 + (i * angle2) - 2, inward - 1);
    points [3] = y + ypart(attribute + angle1 + (i * angle2) - 2, inward - 1);
    points [4] = x + xpart(attribute - angle1 + (i * angle2) + 2, inward - 1);
    points [5] = y + ypart(attribute - angle1 + (i * angle2) + 2, inward - 1);
    points [6] = x + xpart(attribute - angle1 + (i * angle2) + 2, outward - 1);
    points [7] = y + ypart(attribute - angle1 + (i * angle2) + 2, outward - 1);
    polygon(bmp, 4, points, col2);

    points [0] = x + xpart(attribute + angle1 + (i * angle2), outward);
    points [1] = y + ypart(attribute + angle1 + (i * angle2), outward);
    points [2] = x + xpart(attribute + angle1 + (i * angle2), inward - 2);
    points [3] = y + ypart(attribute + angle1 + (i * angle2), inward - 2);
    points [4] = x + xpart(attribute - angle1 + (i * angle2), inward - 2);
    points [5] = y + ypart(attribute - angle1 + (i * angle2), inward - 2);
    points [6] = x + xpart(attribute - angle1 + (i * angle2), outward);
    points [7] = y + ypart(attribute - angle1 + (i * angle2), outward);


 pline(bmp, points [0], points [1], points [2], points [3], col1);
 pline(bmp, points [2], points [3], points [4], points [5], col1);
 pline(bmp, points [4], points [5], points [6], points [7], col1);
 pline(bmp, points [0], points [1], points [6], points [7], col1);

  } 
}

void draw_orbiter(BITMAP *bmp, int x, int y, int attribute, int out1, int out2, int out3, int angle1, int angle2, int arms, int col1, int col2)
{

 int i;
 int points [8];
 
 for (i = 0; i < arms; i ++)
 {

    points [0] = x + xpart(attribute + (i * angle2) - 2, out1 + 1);
    points [1] = y + ypart(attribute + (i * angle2) - 2, out1 + 1);
    points [2] = x + xpart(attribute + angle1 + (i * angle2) - 10, out2 - 0);
    points [3] = y + ypart(attribute + angle1 + (i * angle2) - 10, out2 - 0);
    points [4] = x + xpart(attribute + (i * angle2) - 2, out3 - 1);
    points [5] = y + ypart(attribute + (i * angle2) - 2, out3 - 1);
    points [6] = x + xpart(attribute - angle1 + (i * angle2) + 10, out2 - 0);
    points [7] = y + ypart(attribute - angle1 + (i * angle2) + 10, out2 - 0);
    polygon(bmp, 4, points, col2);

    points [0] = x + xpart(attribute + (i * angle2), out1);
    points [1] = y + ypart(attribute + (i * angle2), out1);
    points [2] = x + xpart(attribute + angle1 + (i * angle2), out2);
    points [3] = y + ypart(attribute + angle1 + (i * angle2), out2);
    points [4] = x + xpart(attribute + (i * angle2), out3);
    points [5] = y + ypart(attribute + (i * angle2), out3);
    points [6] = x + xpart(attribute - angle1 + (i * angle2), out2);
    points [7] = y + ypart(attribute - angle1 + (i * angle2), out2);
 pline(bmp, points [0], points [1], points [2], points [3], col1);
 pline(bmp, points [2], points [3], points [4], points [5], col1);
 pline(bmp, points [4], points [5], points [6], points [7], col1);
 pline(bmp, points [0], points [1], points [6], points [7], col1);

  } 
}



void draw_puffer(BITMAP *bmp, int x, int y, int angle, int number, int distance, int size, int incol, int outcol)
{

  int i, x1, y1, apart = 128 / number;

  for (i = 0; i < number; i ++)
  {
   x1 = x + xpart(((angle + i * apart) * 8) % 1024, distance);
   y1 = y + ypart(((angle + i * apart) * 8) % 1024, distance);
   circlefill(bmp, x1, y1, size, incol);
   circle(bmp, x1, y1, size, outcol);
  } 
   
}

void draw_spinner(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2)
{

 int i, x1, x2, x3, y1, y2, y3;
 
 for (i = 0; i < arms; i ++)
 {
    x1 = x + xpart(attribute + angle1 + (i * angle2) - 8, outward - 1);
    y1 = y + ypart(attribute + angle1 + (i * angle2) - 8, outward - 1);
    x2 = x + xpart(attribute - angle1 + (i * angle2) + 8, outward - 1);
    y2 = y + ypart(attribute - angle1 + (i * angle2) + 8, outward - 1);
    x3 = x - xpart(attribute + (i * angle2), inward - 3);
    y3 = y - ypart(attribute + (i * angle2), inward - 3); 
 triangle(bmp, x1, y1, x2, y2, x3, y3, col2);
    x1 = x + xpart(attribute + angle1 + (i * angle2), outward);
    y1 = y + ypart(attribute + angle1 + (i * angle2), outward);
    x2 = x + xpart(attribute - angle1 + (i * angle2), outward);
    y2 = y + ypart(attribute - angle1 + (i * angle2), outward);
    x3 = x - xpart(attribute + (i * angle2), inward);
    y3 = y - ypart(attribute + (i * angle2), inward); 
 pline(bmp, x1, y1, x2, y2, col1);
 pline(bmp, x1, y1, x3, y3, col1);
 pline(bmp, x2, y2, x3, y3, col1);


  } 
}

void draw_overspinner(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2)
{

 int i, x1, x2, x3, y1, y2, y3;
 
 for (i = 0; i < arms; i ++)
 {

    x1 = x + xpart(attribute + angle1 + (i * angle2) - 1, outward - 0);
    y1 = y + ypart(attribute + angle1 + (i * angle2) - 1, outward - 0);
    x2 = x + xpart(attribute - angle1 + (i * angle2) + 1, outward - 0);
    y2 = y + ypart(attribute - angle1 + (i * angle2) + 1, outward - 0);
    x3 = x - xpart(attribute + (i * angle2), inward - 1);
    y3 = y - ypart(attribute + (i * angle2), inward - 1); 
 triangle(bmp, x1, y1, x2, y2, x3, y3, col2);
    x1 = x + xpart(attribute + angle1 + (i * angle2), outward + 1);
    y1 = y + ypart(attribute + angle1 + (i * angle2), outward + 1);
    x2 = x + xpart(attribute - angle1 + (i * angle2), outward + 1);
    y2 = y + ypart(attribute - angle1 + (i * angle2), outward + 1);
    x3 = x - xpart(attribute + (i * angle2), inward);
    y3 = y - ypart(attribute + (i * angle2), inward); 
 pline(bmp, x1, y1, x2, y2, col1);
 pline(bmp, x1, y1, x3, y3, col1);
 pline(bmp, x2, y2, x3, y3, col1);

  } 
}

void draw_waver(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2, RLE_SPRITE *waver_rle, int waver_rle_size)
{

 int i, x1, x2, x3, y1, y2, y3;
 
 for (i = 0; i < arms; i ++)
 {
    x1 = x + xpart(attribute + (i * angle2), outward - 1 + waver_rle_size / 2 + 6) - waver_rle_size;
    y1 = y + ypart(attribute + (i * angle2), outward - 1 + waver_rle_size / 2 + 6) - waver_rle_size;
    draw_rle_sprite(bmp, waver_rle, x1, y1);
    x1 = x + xpart(attribute + angle1 + (i * angle2) - 4, outward - 1);
    y1 = y + ypart(attribute + angle1 + (i * angle2) - 4, outward - 1);
    x2 = x + xpart(attribute - angle1 + (i * angle2) + 4, outward - 1);
    y2 = y + ypart(attribute - angle1 + (i * angle2) + 4, outward - 1);
    x3 = x - xpart(attribute + (i * angle2), inward - 3);
    y3 = y - ypart(attribute + (i * angle2), inward - 3); 
 triangle(bmp, x1, y1, x2, y2, x3, y3, col2);
    x1 = x + xpart(attribute + angle1 + (i * angle2), outward - 2);
    y1 = y + ypart(attribute + angle1 + (i * angle2), outward - 2);
    x2 = x + xpart(attribute - angle1 + (i * angle2), outward - 2);
    y2 = y + ypart(attribute - angle1 + (i * angle2), outward - 2);
    x3 = x - xpart(attribute + (i * angle2), inward);
    y3 = y - ypart(attribute + (i * angle2), inward); 
// pline(bmp, x1, y1, x2, y2, col1);
 pline(bmp, x1, y1, x3, y3, col1);
 pline(bmp, x2, y2, x3, y3, col1);


  } 
}


void draw_spikey(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2, int in_correction, int out_correction)
{

 int i, x1, x2, x3, y1, y2, y3;
 
 for (i = 0; i < arms; i ++)
 {
/*   bordered_triangle(bmp, 
    x - xpart(attribute + angle1 + (i * angle2), inward), 
    y - ypart(attribute + angle1 + (i * angle2), inward), 
    x - xpart(attribute - angle1 + (i * angle2), inward), 
    y - ypart(attribute - angle1 + (i * angle2), inward), 
    x + xpart(attribute + (i * angle2), outward), 
    y + ypart(attribute + (i * angle2), outward), 
    col2, col1);*/
    x1 = x - xpart(attribute + angle1 + (i * angle2), inward + in_correction);
    y1 = y - ypart(attribute + angle1 + (i * angle2), inward + in_correction);
    x2 = x - xpart(attribute - angle1 + (i * angle2), inward + in_correction);
    y2 = y - ypart(attribute - angle1 + (i * angle2), inward + in_correction);
    x3 = x + xpart(attribute + (i * angle2), outward + out_correction);
    y3 = y + ypart(attribute + (i * angle2), outward + out_correction); 
 triangle(bmp, x1, y1, x2, y2, x3, y3, col2);
    x1 = x - xpart(attribute + angle1 + (i * angle2), inward);
    y1 = y - ypart(attribute + angle1 + (i * angle2), inward);
    x2 = x - xpart(attribute - angle1 + (i * angle2), inward);
    y2 = y - ypart(attribute - angle1 + (i * angle2), inward);
    x3 = x + xpart(attribute + (i * angle2), outward);
    y3 = y + ypart(attribute + (i * angle2), outward); 
 pline(bmp, x1, y1, x2, y2, col1);
 pline(bmp, x1, y1, x3, y3, col1);
 pline(bmp, x2, y2, x3, y3, col1);

  } 
}

void draw_blatter(BITMAP *bmp, int x, int y, int number, int distance, int rotation, int size, int col1, int col2)
{
 int i, xp, yp;
 int angle_difference = ANGLE_FULL / number;
 
 for (i = 0; i < number; i ++)
 {
  xp = x + xpart(rotation + angle_difference * i, distance);
  yp = y + ypart(rotation + angle_difference * i, distance);
  circlefill(bmp, xp, yp, size, col2);
  circle(bmp, xp, yp, size, col1);
 }

}


int get_circler_colour(int x1)
{
     switch(x1)
     {
      case 0: return TRANS_YELLOW;
      case 1: return TRANS_ORANGE;
      case 2: return TRANS_LGREY;
      case 3: return TRANS_DGREY;
     }

return TRANS_PURPLE;
}

void draw_clouds(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 int i, x, y, x2 = 0, y2 = 0;

 for (i = 0; i < NO_CLOUDS; i ++)
 {
  if (cloud[i].type == CLOUD_NONE)
   continue;
  if (cloud[i].x + 24000 < centre_x - (max_x / 2) * GRAIN
      || cloud[i].x - 24000 > centre_x + (max_x / 2) * GRAIN
      || cloud[i].y + 24000 < centre_y - (max_y / 2) * GRAIN
      || cloud[i].y - 24000 > centre_y + (max_y / 2) * GRAIN)
      continue;
  x = (cloud[i].x / GRAIN) - (centre_x / GRAIN);
  y = (cloud[i].y / GRAIN)  - (centre_y / GRAIN);
  x2 = (cloud[i].x2 / GRAIN)  - (centre_x / GRAIN);
  y2 = (cloud[i].y2 / GRAIN)  - (centre_y / GRAIN);
  draw_a_cloud(bmp, i, x + (max_x / 2), y + (max_y / 2), x2 + (max_x / 2), y2 + (max_y / 2));
  
 }

}



void draw_a_cloud(BITMAP *bmp, int dr, int x, int y, int x2, int y2)
{
 int xa, ya, xb, yb, xd, yd, cs, c2, points [8];

#ifdef DEBUG_DISPLAY
 if (debug_info == 1)
   textprintf_ex(bmp, small_font, x, y, COLOUR_BLUE8, COLOUR_BLACK, "ty%i to%i ", cloud[dr].type, cloud[dr].timeout);
#endif

 switch(cloud[dr].type)
 {
  case CLOUD_SHATTER1:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   for (cs = 0; cs < 8 ; cs ++)
   {
    c2 = (cloud[dr].timeout * 12) + cs * ANGLE_FULL / 8;
    if (dr % 2 == 0)
     c2 = ANGLE_FULL - ((cloud[dr].timeout * 12) + cs * ANGLE_FULL / 8);
/*    switch(cs)
    {
     default:
     case 0: xa = TRANS_LBLUE; break;
     case 1: xa = TRANS_DBLUE; break;
     case 2: xa = TRANS_DRED; break;
     case 3: xa = TRANS_LRED; break;
     case 4: xa = TRANS_ORANGE; break;
     case 5: xa = TRANS_PURPLE; break;
    }*/
    xa = TRANS_DRED;
    if (cs % 2 == 1)
     xa = TRANS_ORANGE;
    xb = cloud[dr].timeout;
//    if (xb > 12) xb = 12;
    if (cs % 2 == 0)
     xb = (xb * 13) / 10;
    triangle(bmp, x, y,
     x + xpart(c2, xb), y + ypart(c2, xb),
     x + xpart(c2 + ANGLE_FULL / 8, xb), y + ypart(c2 + ANGLE_FULL / 8, xb),
     xa);
   }
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);      
   break;
  case CLOUD_SHATTER2:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   c2 = cloud[dr].angle;
   xa = TRANS_WHITE;
   xb = cloud[dr].timeout;   
   if (xb < 12)
    xa = TRANS_LBLUE; // purple
   if (xb < 9)
    xa = TRANS_LBLUE;
   if (xb < 5)
    xa = TRANS_DBLUE;
   x2 = 34 - xb;
   if (x2 < 0)
    x2 = 0; 
   y2 = xb * 2; // angle - starts at 128 and falls
   if (y2 > 64)
    y2 = 64;
   ya = 47 - xb / 3; 
   for (cs = 0; cs < 8 ; cs ++)
   {
    triangle(bmp, x + xpart(c2 + (cs * ANGLE_1_EIGHTH), x2), y + ypart(c2 + (cs * ANGLE_1_EIGHTH), x2),
     x + xpart(c2 - y2 + (cs * ANGLE_1_EIGHTH), ya), y + ypart(c2 - y2 + (cs * ANGLE_1_EIGHTH), ya),
     x + xpart(c2 + y2 + (cs * ANGLE_1_EIGHTH), ya), y + ypart(c2 + y2 + (cs * ANGLE_1_EIGHTH), ya),
     xa);
   }
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);      
   break;
  case CLOUD_TWISTY_CIRCLE:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   c2 = cloud[dr].timeout / 5;
   yd = (50 - cloud[dr].timeout) / 2;
   xb = cloud[dr].colour [0];
   if (cloud[dr].timeout < 20)
    xb = cloud[dr].colour [1];
   if (cloud[dr].timeout < 15)
    xb = cloud[dr].colour [2];
   if (cloud[dr].timeout < 10)
    xb = cloud[dr].colour [3];
   for (xa = 0; xa < 6; xa ++)
   {
    cs = (((cloud[dr].timeout % 32) * 32) + cloud[dr].angle + xa * (ANGLE_FULL / 6)) * cloud[dr].status;
    cs += ANGLE_FULL;
    cs %= ANGLE_FULL;
    circlefill(bmp, 
     x + (xpart(cs, yd)), 
     y + (ypart(cs, yd)), 
     c2, xb);
   }
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);      
  
   break;
  case CLOUD_TWIRLY_CIRCLE:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   c2 = cloud[dr].timeout / 5;
   yd = (50 - cloud[dr].timeout);
   xb = TRANS_YELLOW;
   if (cloud[dr].timeout < 20)
    xb = TRANS_ORANGE;
   if (cloud[dr].timeout < 15)
    xb = TRANS_LRED;
   if (cloud[dr].timeout < 10)
    xb = TRANS_DRED;
   for (xa = 0; xa < 6; xa ++)
   {
    cs = 50 - cloud[dr].timeout + xa * (ANGLE_FULL / 6);
    circlefill(bmp, 
     x + (xpart(cs, yd)), 
     y + (ypart(cs, yd)), 
     c2, xb);
   }
   circlefill(bmp, x, y, cloud[dr].timeout / 4 + 1, TRANS_YELLOW);
   circlefill(bmp, x, y, cloud[dr].timeout / 3 + 1, TRANS_LGREEN);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);      
   break;
  case CLOUD_FLAK_BURST:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   switch((int) cloud[dr].timeout / 10)
   {
    default:
//    case 0: xa = TRANS_DGREY; break;
    case 1: xa = TRANS_DRED; break;
    case 2: xa = TRANS_LRED; break;
    case 3: xa = TRANS_ORANGE; break;
    case 4: xa = TRANS_YELLOW; break;
    case 5: xa = TRANS_WHITE; break;
   }
//   circlefill(bmp, x, y, 8 + cloud[dr].status * (7 - (cloud[dr].timeout / 10)), xa);
   circlefill(bmp, x, y, 3 + cloud[dr].status * (7 - (cloud[dr].timeout / 10)), xa);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);      
   break;
  case CLOUD_BLUE_BLOB: 
   if (cloud[dr].timeout > 7)
   {
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_3 + counter % 2], (x) - 2, (y) - 2);
    break;
   } 
   if (cloud[dr].timeout > 4)
   {
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_5], (x) - 1, (y) - 1);
    break;
   } 
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   if (cloud[dr].timeout > 2)
    putpixel(bmp, x, y, TRANS_LBLUE);
     else
      putpixel(bmp, x, y, TRANS_DBLUE);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;
  case CLOUD_ORANGE_BLOB: 
/*   if (cloud[dr].timeout > 7)
   {
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_ORANGE_BLOB_3], (x) - 2, (y) - 2);
    break;
   } 
   if (cloud[dr].timeout > 4)
   {
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_5], (x) - 1, (y) - 1);
    break;
   } */
   if (cloud[dr].timeout > 2)
   {
    x2 = (cloud[dr].timeout - 2) / 2;
    if (x2 > 2) x2 = 2;
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_ORANGE_BLOB_7 - x2], (x) - 1 - x2, (y) - 1 - x2);
    break;
   }   
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   if (cloud[dr].timeout > 0)
    putpixel(bmp, x, y, TRANS_LRED);
     else
      putpixel(bmp, x, y, TRANS_DRED);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;
  case CLOUD_ORANGE_BLOB2:
   if (cloud[dr].timeout > 4)
   {
    x2 = (cloud[dr].timeout - 4);// / 2;
    if (x2 > 6) x2 = 6;
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_ORANGE_BLOB_7 - x2], (x) - 1 - x2 / 2, (y) - 1 - x2 / 2
    );
    break;
   }   
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   if (cloud[dr].timeout > 2)
    putpixel(bmp, x, y, TRANS_LRED);
     else
      putpixel(bmp, x, y, TRANS_DRED);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;
  case CLOUD_BLUE_BLOB2: 
   if (cloud[dr].timeout > 4)
   {
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_1], (x) - 3, (y) - 3);
    break;
   } 
   if (cloud[dr].timeout > 3)
   {
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_3], (x) - 2, (y) - 2);
    break;
   } 
   if (cloud[dr].timeout > 2)
   {
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_5], (x) - 1, (y) - 1);
    break;
   } 
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   if (cloud[dr].timeout > 1)
    putpixel(bmp, x, y, TRANS_LBLUE);
     else
      putpixel(bmp, x, y, TRANS_DBLUE);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;
  case CLOUD_GREEN_BLOB: 
   if (cloud[dr].timeout > 2)
   {
    x2 = (cloud[dr].timeout - 2) / 2;
    if (x2 > 2) x2 = 2;
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_GREEN_BLOB_7 - x2], (x) - 1 - x2, (y) - 1 - x2);
    break;
   }   
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   if (cloud[dr].timeout > 0)
    putpixel(bmp, x, y, TRANS_LGREEN);
     else
      putpixel(bmp, x, y, TRANS_DGREEN);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;
  case CLOUD_GREEN_BLOB2:
   if (cloud[dr].timeout > 4)
   {
    x2 = (cloud[dr].timeout - 4);// / 2;
    if (x2 > 6) x2 = 6;
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_GREEN_BLOB_7 - x2], (x) - 1 - x2 / 2, (y) - 1 - x2 / 2
    );
    break;
   }   
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   if (cloud[dr].timeout > 2)
    putpixel(bmp, x, y, TRANS_LGREEN);
     else
      putpixel(bmp, x, y, TRANS_DGREEN);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;
/*  case CLOUD_BLUE_BLOB2:
   if (cloud[dr].timeout > 4)
   {
    x2 = (cloud[dr].timeout - 4);// / 2;
    if (x2 > 6) x2 = 6;
    draw_trans_rle_sprite(bmp, small3_rle [RLE_SMALL3_BLUE_BLOB_7 - x2], (x) - 1 - x2 / 2, (y) - 1 - x2 / 2
    );
    break;
   }   
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   if (cloud[dr].timeout > 2)
    putpixel(bmp, x, y, TRANS_LBLUE);
     else
      putpixel(bmp, x, y, TRANS_DBLUE);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);   
   break;*/
  case CLOUD_BANG:
   xa = cloud[dr].timeout / 10;
   if (xa > 50)
    xa = 50;  
   draw_trans_sprite(bmp, redbang_bmp [xa], x - xa, y - xa);
   break; 
  case CLOUD_TRAIL1:
  circlefill(bmp, x, y, cloud[dr].timeout / 20 + 1, COLOUR_RED8);
   break;
  case CLOUD_FADING_LINE:
   if ((cloud[dr].timeout / 2) > 3)
    xa = cloud[dr].colour [3];
     else
     {
      if (cloud[dr].timeout == 1)
       xa = cloud[dr].colour [0];
        else
         xa = cloud[dr].colour [(cloud[dr].timeout) / 2 - 1];
     } 
//   if (cloud[dr].timeout > 4)
//    xa = cloud[dr].colour [0];
//     else
//      xa = cloud[dr].colour [4 - cloud[dr].timeout];
   pline(bmp, x, y, x2, y2, xa);
   break;
  case CLOUD_TRANS_FADING_LINE:
   if ((cloud[dr].timeout / 2) > 3)
    xa = cloud[dr].colour [3];
     else
     {
      if (cloud[dr].timeout == 1)
       xa = cloud[dr].colour [0];
        else
         xa = cloud[dr].colour [(cloud[dr].timeout) / 2 - 1];
     } 
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   pline(bmp, x, y, x2, y2, xa);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_SHRINKING_FADING_CIRCLE:
   cs = cloud[dr].timeout / 100;
   if (cloud[dr].tickrate < 0)
   {
      xa = cloud[dr].colour [0];
   }
    else
    {
     xa = cloud[dr].colour [3];
     if (cs > 10)
      xa = cloud[dr].colour [2];
     if (cs > 20)
      xa = cloud[dr].colour [1];
     if (cs > 35)
      xa = cloud[dr].colour [0];
    }  
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circle(bmp, x, y, cs, xa);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_SHRINKING_CIRCLE:
   cs = cloud[dr].timeout / 100;
   if (cs > 7)
    cs = 7;
   xa = cloud[dr].colour [0] - 7 + cs;
   circle(bmp, x, y, cloud[dr].timeout / 100, xa);
   break;
  case CLOUD_SMALL_SHRINKING_CIRCLE:
   cs = cloud[dr].timeout / 100;
   if (cs > 3)
    cs = 3;
   xa = cloud[dr].colour [0] - 6 + cs * 2;
   circle(bmp, x, y, cloud[dr].timeout / 100, xa);
   break;
  case CLOUD_SHRINKING_LINE:
   // colour [0] must be transparent, [1] non-transparent
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout < 140)
    {
     cs = cloud[dr].colour [1];
     cs -= 6;
     cs += cloud[dr].timeout / 20;
    }
     else
      drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   xa = x + xpart(cloud[dr].angle, cloud[dr].timeout / 10);
   ya = y + ypart(cloud[dr].angle, cloud[dr].timeout / 10);
//   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   pline(bmp, x, y, xa, ya, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_LINE_SHADOW:
   cs = cloud[dr].angle + ANGLE_QUARTER;
   c2 = cloud[dr].timeout / 10;
   xa = x + xpart(cs, c2);
   ya = y + ypart(cs, c2);
   xb = x - xpart(cs, c2);
   yb = y - ypart(cs, c2);
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   pline(bmp, xa, ya, xb, yb, cloud[dr].colour [0]);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_SHIELD_LINE:
   cs = cloud[dr].angle - ANGLE_QUARTER;
   c2 = cloud[dr].timeout / 3;
   xa = x + xpart(cs, c2);
   ya = y + ypart(cs, c2);
   xb = x - xpart(cs, c2);
   yb = y - ypart(cs, c2);
   xd = cloud[dr].timeout / 20;
   if (xd > 4)
    xd = 4;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   pline(bmp, xa, ya, xb, yb, cloud[dr].colour [xd]);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

/*  case CLOUD_SHIELD_LINE:
   cs = ANGLE_FULL - ((cloud[dr].angle - ANGLE_HALF) % ANGLE_FULL);
   c2 = cloud[dr].timeout / 3;
//   xa = x + xpart(cs, c2);
//   ya = y + ypart(cs, c2);
//   xb = x - xpart(cs, c2);
//   yb = y - ypart(cs, c2);
   xd = cloud[dr].timeout / 20;
   if (xd > 4)
    xd = 4;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   arc(bmp, x, y, itofix(cs / 4 - c2), itofix(cs / 4 + c2), cloud[dr].status / GRAIN, cloud[dr].colour [xd]);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
*/
   break;
  case CLOUD_SQUAREY:
    points [0] = x + xpart(cloud[dr].angle + ANGLE_1_SIXTEENTH - 2, 40 - 1);
    points [1] = y + ypart(cloud[dr].angle + ANGLE_1_SIXTEENTH - 2, 40 - 1);
    points [2] = x + xpart(cloud[dr].angle + ANGLE_1_SIXTEENTH - 2, 22 - 1);
    points [3] = y + ypart(cloud[dr].angle + ANGLE_1_SIXTEENTH - 2, 22 - 1);
    points [4] = x + xpart(cloud[dr].angle - ANGLE_1_SIXTEENTH + 2, 22 - 1);
    points [5] = y + ypart(cloud[dr].angle - ANGLE_1_SIXTEENTH + 2, 22 - 1);
    points [6] = x + xpart(cloud[dr].angle - ANGLE_1_SIXTEENTH + 2, 40 - 1);
    points [7] = y + ypart(cloud[dr].angle - ANGLE_1_SIXTEENTH + 2, 40 - 1);
    polygon(bmp, 4, points, GC_BLUE5);

    points [0] = x + xpart(cloud[dr].angle + ANGLE_1_SIXTEENTH, 40);
    points [1] = y + ypart(cloud[dr].angle + ANGLE_1_SIXTEENTH, 40);
    points [2] = x + xpart(cloud[dr].angle + ANGLE_1_SIXTEENTH, 22 - 2);
    points [3] = y + ypart(cloud[dr].angle + ANGLE_1_SIXTEENTH, 22 - 2);
    points [4] = x + xpart(cloud[dr].angle - ANGLE_1_SIXTEENTH, 22 - 2);
    points [5] = y + ypart(cloud[dr].angle - ANGLE_1_SIXTEENTH, 22 - 2);
    points [6] = x + xpart(cloud[dr].angle - ANGLE_1_SIXTEENTH, 40);
    points [7] = y + ypart(cloud[dr].angle - ANGLE_1_SIXTEENTH, 40);

/*    points [0] = x + xpart(ANGLE_1_SIXTEENTH + cloud[dr].angle - 2, 40 - 1);
    points [1] = y + ypart(ANGLE_1_SIXTEENTH + cloud[dr].angle - 2, 40 - 1);
    points [2] = x + xpart(ANGLE_1_SIXTEENTH + cloud[dr].angle - 2, 22 - 1);
    points [3] = y + ypart(ANGLE_1_SIXTEENTH + cloud[dr].angle - 2, 22 - 1);
    points [4] = x + xpart(ANGLE_1_SIXTEENTH - cloud[dr].angle + 2, 22 - 1);
    points [5] = y + ypart(ANGLE_1_SIXTEENTH - cloud[dr].angle + 2, 22 - 1);
    points [6] = x + xpart(ANGLE_1_SIXTEENTH - cloud[dr].angle + 2, 40 - 1);
    points [7] = y + ypart(ANGLE_1_SIXTEENTH - cloud[dr].angle + 2, 40 - 1);
    polygon(bmp, 4, points, GC_BLUE5);

    points [0] = x + xpart(ANGLE_1_SIXTEENTH + cloud[dr].angle, 40);
    points [1] = y + ypart(ANGLE_1_SIXTEENTH + cloud[dr].angle, 40);
    points [2] = x + xpart(ANGLE_1_SIXTEENTH + cloud[dr].angle, 22 - 2);
    points [3] = y + ypart(ANGLE_1_SIXTEENTH + cloud[dr].angle, 22 - 2);
    points [4] = x + xpart(ANGLE_1_SIXTEENTH - cloud[dr].angle, 22 - 2);
    points [5] = y + ypart(ANGLE_1_SIXTEENTH - cloud[dr].angle, 22 - 2);
    points [6] = x + xpart(ANGLE_1_SIXTEENTH - cloud[dr].angle, 40);
    points [7] = y + ypart(ANGLE_1_SIXTEENTH - cloud[dr].angle, 40);
*/
    pline(bmp, points [0], points [1], points [2], points [3], GC_BLUE1);
    pline(bmp, points [2], points [3], points [4], points [5], GC_BLUE1);
    pline(bmp, points [4], points [5], points [6], points [7], GC_BLUE1);
    pline(bmp, points [0], points [1], points [6], points [7], GC_BLUE1);
    break; 
  case CLOUD_BLOCK1:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 7)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 5)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 2)
    cs = cloud[dr].colour [3];
   points [0] = x + xpart(cloud[dr].angle + ANGLE_QUARTER, cloud[dr].timeout);    
   points [1] = y + ypart(cloud[dr].angle + ANGLE_QUARTER, cloud[dr].timeout);    
   points [2] = x + xpart(cloud[dr].angle - ANGLE_QUARTER, cloud[dr].timeout);    
   points [3] = y + ypart(cloud[dr].angle - ANGLE_QUARTER, cloud[dr].timeout);    
   points [4] = x2 + xpart(cloud[dr].angle - ANGLE_QUARTER, cloud[dr].timeout);    
   points [5] = y2 + ypart(cloud[dr].angle - ANGLE_QUARTER, cloud[dr].timeout);    
   points [6] = x2 + xpart(cloud[dr].angle + ANGLE_QUARTER, cloud[dr].timeout);    
   points [7] = y2 + ypart(cloud[dr].angle + ANGLE_QUARTER, cloud[dr].timeout);    
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   polygon(bmp, 4, points, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_BLOCK2:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 60)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 40)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 20)
    cs = cloud[dr].colour [3];
   int points2 [8];
   c2 = (cloud[dr].timeout * 5) % ANGLE_FULL;
   if (cloud[dr].status == 1)
    c2 = ANGLE_FULL - c2;
   xd = cloud[dr].timeout / 3; 
   points2 [0] = x + xpart(c2, xd);
   points2 [1] = y + ypart(c2, xd);
   points2 [2] = x + xpart(c2 + ANGLE_QUARTER, xd);    
   points2 [3] = y + ypart(c2 + ANGLE_QUARTER, xd);    
   points2 [4] = x + xpart(c2 + ANGLE_HALF, xd);    
   points2 [5] = y + ypart(c2 + ANGLE_HALF, xd);    
   points2 [6] = x + xpart(c2 - ANGLE_QUARTER, xd);    
   points2 [7] = y + ypart(c2 - ANGLE_QUARTER, xd);    
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   polygon(bmp, 4, points2, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
   case CLOUD_BLOCK3:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 60)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 40)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 20)
    cs = cloud[dr].colour [3];
   int points3 [8];
   c2 = (cloud[dr].timeout + cloud[dr].angle) % ANGLE_FULL;
   if (cloud[dr].status == 1)
    c2 = ANGLE_FULL - c2;
//   c2 = ANGLE_FULL - cloud[dr].angle; 
   xd = cloud[dr].timeout / 3; 
   yd = (100 - cloud[dr].timeout) / 2;
   if (yd < 3) yd = 3;
   points3 [0] = x + xpart(c2, xd);
   points3 [1] = y + ypart(c2, xd);
   points3 [2] = x + xpart(c2 + ANGLE_QUARTER, xd);    
   points3 [3] = y + ypart(c2 + ANGLE_QUARTER, xd);    
   points3 [4] = x + xpart(c2 + ANGLE_HALF, xd);    
   points3 [5] = y + ypart(c2 + ANGLE_HALF, xd);    
   points3 [6] = x + xpart(c2 - ANGLE_QUARTER, xd);    
   points3 [7] = y + ypart(c2 - ANGLE_QUARTER, xd);    
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   polygon(bmp, 4, points3, cs);
   xd /= 2;
   c2 += ANGLE_1_EIGHTH;
   points3 [0] = x + xpart(c2, xd);
   points3 [1] = y + ypart(c2, xd);
   points3 [2] = x + xpart(c2 + ANGLE_QUARTER, yd);
   points3 [3] = y + ypart(c2 + ANGLE_QUARTER, yd);
   points3 [4] = x + xpart(c2 + ANGLE_HALF, xd);
   points3 [5] = y + ypart(c2 + ANGLE_HALF, xd);
   points3 [6] = x + xpart(c2 - ANGLE_QUARTER, yd);
   points3 [7] = y + ypart(c2 - ANGLE_QUARTER, yd);
   polygon(bmp, 4, points3, cs);
   points3 [0] = x + xpart(c2, yd);
   points3 [1] = y + ypart(c2, yd);
   points3 [2] = x + xpart(c2 + ANGLE_QUARTER, xd);
   points3 [3] = y + ypart(c2 + ANGLE_QUARTER, xd);
   points3 [4] = x + xpart(c2 + ANGLE_HALF, yd);
   points3 [5] = y + ypart(c2 + ANGLE_HALF, yd);
   points3 [6] = x + xpart(c2 - ANGLE_QUARTER, xd);
   points3 [7] = y + ypart(c2 - ANGLE_QUARTER, xd);
   polygon(bmp, 4, points3, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
/*   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 60)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 40)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 20)
    cs = cloud[dr].colour [3];
   int points2 [8];
   c2 = (cloud[dr].timeout * 5) % ANGLE_FULL;
   if (cloud[dr].status == 1)
    c2 = ANGLE_FULL - c2;
   xd = cloud[dr].timeout / 3; 
   yd = 100 - cloud[dr].timeout;
   if (yd < 3) yd = 3;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   rectfill(bmp, x - xd, y - xd, x + xd, y + xd, cs);
   rectfill(bmp, x - xd / 4, y - yd * 2, x + xd / 4, y + yd * 2, cs);
   rectfill(bmp, x - yd * 2, y - xd / 4, x + yd * 2, y + xd / 4, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);*/
   break;
  case CLOUD_BLOCK4:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 7)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 5)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 3)
    cs = cloud[dr].colour [3];
   int points4 [8];
   xd = cloud[dr].timeout / 2;
   points4 [0] = x + xpart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   points4 [1] = y + ypart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   points4 [2] = x + xpart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points4 [3] = y + ypart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points4 [4] = x2 + xpart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points4 [5] = y2 + ypart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points4 [6] = x2 + xpart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   points4 [7] = y2 + ypart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   polygon(bmp, 4, points4, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_BLOCK5:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 70)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 50)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 30)
    cs = cloud[dr].colour [3];
   int points5 [8];
   xd = cloud[dr].status;
   points5 [0] = x + xpart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   points5 [1] = y + ypart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   points5 [2] = x + xpart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points5 [3] = y + ypart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points5 [4] = x2 + xpart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points5 [5] = y2 + ypart(cloud[dr].angle - ANGLE_QUARTER, xd);    
   points5 [6] = x2 + xpart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   points5 [7] = y2 + ypart(cloud[dr].angle + ANGLE_QUARTER, xd);    
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   polygon(bmp, 4, points5, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_THICK_SHOCK_CIRCLE:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 7)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 5)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 3)
    cs = cloud[dr].colour [3];
   xd = cloud[dr].timeout;// / 2;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, xd, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;

  case CLOUD_SPECK:
   xa = cloud[dr].colour [0];
   if (cloud[dr].timeout < 14)
    xa = cloud[dr].colour [1];
   if (cloud[dr].timeout < 11)
    xa = cloud[dr].colour[2];
   if (cloud[dr].timeout < 8)
    xa = cloud[dr].colour[3];
   if (cloud[dr].timeout < 4)
    xa = cloud[dr].colour[4];
   putpixel(bmp, x, y, xa);
   break;
  case CLOUD_FLECK:
    cs = cloud[dr].colour [0];
   if (cloud[dr].timeout < 8)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout < 6)
    cs = cloud[dr].colour[2];
   if (cloud[dr].timeout < 4)
    cs = cloud[dr].colour[3];
   if (cloud[dr].timeout < 2)
    cs = cloud[dr].colour[4];
   xb = x - xpart(cloud[dr].angle, cloud[dr].timeout); 
   yb = y - ypart(cloud[dr].angle, cloud[dr].timeout); 
   pline(bmp, x, y, xb, yb, cs);
//   circlefill(bmp, x, y, 4, cs);
   break;
  case CLOUD_SMALL_FADING_CIRCLE:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 30)
    cs -= (30 - cloud[dr].timeout) / 5;
//    cs -= 2;
//     else
//      if (cloud[dr].timeout <= 20)
//       cs -= 4;
//        else
//         if (cloud[dr].timeout <= 10)
//          cs -= 6;
//   cs += (cloud[dr].timeout * 8) / cloud[dr].status - 8;
   circlefill(bmp, x, y, cloud[dr].timeout / 10, cs);
   break;
  case CLOUD_LARGE_FADING_CIRCLE:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout < 300)
    cs -= (300 - cloud[dr].timeout) / 50;
//    cs -= 2;
//     else
//      if (cloud[dr].timeout <= 200)
//       cs -= 4;
//        else
//         if (cloud[dr].timeout <= 100)
//          cs -= 6;
//   cs += (cloud[dr].timeout * 8) / cloud[dr].status - 8;
   circlefill(bmp, x, y, cloud[dr].timeout / 10, cs);
   break;
  case CLOUD_FLAME_CIRCLE: 
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, cloud[dr].timeout / 20, cloud[dr].colour [0]);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_BIG_BLAST_CIRCLE: // drags 
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 60)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 45)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 30)
    cs = cloud[dr].colour [3];
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, cloud[dr].timeout / 10, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_LARGE_TRANS_FADING_CIRCLE:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 50)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 35)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 20)
    cs = cloud[dr].colour [3];
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, cloud[dr].timeout / 10, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_DRIVE_CIRCLE: // identical except for drag 
  case CLOUD_TRANS_FADING_CIRCLE:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 22)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 19)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 14)
    cs = cloud[dr].colour [3];
//    cs -= 2;
//     else
//      if (cloud[dr].timeout <= 20)
//       cs -= 4;
//        else
//         if (cloud[dr].timeout <= 10)
//          cs -= 6;
//   cs += (cloud[dr].timeout * 8) / cloud[dr].status - 8;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, cloud[dr].timeout / 10, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_SEEKER_CIRCLE: // just like below but drags
  case CLOUD_MED_TRANS_FADING_CIRCLE:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 32)
    cs = cloud[dr].colour [1];
   if (cloud[dr].timeout <= 25)
    cs = cloud[dr].colour [2];
   if (cloud[dr].timeout <= 16)
    cs = cloud[dr].colour [3];
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, cloud[dr].timeout / 10, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_BLAST_CIRCLE:
   cs = cloud[dr].colour [0];
   if (cloud[dr].timeout <= 25)
    cs = cloud[dr].colour [1];
//     else
//     {
      if (cloud[dr].timeout <= 15)
       cs = cloud[dr].colour [2];
//        else
//        {
         if (cloud[dr].timeout <= 9)
          cs = cloud[dr].colour [3];
//        }
//     }  
//   cs += (cloud[dr].timeout * 8) / cloud[dr].status - 8;
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, cloud[dr].timeout / 10, cs);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_SMALL_TRANS_CIRCLE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  circlefill(bmp, x, y, cloud[dr].timeout / 100, cloud[dr].colour [0]);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_SMALL_CIRCLE:
  circlefill(bmp, x, y, cloud[dr].timeout / 100, cloud[dr].colour [0]);
   break;
  case CLOUD_MED_TRANS_CIRCLE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  circlefill(bmp, x, y, cloud[dr].timeout / 70, cloud[dr].colour [0]);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_DISTORT:
    clear_bitmap(distortion_mask);
    circlefill(distortion_mask, cloud[dr].timeout / 70, cloud[dr].timeout / 70, 9, 1);
    distortion(bmp, x, y, 19, 19, grand(31) - 15, grand(31) - 15);  
    break; 
  case CLOUD_MED_CIRCLE:
  circlefill(bmp, x, y, cloud[dr].timeout / 70, cloud[dr].colour [0]);
   break;
  case CLOUD_LARGE_TRANS_CIRCLE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  circlefill(bmp, x, y, cloud[dr].timeout / 40, cloud[dr].colour [0]);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_LARGE_CIRCLE:
  circlefill(bmp, x, y, cloud[dr].timeout / 40, cloud[dr].colour [0]);
   break;
  case CLOUD_SHOCKWAVE:
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
//  if (cloud[dr].timeout > 80)
//   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10, cloud[dr].colour [4]);
  if (cloud[dr].timeout > 90)
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 4, cloud[dr].colour [3]);
  if (cloud[dr].timeout > 60)
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 3, cloud[dr].colour [2]);
  if (cloud[dr].timeout > 30)
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 2, cloud[dr].colour [1]);
  circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 1, cloud[dr].colour [0]);
//  text_mode(COLOUR_RED8);
//  textprintf_ex(bmp, small_font, x, y, COLOUR_BLUE8, COLOUR_BLACK, "s %i t% i ", cloud[dr].status, cloud[dr].timeout);
//  text_mode(-1);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case CLOUD_GROWING_CIRCLE:
   cs = cloud[dr].colour [0];
   c2 = (cloud[dr].timeout - cloud[dr].status) / 10;
   if (c2 < -5)
    c2 = -5;
   cs += c2;
   circle(bmp, x, y, (cloud[dr].status - cloud[dr].timeout) / 10 + 4, cs);
   break;
  case CLOUD_PULSER1_V:
      if (cloud[dr].status == 1)
       draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER1_V], x, y);
        else
         draw_sprite_v_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER1_V], x, y);
   break; 
  case CLOUD_PULSER1_H:  
      if (cloud[dr].status == 0)
       draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER1_H], x, y);
        else 
         draw_sprite_h_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER1_H], x, y);
    break;
  case CLOUD_PULSER2_V:
      if (cloud[dr].status == 1)
       draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER2_V], x, y);
        else
         draw_sprite_v_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER2_V], x, y);
   break;
  case CLOUD_PULSER2_H:
      if (cloud[dr].status == 0)
       draw_sprite(bmp, enemy_bmps [BMP_ENEMY_PULSER2_H], x, y);
        else
         draw_sprite_h_flip(bmp, enemy_bmps [BMP_ENEMY_PULSER2_H], x, y);
   break;
/*  case CLOUD_TRI1:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER1], x - 36, y - 36);
   break;
  case CLOUD_TRI2:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER2], x - 35, y - 35);
   break;
  case CLOUD_TRI3:
   draw_rle_sprite(bmp, enemy9_rle [RLE_ENEMY9_TRIANGLER3], x - 35, y - 35);
   break;*/

 }

}




void draw_lights(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 int i, x, y;

 for (i = 0; i < NO_LIGHTS; i ++)
 {
  if (light[i].type == LIGHT_NONE)
   break; 

  if (light[i].x + light[i].size * GRAIN < centre_x - (max_x / 2) * GRAIN
      || light[i].x - light[i].size * GRAIN > centre_x + (max_x / 2) * GRAIN
      || light[i].y + light[i].size * GRAIN < centre_y - (max_y / 2) * GRAIN
      || light[i].y - light[i].size * GRAIN > centre_y + (max_y / 2) * GRAIN)
      continue;
  x = (light[i].x / GRAIN) - (centre_x / GRAIN);
  y = (light[i].y / GRAIN)  - (centre_y / GRAIN);
  draw_a_light(bmp, light[i].size, x + (max_x / 2), y + (max_y / 2));
  
 }

}

void draw_effects(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 int i, x, y, x1 = 0, y1 = 0, x2 = 0, y2 = 0;


 for (i = 0; i < MAX_EFFECTS; i ++)
 {
  if (effect[i].type == EFFECT_NONE)
   continue;
  if (effect[i].x1 > effect[i].x2)
  {
   x1 = effect[i].x2;
   x2 = effect[i].x1;
  }
   else
   {
    x1 = effect[i].x1;
    x2 = effect[i].x2; 
   }
  if (effect[i].y1 > effect[i].y2)
  {
   y1 = effect[i].y2;
   y2 = effect[i].y1;
  }
   else
   {
    y1 = effect[i].y1;
    y2 = effect[i].y2; 
   }
  if (x2 < centre_x - (max_x / 2) * GRAIN
      || x1 > centre_x + (max_x / 2) * GRAIN
      || y2 < centre_y - (max_y / 2) * GRAIN
      || y1 > centre_y + (max_y / 2) * GRAIN)
      continue;
  x = (effect[i].x / GRAIN) - (centre_x / GRAIN);
  y = (effect[i].y / GRAIN)  - (centre_y / GRAIN);
  draw_an_effect(bmp, i, x + (max_x / 2), y + (max_y / 2));
  
 }

}

void draw_wave(BITMAP *bmp, int x, int y, int count, int size)
{
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   int a = (count / 2) % 16;
   int b = size + a - 8;
   int c;
   switch((int) a / 2 + grand(3) - grand(3))
   {
    default:
    case 7:
    case 0: c = TRANS_DGREY; break;
    case 6:
    case 1: c = TRANS_DGREEN; break;
    case 2:
    case 5: c = TRANS_LGREEN; break;
    case 3:
    case 4: c = TRANS_YELLOW; break;
/*    case 7:
    case 0: c = TRANS_DGREEN; break;
    case 6:
    case 1: c = TRANS_LGREEN; break;
    case 4:
    case 5:
    case 3:
    default:
    case 2: c = TRANS_YELLOW; break;*/
   }
   circle(bmp, x, y, b, c);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}

void draw_an_effect(BITMAP *bmp, int dr, int x, int y)
{
 int x1, y1, a,b,c;
 
 switch(effect[dr].type)
 {
  case EFFECT_WAVE1:
  case EFFECT_WAVE2:
   draw_wave(bmp, x, y, effect [dr].things [0], effect [dr].things [1]);
   a = effect [dr].things [0] - 16 + 255;
   draw_wave(bmp, x, y, a, effect [dr].things [1]);
   break;
  case EFFECT_ZAP_AIM:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   b = xpart(enemy[dr].counter * 32, 6);
   x1 = TRANS_LBLUE;
   for (c = 0; c < 6; c ++)
   {
    b *= -1;
    a = (effect[dr].things [0] * 4 + c * (ANGLE_FULL / 6)) & 1023;
    triangle(bmp,
     x + xpart(a, 25 + b), y + ypart(a, 25 + b),
     x + xpart(a + ANGLE_1_32, 42 + b), y + ypart(a + ANGLE_1_32, 42 + b),
     x + xpart(a - ANGLE_1_32, 42 + b), y + ypart(a - ANGLE_1_32, 42 + b),
     x1);     
   }
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case EFFECT_REDBEAM:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
if (effect[dr].things [0] > 100)
    a = (150 - effect[dr].things [0]) * 6;
     else
      a = 300;  
   if (effect[dr].things [0] < 25)
    a = effect[dr].things [0] * 12;   
   if (effect[dr].things [0] > 100)
    b = (150 - effect[dr].things [0]) / 6 + 2;// + pulsate(64, 4, effect[dr].things [2]);
     else
      b = 10 + grand(3);//pulsate(32, 2, effect[dr].things [2]);
   if (effect[dr].things [0] < 40)
    b = effect[dr].things [0] / 4 +  + grand(3);//pulsate(32, 2, effect[dr].things [2]);   
   if (b < 1)
    b = 1; 
   if (effect[dr].things [0] > 100)
    c = (150 - effect[dr].things [0]) / 12 + 1;// + pulsate(64, 2, effect[dr].things [2]);
     else
      c = 5 + grand(3);// + pulsate(64, 1, effect[dr].things [2]);
   if (effect[dr].things [0] < 40)
    c = effect[dr].things [0] / 8 + grand(3);   
   if (c < 1)
    c = 1;
   circlefill(bmp, x, y, b, TRANS_YELLOW);
   circlefill(bmp, x, y, (b * 13) / 10, TRANS_LRED);
   circlefill(bmp, x, y, (b * 15) / 10, TRANS_DRED);
   draw_a_light(bmp, (b * 25) / 10, x, y);
   x1 = x + xpart(effect[dr].things [1], a);
   y1 = y + ypart(effect[dr].things [1], a);
   circlefill(bmp, x1, y1, b * 2, TRANS_YELLOW);
   circlefill(bmp, x1, y1, (b * 13) / 5, TRANS_LRED);
   circlefill(bmp, x1, y1, (b * 15) / 5, TRANS_DRED);
   draw_a_light(bmp, (b * 25) / 5, x1, y1);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], c, c * 2, TRANS_YELLOW);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], (c + 1), (c + 1) * 2, TRANS_DRED);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], c + 3, (c + 3) * 2, LIGHT_1);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], c + 6, (c + 6) * 2, LIGHT_2);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
  case EFFECT_BLUEBEAM:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
if (effect[dr].things [0] > 100)
    a = (150 - effect[dr].things [0]) * 6;
     else
      a = 300;  
   if (effect[dr].things [0] < 25)
    a = effect[dr].things [0] * 12;   
   if (effect[dr].things [0] > 100)
    b = (150 - effect[dr].things [0]) / 6 + 2;// + pulsate(64, 4, effect[dr].things [2]);
     else
      b = 10 + grand(3);//pulsate(32, 2, effect[dr].things [2]);
   if (effect[dr].things [0] < 40)
    b = effect[dr].things [0] / 4 +  + grand(3);//pulsate(32, 2, effect[dr].things [2]);   
   if (b < 1)
    b = 1; 
   if (effect[dr].things [0] > 100)
    c = (150 - effect[dr].things [0]) / 12 + 1;// + pulsate(64, 2, effect[dr].things [2]);
     else
      c = 5 + grand(3);// + pulsate(64, 1, effect[dr].things [2]);
   if (effect[dr].things [0] < 40)
    c = effect[dr].things [0] / 8 + grand(3);   
   if (c < 1)
    c = 1;
   circlefill(bmp, x, y, b, TRANS_WHITE);
   circlefill(bmp, x, y, (b * 13) / 10, TRANS_LBLUE);
   circlefill(bmp, x, y, (b * 15) / 10, TRANS_DBLUE);
   draw_a_light(bmp, (b * 25) / 10, x, y);
   x1 = x + xpart(effect[dr].things [1], a);
   y1 = y + ypart(effect[dr].things [1], a);
   circlefill(bmp, x1, y1, b * 2, TRANS_WHITE);
   circlefill(bmp, x1, y1, (b * 13) / 5, TRANS_LBLUE);
   circlefill(bmp, x1, y1, (b * 15) / 5, TRANS_DBLUE);
   draw_a_light(bmp, (b * 25) / 5, x1, y1);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], c, c * 2, TRANS_WHITE);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], (c + 1), (c + 1) * 2, TRANS_LBLUE);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], c + 3, (c + 3) * 2, LIGHT_1);
   beam_points(bmp, x, y, x1, y1, effect[dr].things [1], c + 6, (c + 6) * 2, LIGHT_2);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
   break;
 }

}

void beam_points(BITMAP *bmp, int x1, int y1, int x2, int y2, int angle, int out1, int out2, int colour)
{
 int points [8];
 
 points [0] = x1 + xpart(angle - ANGLE_QUARTER, out1);
 points [1] = y1 + ypart(angle - ANGLE_QUARTER, out1);
 points [2] = x1 + xpart(angle + ANGLE_QUARTER, out1);
 points [3] = y1 + ypart(angle + ANGLE_QUARTER, out1);
 points [4] = x2 + xpart(angle + ANGLE_QUARTER, out2);
 points [5] = y2 + ypart(angle + ANGLE_QUARTER, out2);
 points [6] = x2 + xpart(angle - ANGLE_QUARTER, out2);
 points [7] = y2 + ypart(angle - ANGLE_QUARTER, out2);
 
 polygon(bmp, 4, points, colour);

}

void draw_a_light(BITMAP *bmp, int size, int x, int y)
{
// int i = light[dr].size;
 if (size > 99)
  size = 99;
  
// switch(light[dr].type)
// {
//  case LIGHT_NORMAL:
/*   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   circlefill(bmp, x, y, light[dr].size + 1, LIGHT_1);
   circlefill(bmp, x, y, ((light[dr].size * 9) / 10) + 1, LIGHT_2);
   drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);*/
   draw_trans_rle_sprite(bmp, light_rle [size], x - size - 1, y - size - 1);
//   break;
// }

}




void draw_pickups(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 int i, x, y;

 for (i = 0; i < NO_PICKUPS; i ++)
 {
  if (pickup[i].type == PICKUP_NONE)
   continue;
  if (pickup[i].x + 12000 < centre_x - (max_x / 2) * GRAIN
      || pickup[i].x - 12000 > centre_x + (max_x / 2) * GRAIN
      || pickup[i].y + 12000 < centre_y - (max_y / 2) * GRAIN
      || pickup[i].y - 12000 > centre_y + (max_y / 2) * GRAIN)
      continue;
  x = (pickup[i].x / GRAIN) - (centre_x / GRAIN);
  y = (pickup[i].y / GRAIN) - (centre_y / GRAIN);
  
  draw_a_pickup(bmp, i, x + (max_x / 2), y + (max_y / 2));
  
 }

}

void get_pickup_colour(int st, int cols [3])
{
 if (game.mode_purple == 1)
 {
   cols [0] = TRANS_PURPLE;
   cols [1] = TRANS_PURPLE;
   return;
 }
 switch(st)
 {
  case ABILITY_PRIMARY:
 cols [0] = TRANS_YELLOW;
 cols [1] = TRANS_ORANGE;
//   cols [0] = COLOUR_YELLOW7;
   return;
  case ABILITY_DRIVE:
   cols [0] = TRANS_LGREEN;
   cols [1] = TRANS_DGREEN;
   return;
  case ABILITY_DEFENCE:
   cols [0] = TRANS_LBLUE;
   cols [1] = TRANS_DBLUE;
//   cols [0] = COLOUR_BLUE7;
   return;
  case ABILITY_SECONDARY:
   cols [0] = TRANS_DRED;
   cols [1] = TRANS_DRED;
//   cols [0] = COLOUR_RED7;
   return;
 }
}


void draw_a_pickup(BITMAP *bmp, int dr, int x, int y)
{
 int x1, y1, x2;

 int col;
 int cols [3] = {COLOUR_YELLOW8, COLOUR_YELLOW4, COLOUR_RED4};
 char dstr [30];

 switch(pickup[dr].type)
 {
    case PICKUP_SQUARE:
   drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
   int points [8];
   get_pickup_colour(pickup[dr].subtype, cols);
   x1 = (counter * 16) & 1023;
   x2 = 8 + pulsate(32, 4, counter);
   points [0] = x + xpart(x1, x2);
   points [1] = y + ypart(x1, x2);
   points [2] = x + xpart(x1 + ANGLE_QUARTER, x2);
   points [3] = y + ypart(x1 + ANGLE_QUARTER, x2);
   points [4] = x - xpart(x1, x2);
   points [5] = y - ypart(x1, x2);
   points [6] = x - xpart(x1 + ANGLE_QUARTER, x2);
   points [7] = y - ypart(x1 + ANGLE_QUARTER, x2);
   polygon(bmp, 4, points, cols [0]);
   x1 = 1024 - ((counter * 8) & 1023);
   x2 = 10;// + pulsate(64, 4, counter);
   points [0] = x + xpart(x1, x2);
   points [1] = y + ypart(x1, x2);
   points [2] = x + xpart(x1 + ANGLE_QUARTER, x2);
   points [3] = y + ypart(x1 + ANGLE_QUARTER, x2);
   points [4] = x - xpart(x1, x2);
   points [5] = y - ypart(x1, x2);
   points [6] = x - xpart(x1 + ANGLE_QUARTER, x2);
   points [7] = y - ypart(x1 + ANGLE_QUARTER, x2);
   pline(bmp, points [0], points [1], points [2], points [3], cols [0]);
   pline(bmp, points [2], points [3], points [4], points [5], cols [0]);
   pline(bmp, points [4], points [5], points [6], points [7], cols [0]);
   pline(bmp, points [6], points [7], points [0], points [1], cols [0]);
     draw_a_light(bmp, 15 + pulsate(32, 4, counter), x, y);
    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    if (options.colour_text == 1)
     write_colour_text(bmp, x, y, pickup[dr].subtype);
    break;
    case PICKUP_CIRCLE:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     get_pickup_colour(pickup[dr].subtype, cols);
     circlefill(bmp, x, y, 4 + pulsate(32, 3, counter), cols[0]);
     circlefill(bmp, x + xpart(counter * 16, 10), y + ypart(counter * 16, 10), 3 - pulsate(16, 2, counter), cols[0]);
     circlefill(bmp, x + xpart(counter * 16 + ANGLE_HALF, 10), y + ypart(counter * 16 + ANGLE_HALF, 10), 3 - pulsate(16, 2, counter), cols[0]);
//     circle(bmp, x, y, 7, cols [0]);
     draw_a_light(bmp, 15 + pulsate(32, 4, counter), x, y);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     if (options.colour_text == 1)
      write_colour_text(bmp, x, y, pickup[dr].subtype);
     break;    
    case PICKUP_TRIANGLE:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     get_pickup_colour(pickup[dr].subtype, cols);
     x1 = counter * 16 % 1023;
     x2 = 9 + pulsate(16, 3, counter);
     triangle(bmp, x + xpart(x1, x2), y + ypart(x1, x2),
                   x + xpart(x1 + ANGLE_FULL / 3, x2), y + ypart(x1 + ANGLE_FULL / 3, x2),
                   x + xpart(x1 - ANGLE_FULL / 3, x2), y + ypart(x1 - ANGLE_FULL / 3, x2),
                   cols[0]);
     x1 = ANGLE_FULL - x1;
     x2 = 15 - pulsate(16, 5, counter);
     triangle(bmp, x + xpart(x1, x2), y + ypart(x1, x2),
                   x + xpart(x1 + ANGLE_FULL / 3, 4), y + ypart(x1 + ANGLE_FULL / 3, 4),
                   x + xpart(x1 - ANGLE_FULL / 3, 4), y + ypart(x1 - ANGLE_FULL / 3, 4),
                   cols[0]);
     triangle(bmp, x + xpart(x1, 4), y + ypart(x1, 4),
                   x + xpart(x1 + ANGLE_FULL / 3, x2), y + ypart(x1 + ANGLE_FULL / 3, x2),
                   x + xpart(x1 - ANGLE_FULL / 3, 4), y + ypart(x1 - ANGLE_FULL / 3, 4),
                   cols[0]);
     triangle(bmp, x + xpart(x1, 4), y + ypart(x1, 4),
                   x + xpart(x1 + ANGLE_FULL / 3, 4), y + ypart(x1 + ANGLE_FULL / 3, 4),
                   x + xpart(x1 - ANGLE_FULL / 3, x2), y + ypart(x1 - ANGLE_FULL / 3, x2),
                   cols[0]);
//     triangle(bmp, x, y - 5, x - 4, y + 2, x + 4, y + 2, cols [0]);
//     circlefill(bmp, x, y, 7, TRANS_LRED);
//     circle(bmp, x, y, 7, TRANS_YELLOW);
     draw_a_light(bmp, 15 + pulsate(32, 4, counter), x, y);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     if (options.colour_text == 1)
      write_colour_text(bmp, x, y, pickup[dr].subtype);
     break;    
/*    case PICKUP_CROSS:
//     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     get_pickup_colour(pickup[dr].subtype, cols);
     rectfill(bmp, x - 2, y - 5, x + 2, y + 5, cols [0]);
     rectfill(bmp, x - 5, y - 2, x + 5, y + 2, cols [0]);
//     rect(bmp, x - 5, y - 5, x + 5, y + 5, TRANS_DBLUE);
//     line(bmp, x - 5, y - 5, 
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    break;*/
//    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
    case PICKUP_PRESYMBOL:
//     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
//     rectfill(bmp, x - 2, y - 5, x + 2, y + 5, COLOUR_WHITE);
//     rectfill(bmp, x - 5, y - 2, x + 5, y + 2, COLOUR_WHITE);
//     rect(bmp, x - 5, y - 5, x + 5, y + 5, TRANS_DBLUE);
//     line(bmp, x - 5, y - 5, 
//     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

//   circlefill(bmp, x + xpart(counter * 16, 10), y + ypart(counter * 16, 10), 3 - pulsate(16, 2, counter), cols[0]);
//   circlefill(bmp, x + xpart(counter * 16 + ANGLE_HALF, 10), y + ypart(counter * 16 + ANGLE_HALF, 10), 3 - pulsate(16, 2, counter), cols[0]);
//   putpixel(bmp, x + xpart(counter * 16, 10), y + ypart(counter * 16, 10), COLOUR_GREY5 + pulsate(64, 2, counter));
//   putpixel(bmp, x + xpart(counter * 16 + ANGLE_QUARTER, 10), y + ypart(counter * 16 + ANGLE_QUARTER, 10), COLOUR_GREY5 + pulsate(64, 2, counter));
//   putpixel(bmp, x - xpart(counter * 16, 10), y - ypart(counter * 16, 10), COLOUR_GREY5 + pulsate(64, 2, counter));
//   putpixel(bmp, x - xpart(counter * 16 + ANGLE_QUARTER, 10), y - ypart(counter * 16 + ANGLE_QUARTER, 10), COLOUR_GREY5 + pulsate(64, 2, counter));
   x1 = pickup[dr].timeout / 3 + 3;
   x2 = (64 - pickup[dr].timeout) / 18;
   cols [0] = COLOUR_WHITE - pickup[dr].timeout / 20;//COLOUR_GREY5 + pulsate(64, 2, counter);
   circlefill(bmp, x + xpart(counter * 32, x1), y + ypart(counter * 32, x1), x2, cols[0]);
   circlefill(bmp, x + xpart(counter * 32 + ANGLE_QUARTER, x1), y + ypart(counter * 32 + ANGLE_QUARTER, x1), x2, cols[0]);
   circlefill(bmp, x - xpart(counter * 32, x1), y - ypart(counter * 32, x1), x2, cols[0]);
   circlefill(bmp, x - xpart(counter * 32 + ANGLE_QUARTER, x1), y - ypart(counter * 32 + ANGLE_QUARTER, x1), x2, cols[0]);
//   circlefill(bmp, x + xpart(counter * 16, x1), y + ypart(counter * 16, x1), 3 - pulsate(16, 2, counter), cols[0]);
//   circlefill(bmp, x + xpart(counter * 16 + ANGLE_QUARTER, x1), y + ypart(counter * 16 + ANGLE_QUARTER, x1), 3 - pulsate(16, 2, counter), cols[0]);
//   circlefill(bmp, x - xpart(counter * 16, x1), y - ypart(counter * 16, x1), 3 - pulsate(16, 2, counter), cols[0]);
//   circlefill(bmp, x - xpart(counter * 16 + ANGLE_QUARTER, x1), y - ypart(counter * 16 + ANGLE_QUARTER, x1), 3 - pulsate(16, 2, counter), cols[0]);
//   draw_a_light(bmp, 15 + pulsate(64, 4, counter), x, y);
    draw_a_light(bmp, 8 + (64 - pickup[dr].timeout) / 5, x, y);
    break;
    case PICKUP_PRESECONDARY:
//     circlefill(bmp, x, y, 10 + xpart((counter * 64) % ANGLE_FULL, 5), COLOUR_GREY4);
     x1 = TRANS_DGREY;
     x2 = counter % 32;
     if (x2 < 20) x1 = TRANS_LGREY;
     if (x2 < 10) x1 = TRANS_WHITE;
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circle(bmp, x, y, 3 + x2 / 2, x1);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     secondary_name(pickup[dr].subtype, dstr);
     textprintf_centre_ex(bmp, small_font, x, y + 12, COLOUR_GREY3, -1, dstr);
    break;
    case PICKUP_SECONDARY:
     x1 = TRANS_DGREY;
     x2 = counter % 32;
     if (x2 < 29) x1 = TRANS_LGREY;
//     if (x2 < 26) x1 = TRANS_DRED;
     if (x2 < 23) x1 = TRANS_LRED;
     if (x2 < 18) x1 = TRANS_ORANGE;
     if (x2 < 13) x1 = TRANS_YELLOW;
     if (x2 < 8) x1 = TRANS_WHITE;
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     circle(bmp, x, y, 3 + x2 / 2, x1);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     circlefill(bmp, x, y, 3 + pulsate(64, 2, counter), COLOUR_YELLOW4 + pulsate(64, 3, counter));
     secondary_name(pickup[dr].subtype, dstr);
     textprintf_centre_ex(bmp, small_font, x, y + 12, COLOUR_YELLOW4 + pulsate(16, 2, counter), -1, dstr);
     draw_a_light(bmp, 10 + pulsate(64, 5, counter), x, y);
     break;    
  case PICKUP_REPAIR:
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     rectfill(bmp, x - 6 - pulsate(64, 3, counter), y - 3, x + 6 + pulsate(64, 3, counter), y + 3, TRANS_ORANGE);
     rectfill(bmp, x - 3, y - 6 - pulsate(64, 3, counter), x + 3, y + 6 + pulsate(64, 3, counter), TRANS_ORANGE);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 10 + pulsate(64, 3, counter), x, y);
   break;


  default:
//  case PICKUP_UPGRADE3:
   col = COLOUR_PURPLE8;
//  case PICKUP_UPGRADE2:
//   if (pickup[dr].type == PICKUP_UPGRADE2)
//   col = COLOUR_RED8;
//  case PICKUP_UPGRADE1:
//  circlefill(bmp, x, y, 4, COLOUR_YELLOW4 + (pickup[dr].counter / 8) % 4);
//  circlefill(bmp, x, y, 1 + (pickup[dr].counter / 4) % 5, COLOUR_YELLOW6 + (pickup[dr].counter / 8) % 3);
  circlefill(bmp, x, y, 2, col - (pickup[dr].counter / 8) % 2);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9)) * 7;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9)) * 7;
//  circlefill(bmp, x + x1, y + y1, 1, COLOUR_YELLOW8 - (pickup[dr].counter / 8) % 3);
//  circlefill(bmp, x - x1, y - y1, 1, COLOUR_YELLOW8 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x + x1, y + y1, col - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - (pickup[dr].counter / 8) % 3);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 7;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 7;
  putpixel(bmp, x + x1, y + y1, col - 4 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - 4 - (pickup[dr].counter / 8) % 3);
  x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 7;
  y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 7;
  putpixel(bmp, x + x1, y + y1, col - 2 - (pickup[dr].counter / 8) % 3);
  putpixel(bmp, x - x1, y - y1, col - 2 - (pickup[dr].counter / 8) % 3);
   break;
/*  case PICKUP_SHIP:
   col = COLOUR_BLUE8;
   x1 = cos(angle_to_radians(pickup[dr].counter * 9)) * 8;
   y1 = sin(angle_to_radians(pickup[dr].counter * 9)) * 8;
   putpixel(bmp, x + x1, y + y1, col - (pickup[dr].counter / 8) % 3);
   putpixel(bmp, x - x1, y - y1, col - (pickup[dr].counter / 8) % 3);
   x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 8;
   y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 4) * 8;
   putpixel(bmp, x + x1, y + y1, col - 4 - (pickup[dr].counter / 8) % 3);
   putpixel(bmp, x - x1, y - y1, col - 4 - (pickup[dr].counter / 8) % 3);
   x1 = cos(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 8;
   y1 = sin(angle_to_radians(pickup[dr].counter * 9) - PI / 8) * 8;
   putpixel(bmp, x + x1, y + y1, col - 2 - (pickup[dr].counter / 8) % 3);
   putpixel(bmp, x - x1, y - y1, col - 2 - (pickup[dr].counter / 8) % 3);
   rectfill(bmp, x - 4, y - 4, x + 4, y + 5, COLOUR_PURPLE1);
   rect(bmp, x - 4, y - 4, x + 4, y + 5, COLOUR_PURPLE6);
   textprintf_ex(bmp, small_font, x - 2, y - 5, col - (pickup[dr].counter / 8) % 4, -1, "X");
   break;*/
 }

}

void write_colour_text(BITMAP *bmp, int x, int y, int which)
{
 if (game.mode_purple == 1)
 {
  textprintf_centre_ex(bmp, small_font, x, y, COLOUR_WHITE, -1, "Purple"); // heh
  return;
 }
 
 switch(which)
 {
  case ABILITY_PRIMARY:
   textprintf_centre_ex(bmp, small_font, x, y, COLOUR_WHITE, -1, "Yellow");
   break;
  case ABILITY_SECONDARY:
   textprintf_centre_ex(bmp, small_font, x, y, COLOUR_WHITE, -1, "Red");
   break;
  case ABILITY_DRIVE:
   textprintf_centre_ex(bmp, small_font, x, y, COLOUR_WHITE, -1, "Green");
   break;
  case ABILITY_DEFENCE:
   textprintf_centre_ex(bmp, small_font, x, y, COLOUR_WHITE, -1, "Blue");
   break;
 } 
}

void draw_eyes(BITMAP *bmp, int max_x, int max_y, int play, int centre_x, int centre_y)
{

 if (arena.hostile == 0)
  return;

 int i, x, y;
   i = grand(arena.eyes_on_level);
   if (arena.eye_x [i] == 0)
    return;

 for (i = 0; i < arena.eyes_on_level; i ++)
 {
  if (arena.eye_x [i] == 0)
   continue; // probably unnecessary
  if (arena.eye_x [i] + 3000 < centre_x - (max_x / 2) * GRAIN
      || arena.eye_x [i] - 3000 > centre_x + (max_x / 2) * GRAIN
      || arena.eye_y [i] + 3000 < centre_y - (max_y / 2) * GRAIN
      || arena.eye_y [i] - 3000 > centre_y + (max_y / 2) * GRAIN)
      continue;
  x = (arena.eye_x [i] / GRAIN) - (centre_x / GRAIN);
  y = (arena.eye_y [i] / GRAIN) - (centre_y / GRAIN);
  
  draw_an_eye(bmp, i, x + (max_x / 2), y + (max_y / 2));
  
 }

}


void draw_an_eye(BITMAP *bmp, int i, int x, int y)
{

// int eye_colours [5] = {TRANS_DRED, TRANS_LRED, TRANS_ORANGE, TRANS_YELLOW, TRANS_WHITE};
 // need to put this in arena so we can have eyes of different colours

// int angle;
// int angle_plus;

    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
/*     angle = (counter * 16) & 1023;
     if (i % 2 == 0)
      angle = ANGLE_FULL - angle;
     for (j = 0; j < 4; j ++)
     {
      circlefill(bmp, x + grand(3) - 1 + xpart(angle + (j * ANGLE_QUARTER), 13 + pulsate(64, 4, counter)),
        y + grand(3) - 1 + ypart(angle + (j * ANGLE_QUARTER), 13 + pulsate(64, 4, counter)),
        6 + grand(3), TRANS_DRED);
     }*/
     
     circlefill(bmp, x + grand(3) - 1, y + grand(3) - 1, 13 + pulsate(64, 4, counter), arena.eye_colour3);
     circlefill(bmp, x + grand(3) - 1, y + grand(3) - 1, 9 + pulsate(64, 3, counter), arena.eye_colour2);
     drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
     draw_a_light(bmp, 19 + pulsate(64, 3, counter) + grand(3), x + grand(3) - 1, y + grand(3) - 1);
     circlefill(bmp, x + grand(3) - 1, y + grand(3) - 1, 2 + grand(2), arena.eye_colour1);

}



void draw_grid(BITMAP *bmp, int max_x, int max_y, int play, int colour, int edge_colour, int x_offset, int centre_x, int centre_y, int finetune_x, int finetune_y)
{


 int i;//, x, y;


 int no_x = max_x / GRID_WIDTH + 2;
 int no_y = max_y / GRID_WIDTH + 2 + special_600_y;

 int ax = centre_x;// + inflicteda;
 int ay = centre_y;// + inflictede;

 int min_x = 0;
 int min_y = 0;
 int dmax_x = max_x;
 int dmax_y = max_y;
 int draw_x = 0;
 int draw_y = 0;

// char highlight = 0;

 int j;
 int tiled;
 int tx, ty;

 if (ax / GRAIN < max_x / 2)
 {
  min_x = (max_x / 2) - ax / GRAIN;
 }
 if (ax / GRAIN > arena.max_x / GRAIN - max_x / 2)
 {
  dmax_x = (max_x / 2) + arena.max_x / GRAIN - ax / GRAIN;
 }
 if (ay / GRAIN < max_y / 2)
 {
  min_y = (max_y / 2) - ay / GRAIN;
 }
 if (ay / GRAIN > arena.max_y / GRAIN - max_y / 2)
 {
  dmax_y = (max_y / 2) + arena.max_y / GRAIN - ay / GRAIN;
 }

// parallax background:
 if (FALSE) //game.mode_void == 1)
 {
  clear_to_color(bmp, GC_GREY2);
  return;
 }
  else
   draw_rle_sprite(bmp, tile_background, 0 - (ax / (int) (GRAIN * 1.2)) % GRID_WIDTH, 0 - (ay / (int) (GRAIN * 1.2)) % GRID_WIDTH);
//#ifdef DEBUG_DISPLAY
// textprintf_centre_ex(bmp, small_font, 200, 
//   200, COLOUR_WHITE, -1, "(%i, %i)", 0 - (ax / (int) (GRAIN * 1.2)) % GRID_WIDTH, 0 - (ay / (int) (GRAIN * 1.2)) % GRID_WIDTH);
//#endif   

// parallax background: (old, slower way)
/*
 for (i = -1; i < no_x; i ++)
 {
//  if ((i - 3) * GRID_WIDTH + ax / GRAIN >= arena.max_x / GRAIN + max_x / 2 - 94)
  if ((i - 3) * GRID_WIDTH + ax / GRAIN >= arena.max_x / GRAIN + max_x / 2 - 124 - finetune_x)
  {
//   if ((i - 3) * GRID_WIDTH + ax / GRAIN >= arena.max_x / GRAIN + max_x / 2 - 94)
   if ((i - 3) * GRID_WIDTH + ax / GRAIN >= arena.max_x / GRAIN + max_x / 2 - 124 - finetune_x)
    break;
  }
  if ((i + 1) * GRID_WIDTH <= min_x - 31)
  {
   continue;
  }
  if ((i + 1) * GRID_WIDTH <= min_x - 21 && min_x != 0)
  {
   continue;
  }
  for (j = 0; j < no_y; j ++)
  {
   if ((j - 1) * GRID_WIDTH + ay / GRAIN >= arena.max_y / GRAIN + max_y / 2 + 15)//+ 64)
   {
    if ((j - 1) * GRID_WIDTH + ay / GRAIN >= arena.max_y / GRAIN + max_y / 2 + 15) //64)
     break;
   }
//   if (min_y > 0 && (j + 1) * GRID_WIDTH <= min_y + 9 + (special_600_y * (ay < GRAIN * GRID_WIDTH)))
   if (min_y > 0 && (j + 1) * GRID_WIDTH <= min_y + 9) // with special_600_y removed so it can be used in calculation of no_y instead
   {
    continue;
   }
    {
     draw_rle_sprite(bmp, tile_rle [0], (i * GRID_WIDTH) - (ax / (int) (GRAIN * 1.2)) % GRID_WIDTH + 15 + finetune_x, (j * GRID_WIDTH) - (ay / (int) (GRAIN * 1.2)) % GRID_WIDTH - 15 + finetune_y);
    }
  }
 }

// end parallax background
*/

 for (i = -1; i < no_x; i ++)
 {
  for (j = 0; j < no_y; j ++)
  {
    draw_x = (i * GRID_WIDTH) - (ax / GRAIN) % GRID_WIDTH + 15 + finetune_x;
    draw_y = (j * GRID_WIDTH) - (ay / GRAIN) % GRID_WIDTH - 15 + finetune_y;
    if (draw_x < -GRID_WIDTH || draw_x > scr_x)
     continue;
    if (draw_y < -GRID_WIDTH || draw_y > scr_y)
     continue;
    tx = ((ax / GRID_WIDTH) / GRAIN) + x_offset + i - (max_x / GRID_WIDTH) + 8;
    ty = ((ay / GRID_WIDTH) / GRAIN) + j - (max_y / GRID_WIDTH) + 6;
    if (tx < 0 || tx > arena.max_x / 50000 + 4 || ty < 0 || ty > arena.max_y / 50000 + 4)
    {
     tiled = getpixel(level_bmp, 0, 0);
    }
     else
      tiled = getpixel(level_bmp, tx, ty);
   if (tiled < 0 || tiled > NO_TILE_RLES || tile_rle [tiled] == NULL)
   {
//    draw_rle_sprite(bmp, tile_rle [], (i * GRID_WIDTH) - (ax / GRAIN) % GRID_WIDTH + 15 + finetune_x, (j * GRID_WIDTH) - (ay / GRAIN) % GRID_WIDTH - 15 + finetune_y);
#ifdef DEBUG_DISPLAY   
    textprintf_centre_ex(bmp, small_font, (i * GRID_WIDTH) - (ax / GRAIN) % GRID_WIDTH + 40, 
     (j * GRID_WIDTH) - (ay / GRAIN) % GRID_WIDTH + 10, COLOUR_WHITE, -1, "%i %i %i", tx, ty, tiled);
#endif     
   }
    else
    {
     if (tiled > 0)
      draw_rle_sprite(bmp, tile_rle [tiled], draw_x, draw_y);
    }
//    textprintf_centre_ex(bmp, small_font, draw_x, 
//     draw_y, COLOUR_WHITE, -1, "%i %i %i", tx, ty, tiled);
  }
 }
return;

/*
 for (i = -1; i < no_x; i ++)
 {
  if ((i + 2) * GRID_WIDTH + ax / GRAIN >= arena.max_x / GRAIN + max_x / 2 - 14)
  {
//   highlight = 1; // was - 2
   if ((i - 3) * GRID_WIDTH + ax / GRAIN >= arena.max_x / GRAIN + max_x / 2 - 14)
    break;
  }
  if ((i + 2) * GRID_WIDTH <= min_x - 71)
  {
//   highlight = 1;
   continue;
  }
  if ((i + 2) * GRID_WIDTH <= min_x - 21 && min_x != 0)
  {
//   highlight = 1;
   continue;
  }
  for (j = 0; j < no_y; j ++)
  {
   if ((j + 2) * GRID_WIDTH + ay / GRAIN >= arena.max_y / GRAIN + max_y / 2 + visible_grids_y)//+ 64)
   {
    if ((j - 2) * GRID_WIDTH + ay / GRAIN >= arena.max_y / GRAIN + max_y / 2 + visible_grids_y) //64)
     break;
   }
   if (min_y > 0 && (j + 2) * GRID_WIDTH <= min_y + 9 + (special_600_y * (ay < GRAIN * GRID_WIDTH)))
   {
    continue;
   }
    tx = ((ax / GRID_WIDTH) / GRAIN) + x_offset + i - (max_x / GRID_WIDTH) + 8;
    ty = ((ay / GRID_WIDTH) / GRAIN) + j - (max_y / GRID_WIDTH) + 6;
   tiled = getpixel(level_bmp, tx, ty);
   if (tiled < 0 || tiled > NO_TILE_RLES || tile_rle [tiled] == NULL)
   {
#ifdef DEBUG_DISPLAY   
    textprintf_centre_ex(bmp, small_font, (i * GRID_WIDTH) - (ax / GRAIN) % GRID_WIDTH + 40, 
     (j * GRID_WIDTH) - (ay / GRAIN) % GRID_WIDTH + 10, COLOUR_WHITE, -1, "%i %i %i", tx, ty, tiled);
#endif     
   }
    else
    {
     if (tiled > 0)
//      draw_rle_sprite(bmp, tile_rle [tiled], (i * GRID_WIDTH) - (ax / GRAIN) % GRID_WIDTH + 15 + finetune, (j * GRID_WIDTH) - (ay / GRAIN) % GRID_WIDTH - 15);
      draw_rle_sprite(bmp, tile_rle [tiled], (i * GRID_WIDTH) - (ax / GRAIN) % GRID_WIDTH + 15 + finetune_x, (j * GRID_WIDTH) - (ay / GRAIN) % GRID_WIDTH - 15 + finetune_y);
    }
  }
 }
return;
*/ 
/*
old lacewing line drawing?

// float star_angle;
// int distance;

 for (i = 0; i < no_x; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
    break;
  }
  if (i * GRID_WIDTH <= min_x - 6)
  {
   highlight = 1;
   continue;
  }
  x = i * GRID_WIDTH - ((ax + x_offset) / GRAIN) % GRID_WIDTH;
//  x = i * GRID_WIDTH - (actor[player[play].actor_controlled].y / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, x, min_y - 4, x, dmax_y, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, x, min_y - 4, x, dmax_y, colour);
 }
 
 for (i = 0; i < no_y; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
    break;
  }
  if (min_y > 0 && i * GRID_WIDTH <= min_y + 9 + (special_600_y * (ay < GRAIN * GRID_WIDTH)))
  {
   highlight = 1;
   continue;
  }
  y = i * GRID_WIDTH - ((ay + grid_offset_y) / GRAIN) % GRID_WIDTH;
//  y = i * GRID_WIDTH + (actor[player[play].actor_controlled].x / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, colour);
 }

*/
/*

WITHOUT TILES
 int i, x, y;

 int no_x = max_x / GRID_WIDTH + 2;
 int no_y = max_y / GRID_WIDTH + 2;

 int ax = actor[player[play].actor_controlled].x;// + inflicteda;
 int ay = actor[player[play].actor_controlled].y;// + inflictede;

 int min_x = 0;
 int min_y = 0;
 int dmax_x = max_x;
 int dmax_y = max_y;

 char highlight = 0;

 if (ax / GRAIN < max_x / 2)
 {
  min_x = (max_x / 2) - ax / GRAIN;
 }
 if (ax / GRAIN > arena.max_x / GRAIN - max_x / 2)
 {
  dmax_x = (max_x / 2) + arena.max_x / GRAIN - ax / GRAIN;
 }
 if (ay / GRAIN < max_y / 2)
 {
  min_y = (max_y / 2) - ay / GRAIN;
 }
 if (ay / GRAIN > arena.max_y / GRAIN - max_y / 2)
 {
  dmax_y = (max_y / 2) + arena.max_y / GRAIN - ay / GRAIN;
 }
 

// float star_angle;
// int distance;

 for (i = 0; i < no_x; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ax / GRAIN > arena.max_x / GRAIN + max_x / 2 + 50)
    break;
  }
  if (i * GRID_WIDTH <= min_x - 6)
  {
   highlight = 1;
   continue;
  }
  x = i * GRID_WIDTH - ((ax + x_offset) / GRAIN) % GRID_WIDTH;
//  x = i * GRID_WIDTH - (actor[player[play].actor_controlled].y / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, x, min_y - 4, x, dmax_y, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, x, min_y - 4, x, dmax_y, colour);
 }
 
 for (i = 0; i < no_y; i ++)
 {
  if ((i + 1) * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
  {
   highlight = 1;
   if (i * GRID_WIDTH + ay / GRAIN > arena.max_y / GRAIN + max_y / 2 + 64)
    break;
  }
  if (min_y > 0 && i * GRID_WIDTH <= min_y + 9 + (special_600_y * (ay < GRAIN * GRID_WIDTH)))
  {
   highlight = 1;
   continue;
  }
  y = i * GRID_WIDTH - ((ay + grid_offset_y) / GRAIN) % GRID_WIDTH;
//  y = i * GRID_WIDTH + (actor[player[play].actor_controlled].x / GRAIN) % GRID_WIDTH;
  if (highlight == 1)
  {
   pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, edge_colour);
   highlight = 0;
  }
   else
    pline(bmp, min_x - 4, y - 15, dmax_x, y - 15, colour);
 }

*/
}


int pline(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour)
{
 if (x1 < -100 || y1 < -100 || x1 > 1300 || y1 > 1000
     || x2 < -100 || y2 < -100 || x2 > 1300 || y2 > 1000)
      return 0;
 line(bmp, x1, y1, x2, y2, colour);
 return 1;
}

void bordered_rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int colour1, int colour2)
{
   rectfill(bmp, x1, y1, x2, y2, colour1);
   rect(bmp, x1, y1, x2, y2, colour2);
//   hline(bmp, x1, y1, x2, colour2);
///   hline(bmp, x1, y2, x2, colour2);
//   vline(bmp, x1, y1, y2, colour2);
//   vline(bmp, x2, y1, y2, colour2);

}

void bordered_triangle(BITMAP *bmp, int x1, int y1, int x2, int y2, int x3, int y3, int col1, int col2)
{
 triangle(bmp, x1, y1, x2, y2, x3, y3, col1);
 pline(bmp, x1, y1, x2, y2, col2);
 pline(bmp, x1, y1, x3, y3, col2);
 pline(bmp, x2, y2, x3, y3, col2);
}

void indicate_fps(BITMAP *bmp, int play)
{
// textprintf_ex(bmp, small_font, 20, 110, 5, "Xs %i  Ys %i  TS %f  DS %f  Drag %i",
//  actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed, hypot(actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed), hypot(actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed) * game.drag, actor[player[play].actor_controlled].drag_amount);
// textprintf_ex(bmp, small_font, 20, 60, 5, "Stack %i", stackavail());
// textprintf(bmp, small_font, 20, 90, 5, "Slack %i", slacktime);
// textprintf(bmp, small_font, 20, 100, 5, "Lock %i", actor[0].lock);
//textprintf_ex(bmp, small_font, 20, 100, COLOUR_GREY5, -1, "Fps %i", frames_per_second);
// textprintf_ex(bmp, small_font, 20, 50, 5, -1, "Fps} %i", frames_per_second);
#ifdef DEBUG_DISPLAY
 textprintf_ex(bmp, small_font, 20, 90, 5, -1, "Fps %i", frames_per_second);
 textprintf_ex(bmp, small_font, 20, 100, 5, -1, "Slack %i", slacktime);
 textprintf_ex(bmp, small_font, 20, 110, 5, -1, "LSlack %i", long_slacktime_store);
 int i, c = 0;
 for (i = 0; i < NO_ENEMIES; i ++)
 {
  if (enemy[i].type != ENEMY_NONE)
   c++;
 }
 textprintf_ex(bmp, small_font, 20, 120, 5, -1, "e %i", c);
 c = 0;
 for (i = 0; i < NO_BULLETS; i ++)
 {
  if (bullet[i].type != BULLET_NONE)
   c++;
 }
 textprintf_ex(bmp, small_font, 20, 130, 5, -1, "b %i", c);
 c = 0;
 for (i = 0; i < NO_CLOUDS; i ++)
 {
  if (cloud[i].type != CLOUD_NONE)
   c++;
 }
 textprintf_ex(bmp, small_font, 20, 140, 5, -1, "c %i", c);
 
 textprintf_ex(bmp, small_font, 20, 150, 5, -1, "%i %i %i %i", debug_sound [0], debug_sound [1], debug_sound [2], debug_sound [3]);


 if (slack_graph_pos < 0 || slack_graph_pos > 99)
  slack_graph_pos = 0;
  
 slack_graph [slack_graph_pos] = slacktime;

 c = slack_graph_pos;
 for (i = 0; i < 100; i ++)
 {
  if (slack_graph [c] <= 5)
   vline(bmp, 110 - i, 300, 160, COLOUR_RED5);
    else
     vline(bmp, 110 - i, 300, 300 - (slack_graph [c] / 6000), COLOUR_YELLOW8);
  c --;
  if (c < 0)
   c = 99;   
 }

 slack_graph_pos ++;






 if (fps_graph_pos < 0 || fps_graph_pos > 99)
  fps_graph_pos = 0;
  
 fps_graph [fps_graph_pos] = frames_per_second;

 c = fps_graph_pos;
 for (i = 0; i < 100; i ++)
 {
  if (fps_graph [c] <= 25)
   vline(bmp, 110 - i, 360, 310, COLOUR_RED5);
    else
     vline(bmp, 110 - i, 360, 360 - (fps_graph [c] - 25) * 4, COLOUR_GREEN8);
  c --;
  if (c < 0)
   c = 99;   
 }
 
 hline(bmp, 10, 324, 110, COLOUR_WHITE);

 fps_graph_pos ++;




/* textprintf_ex(bmp, small_font, 20, 120, 5, -1, "Var1 %i", var1);
 textprintf_ex(bmp, small_font, 20, 130, 5, -1, "Var2 %i", var2);
 textprintf_ex(bmp, small_font, 20, 140, 5, -1, "Var3 %i", var3);*/
#endif
/* textprintf(bmp, font, 20, 110, 5, "Xs} %i  Ys} %i  Drag} %i",
  actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed, actor[player[play].actor_controlled].drag_amount);
 textprintf(bmp, font, 20, 140, 5, "X} %i  Y} %i ",
  actor[player[play].actor_controlled].x, actor[player[play].actor_controlled].y);
 textprintf(bmp, font, 20, 170, 5, "Angle} %i  Wants} %i Att} %i Rec} %i ",
  enemy[0].angle, enemy[0].attribute [ATTRIB_SWERVER_ANGLE], enemy[0].attacking, enemy[0].recycle);
 textprintf(bmp, font, 20, 200, 5, "Lock} %i ", actor[0].lock);*/
/* textprintf(bmp, small_font, 20, 50, 5, "FPS %i", frames_per_second);
 textprintf(bmp, small_font, 20, 60, 5, "SLACK %i", slacktime);
 textprintf(bmp, small_font, 20, 70, 5, "XS %i  YS %i  DRAG %i",
  actor[player[play].actor_controlled].x_speed, actor[player[play].actor_controlled].y_speed, actor[player[play].actor_controlled].drag_amount);
 textprintf(bmp, small_font, 20, 80, 5, "X %i  Y %i ",
  actor[player[play].actor_controlled].x, actor[player[play].actor_controlled].y);
 textprintf(bmp, small_font, 20, 90, 5, "ANGLE %i  WANTS %i ATT %i REC %i ",
  enemy[0].angle, enemy[0].attribute [ATTRIB_SWERVER_ANGLE], enemy[0].attacking, enemy[0].recycle);
 textprintf(bmp, small_font, 20, 100, 5, "LOCK %i ", actor[0].lock);
 textprintf(bmp, small_font, 20, 110, 5, "Angle %i ", actor[0].angle / 16);*/
// textprintf(bmp, font, 20, 160, 5, "inflicteda %i inflictede %i ", inflicteda, inflictede);
// textprintf(bmp, font, 20, 170, 5, "angle %i wants %i mode %i attacking %i ",
//   enemy[0].angle, 0, enemy[0].attribute [ATTRIB_FIGHTER_MODE], enemy[0].attacking);
/* textprintf(bmp, font, 20, 180, 5, "hive %i mode %i attack %i hive at13 %i hive at24 %i xd %i yd %i",
   enemy[1].attribute [ATTRIB_BEE_HIVE], enemy[1].attribute [ATTRIB_BEE_MODE], enemy[1].attacking,
   enemy[enemy[1].attribute [ATTRIB_BEE_HIVE]].attribute [ATTRIB_HIVE_ATTACK13], enemy[enemy[1].attribute [ATTRIB_BEE_HIVE]].attribute [ATTRIB_HIVE_ATTACK24],
   enemy[1].attribute [ATTRIB_BEE_X_DEST], enemy[1].attribute [ATTRIB_BEE_Y_DEST]);*/
}

// no inter-level announcements any more, but this still clears the bitmaps
void level_announcement(void)
{

 clear_bitmap(screen);
 if (player1 != NULL)
  clear_bitmap(player1);
 if (player2 != NULL)
  clear_bitmap(player2);


 return;
 
// int waiting = 30;

// if (arena.level == 1)
//  waiting = 60;
//   else
//    return; // actually, it's better not to have a big level sign thing... it's an annoying delay.

//#ifdef DEBUG_DISPLAY
// waiting = 2;
//#endif
/*
 ticked = 0;

 do
 {
  while(ticked > 0)
  {
   waiting --;
   ticked --;
  };
//  if ((waiting / 5) % 2 == 0)
  {
   textprintf_centre_ex(screen, font2, 320 + text_offset_x, 220 + text_offset_y, COLOUR_YELLOW8 - (waiting / 2) % 4, -1, "{___get___ready___}");
   textprintf_centre_ex(screen, font, 320 + text_offset_x, 220 + text_offset_y, -1, -1, "{___get___ready___}");
  }
//    else
//    {
//     textprintf_centre_ex(screen, font2, 320 + text_offset_x, 220 + text_offset_y, COLOUR_BLACK, -1, "{___get___ready___}");
//     textprintf_centre_ex(screen, font, 320 + text_offset_x, 220 + text_offset_y, COLOUR_BLACK, -1, "{___get___ready___}");
//    }
// textprintf_centre_ex(screen, font2, 320 + text_offset_x, 180 + text_offset_y, COLOUR_ORANGE8 - (waiting / 3) % 4, -1, "__{level__%i__}", arena.level);
// textprintf_centre_ex(screen, font, 320 + text_offset_x, 180 + text_offset_y, -1, -1, "{__level__%i__}", arena.level);

    
 } while (waiting > 0);
*/
/*
 int v;
 
 for (v = 0; v < 20; v ++)
 {
  textprintf_ex(screen, small_font, 20, v * 10 + 20, COLOUR_GREEN8, -1, "%i %f %i %i %i", v, cos_table [v], v & 1023, cos_table [v & 1023], xpart(v, 1000));
   
 }

  textprintf_ex(screen, small_font, 20, v * 10 + 30, COLOUR_GREEN8, -1, "X to continue...");

 do
 {
 
 } while (key [KEY_X] == 0);
*/

// clear_bitmap(screen);

}



/*void display_messages(BITMAP *bmp, int play, int max_x, int max_y)
{

// return;

 int i, j;
 int col1, col2;
 int y_loc;
// int x_offset;

// FONT *msgfont1;
// FONT *msgfont2;

 for (i = 0; i < NO_MESSAGES; i ++)
 {
  if (strlen(messages [play] [i]) == 0)
   continue;
  if (message_counter [play] [i] > 100)
  {
   kill_message(play, i);
   continue;
  } // can be killed later also
  
  message_counter [play] [i] ++;
  int count = message_counter [play] [i];
  y_loc = 150 + i * 12;
//  msgfont1 = font;
//  msgfont2 = font2;
  

    if (count > 40)
    {
     kill_message(play, i);
     continue;
    }
    
    switch(count / 5)
    {
     case 0: col1 = COLOUR_WHITE;
      break;
//   case 1: col1 = message_style [play] [i];
//    break;
     default: col1 = message_style [play] [i] - (count / 5) + 1;
      col2 = message_style [play] [i] - 4;
      break;
    }
    for (j = 0; j < 4; j ++)
    {
//     textprintf_centre_ex(bmp, small_font, max_x / 2 + xpart(counter + j * ANGLE_QUARTER, 25), y_loc + ypart(counter + j * ANGLE_QUARTER, 12), col1, -1, messages [play] [i]);
//     county = 1;
//     if (
//     textprintf_centre_ex(bmp, small_font, max_x / 2 + xpart((counter * 12) + j * ANGLE_QUARTER, 25), y_loc + ypart((counter * 16) + j * ANGLE_QUARTER, 12), col2, -1, messages [play] [i]);
//     textprintf_centre_ex(bmp, small_font, max_x / 2 + xpart((counter * 12) + j * ANGLE_QUARTER, 40 - count), y_loc + ypart((counter * 16) + j * ANGLE_QUARTER, 20 - (count / 2)), col2, -1, messages [play] [i]);
     textprintf_centre_ex(bmp, small_font, max_x / 2 + xpart((counter * 12) + j * ANGLE_QUARTER, 80 - (count * 2)), y_loc + ypart((counter * 16) + j * ANGLE_QUARTER, 40 - count), col2, -1, messages [play] [i]);
    }
    textprintf_centre_ex(bmp, small_font, max_x / 2, y_loc, col1, -1, messages [play] [i]);
 } // end loop
}

void kill_message(int play, int i)
{
// return;

 messages [play] [i] [0] = '\0';
 
}


void init_messages(void)
{
// return;

 int i;

 for (i = 0; i < 4; i ++)
 {
  strcpy(messages [0] [i], "");
  strcpy(messages [1] [i], "");
  message_style [0] [i] = STYLE_NONE;
  message_style [1] [i] = STYLE_NONE;
  message_counter [0] [i] = 0;
  message_counter [1] [i] = 0;
 }

}

// It's okay to send a message to a non-existent player
void send_message(int play, const char *msg, int style)
{
return;
// return;
// return;

 int i;

 for (i = 0; i < 4; i ++)
 {
  if (messages [play] [i] [0] == 0 || i == 3)
   break;
 }
 // slot 3 will be overwritten if there're no gaps

 strcpy(messages [play] [i], msg);
 message_style [play] [i] = style;
 message_counter [play] [i] = 0;
 if (game.users > 1)
 {
  message_counter [play] [i] = 10;
 }

}

*/
void display_quit_query(void)
{
 if (game.mode_void == 0)
 {
  drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
  rectfill(screen, 140 + text_offset_x, 150 + text_offset_y, 500 + text_offset_x, 280 + text_offset_y, TRANS_DGREY);
  drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
 } 
 
 rect(screen, 140 + text_offset_x, 150 + text_offset_y, 500 + text_offset_x, 280 + text_offset_y, COLOUR_BLUE2);
 rect(screen, 139 + text_offset_x, 149 + text_offset_y, 501 + text_offset_x, 281 + text_offset_y, COLOUR_BLUE3);
 rect(screen, 138 + text_offset_x, 148 + text_offset_y, 502 + text_offset_x, 282 + text_offset_y, COLOUR_BLUE4);
 rect(screen, 137 + text_offset_x, 147 + text_offset_y, 503 + text_offset_x, 283 + text_offset_y, COLOUR_BLUE5);
 rect(screen, 136 + text_offset_x, 146 + text_offset_y, 504 + text_offset_x, 284 + text_offset_y, COLOUR_BLUE6);

 textprintf_centre_ex(screen, font2, 320 + text_offset_x, 180 + text_offset_y, COLOUR_GREEN6, -1, "{__quit__}");
 textprintf_centre_ex(screen, font, 320 + text_offset_x, 180 + text_offset_y, -1, -1, "{__quit__}");
 textprintf_centre_ex(screen, font2, 320 + text_offset_x, 230 + text_offset_y, COLOUR_BLUE8, -1, "{_y__or__n_}");
 textprintf_centre_ex(screen, font, 320 + text_offset_x, 230 + text_offset_y, -1, -1, "{_y__or__n_}");

}

// needed because the box overlaps the space between p1 and p2 screens
void clear_quit_query(void)
{

// vsync();
// clear_bitmap(screen);

 rectfill(screen, 140 + text_offset_x, 140 + text_offset_y, 510 + text_offset_x, 290 + text_offset_y, 0);
 
}






