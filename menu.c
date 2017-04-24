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
    
File: menu.c
History:
11/9/2003 - Version 1.0 finalised

This file contains:
 - the front end GUI. One of the few files other than display.c & displ_in.c
which are authorised to write directly to the screen.

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
#include "sound.h"

#define NO_SCORES 15
#define SCORE_LENGTH 15

#define GRID_GRAIN 100
//#define NO_BOUNCIES 16

#define NO_KEY KEY_TILDE

#define KP_WAIT_LONG 33
#define KP_WAIT_SHORT 5

// let's try to prevent at least the most obvious form of abuse
#define UNLOCK_PURPLE 23498
#define UNLOCK_VOID 132
#define UNLOCK_GOD 357

extern struct optionstruct options;


//#define FIX_TITLE

//#ifdef FIX_TITLE
//extern RGB palet [256];
//#endif

enum
{
MENU_NONE,
MENU_MAIN,
MENU_OPTIONS,
MENU_SCORES,
MENU_ENTER_SCORES,
MENU_SPECIAL,
MENU_DUEL
};

enum
{
DUEL_OPTIONS,
DUEL_START,
DUEL_MODE,
DUEL_ENEMIES,
DUEL_HANDICAP_P1,
DUEL_HANDICAP_P2,
DUEL_LEADER_HANDICAP,
DUEL_EXIT,
DUEL_END

};

enum
{
SPECIAL_TIME_ATTACK,
SPECIAL_DUEL,
SPECIAL_SPACE1,
SPECIAL_MODES,
SPECIAL_PURPLE,
SPECIAL_VOID,
SPECIAL_GOD,
SPECIAL_SPACE2,
SPECIAL_EXIT,
SPECIAL_END

};

enum
{
RANK_SLAVE,
RANK_DOUBTER,
RANK_UNBELIEVER,
RANK_BLASPHEMER,
RANK_APOSTATE,
RANK_HERETIC,
RANK_HERESIARCH,
RANK_SECULARIST,
RANK_DEICIDE,
RANK_DEMIGOD,
RANK_GODLING,
RANK_UNDERGOD,
RANK_LESSER_GOD,
RANK_GREATER_GOD,
RANK_OVERGOD,
RANK_SUPREME_BEING
};

enum
{
MS_SPINNER,
MS_SPIKEY,
MS_SQUAREY,
MS_BLATTER,
MS_ORBITER,
MS_TYPES
};

int ms_circle_rad;
int ms_circle_col;
int ms_arm_col [2];
int ms_arm_out [2];
int ms_arm_angle[2];
int ms_arm_inward [2];
int ms_arm_number [2];
int ms_arm_rot [2];
int ms_arm_rot_delta [2];
int ms_arm_type [2];
int ms_arm_blatter_size [2];


void menu_display(void);
void start_game_from_menu(int game_type);
void menu_input(void);

void menu_display_main(void);
void menu_display_options(void);
void menu_display_duel(void);
void menu_display_special(void);
void menu_display_scores(void);
void menu_trigger(void);
void display_victory_screen(void);
void menu_to_screen(void);

int acceptable_char(int scode);
int acceptable_name_char(int ascode);
void scancode_to_keyname(int scanc, char sstr [30]);
char scancode_to_asc(int scanc);
char get_shifted_number(char inputted);

void test_speakers(void);
void resolution_change(void);

void menu_sound1(void);
void menu_sound2(void);

// in display.c
void draw_overspinner(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2);
void draw_spikey(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2, int in_correction, int out_correction);
void draw_blatter(BITMAP *bmp, int x, int y, int number, int distance, int rotation, int size, int col1, int col2);
void draw_squarey(BITMAP *bmp, int x, int y, int attribute, int outward, int inward, int angle1, int angle2, int arms, int col1, int col2);
void draw_orbiter(BITMAP *bmp, int x, int y, int attribute, int out1, int out2, int out3, int angle1, int angle2, int arms, int col1, int col2);
void make_ms_move(void);
int ms_colour(void);

void anum(char *str, int num);

BITMAP *menu_bmp;
extern volatile unsigned char ticked;
extern int slacktime;

extern FONT *font2;
extern FONT *small_font;

//BITMAP *title_in;
//BITMAP *title_out;
//RLE_SPRITE *rle_title_i;
RLE_SPRITE *rle_title_o;
RLE_SPRITE *menu_tile;
//RLE_SPRITE *rle_title_s;

BITMAP *upgrade_box1;
BITMAP *upgrade_box2;
BITMAP *upgrade_box3;

int menu_index;
int menu_index_max;
int menu_index_min;
int keypress_wait;
int last_key;
int check_key(int scode);
int check_key_disregard_last_key(int scode);
unsigned char menu_counter;

int which_menu;

void quit_game(void);

void change_menu(int to_which);
int option_index(void);
void enter_keystroke(void);
int option_jump(int direction);
int special_jump(int direction);
void init_config(void);
void save_config(void);

void choose_ships(void);
void display_ship_choice(int players, int p1a, int p2a, int p1c, int p2c);
void display_ship_choice2(int ship, int y, int chosen);

void jam_keys(void);

//char *ship_name_long(int ship);
//char *ship_name_short(int ship);
void ship_description1(int ship, char dstr [200]);
void ship_description2(int ship, char dstr [200]);
//char *upgrade_name_long(int i);

int get_rank(int score, int level);
void rank_name(int rank, char sstr [30]);

void init_ms(void);
void draw_ms(void);

extern RLE_SPRITE *large_ships [12] [5];

struct score_list
{
  int score;
  int ship;
  int level;
  char name [25];
};

struct score_list hs_single [NO_SCORES];
struct score_list hs_coop [NO_SCORES];

int best_ta_time;
char best_ta_name [50];

struct score_list *hscore;

void check_high_scores(void);
int enter_score_name_single(int s);
int enter_score_name_coop(int s);
int enter_score_name_ta(void);
void push_scores_single(int insert, int play);
void push_scores_coop(int insert, int play);
void congratulations(int player);
void unlock_screen(int which);

int entering_score;
int entering_score2;
void draw_scrolling_grid(int min_x, int min_y, int max_x, int max_y, int colour);
void show_grid(int grid_colour, int border_colour);
void make_grid_scroll(void);

int grid_x_speed, grid_y_speed, grid_x, grid_y, grid_x_accel, grid_y_accel;

struct bouncy_list
{
 int x;
 int y;
 int x_speed;
 int y_speed;
 int colour1;
 int colour2;
};


struct pyrostruct
{
 int type;
 int x;
 int y;
 int x_speed;
 int y_speed;
 int colours [4];
 int timeout;
 int size;
 int dist;
};

struct sparklestruct
{
 int type;
 int x;
 int y;
 int x_speed;
 int y_speed;
// int colours [4];
 int size;
 int tickrate;
 int colour;
};

#define NO_PYROS 30
#define NO_SPARKLES 1000

enum
{
PYRO_NONE,
PYRO_ENDING,
PYRO_SPLIT1,
PYRO_SPLIT2,
PYRO_RANDSPLIT
};

enum
{
SPARKLE_NONE,
SPARKLE_CIRCLE
};

struct pyrostruct pyro [NO_PYROS];
struct sparklestruct sparkle [NO_SPARKLES];
int next_pyro;

void init_pyro(void);
void run_pyros(void);
void manage_pyros(void);
void create_pyro(int x, int y, int xs, int ys, int type, int colours [4], int size, int dist, int timeout);
void run_sparkles(void);
void draw_sparkles(void);
void create_sparkle(int x, int y, int xs, int ys, int type, int colour, int size, int tickrate);
void destroy_pyro(int p);
void destroy_sparkle(int s);
void drag_pyro(int p);
void drag_sparkle(int s);
void colourify(int col [4]);
void pyro_burst(int p);
void draw_a_light(BITMAP *bmp, int size, int x, int y); // in display.cc
void pyro_sound(int x, int size);


//struct bouncy_list bouncy [NO_BOUNCIES];

//void init_bouncies(void);
//void make_bouncies_move(void);
//void draw_bouncies(void);

//RGB title_colour1;
//RGB title_colour2;

void init_menus_once_only(void)
{
// text_mode(-1);
 init_config();

 menu_bmp = create_bitmap(640, 480);

 if (!menu_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create menu bitmap! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 menu_counter = 0;
 entering_score = 0;

 grid_x_speed = grand(6001) - 3000;
 grid_y_speed = grand(6001) - 3000;
 grid_x_accel = 0;
 grid_y_accel = 0;

 RGB temp_palette [256];

/* BITMAP *temp_bmp = load_bitmap("gfx\\title_i.bmp", temp_palette);

 if (!temp_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load title_i.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 rle_title_i = get_rle_sprite(temp_bmp);

 if (!rle_title_i)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create rle_title_i! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 destroy_bitmap(temp_bmp);
*/
 BITMAP *temp_bmp = load_bitmap("gfx//gb_title.bmp", temp_palette);

 if (!temp_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load gb_title.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

//#ifdef FIX_TITLE
// save_bitmap("gb_title2.bmp", temp_bmp, palet);
//#endif
 int i, j;
 
 for (i = 0; i < 1000; i ++)
 {
  for (j = 0; j < 1000; j ++)
  {
   switch(getpixel(temp_bmp, i, j))
   {
    case 0: 
    case 255: break; //putpixel(temp_bmp, i, j, 0);
    case 160: putpixel(temp_bmp, i, j, COLOUR_GREY1); break;
    default: putpixel(temp_bmp, i, j, COLOUR_GREY4); break;
   }
  }
 }

 rle_title_o = get_rle_sprite(temp_bmp);

 if (!rle_title_o)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create rle_title_o! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 destroy_bitmap(temp_bmp);
 
/* temp_bmp = create_bitmap(50, 50);
 
 clear_bitmap(temp_bmp);
 
 circlefill(temp_bmp, 25, 25, 8, COLOUR_GREEN3);
 
 menu_tile = get_rle_sprite(temp_bmp);
 
 destroy_bitmap(temp_bmp);*/
 
/*
 temp_bmp = load_bitmap("gfx//title_s.bmp", temp_palette);

 if (!temp_bmp)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load title_s.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 rle_title_s = get_rle_sprite(temp_bmp);

 if (!rle_title_s)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to create rle_title_s! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 destroy_bitmap(temp_bmp);
*/
/* upgrade_box1 = load_bitmap("gfx//upg_box.bmp", temp_palette);

 if (!upgrade_box1)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load upg_box.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 upgrade_box2 = load_bitmap("gfx//upg_box2.bmp", temp_palette);

 if (!upgrade_box2)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load upg_box2.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }

 upgrade_box3 = load_bitmap("gfx//upg_box3.bmp", temp_palette);

 if (!upgrade_box3)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Unable to load upg_box3.bmp! \n\r%s\n\r", allegro_error);
      exit(1);
 }
*/
 last_key = NO_KEY; // something meaningless

 game.duel_mode = 0;
 game.duel_handicap [0] = 1;
 game.duel_handicap [1] = 1;
 game.duel_leader_handicap = 0;
 game.duel_crawlies = 1;
 
}

void menu_loop(void)
{

 which_menu = MENU_MAIN;
 menu_index_max = 4;
 menu_index_min = 0;
 keypress_wait = KP_WAIT_LONG;
 game.type = GAME_SINGLE;

 do
 {
//  grand(100);
  menu_counter++;
//  srand(menu_counter);
  
  set_config_int("Misc", "Seed", grand(10000));
  
  if (check_key_disregard_last_key(last_key) == 0)
   keypress_wait = 0;

  if (keypress_wait > 0)
  {
      keypress_wait --;
  }
  
  make_grid_scroll();
//  if (which_menu == MENU_MAIN || which_menu == MENU_SCORES || which_menu == MENU_ENTER_SCORES)
  if (which_menu == MENU_SCORES || which_menu == MENU_ENTER_SCORES)
   make_ms_move();

  if (ticked == 0)
  {
   clear_bitmap(menu_bmp);
   menu_display();
   menu_to_screen();
  }

  menu_input();

  slacktime = 0;
  do
  {
   slacktime ++;
  } while(ticked == 0);
  ticked --;

 } while(TRUE);


}


void menu_display(void)
{
// text_mode(-1);

 switch(which_menu)
 {
  case MENU_MAIN:
  menu_display_main();
  break;
  case MENU_OPTIONS:
  menu_display_main();
  menu_display_options();
  break;
  case MENU_SPECIAL:
  menu_display_main();
  menu_display_special();
  break;
  case MENU_DUEL:
  menu_display_main();
  menu_display_duel();
  break;
  case MENU_SCORES:
  menu_display_scores();
  break;
  case MENU_ENTER_SCORES:
  menu_display_scores();
  break;
 }

}


void menu_display_main(void)
{

//  text_mode(-1);

//  vsync();

//  RGB title_colour [1];
  static RGB title_colour [1] = {{16,16,16}};
/*
  int mcn = menu_counter * 2;
  int mcn2 = menu_counter;// / 2;
  int mcn3 = menu_counter / 2;

  if (mcn % 64 < 32)
  {
   title_colour[0].r = mcn % 64;
  }
   else
   {
    title_colour[0].r = 63 - (mcn % 64);
   }
  
  if (mcn2 % 64 < 32)
  {
   title_colour[0].g = (63 - mcn2 % 64) / 4;
  }
   else
   {
    title_colour[0].g = (mcn2 % 64) / 4;
   }
  
  if (mcn3 % 64 < 32)
  {
   title_colour[0].b = ((mcn3 + 32) % 64);// / 2;
  }
   else
   {
    title_colour[0].b = (63 - ((mcn3 + 32) % 64));// / 2;
   }
  


  set_color(252, title_colour);*/
/*
  mcn = menu_counter * 2;
  mcn2 = menu_counter;// / 2;
  mcn3 = menu_counter / 2;

  if (mcn % 64 >= 32)
  {
   title_colour[0].b = (mcn + 32) % 64;
  }
   else
   {
    title_colour[0].b = 63 - ((mcn + 32) % 64);
   }
  
  if (mcn2 % 64 >= 32)
  {
   title_colour[0].g = (63 - mcn2 % 64);// / 2;
  }
   else
   {
    title_colour[0].g = (mcn2 % 64);// / 2;
   }
  
  if (mcn3 % 64 >= 32)
  {
   title_colour[0].r = mcn3 % 64;
  }
   else
   {
    title_colour[0].r = 63 - (mcn3 % 64);
   }
*/

  static RGB change [1] = {{1, 0, 0}};


//  if (title_colour [1].r >= 60 || title_colour [1].r <= 5
//      || 
/*
  if (title_colour [0].r >= 60)
   change[0].r = -1 - grand(5);
  if (title_colour [0].g >= 60)
   change[0].g = -1 - grand(5);
  if (title_colour [0].b >= 60)
   change[0].b = -1 - grand(5);
      
  if (title_colour [0].r <= 10)
   change[0].r = 1 + grand(5);
  if (title_colour [0].g <= 10)
   change[0].g = 1 + grand(5);
  if (title_colour [0].b <= 10)
   change[0].b = 1 + grand(5);
  
  title_colour [0].r += change [0].r;
  title_colour [0].g += change [0].g;
  title_colour [0].b += change [0].b;
  
  if (title_colour [0].r > 63)
   title_colour [0].r = 63;
  if (title_colour [0].g > 63)
   title_colour [0].g = 63;
  if (title_colour [0].b > 63)
   title_colour [0].b = 63;
*/  
/*
  if (title_colour [0].r >= 60)
   {
    change[0].r = -1 - grand(5);
    change[0].g *= -1;
    change[0].b *= -1;
   }
  if (title_colour [0].g >= 60)
  {
    change[0].r *= -1;
    change[0].g = -1 - grand(5);
    change[0].b *= -1;
  } 
  if (title_colour [0].b >= 60)
  {
    change[0].r *= -1;
    change[0].g *= -1;
    change[0].b = -1 - grand(5);
  }
      
  if (title_colour [0].r <= 10)
  {
    change[0].r = 1 + grand(5);
    change[0].g *= -1;
    change[0].b *= -1;
  }
  if (title_colour [0].g <= 10)
  {
    change[0].r *= -1;
    change[0].g = 1 + grand(5);
    change[0].b *= -1;
  }
  if (title_colour [0].b <= 10)
  {
    change[0].r *= -1;
    change[0].g *= -1;
    change[0].b = 1 + grand(5);
  }
  
  title_colour [0].r += change [0].r;
  title_colour [0].g += change [0].g;
  title_colour [0].b += change [0].b;
*/  

 if (menu_counter % 2 == 0)
 {

  if (title_colour [0].r >= 55
      || title_colour [0].g >= 55
      || title_colour [0].b >= 55)
      {
       change [0].r = abs(change[0].r) * -1;
       change [0].g = abs(change[0].g) * -1;
       change [0].b = abs(change[0].b) * -1;
       if (grand(5) == 0)
       {
        change[0].r = grand(3) * -1;
        change[0].g = grand(3) * -1;
        change[0].b = grand(3) * -1;
       }
//       change [0].r *= -1;
//       change [0].g *= -1;
//       change [0].b *= -1;
      }
      
  if (title_colour [0].r <= 10
      || title_colour [0].g <= 10
      || title_colour [0].b <= 10)
      {
       if (title_colour [0].r <= 15
         && title_colour [0].g <= 15
         && title_colour [0].b <= 15)
         {
          change[0].r = 0;
          change[0].g = 0;
          change[0].b = 0;
         }
       if (grand(3) != 0)
       {
        change[0].r = grand(3);
        change[0].g *= -1;
        change[0].b *= -1;
       } 
         else
         {
          if (grand(3) == 0)
          {
           change[0].r *= -1;
           change[0].g = grand(3);
           change[0].b *= -1;
          } 
            else
            {
             if (grand(3) == 0)
             {
              change[0].r *= -1;
              change[0].g *= -1;
              change[0].b = grand(3);
          } 
            }
         }  
       if (change[0].r + change[0].g + change[0].b == 0)
        change[0].r = 1;
       
      }
  
  title_colour [0].r += change [0].r;
  title_colour [0].g += change [0].g;
  title_colour [0].b += change [0].b;
  
  if (title_colour [0].r > 55)
   title_colour [0].r = 55;
  if (title_colour [0].g > 55)
   title_colour [0].g = 55;
  if (title_colour [0].b > 55)
   title_colour [0].b = 55;

 }

  set_color(255, title_colour);
  
/*  int mcn = menu_counter;
  int mcn2 = menu_counter / 2;
  int mcn3 = menu_counter / 4;

  if (mcn % 64 < 32)
  {
   title_colour[0].r = mcn % 64;
   title_colour[0].g = (63 - mcn2 % 64) / 4;
   title_colour[0].b = ((mcn3 + 32) % 64);// / 2;
  }
   else
   {
    title_colour[0].r = 63 - (mcn % 64);
    title_colour[0].g = (mcn2 % 64) / 4;
    title_colour[0].b = (63 - ((mcn3 + 32) % 64));// / 2;
   }
  
  set_color(252, title_colour);

  mcn = menu_counter / 4;
  mcn2 = menu_counter / 8;
  mcn3 = menu_counter / 16;

  if (mcn % 64 >= 32)
  {
   title_colour[0].r = mcn3 % 64;
   title_colour[0].g = (63 - mcn2 % 64);// / 2;
   title_colour[0].b = (mcn + 32) % 64;
  }
   else
   {
    title_colour[0].r = 63 - (mcn3 % 64);
    title_colour[0].g = (mcn2 % 64);// / 2;
    title_colour[0].b = 63 - ((mcn + 32) % 64);
   }
  
  set_color(255, title_colour);*/
//  RGB title_colour1;
//RGB title_colour2;

/*  rectfill(menu_bmp, 10, 10, 630, 470, COLOUR_GREY1);
  rect(menu_bmp, 10, 10, 630, 470, COLOUR_GREEN4);
  rect(menu_bmp, 9, 9, 631, 471, COLOUR_GREEN6);
  rect(menu_bmp, 8, 8, 632, 472, COLOUR_GREEN8);

  draw_scrolling_grid(11, 11, 629, 469, COLOUR_GREEN3);
*/
  show_grid(COLOUR_GREEN3, COLOUR_GREEN8);

//  textprintf_centre_ex(menu_bmp, font, 300, 50, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
//  textprintf_centre_ex(menu_bmp, font2, 300, 50, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");


//  draw_rle_sprite(menu_bmp, rle_title_i, 30, 0);
  draw_rle_sprite(menu_bmp, rle_title_o, 10, 105);
//  draw_rle_sprite(menu_bmp, rle_title_s, 280, 173);

  rectfill(menu_bmp, 340, 0, 346, 480, 255);
  rectfill(menu_bmp, 0, 200, 15, 205, 255);
  rectfill(menu_bmp, 640, 200, 615, 205, 255);
  
//  int col1 = COLOUR_GREY3;
//  int col2 = COLOUR_GREY5;
  int scol1 = COLOUR_GREEN8 - (menu_counter / 3) % 3;
  int scol2 = COLOUR_BLUE6 + (menu_counter / 3) % 3;

  scol1 = 255;
  scol2 = 255;

  int mx = 340;
//  int my = 212;
  int my = 5;
  int my2 = 200;
  int mys = 55;

  if (menu_index == 0 && which_menu == MENU_MAIN)
  {
   textprintf_right_ex(menu_bmp, font2, mx, my, scol1, -1, "{____start__game___");
   textprintf_right_ex(menu_bmp, font, mx, my, -1, -1, "{____start__game___");
//   textprintf_right_ex(menu_bmp, font2, 400, 120, COLOUR_WHITE, "Start} Game}");
//   textprintf_right_ex(menu_bmp, font, 400, 120, COLOUR_WHITE, "Start} Game}");
  }
   else
    textprintf_right_ex(menu_bmp, font, mx, my, -1, -1, "{____start__game___");
  
  if (menu_index == 1 && which_menu == MENU_MAIN)
  {
   textprintf_right_ex(menu_bmp, font2, mx, my + mys, scol1, -1, "{____special___");
   textprintf_right_ex(menu_bmp, font, mx, my + mys, -1, -1, "{____special___");
  }
   else
    textprintf_right_ex(menu_bmp, font, mx, my + mys, -1, -1, "{____special___");
  
  if (menu_index == 2 && which_menu == MENU_MAIN)
  {
   textprintf_right_ex(menu_bmp, font2, mx, my2 + mys * 2, scol1, -1, "{____options___");
   textprintf_right_ex(menu_bmp, font, mx, my2 + mys * 2, -1, -1, "{____options___");
  }
   else
    textprintf_right_ex(menu_bmp, font, mx, my2 + mys * 2, -1, -1, "{____options___");
  
  if (menu_index == 3 && which_menu == MENU_MAIN)
  {
   textprintf_right_ex(menu_bmp, font2, mx, my2 + mys * 3, scol1, -1, "{____high__scores___");
   textprintf_right_ex(menu_bmp, font, mx, my2 + mys * 3, -1, -1, "{____high__scores___");
  }
   else
    textprintf_right_ex(menu_bmp, font, mx, my2 + mys * 3, -1, -1, "{____high__scores___");

  if (menu_index == 4 && which_menu == MENU_MAIN)
  {
   textprintf_right_ex(menu_bmp, font2, mx, my2 + mys * 4, scol1, -1, "{____exit___");
   textprintf_right_ex(menu_bmp, font, mx, my2 + mys * 4, -1, -1, "{____exit___");
  }
   else
    textprintf_right_ex(menu_bmp, font, mx, my2 + mys * 4, -1, -1, "{____exit___");
  
/*  textprintf_centre_ex(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre_ex(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");*/

}

void menu_display_options(void)
{

  rectfill(menu_bmp, 180, 50, 440, 400, COLOUR_YELLOW1);
  rect(menu_bmp, 180, 50, 440, 400, COLOUR_YELLOW6);
  rect(menu_bmp, 179, 49, 441, 401, COLOUR_YELLOW7);
  rect(menu_bmp, 178, 48, 442, 402, COLOUR_YELLOW8);

  int i;

  int ox = 350, oy = 11, oys = 75;
  int col;

  char ostr [50];
//  char istr [10];
  char sstr [30];

  for (i = 0; i < 28; i ++)
  {

   col = COLOUR_GREEN4;

   if (i == menu_index)
    col = COLOUR_ORANGE8;
   
   switch(i)
   {
    case 0:
     col = COLOUR_GREY6;
     strcpy(ostr, "General Options");
     break;
    case 1:
     strcpy(ostr, "Sound - ");
     if (options.sound_init == 0)
      strcat(ostr, "Disabled in config file");
       else
       {
        switch(options.sound_mode)
        {
         case SOUNDMODE_OFF:
          strcat(ostr, "Off");
          break;
         case SOUNDMODE_MONO:
          strcat(ostr, "On (Mono)");
          break;
         case SOUNDMODE_STEREO:
          strcat(ostr, "On (Stereo)");
          break;
         case SOUNDMODE_REVERSED:
          strcat(ostr, "On (Reverse Stereo)");
          break;
         default:
          strcat(ostr, "Buggy sound settings?");
          break;
        }
       }
     break;
    case 2:
     strcpy(ostr, "Sound Effects Volume - ");
     if (options.sound_volume == 0)
     {
      strcat(ostr, "Off");
     }
      else
//       strcat(ostr, itoa(options.sound_volume, istr, 10));
         anum(ostr, options.sound_volume);
     break;
    case 3:
     strcpy(ostr, "Ambience Volume - ");
     if (options.ambience_volume == 0)
     {
      strcat(ostr, "Off");
     }
      else
//       strcat(ostr, itoa(options.ambience_volume, istr, 10));
         anum(ostr, options.ambience_volume);
     break;
    case 4:
     strcpy(ostr, "Video Sync - ");
     if (options.run_vsync == 0)
      strcat(ostr, "Off");
       else
        strcat(ostr, "On");
     break;
    case 5:
     strcpy(ostr, "Test Speakers");
     break;
    case 6:
     strcpy(ostr, "Test Keys");
     break;
    case 7:
     strcpy(ostr, "Resolution - ");
        switch(options.resolution)
        {
         case 0:
          strcat(ostr, "640x480 full");
          break;
         case 1:
          strcat(ostr, "800x600 full");
          break;
         case 2:
          strcat(ostr, "640x480 window");
          break;
         case 3:
          strcat(ostr, "800x600 window");
          break;
        }
     break;
    case 8:
     strcpy(ostr, "Colour Text - ");
     if (options.colour_text == 0)
      strcat(ostr, "Off");
       else
        strcat(ostr, "On");
     break;
    case 9:
     strcpy(ostr, "");
     break;
    case 10:
     col = COLOUR_GREY6;
     strcpy(ostr, "Player 1 Keys");
     break;
    case 11:
     strcpy(ostr, "Forwards - ");
     scancode_to_keyname(player[0].keys [CMD_THRUST], sstr);
     strcat(ostr, sstr);
     break;
    case 12:
     strcpy(ostr, "Left - ");
     scancode_to_keyname(player[0].keys [CMD_LEFT], sstr);
     strcat(ostr, sstr);
     break;
    case 13:
     strcpy(ostr, "Right - ");
     scancode_to_keyname(player[0].keys [CMD_RIGHT], sstr);
     strcat(ostr, sstr);
     break;
    case 14:
     strcpy(ostr, "Brake - ");
     scancode_to_keyname(player[0].keys [CMD_BRAKE], sstr);
     strcat(ostr, sstr);
     break;
    case 15:
     strcpy(ostr, "Fire Darts - ");
     scancode_to_keyname(player[0].keys [CMD_FIRE1], sstr);
     strcat(ostr, sstr);
     break;
    case 16:
     strcpy(ostr, "Fire Secondary - ");
     scancode_to_keyname(player[0].keys [CMD_FIRE2], sstr);
     strcat(ostr, sstr);
     break;
/*    case 15:
     strcpy(ostr, "Upgrade - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_UPGRADE]));
     break;
    case 16:
     strcpy(ostr, "Slide Left - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_LEFT1]));
     break;
    case 17:
     strcpy(ostr, "Slide Right - ");
     strcat(ostr, scancode_to_keyname(player[0].keys [CMD_RIGHT1]));
     break;*/
    case 17:
     strcpy(ostr, "Toggle Linked Fire - ");
     scancode_to_keyname(player[0].keys [CMD_LINK], sstr);
     strcat(ostr, sstr);
     break;
    case 18:
     col = COLOUR_GREY6;
     strcpy(ostr, "Player 2 Keys");
     break;
    case 19:
     strcpy(ostr, "Forwards - ");
     scancode_to_keyname(player[1].keys [CMD_THRUST], sstr);
     strcat(ostr, sstr);
     break;
    case 20:
     strcpy(ostr, "Left - ");
     scancode_to_keyname(player[1].keys [CMD_LEFT], sstr);
     strcat(ostr, sstr);
     break;
    case 21:
     strcpy(ostr, "Right - ");
     scancode_to_keyname(player[1].keys [CMD_RIGHT], sstr);
     strcat(ostr, sstr);
     break;
    case 22:
     strcpy(ostr, "Brake - ");
     scancode_to_keyname(player[1].keys [CMD_BRAKE], sstr);
     strcat(ostr, sstr);
     break;
    case 23:
     strcpy(ostr, "Fire Darts - ");
     scancode_to_keyname(player[1].keys [CMD_FIRE1], sstr);
     strcat(ostr, sstr);
     break;
    case 24:
     strcpy(ostr, "Fire Secondary - ");
     scancode_to_keyname(player[1].keys [CMD_FIRE2], sstr);
     strcat(ostr, sstr);
     break;
/*    case 23:
     strcpy(ostr, "Upgrade - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_UPGRADE]));
     break;
    case 27:
     strcpy(ostr, "Slide Left - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_LEFT1]));
     break;
    case 28:
     strcpy(ostr, "Slide Right - ");
     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_RIGHT1]));
     break;*/
    case 25:
     strcpy(ostr, "Toggle Linked Fire - ");
     scancode_to_keyname(player[1].keys [CMD_LINK], sstr);
     strcat(ostr, sstr);
     break;
    case 26:
     strcpy(ostr, "");
//     strcat(ostr, scancode_to_keyname(player[1].keys [CMD_LINK])
     break;
    case 27:
     strcpy(ostr, "Exit");
     break;

   }

   textprintf_right_ex(menu_bmp, small_font, ox, oys + oy * i, col, -1, ostr);

  }

/*
  textprintf_centre_ex(menu_bmp, font, 300, 200, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre_ex(menu_bmp, font2, 300, 200, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre_ex(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre_ex(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");
*/
}

int special_jump(int direction)
{

 if (which_menu != MENU_SPECIAL)
  return 0;

 switch(menu_index)// + direction)
 {
//  case 5:
//  case 6:
  case SPECIAL_MODES:
  case SPECIAL_SPACE1:
  case SPECIAL_SPACE2:
   return 1;
  case SPECIAL_PURPLE:
   if (options.unlock_purple != UNLOCK_PURPLE)
    return 1;
   return 0; 
  case SPECIAL_VOID:
   if (options.unlock_void != UNLOCK_VOID)
    return 1;
   return 0; 
  case SPECIAL_GOD:
   if (options.unlock_god != UNLOCK_GOD)
    return 1;
   return 0; 

 }

 return 0;

}


int option_jump(int direction)
{

 if (which_menu == MENU_SPECIAL)
  return special_jump(direction);

 if (which_menu != MENU_OPTIONS)
  return 0;

 switch(menu_index)// + direction)
 {
//  case 5:
//  case 6:
  case 9:
  case 10:
  case 18:
  case 26:
   return 1;
 }

 return 0;

}



void menu_display_duel(void)
{

  rectfill(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN1);
  rect(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN6);
  rect(menu_bmp, 179, 49, 441, 401, COLOUR_GREEN7);
  rect(menu_bmp, 178, 48, 442, 402, COLOUR_GREEN8);


  textprintf_centre_ex(menu_bmp, font2, 320, 90, COLOUR_RED8 - (menu_counter / 4) % 4, -1, "{_duel_}");
  textprintf_centre_ex(menu_bmp, font, 320, 90, -1, -1, "{_duel_}");
//  textprintf_centre_ex(menu_bmp, font, 320, 110, COLOUR_ORANGE8, "K_e_y_s}");
//  textprintf_centre_ex(menu_bmp, font2, 320, 110, COLOUR_YELLOW8, "K_e_y_s}");


  int i;

  int ox = 320, oy = 11, oys = 200;
  int col;

  char ostr [50];

  for (i = 0; i < DUEL_END; i ++)
  {

   col = COLOUR_GREEN6;

   if (i == menu_index)
    col = COLOUR_YELLOW8;
   
   switch(i)
   {
    case DUEL_OPTIONS:
     col = COLOUR_GREY6;
     strcpy(ostr, "Duel Options - Space or Enter to select");
     break;
    case DUEL_MODE:
     strcpy(ostr, "Victory goes to the ");
     switch(game.duel_mode)
     {
      case DUEL_3_MINUTES:
       strcat(ostr, "winner after 3 minutes");
       break;
      case DUEL_10_MINUTES:
       strcat(ostr, "winner after 10 minutes");
       break;
      case DUEL_10_POINTS:
       strcat(ostr, "first to 10 points");
       break;
      case DUEL_30_POINTS:
       strcat(ostr, "first to 30 points");
       break;
     }
     break;
    case DUEL_ENEMIES:
     strcpy(ostr, "Enemies - ");
     switch(game.duel_crawlies)
     {
       case 0:
       strcat(ostr, "None");
       break;
       case 1:
       strcat(ostr, "A few");
       break;
       case 2:
       strcat(ostr, "Plentiful");
       break;
       case 3:
       strcat(ostr, "Swarming");
       break;
     }
     break;
    case DUEL_LEADER_HANDICAP:
     strcpy(ostr, "Leader Handicap - ");
     switch(game.duel_leader_handicap)
     {
       case 0:
       strcat(ostr, "None");
       break;
       case 1:
       strcat(ostr, "A little");
       break;
       case 2:
       strcat(ostr, "A lot");
       break;
     }
     break;
    case DUEL_HANDICAP_P1:
     strcpy(ostr, "Player 1 Hull - ");
     switch(game.duel_handicap [0])
     {
       case 0:
       strcat(ostr, "75%%");
       break;
       case 1:
       strcat(ostr, "100%%");
       break;
       case 2:
       strcat(ostr, "120%%");
       break;
       case 3:
       strcat(ostr, "150%%");
       break;
     }
     break;
    case DUEL_HANDICAP_P2:
     strcpy(ostr, "Player 2 Hull - ");
     switch(game.duel_handicap [1])
     {
       case 0:
       strcat(ostr, "75%%");
       break;
       case 1:
       strcat(ostr, "100%%");
       break;
       case 2:
       strcat(ostr, "120%%");
       break;
       case 3:
       strcat(ostr, "150%%");
       break;
     }
     break;
    case DUEL_START:
     strcpy(ostr, "Start Duel");
     break;
    case DUEL_EXIT:
     strcpy(ostr, "Exit");
     break;

   }

   textprintf_centre_ex(menu_bmp, small_font, ox, oys + oy * i, col, -1, ostr);

  }

/*
  textprintf_centre_ex(menu_bmp, font, 300, 200, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre_ex(menu_bmp, font2, 300, 200, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre_ex(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre_ex(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");
*/
}

void menu_display_special(void)
{

  rectfill(menu_bmp, 180, 50, 440, 400, COLOUR_BLUE1);
  rect(menu_bmp, 180, 50, 440, 400, COLOUR_BLUE6);
  rect(menu_bmp, 179, 49, 441, 401, COLOUR_BLUE7);
  rect(menu_bmp, 178, 48, 442, 402, COLOUR_BLUE8);


  textprintf_centre_ex(menu_bmp, font2, 320, 90, COLOUR_YELLOW8 - (menu_counter / 4) % 4, -1, "{_special_}");
  textprintf_centre_ex(menu_bmp, font, 320, 90, -1, -1, "{_special_}");


  int i;

  int ox = 320, oy = 11, oys = 200;
  int col;

  char ostr [50];

  for (i = 0; i < SPECIAL_END; i ++)
  {

   col = COLOUR_GREY5;

   if (i == menu_index)
    col = COLOUR_RED8;

   switch(i)
   {
    case SPECIAL_TIME_ATTACK:
     strcpy(ostr, "Start Time Attack");
     break;
    case SPECIAL_DUEL:
     strcpy(ostr, "Start 2-Player Duel");
     break;
    case SPECIAL_MODES:
     strcpy(ostr, "Special Modes");
     col = COLOUR_GREY4;
     break;
    case SPECIAL_PURPLE:
     if (options.unlock_purple != UNLOCK_PURPLE)
     {
      strcpy(ostr, "You haven't unlocked Purple Mode");
      col = COLOUR_GREY3;
      if (i == menu_index)
       col = COLOUR_RED6;
      break;
     }
     strcpy(ostr, "Purple Mode - ");
     switch(game.mode_purple)
     {
      case 0:
       strcat(ostr, "Off");
       break;
      case 1:
       strcat(ostr, "On!");
       col = TRANS_PURPLE;
       if (i == menu_index)
        col = COLOUR_BLUE8;
       break;
     }
     break;
    case SPECIAL_VOID:
     if (options.unlock_void != UNLOCK_VOID)
     {
      strcpy(ostr, "You haven't unlocked Night Mode");
      col = COLOUR_GREY3;
      if (i == menu_index)
       col = COLOUR_RED6;
      break;
     }
     strcpy(ostr, "Night Mode - "); // renamed after I added the palette effect - before it just made the background GC_GREY1 and disabled the scanner
     switch(game.mode_void)
     {
      case 0:
       strcat(ostr, "Off");
       break;
      case 1:
       strcat(ostr, "On!");
       if (i == menu_index)
        col = COLOUR_RED6;
       break;
     }
     break;
    case SPECIAL_GOD:
     if (options.unlock_god != UNLOCK_GOD)
     {
      strcpy(ostr, "You haven't unlocked God Mode");
      col = COLOUR_GREY3;
      if (i == menu_index)
       col = COLOUR_RED6;
      break;
     }
     strcpy(ostr, "God Mode - ");
     switch(game.mode_god)
     {
      case 0:
       strcat(ostr, "Off");
       break;
      case 1:
       strcat(ostr, "On!");
       if (i == menu_index)
        col = COLOUR_RED8;
       break;
     }
     break;
    case SPECIAL_SPACE1:
    case SPECIAL_SPACE2:
     strcpy(ostr, "");
     break; 
    case SPECIAL_EXIT:
     strcpy(ostr, "Exit");
     break;
   }
   textprintf_centre_ex(menu_bmp, small_font, ox, oys + oy * i, col, -1, ostr);

  }

}


int check_key(int scode)
{
 if (keypress_wait == 0)
  last_key = NO_KEY;

 if (last_key == scode)
 {
  if (key [scode])
   return 0;
    else
     last_key = NO_KEY;
 }

 if (key [scode])
 {
  last_key = scode;
  return 1;
 }
 return 0;
}

int check_key_disregard_last_key(int scode)
{
 if (key [scode])
  return 1;

 return 0;
}






void menu_input(void)
{

 clear_keybuf();

 if (keypress_wait > 0)
  return;

 if (which_menu == MENU_ENTER_SCORES)
 {
  int done = 0;

  if (check_key(KEY_ESC))
  {
   save_config();
   change_menu(MENU_SCORES);
//   last_key = KEY_ESC;
//   menu_sound2();
   menu_sound2();
   return;
  }
 
  if (game.type == GAME_SINGLE)
   done = enter_score_name_single(entering_score);//hs_single[entering_score].name);
  if (game.type == GAME_COOP)
   done = enter_score_name_coop(entering_score);//hs_coop[entering_score].name);
  if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
   done = enter_score_name_ta();
   
  if (done == 1)
  {
   save_config();
   change_menu(MENU_SCORES);
  }
  return;
 }


 if (check_key(KEY_2_PAD) || check_key(KEY_DOWN))
 {
  do
  {
   if (menu_index < menu_index_max)
    menu_index ++;
     else
      menu_index = menu_index_min;
  } while (option_jump(1) == 1);
  keypress_wait = KP_WAIT_SHORT;
  menu_sound1();
//  menu_sound1();
 }

 if (check_key(KEY_8_PAD) || check_key(KEY_UP))
 {
  do
  {
   if (menu_index > menu_index_min)
    menu_index --;
     else
      menu_index = menu_index_max;
  } while (option_jump(-1) == 1);
  keypress_wait = KP_WAIT_SHORT;
  menu_sound1();
//  menu_sound1();
 }

 if (check_key(KEY_ESC))
 {
  switch(which_menu)
  {
   case MENU_MAIN:
    if (keypress_wait == 0 && last_key != KEY_ESC)
    {
     menu_sound2();
     quit_game();
    }
    break;
   case MENU_DUEL:
   case MENU_OPTIONS:
   case MENU_SPECIAL:
    change_menu(MENU_MAIN);
    keypress_wait = KP_WAIT_LONG;
    menu_sound2();
    break;
   case MENU_SCORES:
    change_menu(MENU_MAIN);
    keypress_wait = KP_WAIT_LONG;
    menu_sound2();
    break;
  }
 }

// this has to be after the key_esc thing or pressing escape in some
//  sub-menus will trigger it, as key_waiting won't be checked after
//  the submenus return the menu_trigger call.
// if (key [KEY_ENTER] || key [KEY_ENTER_PAD] || key [KEY_SPACE])
 //if (check_key(KEY_ENTER) || check_key(KEY_ENTER_PAD) || check_key(KEY_SPACE))
 if (check_key(KEY_ENTER) || check_key(KEY_SPACE))
 {
  menu_trigger();
 }


}


void menu_trigger(void)
{

 keypress_wait = KP_WAIT_LONG;

 switch(which_menu)
 {
  case MENU_MAIN:
   switch(menu_index)
   {
    case 0:
     menu_sound2();
     choose_ships();
//     start_game_from_menu(GAME_SINGLE);
     // if it's going to be coop, this is set after each player has had a chance to join
     break;
//    case 1:
//     start_game_from_menu(GAME_COOP);
//     break;
    case 1:
     change_menu(MENU_SPECIAL);
     menu_sound2();
//      display_victory_screen();
     break;
    case 2:
     change_menu(MENU_OPTIONS);
     menu_sound2();
     break;
    case 3:
     change_menu(MENU_SCORES);
     menu_sound2();
//     high_score_list();
     break;
    case 4:
     menu_sound2();
     quit_game();
     break;
   }
   break;
  case MENU_OPTIONS:
   if (menu_index == 27)
   {
    menu_sound2();
    change_menu(MENU_MAIN);
    break;
   }
   if (menu_index < 10)
   {
    switch(menu_index)
    {
     case 1:
      if (options.sound_mode == SOUNDMODE_REVERSED)
       options.sound_mode = SOUNDMODE_OFF;
        else
         options.sound_mode++;
      menu_sound1();
      return;
     case 2:
      if (options.sound_volume >= 100)
       options.sound_volume = 0;
        else
         options.sound_volume += 20;
      menu_sound1();
      return;
     case 3:
      if (options.ambience_volume >= 100)
       options.ambience_volume = 0;
        else
         options.ambience_volume += 20;
      menu_sound1();
      return;
     case 4:
      if (options.run_vsync == 1)
       options.run_vsync = 0;
        else
         options.run_vsync = 1;
      menu_sound1();
      return;
     case 5:
      test_speakers();
      ticked = 0;
// test speakers
      return;
     case 6:
      menu_sound2();
      jam_keys();
      return;
     case 7: 
      if (options.resolution == 3)
       options.resolution = 0;
        else
         options.resolution ++;
/*      if (options.resolution == GMODE_800_600)
       options.resolution = GMODE_640_480;
        else
         options.resolution = GMODE_800_600;*/
      resolution_change();   
      menu_sound1();
      break;
     case 8:
      if (options.colour_text == 1)
       options.colour_text = 0;
        else
         options.colour_text = 1;
      menu_sound1();
      return;

    }
   }
   if (option_index() != -1)
   {
    menu_sound2();
    enter_keystroke();
    menu_sound2();
   }
   break;
  case MENU_SCORES:
   menu_sound2();
   change_menu(MENU_MAIN);
   break;
  case MENU_ENTER_SCORES:
   menu_sound2();
   change_menu(MENU_MAIN);
   break; // return from this through enter_scores (or whatever)
  case MENU_SPECIAL:
   switch(menu_index)
   {
    case SPECIAL_DUEL:
     menu_sound2();
     change_menu(MENU_DUEL);
     break;
    case SPECIAL_TIME_ATTACK: 
     game.type = GAME_TIME_ATTACK;
     menu_sound2();
     choose_ships();
     break;
    case SPECIAL_EXIT:
     menu_sound2();
     change_menu(MENU_MAIN);
     break;
    case SPECIAL_PURPLE:
     if (options.unlock_purple == 0)
      break;
     if (game.mode_purple == 0)
      game.mode_purple = 1;
       else
        game.mode_purple = 0;
     menu_sound2();
     break;
    case SPECIAL_VOID:
     if (options.unlock_void == 0)
      break;
     if (game.mode_void == 0)
     {
      game.mode_void = 1;
      set_dark_palette();
     } 
       else
       {
        game.mode_void = 0;
        set_light_palette();
       } 
     menu_sound2();
     break;
    case SPECIAL_GOD:
     if (options.unlock_god == 0)
      break;
     if (game.mode_god == 0)
      game.mode_god = 1;
       else
        game.mode_god = 0;
     menu_sound2();
     break;
    } 
   break; 
  case MENU_DUEL:
   switch(menu_index)
   {
    case DUEL_START:
     game.type = GAME_DUEL;
     menu_sound2();
     choose_ships();
     break;
    case DUEL_ENEMIES:
     if (game.duel_crawlies < 3)
      game.duel_crawlies ++;
       else
        game.duel_crawlies = 0;
     menu_sound2();
     break;
    case DUEL_MODE:
     if (game.duel_mode < 3)
      game.duel_mode ++;
       else
        game.duel_mode = 0;
     menu_sound2();
     break;
    case DUEL_LEADER_HANDICAP:
     if (game.duel_leader_handicap < 2)
      game.duel_leader_handicap ++;
       else
        game.duel_leader_handicap = 0;
     menu_sound2();
     break;
    case DUEL_HANDICAP_P1:
     if (game.duel_handicap [0] < 3)
      game.duel_handicap [0] ++;
       else
        game.duel_handicap [0] = 0;
     menu_sound2();
     break;
    case DUEL_HANDICAP_P2:
     if (game.duel_handicap [1] < 3)
      game.duel_handicap [1] ++;
       else
        game.duel_handicap [1] = 0;
     menu_sound2();
     break;
    case DUEL_EXIT:
     change_menu(MENU_MAIN);
     menu_sound2();
     break;
   }
   break;
   
 }

}


void enter_keystroke(void)
{
 rectfill(screen, 400, 210, 550, 280, COLOUR_GREY2);
 rect(screen, 400, 210, 550, 280, COLOUR_GREY6);
 rect(screen, 401, 211, 549, 279, COLOUR_GREY5);
 rect(screen, 402, 212, 548, 278, COLOUR_GREY4);
 rect(screen, 403, 213, 547, 277, COLOUR_GREY3);
 textprintf_right_ex(screen, small_font, 500, 240, COLOUR_WHITE, -1, "Press a key...");
          
 clear_keybuf();
 int inputted = KEY_ESC;

 int i;

 rest(200); // to clear the enter or space key

 do
 {
  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (check_key(i))
   {
    inputted = i;
   }
  }
 }
   while(acceptable_char(inputted) == 0);
/*
  do
  {
   inputted = readkey();
  }
   while(acceptable_char(inputted >> 8) == 0);

  inputted = inputted >> 8;
*/
  if (option_index() == -1)
   return;

  if (menu_index < 19)
   player[0].keys [option_index()] = inputted;
    else
     player[1].keys [option_index()] = inputted;

  ticked = 0;


}

int option_index(void)
{
  switch(menu_index)
  {
   case 11:
   case 19:
    return CMD_THRUST;
   case 12:
   case 20:
    return CMD_LEFT;
   case 13:
   case 21:
    return CMD_RIGHT;
   case 14:
   case 22:
    return CMD_BRAKE;
   case 15:
   case 23:
    return CMD_FIRE1;
   case 16:
   case 24:
    return CMD_FIRE2;
//   case 15:
//   case 26:
//    return CMD_UPGRADE;
//   case 16:
//   case 27:
//    return CMD_LEFT1;
//   case 17:
//   case 28:
//    return CMD_RIGHT1;
   case 17:
   case 25:
    return CMD_LINK;
  }

 return -1;
 
}


void test_speakers(void)
{
 rectfill(screen, 400, 210, 550, 280, COLOUR_GREY2);
 rect(screen, 400, 210, 550, 280, COLOUR_GREY6);
 rect(screen, 401, 211, 549, 279, COLOUR_GREY5);
 rect(screen, 402, 212, 548, 278, COLOUR_GREY4);
 rect(screen, 403, 213, 547, 277, COLOUR_GREY3);
 if (options.sound_init == 0)
 {
  textprintf_right_ex(screen, small_font, 520, 240, COLOUR_WHITE, -1, "Sound disabled.");
  rest(1000);
  return;
 }
 textprintf_right_ex(screen, small_font, 520, 240, COLOUR_WHITE, -1, "Testing left speaker. . .");
 if (options.sound_mode == SOUNDMODE_REVERSED)
  play_wav2(NWAV_PPIPE, 800, 255, 0);
   else
    play_wav2(NWAV_PPIPE, 800, 255, 255);


 rest(1000);
 rectfill(screen, 404, 214, 546, 276, COLOUR_GREY2);
 textprintf_right_ex(screen, small_font, 520, 240, COLOUR_WHITE, -1, "Testing right speaker. . .");
 if (options.sound_mode == SOUNDMODE_REVERSED)
  play_wav2(NWAV_PPIPE, 800, 255, 255);
   else
    play_wav2(NWAV_PPIPE, 800, 255, 0);
 rest(1000);
}

void resolution_change(void)
{
 rectfill(screen, 300, 210, 600, 340, COLOUR_BLUE2);
 rect(screen, 300, 210, 600, 340, COLOUR_BLUE6);
 rect(screen, 301, 211, 599, 339, COLOUR_BLUE5);
 rect(screen, 302, 212, 598, 338, COLOUR_BLUE4);
 rect(screen, 303, 213, 597, 337, COLOUR_BLUE3);
  textprintf_centre_ex(screen, small_font, 450, 240, COLOUR_WHITE, -1, "Screen resolution changed.");
  textprintf_centre_ex(screen, small_font, 450, 250, COLOUR_WHITE, -1, "You have to restart the game");
  textprintf_centre_ex(screen, small_font, 450, 260, COLOUR_WHITE, -1, "for this change to take effect.");
  textprintf_centre_ex(screen, small_font, 450, 280, COLOUR_WHITE, -1, "If it doesn't work, edit the file");
  textprintf_centre_ex(screen, small_font, 450, 290, COLOUR_WHITE, -1, "overgod.cfg in this directory and");
  textprintf_centre_ex(screen, small_font, 450, 300, COLOUR_WHITE, -1, "change the resolution setting to zero.");
  rest(100);
  textprintf_centre_ex(screen, small_font, 450, 320, COLOUR_GREY6, -1, "Press 'Y' to continue.");
  do
  {}
  while (key[KEY_Y] == 0);

//  last_key = KEY_SPACE;
//  keypress_wait = KP_WAIT_LONG;
}
          
void start_game_from_menu(int game_type)
{

 game.type = game_type;

 vsync();
 clear_bitmap(screen);

 begin_game();

 ticked = 0;

 game_loop();

 clear_bitmap(screen);

 if (arena.level == 16)
 {
  display_victory_screen();
 }
 
 if (arena.level > 5 && options.unlock_purple == 0)
 {
  options.unlock_purple = UNLOCK_PURPLE;
  set_config_int("Unlock", "Purple", UNLOCK_PURPLE);
  unlock_screen(UNLOCK_PURPLE);
 } 
 if (arena.level > 10 && options.unlock_void == 0)
 {
  options.unlock_void = UNLOCK_VOID;
  set_config_int("Unlock", "Void", UNLOCK_VOID);
  unlock_screen(UNLOCK_VOID);
 } 
 if (arena.level > 15 && options.unlock_god == 0)
 {
  options.unlock_god = UNLOCK_GOD;
  set_config_int("Unlock", "God", UNLOCK_GOD);
  unlock_screen(UNLOCK_GOD);
 } 


 check_high_scores();

}


void change_menu(int to_which)
{

 which_menu = to_which;
 keypress_wait = KP_WAIT_LONG;

 switch(which_menu)
 {
  default:
  case MENU_MAIN:
    game.type = GAME_SINGLE;
    which_menu = MENU_MAIN;
    menu_index = 0;
    menu_index_max = 4;
    menu_index_min = 0;
    save_config();
    break;
  case MENU_OPTIONS:
    which_menu = MENU_OPTIONS;
    menu_index = 1;
    menu_index_max = 27;
    menu_index_min = 1;
    break;
  case MENU_SPECIAL:
    which_menu = MENU_SPECIAL;
    menu_index = 0;
    menu_index_max = SPECIAL_END - 1;
    menu_index_min = 0;
    break;
  case MENU_DUEL:
    which_menu = MENU_DUEL;
    menu_index = 1;
    menu_index_max = DUEL_END - 1;
    menu_index_min = 1;
    break;
  case MENU_SCORES:
    init_ms();
    which_menu = MENU_SCORES;
    menu_index = 1;
    menu_index_max = 31;
    menu_index_min = 1;
    break;
  case MENU_ENTER_SCORES:
    init_ms();
    which_menu = MENU_ENTER_SCORES;
    menu_index = 1;
    menu_index_max = 31;
    menu_index_min = 1;
    break;
 }

}






void choose_ships(void)
{

 player[0].ship = 0;
 player[1].ship = 0;
 game.single_player = 0;
 game.users = 1;

 int county = 0;

 int keypress_wait1 = KP_WAIT_SHORT;
 int keypress_wait2 = KP_WAIT_SHORT;
 int waiting = 5;

 int p1_active = 0;
 int p2_active = 0;
 int p1_chosen = 0;
 int p2_chosen = 0;

 int game_type = -1;

 if (game.type == GAME_DUEL)
 {
  p1_active = 1;
  p2_active = 1;
 }

 ticked = 0;

 do
 {
  menu_counter ++;
 
  if (p1_active && !p1_chosen)// && keypress_wait1 == 0)
  {
   if (check_key(player[0].keys [CMD_LEFT]))
   {
    if (player[0].ship == 0)
     player[0].ship = 8;
      else
       player[0].ship --;
    menu_sound1();
    keypress_wait1 = waiting;
   }
   if (check_key(player[0].keys [CMD_RIGHT]))
   {
    player[0].ship ++;
    if (player[0].ship == 9)
     player[0].ship = 0;
    keypress_wait1 = waiting;
    menu_sound1();
   }
   if (check_key(player[0].keys [CMD_FIRE1]))
   {
    p1_chosen = 1;
    keypress_wait1 = waiting;
    menu_sound2();
   }
   if (check_key(player[0].keys [CMD_THRUST]))
   {
    if (player[0].ship < 3)
     player[0].ship += 6;
      else
       player[0].ship -= 3;
    keypress_wait1 = waiting;
   menu_sound1();
   }
   if (check_key(player[0].keys [CMD_BRAKE]))
   {
    if (player[0].ship > 5)
     player[0].ship -= 6;
      else
       player[0].ship += 3;
    keypress_wait1 = waiting;
    menu_sound1();
   }
  }
   else
   {
    if (check_key(player[0].keys [CMD_FIRE1]))// && keypress_wait1 == 0)
    {
     p1_active = 1;
     keypress_wait1 = waiting;
     menu_sound2();
   }
   }
   
  if (p2_active && !p2_chosen)// && keypress_wait2 == 0)
  {
   if (check_key(player[1].keys [CMD_LEFT]))
   {
    if (player[1].ship == 0)
     player[1].ship = 8;
      else
       player[1].ship --;
    keypress_wait2 = waiting;
    menu_sound1();
   }
   if (check_key(player[1].keys [CMD_RIGHT]))
   {
    player[1].ship ++;
    if (player[1].ship == 9)
     player[1].ship = 0;
    keypress_wait2 = waiting;
    menu_sound1();
   }
   if (check_key(player[1].keys [CMD_FIRE1]))
   {
    p2_chosen = 1;
    keypress_wait2 = waiting;
    menu_sound2();
   }
   if (check_key(player[1].keys [CMD_THRUST]))
   {
    if (player[1].ship < 3)
     player[1].ship += 6;
      else
       player[1].ship -= 3;
    keypress_wait2 = waiting;
    menu_sound1();
   }
   if (check_key(player[1].keys [CMD_BRAKE]))
   {
    if (player[1].ship > 5)
     player[1].ship -= 6;
      else
       player[1].ship += 3;
    keypress_wait2 = waiting;
    menu_sound1();
   }
  }
   else
   {
    if (check_key(player[1].keys [CMD_FIRE1]))// && keypress_wait2 == 0)
    {
     p2_active = 1;
     keypress_wait2 = waiting;
     menu_sound2();
    }
   }

  if (keypress_wait1 > 0)
   keypress_wait1 --;
  if (keypress_wait2 > 0)
   keypress_wait2 --;

  make_grid_scroll();

  if (ticked == 0)
  {
   display_ship_choice(0, p1_active, p2_active, p1_chosen, p2_chosen);
   menu_to_screen();
  }

  do
  {
   county ++;
  } while (ticked == 0);

  ticked --;

  if (p1_chosen == 1 && p2_active == 0)
  {
   if (game.type == GAME_TIME_ATTACK)
    game_type = GAME_TIME_ATTACK;
     else
      game_type = GAME_SINGLE;
   game.single_player = 0;
  }
  if (p2_chosen == 1 && p1_active == 0)
  {
   if (game.type == GAME_TIME_ATTACK)
    game_type = GAME_TIME_ATTACK;
     else
      game_type = GAME_SINGLE;
   game.single_player = 1;
  }
  if (p1_chosen == 1 && p2_chosen == 1)
  {
   game_type = GAME_COOP;
   if (game.type == GAME_DUEL)
    game_type = GAME_DUEL;
   if (game.type == GAME_TIME_ATTACK)
    game_type = GAME_TIME_ATTACK_COOP;
  }

  if (game_type == GAME_TIME_ATTACK_COOP || game_type == GAME_TIME_ATTACK)
   change_menu(MENU_MAIN);

  if (game_type != -1)
  {
   if (player[0].ship == 8)
    player[0].ship = grand(8);
   if (player[1].ship == 8)
    player[1].ship = grand(8);
   start_game_from_menu(game_type);
   return;
  }

  if (check_key(KEY_ESC))
  {
   keypress_wait = KP_WAIT_LONG;
   last_key = KEY_ESC;
//   rest(200);
   game.type = GAME_SINGLE;
   menu_sound2();
   return;
  }
 }
  while(TRUE);
}


void display_ship_choice(int players, int p1a, int p2a, int p1c, int p2c)
{

 clear_bitmap(menu_bmp);

  show_grid(COLOUR_GREEN3, COLOUR_GREEN8);

// int i, j;

 if (!p1a)
 {
  textprintf_centre_ex(menu_bmp, font2, 320, 120, COLOUR_GREEN2 + (menu_counter / 4) % 7, -1, "{player__one__fire__to__start}");
  textprintf_centre_ex(menu_bmp, font, 320, 120, -1, -1, "{player__one__fire__to__start}");
 }
  else
  {
   display_ship_choice2(player [0].ship, 0, p1c);
  }

 if (!p2a)
 {
  textprintf_centre_ex(menu_bmp, font2, 320, 360, COLOUR_GREEN2 + (menu_counter / 4) % 7, -1, "{player__two__fire__to__start}");
  textprintf_centre_ex(menu_bmp, font, 320, 360, -1, -1, "{player__two__fire__to__start}");
 }
  else
  {
   display_ship_choice2(player [1].ship, 225, p2c);
  }

 
}


void display_ship_choice2(int ship, int y, int chosen)
{
 int i,j;

 int bx, by;
 int col, sprite_drawn = 0;
 int line_colour = COLOUR_ORANGE8 - (menu_counter / 4) % 6;
 int height = 0;

 char descr_string [200];
//  show_grid(COLOUR_GREEN3, COLOUR_GREEN8);
 int x1 = 436, y1 = 80 + y;

 for (i = 0; i < 3; i ++)
 {
  for (j = 0; j < 3; j ++)
  {
    bx = 10 + i * 64;
    by = y + 10 + j * 64 + i * 5;
    col = COLOUR_YELLOW1;
    sprite_drawn = RLE_LSHIP_GREEN;
    if (i + j * 3 == ship)
    {
     col = COLOUR_YELLOW3;
     sprite_drawn = RLE_LSHIP_YELLOW;
     if (chosen)
     {
      col = COLOUR_GREEN3;
      sprite_drawn = RLE_LSHIP_GREEN;
     }
     rectfill(menu_bmp, bx, by, bx + 49, by + 49, col);
     draw_rle_sprite(menu_bmp, large_ships [i + j * 3] [sprite_drawn], bx, by);
     height = by + 1;
     hline(menu_bmp, bx - 1, by - 1, bx + 15, line_colour);
     hline(menu_bmp, bx + 50, by - 1, bx + 34, line_colour);
     hline(menu_bmp, bx - 1, by + 50, bx + 15, line_colour);
     hline(menu_bmp, bx + 50, by + 50, bx + 34, line_colour);
     vline(menu_bmp, bx - 1, by - 1, by + 50, line_colour);
     vline(menu_bmp, bx + 50, by - 1, by + 50, line_colour);
     hline(menu_bmp, bx + 50, height, x1 - 180, line_colour);
    }
     else
     {
      rectfill(menu_bmp, bx, by, bx + 49, by + 49, col);
      draw_rle_sprite(menu_bmp, large_ships [i + j * 3] [sprite_drawn], bx, by);
     } 
  }
 }

// rectfill(menu_bmp, x1 - 100, y1 - 60, x1 + 100, y1 + 60, COLOUR_GREY1);
// rectfill(menu_bmp, x1 - 95, y1 - 55, x1 + 95, y1 + 55, COLOUR_GREY5);
 rectfill(menu_bmp, x1 - 155, y1 - 40, x1 + 155, y1 + 40, COLOUR_GREY1);
 rect(menu_bmp, x1 - 154, y1 - 39, x1 + 154, y1 + 39, COLOUR_GREY4);
 rect(menu_bmp, x1 - 153, y1 - 38, x1 + 153, y1 + 38, COLOUR_GREY4);
 rect(menu_bmp, x1 - 151, y1 - 36, x1 + 151, y1 + 36, COLOUR_GREY4);
 rect(menu_bmp, x1 - 150, y1 - 35, x1 + 150, y1 + 35, COLOUR_GREY4);

 rect(menu_bmp, x1 - 152, y1 - 37, x1 + 152, y1 + 37, line_colour);
 hline(menu_bmp, x1 - 152, y1, x1 - 180, line_colour);
 vline(menu_bmp, x1 - 180, y1, height, line_colour);
 
 if (chosen)
 {
//  textprintf_centre_ex(menu_bmp, font, 436, y + 20, COLOUR_YELLOW8 - (menu_counter / 4) % 4, -1, ship_name_long(ship));
//  textprintf_centre_ex(menu_bmp, font2, 436, y + 20, COLOUR_ORANGE4 + (menu_counter / 8) % 4, -1, ship_name_long(ship));
//  textprintf_centre_ex(menu_bmp, font2, 436, y + 20, COLOUR_YELLOW8 - (menu_counter / 4) % 4, -1, ship_name_long(ship));
//  textprintf_centre_ex(menu_bmp, font, 436, y + 20, -1, -1, ship_name_long(ship));
 }
  else
  {
//   textprintf_centre_ex(menu_bmp, font2, 436, y + 20, COLOUR_RED8 - (menu_counter / 4) % 4, -1, ship_name_long(ship));
//   textprintf_centre_ex(menu_bmp, font2, 436, y + 20, COLOUR_BLUE4 + (menu_counter / 8) % 4, -1, ship_name_long(ship));
//   textprintf_centre_ex(menu_bmp, font, 436, y + 20, -1, -1, ship_name_long(ship));
  }
// textprintf_ex(menu_bmp, font2, 280, y + 20, COLOUR_WHITE, "%i", ship);

   ship_description1(ship, descr_string);
   textprintf_centre_ex(menu_bmp, small_font, 436, y + 70, COLOUR_YELLOW7, -1, descr_string);
   ship_description2(ship, descr_string);
   textprintf_centre_ex(menu_bmp, small_font, 436, y + 80, COLOUR_YELLOW7, -1, descr_string);
/*
 int line_colour = 255;
 int box_colour = 255;

 RGB lcolour [1];
 int mcn = menu_counter * 2;

 if (mcn % 128 < 64)
 {
  lcolour[0].r = 8 + (63 - mcn % 64) / 7;
//  lcolour[0].g = (63 - mcn % 64);
  lcolour[0].g = 8 + (63 - mcn % 64) / 4;
  lcolour[0].b = 0;//((mcn + 32) % 64);// / 2;
 }
  else
  {
   lcolour[0].r = 8 + (mcn % 64) / 7;
//   lcolour[0].g = (mcn % 64);
   lcolour[0].g = 8 + (mcn % 64) / 4;
   lcolour[0].b = 0;//(63 - ((mcn + 32) % 64));// / 2;
  }
  
// vsync(); MAY NEED TO REPLACE THIS!!!

 set_color(255, lcolour);

 if (ship != 15)
 {
  line(menu_bmp, 431, y + 110, 431, y + 125, line_colour);
  line(menu_bmp, 439, y + 110, 439, y + 125, line_colour);
  line(menu_bmp, 431, y + 125, 439, y + 125, line_colour);
  line(menu_bmp, 435, y + 125, 435, y + 135, line_colour);
  line(menu_bmp, 435, y + 135, 250, y + 135, line_colour);
  line(menu_bmp, 250, y + 135, 250, y + 160, line_colour);
  line(menu_bmp, 250, y + 160, 615, y + 160, line_colour);
  line(menu_bmp, 615, y + 160, 615, y + 185, line_colour);
  line(menu_bmp, 615, y + 185, 250, y + 185, line_colour);
  line(menu_bmp, 250, y + 185, 250, y + 210, line_colour);
  line(menu_bmp, 250, y + 210, 590, y + 210, line_colour);

  rectfill(menu_bmp, 397, y + 103, 472, y + 117, box_colour);
  draw_sprite(menu_bmp, upgrade_box3, 395, y + 93);

  for (i = 0; i < 4; i ++)
  {
   rectfill(menu_bmp, 263 + i * 90, y + 153, 336 + i * 90, y + 168, box_colour);
   rectfill(menu_bmp, 263 + i * 90, y + 203, 336 + i * 90, y + 218, box_colour);
   draw_sprite(menu_bmp, upgrade_box1, 260 + i * 90, y + 143);
   if (i == 3)
    draw_sprite(menu_bmp, upgrade_box2, 260 + i * 90, y + 193);
     else
      draw_sprite(menu_bmp, upgrade_box1, 260 + i * 90, y + 193);
  }

 }
*/
 
}


void show_grid(int grid_colour, int border_colour)
{

//  rectfill(menu_bmp, 10, 10, 630, 470, COLOUR_GREY1);
//  rect(menu_bmp, 10, 10, 630, 470, border_colour - 4);
//  rect(menu_bmp, 9, 9, 631, 471, border_colour - 2);
//  rect(menu_bmp, 8, 8, 632, 472, border_colour);

  draw_scrolling_grid(11, 11, 629, 469, grid_colour);

}


void menu_display_scores(void)
{

/*  rectfill(menu_bmp, 10, 50, 540, 400, COLOUR_GREEN1);
  rect(menu_bmp, 10, 50, 540, 400, COLOUR_GREEN4);
  rect(menu_bmp, 9, 49, 541, 401, COLOUR_GREEN6);
  rect(menu_bmp, 8, 48, 542, 402, COLOUR_GREEN8);*/

  show_grid(COLOUR_YELLOW3, COLOUR_YELLOW8);
  draw_ms();


  textprintf_centre_ex(menu_bmp, font2, 320, 20, COLOUR_ORANGE8 - (menu_counter / 2) % 4, -1, "{___high___scores___}");
  textprintf_centre_ex(menu_bmp, font, 320, 20, -1, -1, "{___high___scores___}");



  int i;

  int ox = 205, oy = 10, oys = 95;
  int col;

  textprintf_centre_ex(menu_bmp, small_font, 320, oys - 30, COLOUR_ORANGE8, -1, "Single Player");

  textprintf_ex(menu_bmp, small_font, ox, oys - 10, COLOUR_YELLOW8, -1, "Name");
  textprintf_right_ex(menu_bmp, small_font, ox + 90, oys - 10, COLOUR_YELLOW8, -1, "Level");
//  textprintf_right_ex(menu_bmp, small_font, ox + 190, oys - 10, COLOUR_YELLOW8, -1, "Craft");
  textprintf_right_ex(menu_bmp, small_font, ox + 200, oys - 10, COLOUR_YELLOW8, -1, "Score");
  textprintf_ex(menu_bmp, small_font, ox + 220, oys - 10, COLOUR_YELLOW8, -1, "Rank");


  char sstr [50];
//  char istr [20];

  for (i = 0; i < NO_SCORES; i ++)
  {

   col = COLOUR_GREEN5;

   if (which_menu == MENU_ENTER_SCORES)
    col = COLOUR_GREEN3;

   if (game.type == GAME_SINGLE && i == entering_score && which_menu == MENU_ENTER_SCORES)
    col = COLOUR_YELLOW4 + (menu_counter / 4) % 4;

   strcpy(sstr, hs_single[i].name);
   textprintf_ex(menu_bmp, small_font, ox, oys + oy * i, col, -1, sstr);

   if (hs_single[i].level == 16)
    strcpy(sstr, "<V>");
     else
//      strcpy(sstr, itoa(hs_single[i].level, istr, 10));
      {
       strcpy(sstr, "");
       anum(sstr, hs_single[i].level);
      } 
   textprintf_right_ex(menu_bmp, small_font, ox + 90, oys + oy * i, col, -1, sstr);

//   strcpy(sstr, ship_name_short(hs_single[i].ship));
//   textprintf_right_ex(menu_bmp, small_font, ox + 190, oys + oy * i, col, -1, sstr);

//   strcpy(sstr, itoa(hs_single[i].score, istr, 10));
//   strcpy(sstr, "");
//   anum(sstr, );
   textprintf_right_ex(menu_bmp, small_font, ox + 200, oys + oy * i, col, -1, "%i", hs_single[i].score);

   rank_name(get_rank(hs_single[i].score, hs_single[i].level), sstr);
   if (hs_single[i].level == 16)
    col = COLOUR_YELLOW8;
   textprintf_ex(menu_bmp, small_font, ox + 220, oys + oy * i, col, -1, sstr);


  }

  oys = 290;

  textprintf_centre_ex(menu_bmp, small_font, 320, oys - 30, COLOUR_ORANGE8, -1, "Cooperative - best player only");

  textprintf_ex(menu_bmp, small_font, ox, oys - 10, COLOUR_YELLOW8, -1, "Name");
  textprintf_right_ex(menu_bmp, small_font, ox + 90, oys - 10, COLOUR_YELLOW8, -1, "Level");
//  textprintf_right_ex(menu_bmp, small_font, ox + 190, oys - 10, COLOUR_YELLOW8, -1, "Craft");
  textprintf_right_ex(menu_bmp, small_font, ox + 200, oys - 10, COLOUR_YELLOW8, -1, "Score");
  textprintf_ex(menu_bmp, small_font, ox + 220, oys - 10, COLOUR_YELLOW8, -1, "Rank");

  for (i = 0; i < NO_SCORES; i ++)
  {

   col = COLOUR_GREEN5;

   if (which_menu == MENU_ENTER_SCORES)
    col = COLOUR_GREEN3;

   if (game.type == GAME_COOP && i == entering_score && which_menu == MENU_ENTER_SCORES)
    col = COLOUR_YELLOW4 + (menu_counter / 2) % 4;

   strcpy(sstr, hs_coop[i].name);
   textprintf_ex(menu_bmp, small_font, ox, oys + oy * i, col, -1, sstr);

   if (hs_coop[i].level == 16)
    strcpy(sstr, "<V>");
     else
      {
       strcpy(sstr, "");
       anum(sstr, hs_coop[i].level);
      } 
//      strcpy(sstr, itoa(hs_coop[i].level, istr, 10));
/*   if (game.mode_purple == 1)
    textprintf_right_ex(menu_bmp, small_font, ox + 98, oys + oy * i, TRANS_PURPLE, -1, "p");
   if (game.mode_void == 1)
    textprintf_right_ex(menu_bmp, small_font, ox + 106, oys + oy * i, COLOUR_WHITE, -1, "v");
   if (game.mode_god == 1)
    textprintf_right_ex(menu_bmp, small_font, ox + 114, oys + oy * i, COLOUR_WHITE, -1, "g");*/
   textprintf_right_ex(menu_bmp, small_font, ox + 90, oys + oy * i, col, -1, sstr);

//   strcpy(sstr, ship_name_short(hs_coop[i].ship));
//   textprintf_right_ex(menu_bmp, small_font, ox + 190, oys + oy * i, col, -1, sstr);

//   strcpy(sstr, itoa(hs_coop[i].score, istr, 10));
//   strcpy(sstr, "");
//   anum(sstr, hs_coop[i].score);
   textprintf_right_ex(menu_bmp, small_font, ox + 200, oys + oy * i, col, -1, "%i", hs_coop[i].score);
   
   if (hs_coop[i].level == 16)
    col = COLOUR_YELLOW8;
   rank_name(get_rank(hs_coop[i].score, hs_coop[i].level), sstr);
   textprintf_ex(menu_bmp, small_font, ox + 220, oys + oy * i, col, -1, sstr);

  }

  char tastr [100];
  
  int hours;
  int minutes;
  int seconds;
  
  strcpy(tastr, "Longest Time Attack survivor - ");
  strcat(tastr, best_ta_name);
  strcat(tastr, " (");
  hours = best_ta_time / 120000;
  minutes = best_ta_time / 2000 - (hours * 60);
  seconds = (best_ta_time / 33.3333) - (minutes * 60);
/*  if (hours > 0)
  {
   strcat(tastr, itoa(hours, istr, 10));
   strcat(tastr, ":");
   if (minutes < 10)
    strcat(tastr, "0");
  } 
  strcat(tastr, itoa(minutes, istr, 10));
  strcat(tastr, ":");
  if (seconds < 10)
   strcat(tastr, "0");
  strcat(tastr, itoa(seconds, istr, 10));
  strcat(tastr, ")");*/
  
  col = COLOUR_GREY5;

  if ((game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
    && which_menu == MENU_ENTER_SCORES)
    col = COLOUR_YELLOW4 + (menu_counter / 2) % 4;
    
//  textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, tastr);
  if (hours > 0)
  {
   if (minutes < 10)
   {
    if (seconds < 10)
    {
     textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s%i:0%i:0%i)", tastr, hours, minutes, seconds);
    }
     else
      textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s%i:0%i:%i)", tastr, hours, minutes, seconds);
   }
    else
    {
     if (seconds < 10)
     {
      textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s%i:%i:0%i)", tastr, hours, minutes, seconds);
     }
      else
       textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s%i:%i:%i)", tastr, hours, minutes, seconds);
    }
  }
   else
   {
    if (minutes < 10)
    {
     if (seconds < 10)
     {
      textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s0%i:0%i)", tastr, minutes, seconds);
     }
      else
       textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s0%i:%i)", tastr, minutes, seconds);
    }
     else
     {
      if (seconds < 10)
      {
       textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s%i:0%i)", tastr, minutes, seconds);
      }
       else
        textprintf_centre_ex(menu_bmp, small_font, 320, 460, col, -1, "%s%i:%i)", tastr, minutes, seconds);
     }   
   }


/*
  textprintf_centre_ex(menu_bmp, font, 300, 200, COLOUR_YELLOW8 - (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre_ex(menu_bmp, font2, 300, 200, COLOUR_RED1 + (menu_counter / 4) % 7, "L___a___c___e___w___i___n___g}");
  textprintf_centre_ex(menu_bmp, font, 300, 300, COLOUR_YELLOW7, "Space} For} 1P}  Enter} For} 2P}...");
  textprintf_centre_ex(menu_bmp, font2, 300, 300, COLOUR_RED7, "Space} For} 1P}  Enter} For} 2P}...");
*/
}

int get_rank(int score, int level)
{


 if (level == 16)
 {
  if (score < 18000)
   return RANK_DEMIGOD;
  if (score < 19000)
   return RANK_GODLING;
  if (score < 20000)
   return RANK_UNDERGOD;
  if (score < 21000)
   return RANK_LESSER_GOD;
  if (score < 23000)
   return RANK_GREATER_GOD;
  if (score < 26000)
   return RANK_OVERGOD;
  return RANK_SUPREME_BEING;
 }

 if (score < 500)
  return RANK_SLAVE;
 if (score < 1000)
  return RANK_DOUBTER;
 if (score < 2000)
  return RANK_UNBELIEVER;
 if (score < 4000)
  return RANK_BLASPHEMER;
 if (score < 8000)
  return RANK_APOSTATE;
 if (score < 12000)
  return RANK_HERETIC;
 if (score < 15000)
  return RANK_HERESIARCH;
 if (score < 20000)
  return RANK_SECULARIST;
 return RANK_DEICIDE;

}


void rank_name(int rank, char sstr [30])
{

 switch(rank)
 {
  case RANK_SLAVE:
   strcpy(sstr, "Slave");
   break;
  case RANK_DOUBTER:
   strcpy(sstr, "Doubter");
   break;
  case RANK_UNBELIEVER:
   strcpy(sstr, "Unbeliever");
   break;
  case RANK_BLASPHEMER:
   strcpy(sstr, "Blasphemer");
   break;
  case RANK_APOSTATE:
   strcpy(sstr, "Apostate");
   break;
  case RANK_HERETIC:
   strcpy(sstr, "Heretic");
   break;
  case RANK_HERESIARCH: // jlb
   strcpy(sstr, "Heresiarch");
   break;
  case RANK_SECULARIST:
   strcpy(sstr, "Secularist");
   break;
  case RANK_DEICIDE:
   strcpy(sstr, "Deicide");
   break;
  case RANK_DEMIGOD:
   strcpy(sstr, "DEMIGOD");
   break;
  case RANK_GODLING:
   strcpy(sstr, "GODLING");
   break;
  case RANK_UNDERGOD:
   strcpy(sstr, "UNDERGOD");
   break;
  case RANK_LESSER_GOD:
   strcpy(sstr, "LESSER GOD");
   break;
  case RANK_GREATER_GOD:
   strcpy(sstr, "GREATER GOD");
   break;
  case RANK_OVERGOD:
   strcpy(sstr, "OVERGOD");
   break;
  case RANK_SUPREME_BEING:
   strcpy(sstr, "SUPREME BEING");
   break;
 
 }

}

void draw_scrolling_grid(int min_x, int min_y, int max_x, int max_y, int colour)
{

 int i, j, x1, y1;

 int x_offset = (grid_x / GRAIN) % 50;
 int y_offset = (grid_y / GRAIN) % 50;

 for (i = -3; i < 16; i ++)
 {
  for (j = -3; j < 16; j ++)
  {
   x1 = i * 50 + x_offset;
   if (x1 < min_x - 80 || x1 > max_x + 50)
    continue;
   y1 = j * 50 + y_offset;
   if (y1 < min_y - 80 || y1 > max_y + 50)
    continue;
   draw_rle_sprite(menu_bmp, menu_tile, x1, y1);
  } 
 }

/* for (i = 0; i < 15; i ++)
 {
   x1 = i * 50 + x_offset;
   if (x1 < min_x - 50 || x1 > max_x + 50)
    continue;
   draw_rle_sprite(menu_bmp, menu_tile, x1 
//   vline(menu_bmp, x1, min_y, max_y, colour);
 }
 for (j = 0; j < 12; j ++)
 {
   y1 = j * 50 + y_offset;
   if (y1 < min_y - 50 || y1 > max_y + 50)
    continue;
//   hline(menu_bmp, min_x, y1, max_x, colour);
 }
*/
}


void menu_to_screen(void)
{
   switch(graphics_mode)
   {
    default: // 640x480   
     blit(menu_bmp, screen, 0, 0, 0, 0, 640, 480);
     break;
    case GMODE_800_600: 
     blit(menu_bmp, screen, 0, 0, 80, 60, 640, 480);
     break;
    case GMODE_1024_768: 
     blit(menu_bmp, screen, 0, 0, 192, 144, 640, 480);
     break;
   }  
}


void make_grid_scroll(void)
{

 grid_x += grid_x_speed;
 grid_y += grid_y_speed;

 grid_x %= 50000;
 grid_y %= 50000;

 grid_x_speed += grid_x_accel;
 if (grid_x_speed > 3000) grid_x_speed = 3000;
 if (grid_x_speed < -3000) grid_x_speed = -3000;
 grid_y_speed += grid_y_accel;
  if (grid_y_speed > 3000) grid_y_speed = 3000;
  if (grid_y_speed < -3000) grid_y_speed = -3000;

 if (menu_counter % 64 == 0)
 {
  grid_x_accel = grand(301) - 150;
//  if (grid_x_speed > 3000) grid_x_speed = 3000;
//  if (grid_x_speed < -3000) grid_x_speed = -3000;

  grid_y_accel = grand(301) - 150;
//  if (grid_y_speed > 3000) grid_y_speed = 3000;
//  if (grid_y_speed < -3000) grid_y_speed = -3000;
 }

}

/*void init_bouncies(void)
{
 int i;

 for (i = 0; i < NO_BOUNCIES; i ++)
 {

  bouncy[i].x = (grand(500) + 20) * GRAIN;
  bouncy[i].y = (grand(400) + 20) * GRAIN;
  bouncy[i].x_speed = (900 + grand(800));
  if (grand(2) == 0)
   bouncy[i].x_speed *= -1;
  bouncy[i].y_speed = (900 + grand(800));
  if (grand(2) == 0)
   bouncy[i].y_speed *= -1;
  bouncy[i].colour1 = COLOUR_GREEN1;
  bouncy[i].colour2 = COLOUR_GREEN6;
  if (grand(25) == 0)
  {
   bouncy[i].colour1 = COLOUR_GREY1;
   bouncy[i].colour2 = COLOUR_GREY6;
  }
  if (grand(15) == 0)
  {
   bouncy[i].colour1 = COLOUR_PURPLE1;
   bouncy[i].colour2 = COLOUR_PURPLE6;
  }
  if (grand(8) == 0)
  {
   bouncy[i].colour1 = COLOUR_BLUE1;
   bouncy[i].colour2 = COLOUR_BLUE6;
  }
  if (grand(5) == 0)
  {
   bouncy[i].colour1 = COLOUR_RED1;
   bouncy[i].colour2 = COLOUR_RED6;
  }
  if (grand(5) == 0)
  {
   bouncy[i].colour1 = COLOUR_ORANGE1;
   bouncy[i].colour2 = COLOUR_ORANGE6;
  }
  if (grand(5) == 0)
  {
   bouncy[i].colour1 = COLOUR_YELLOW1;
   bouncy[i].colour2 = COLOUR_YELLOW6;
  }
 }


}

void make_bouncies_move(void)
{
 int i;
// int j;

 for (i = 0; i < NO_BOUNCIES; i ++)
 {
  if (bouncy[i].x_speed == 0 && bouncy[i].y_speed == 0) return;

  if (bouncy[i].x + bouncy[i].x_speed <= 18 * GRAIN
      || bouncy[i].x + bouncy[i].x_speed >= 622 * GRAIN)
  {
   bouncy[i].x_speed *= -1;
  }
  if (bouncy[i].y + bouncy[i].y_speed <= 18 * GRAIN
      || bouncy[i].y + bouncy[i].y_speed >= 462 * GRAIN)
  {
   bouncy[i].y_speed *= -1;
  }


  bouncy[i].x += bouncy[i].x_speed;
  bouncy[i].y += bouncy[i].y_speed;

 }

}


void draw_bouncies(void)
{

 int i;

 for (i = 0; i < NO_BOUNCIES; i ++)
 {
  circlefill(menu_bmp, bouncy[i].x / GRAIN, bouncy[i].y / GRAIN, 7, bouncy[i].colour1);
  circle(menu_bmp, bouncy[i].x / GRAIN, bouncy[i].y / GRAIN, 7, bouncy[i].colour2);
 }

}

*/


void check_high_scores(void)
{

 int i;
 int entry = -1;
 int better_player = 0;
 
 if (game.type == GAME_TIME_ATTACK || game.type == GAME_TIME_ATTACK_COOP)
 {
  if (game.ta_total_time > best_ta_time)
  {
   best_ta_time = game.ta_total_time;
   strcpy(best_ta_name, "_");
   change_menu(MENU_ENTER_SCORES);
  } 
   else
    change_menu(MENU_SCORES);
  return;  
 }

 if (game.type == GAME_SINGLE)
 {
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[game.single_player].score > hs_single[i].score)
   {
    entry = i;
    break;
   }
  }
  if (entry != -1)
  {
   push_scores_single(entry, game.single_player);
   entering_score = entry;
//   exit(88);
//    change_menu(MENU_SCORES);
//   congratulations(-1);
   change_menu(MENU_ENTER_SCORES);
  }
   else
    change_menu(MENU_SCORES);
 }

 if (game.type == GAME_COOP)
 {
  if (player[0].score > player[1].score) better_player = 0;
   else better_player = 1;
 
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[better_player].score > hs_coop[i].score)
   {
    entry = i;
    break;
   }
  }
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[better_player].score > hs_coop[i].score)
   {
    entry = i;
    break;
   }
  }
  if (entry != -1)
  {
   push_scores_coop(entry, better_player);
   entering_score = entry;
//   exit(88);
//    change_menu(MENU_SCORES);
//   entering_score2 = better_player;
   change_menu(MENU_ENTER_SCORES);
  }
   else
    change_menu(MENU_SCORES);

/* if (game.type == GAME_COOP)
 {
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[0].score > hs_single[i].score)
   {
    entry = i;
    break;
   }
  }
  for (i = 0; i < NO_SCORES; i ++)
  {
   if (player[1].score > hs_single[i].score)
   {
    entry = i;
    break;
   }
  }
  if (entry != -1 || entry2 != -1)
  {
   push_scores_coop(entry, game.single_player);
   entering_score = entry;
//   exit(88);
//    change_menu(MENU_SCORES);
   change_menu(MENU_ENTER_SCORES);
  }
   else
    change_menu(MENU_SCORES);
*/

 }


}


void congratulations(int player)
{

 rectfill(screen, 140, 150, 500, 280, COLOUR_ORANGE1);
 rect(screen, 140, 150, 500, 280, COLOUR_ORANGE2);
 rect(screen, 139, 149, 501, 281, COLOUR_ORANGE3);
 rect(screen, 138, 148, 502, 282, COLOUR_ORANGE4);
 rect(screen, 137, 147, 503, 283, COLOUR_ORANGE5);
 rect(screen, 136, 146, 504, 284, COLOUR_ORANGE6);




 if (player == -1)
 {
  textprintf_centre_ex(screen, font2, 320, 180, COLOUR_ORANGE6, -1, "congratulations}!");
  textprintf_centre_ex(screen, font, 320, 180, COLOUR_YELLOW8, -1, "congratulations}!");
  textprintf_centre_ex(screen, font2, 320, 230, COLOUR_ORANGE8, -1, "you} got} a} high} score}!");
  textprintf_centre_ex(screen, font, 320, 230, COLOUR_YELLOW6, -1, "you} got} a} high} score}!");
// textprintf_right_ex(screen, small_font, 500, 240, COLOUR_WHITE, "Press a key...");
 }
   else
   {
    textprintf_centre_ex(screen, font2, 320, 180, COLOUR_ORANGE6, -1, "congratulations}!");
    textprintf_centre_ex(screen, font, 320, 180, COLOUR_YELLOW8, -1, "congratulations}!");
    textprintf_centre_ex(screen, font2, 320, 230, COLOUR_ORANGE8, -1, "player} %i got} a} high} score}!", player + 1);
    textprintf_centre_ex(screen, font, 320, 230, COLOUR_YELLOW6, -1, "player} %i got} a} high} score}!", player + 1);
   }

 rest(200);

 rect(screen, 140, 150, 500, 280, COLOUR_YELLOW2);
 rect(screen, 139, 149, 501, 281, COLOUR_YELLOW3);
 rect(screen, 138, 148, 502, 282, COLOUR_YELLOW4);
 rect(screen, 137, 147, 503, 283, COLOUR_YELLOW5);
 rect(screen, 136, 146, 504, 284, COLOUR_YELLOW6);

 clear_keybuf();

 readkey();

 keypress_wait = KP_WAIT_SHORT;

}

void congratulations_ta(void)
{

 rectfill(screen, 140, 150, 500, 280, COLOUR_ORANGE1);
 rect(screen, 140, 150, 500, 280, COLOUR_ORANGE2);
 rect(screen, 139, 149, 501, 281, COLOUR_ORANGE3);
 rect(screen, 138, 148, 502, 282, COLOUR_ORANGE4);
 rect(screen, 137, 147, 503, 283, COLOUR_ORANGE5);
 rect(screen, 136, 146, 504, 284, COLOUR_ORANGE6);




  textprintf_centre_ex(screen, font2, 320, 180, COLOUR_ORANGE6, -1, "congratulations}!");
  textprintf_centre_ex(screen, font, 320, 180, COLOUR_YELLOW8, -1, "congratulations}!");
  textprintf_centre_ex(screen, small_font, 320, 230, COLOUR_ORANGE8, -1, "You made a new Time Attack record!");

 rest(200);

 rect(screen, 140, 150, 500, 280, COLOUR_YELLOW2);
 rect(screen, 139, 149, 501, 281, COLOUR_YELLOW3);
 rect(screen, 138, 148, 502, 282, COLOUR_YELLOW4);
 rect(screen, 137, 147, 503, 283, COLOUR_YELLOW5);
 rect(screen, 136, 146, 504, 284, COLOUR_YELLOW6);

 clear_keybuf();

 readkey();

 keypress_wait = KP_WAIT_SHORT;

}


void push_scores_single(int insert, int play)
{

 int i = NO_SCORES - 2;

 if (insert < NO_SCORES - 1)
 {
  for (i = NO_SCORES - 2; i > insert - 1; i --)
  {
   hs_single[i + 1].score = hs_single[i].score;
   hs_single[i + 1].level = hs_single[i].level;
   hs_single[i + 1].ship = hs_single[i].ship;
   strcpy(hs_single[i + 1].name, hs_single[i].name);
  }
 }
 
 i ++;

 hs_single[i].score = player[play].score;
 hs_single[i].level = arena.level;
 hs_single[i].ship = actor[player[play].actor_controlled].ship;
 strcpy(hs_single[i].name, "_");

}

// my attempt to use one push_scores function with pointers failed.
void push_scores_coop(int insert, int play)
{

 int i = NO_SCORES - 2;

 if (insert < NO_SCORES - 1)
 {
  for (i = NO_SCORES - 2; i > insert - 1; i --)
  {
   hs_coop[i + 1].score = hs_coop[i].score;
   hs_coop[i + 1].level = hs_coop[i].level;
   hs_coop[i + 1].ship = hs_coop[i].ship;
   strcpy(hs_coop[i + 1].name, hs_coop[i].name);
  }
 }

 i ++;

// char istr [10];

 hs_coop[i].score = player[play].score;
 hs_coop[i].level = arena.level;
 hs_coop[i].ship = actor[player[play].actor_controlled].ship;
 strcpy(hs_coop[i].name, "P");
 if (play == 0)
  strcat(hs_coop[i].name, "1");
   else
    strcat(hs_coop[i].name, "2");
 strcat(hs_coop[i].name, "_");

}



int enter_score_name_single(int s)
{
/* rectfill(screen, 400, 210, 550, 280, COLOUR_GREY2);
 rect(screen, 400, 210, 550, 280, COLOUR_GREY6);
 rect(screen, 401, 211, 549, 279, COLOUR_GREY5);
 rect(screen, 402, 212, 548, 278, COLOUR_GREY4);
 rect(screen, 403, 213, 547, 277, COLOUR_GREY3);
 textprintf_right_ex(screen, small_font, 500, 240, COLOUR_WHITE, "Press a key...");
*/


// clear_keybuf();

 char inputted; // = KEY_ESC;
 int shifter = key [KEY_LSHIFT] + key [KEY_RSHIFT];

// int i;

// rest(200); // to clear the enter or space key

// if (keypressed() != TRUE)
//  return 0;

 int i;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (check_key(i))
   {
    break;
   }
   if (i == KEY_CAPSLOCK)
    return 0;
  }

 if (i == KEY_ESC || i == KEY_ENTER || i == KEY_ENTER_PAD) // escape, enter
 {
  hs_single[s].name [strlen(hs_single[s].name) - 1] = '\0'; // remove the _
  keypress_wait = KP_WAIT_SHORT;
  menu_sound2();
  return 1;
 }

 if (i == KEY_BACKSPACE) // backspace
 {
  if (strlen(hs_single[s].name) > 1)
  {
   hs_single[s].name [strlen(hs_single[s].name) - 2] = '\0';
   strcat(hs_single[s].name, "_");
   keypress_wait = KP_WAIT_SHORT;
   menu_sound1();
   return 0;
  }
 }

// inputted = readkey() & 0xff;

 inputted = scancode_to_asc(i);

 if (inputted == '\0')
  return 0;

// if (acceptable_name_char(inputted) == 0)
//  return 0;

 if (strlen(hs_single[s].name) > SCORE_LENGTH || text_length(small_font, hs_single[s].name) > 75)
  return 0;

 if (shifter != 0)
 {
  if (inputted >= 'a' && inputted <= 'z')
   inputted = 'A' + inputted - 'a';
  if (inputted >= '0' && inputted <= '9')
   inputted = get_shifted_number(inputted);
 }

 int slen = strlen(hs_single[s].name);

 hs_single[s].name [slen - 1] = inputted;
 hs_single[s].name [slen] = '\0';

 strcat(hs_single[s].name, "_");

 keypress_wait = KP_WAIT_SHORT;
 menu_sound1();

 return 0;
 
}


// obviously there should be 1 function for this, not 3. But I had trouble with that so here we are.
int enter_score_name_ta(void)
{

 char inputted; 
 int shifter = key [KEY_LSHIFT] + key [KEY_RSHIFT];

 int i;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (check_key(i))
   {
    break;
   }
   if (i == KEY_CAPSLOCK)
    return 0;
  }

 if (i == KEY_ESC || i == KEY_ENTER || i == KEY_ENTER_PAD) // escape, enter
 {
  best_ta_name [strlen(best_ta_name) - 1] = '\0'; // remove the _
  keypress_wait = KP_WAIT_SHORT;
  menu_sound2();
  return 1;
 }

 if (i == KEY_BACKSPACE) // backspace
 {
  if (strlen(best_ta_name) > 1)
  {
   best_ta_name [strlen(best_ta_name) - 2] = '\0';
   strcat(best_ta_name, "_");
   keypress_wait = KP_WAIT_SHORT;
   menu_sound1();
   return 0;
  }
 }


 inputted = scancode_to_asc(i);

 if (inputted == '\0')
  return 0;

 if (strlen(best_ta_name) > SCORE_LENGTH || text_length(small_font, best_ta_name) > 75)
  return 0;

 if (shifter != 0)
 {
  if (inputted >= 'a' && inputted <= 'z')
   inputted = 'A' + inputted - 'a';
  if (inputted >= '0' && inputted <= '9')
   inputted = get_shifted_number(inputted);
 }

 int slen = strlen(best_ta_name);

 best_ta_name [slen - 1] = inputted;
 best_ta_name [slen] = '\0';

 strcat(best_ta_name, "_");

 keypress_wait = KP_WAIT_SHORT;
 menu_sound1();

 return 0;
 
}



int enter_score_name_coop(int s)
{

 char inputted;
 int shifter = key [KEY_LSHIFT] + key [KEY_RSHIFT];

 int i;

  for (i = KEY_A; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (check_key(i))
   {
    break;
   }
   if (i == KEY_CAPSLOCK)
    return 0;
  }

 if (i == KEY_ESC || i == KEY_ENTER || i == KEY_ENTER_PAD) // escape, enter
 {
  hs_coop[s].name [strlen(hs_coop[s].name) - 1] = '\0'; // remove the _
  keypress_wait = KP_WAIT_LONG;
  menu_sound2();
  return 1;
 }

 if (i == KEY_BACKSPACE) // backspace
 {
  if (strlen(hs_coop[s].name) > 1)
  {
   hs_coop[s].name [strlen(hs_coop[s].name) - 2] = '\0';
   strcat(hs_coop[s].name, "_");
   keypress_wait = KP_WAIT_SHORT;
   menu_sound1();
   return 0;
  }
 }

 inputted = scancode_to_asc(i);

 if (inputted == '\0')
  return 0;

 if (strlen(hs_coop[s].name) > SCORE_LENGTH || text_length(small_font, hs_coop[s].name) > 75)
  return 0;

 if (shifter != 0)
 {
  if (inputted >= 'a' && inputted <= 'z')
   inputted = 'A' + inputted - 'a';
  if (inputted >= '0' && inputted <= '9')
   inputted = get_shifted_number(inputted);
//  if (inputted >= '0' && inputted <= '9')
//   inputted -= 15;
 }

 int slen = strlen(hs_coop[s].name);

 hs_coop[s].name [slen - 1] = inputted;
 hs_coop[s].name [slen] = '\0';

 strcat(hs_coop[s].name, "_");

 keypress_wait = KP_WAIT_SHORT;
 menu_sound1();

 return 0;
 
}









void quit_game(void)
{

      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
//      clrscr();
//      allegro_message("\n\rBye bye!                           ");
//      free(palet);/
      exit(0);

}

char get_shifted_number(char inputted)
{

  switch(inputted)
  {
   case '1': return '!';
   case '2': return '@';
   case '3': return '#';
   case '4': return '$';
   case '5': return '$';
   case '6': return '^';
   case '7': return '&';
   case '8': return '*';
   case '9': return '(';
   case '0': return ')';
  }

  return inputted;

}

void jam_keys(void)
{

 char sstr [30];
 
 do
 {

  rectfill(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN1);
  rect(menu_bmp, 180, 50, 440, 400, COLOUR_GREEN4);
  rect(menu_bmp, 179, 49, 441, 401, COLOUR_GREEN6);
  rect(menu_bmp, 178, 48, 442, 402, COLOUR_GREEN8);

  textprintf_centre_ex(menu_bmp, font2, 320, 90, COLOUR_YELLOW8, -1, "{t_e_s_t}");
  textprintf_centre_ex(menu_bmp, font, 320, 90, -1, -1, "{t_e_s_t}");
  textprintf_centre_ex(menu_bmp, font2, 320, 130, COLOUR_YELLOW8, -1, "{k_e_y_s}");
  textprintf_centre_ex(menu_bmp, font, 320, 130, -1, -1, "{k_e_y_s}");

  textprintf_ex(menu_bmp, small_font, 200, 210, COLOUR_YELLOW8, -1, "Most keyboards refuse to register certain");
  textprintf_ex(menu_bmp, small_font, 200, 220, COLOUR_YELLOW8, -1, "combinations of keys.  Try pressing the keys you want");
  textprintf_ex(menu_bmp, small_font, 200, 230, COLOUR_YELLOW8, -1, "to use to test for conflicts.  Press Escape to exit.");

  int i;
  int row = 0;

  for (i = 0; i < KEY_CAPSLOCK + 1; i ++)
  {
   if (key [i] != 0)
   {
    scancode_to_keyname(i, sstr);
    textprintf_ex(menu_bmp, small_font, 200, 250 + row * 10, COLOUR_ORANGE8, -1, sstr);
    row ++;
   }
  }

  menu_to_screen();
 }
  while(!key [KEY_ESC]);

 keypress_wait = KP_WAIT_LONG;
 last_key = KEY_ESC;

 ticked = 0;

}




int acceptable_char(int scode)
{
// if (ascode >= 32 && ascode <= 126) return 1;
 switch(scode)
 {
  case KEY_ESC:
   keypress_wait = KP_WAIT_SHORT;
   return 0;
  case KEY_ENTER:
  case KEY_ENTER_PAD:
   keypress_wait = KP_WAIT_SHORT;
   return 0;
 }
 return 1;
}

int acceptable_name_char(int ascode)
{
 if (ascode >= 32 && ascode <= 126) return 1;

 return 0;
}


void scancode_to_keyname(int scanc, char sstr [30])
{

switch(scanc)
{
case KEY_A: strcpy(sstr, " a"); break;
case KEY_B: strcpy(sstr, " b"); break;
case KEY_C: strcpy(sstr, " c"); break;
case KEY_D: strcpy(sstr, " d"); break;
case KEY_E: strcpy(sstr, " e"); break;
case KEY_F: strcpy(sstr, " f"); break;
case KEY_G: strcpy(sstr, " g"); break;
case KEY_H: strcpy(sstr, " h"); break;
case KEY_I: strcpy(sstr, " i"); break;
case KEY_J: strcpy(sstr, " j"); break;
case KEY_K: strcpy(sstr, " k"); break;
case KEY_L: strcpy(sstr, " l"); break;
case KEY_M: strcpy(sstr, " m"); break;
case KEY_N: strcpy(sstr, " n"); break;
case KEY_O: strcpy(sstr, " o"); break;
case KEY_P: strcpy(sstr, " p"); break;
case KEY_Q: strcpy(sstr, " q"); break;
case KEY_R: strcpy(sstr, " r"); break;
case KEY_S: strcpy(sstr, " s"); break;
case KEY_T: strcpy(sstr, " t"); break;
case KEY_U: strcpy(sstr, " u"); break;
case KEY_V: strcpy(sstr, " v"); break;
case KEY_W: strcpy(sstr, " w"); break;
case KEY_X: strcpy(sstr, " x"); break;
case KEY_Y: strcpy(sstr, " y"); break;
case KEY_Z: strcpy(sstr, " z"); break;
case KEY_0: strcpy(sstr, " 0"); break;
case KEY_1: strcpy(sstr, " 1"); break;
case KEY_2: strcpy(sstr, " 2"); break;
case KEY_3: strcpy(sstr, " 3"); break;
case KEY_4: strcpy(sstr, " 4"); break;
case KEY_5: strcpy(sstr, " 5"); break;
case KEY_6: strcpy(sstr, " 6"); break;
case KEY_7: strcpy(sstr, " 7"); break;
case KEY_8: strcpy(sstr, " 8"); break;
case KEY_9: strcpy(sstr, " 9"); break;
case KEY_0_PAD: strcpy(sstr, " Numpad Ins"); break;
case KEY_1_PAD: strcpy(sstr, " Numpad 1"); break;
case KEY_2_PAD: strcpy(sstr, " Numpad 2"); break;
case KEY_3_PAD: strcpy(sstr, " Numpad 3"); break;
case KEY_4_PAD: strcpy(sstr, " Numpad 4"); break;
case KEY_5_PAD: strcpy(sstr, " Numpad 5"); break;
case KEY_6_PAD: strcpy(sstr, " Numpad 6"); break;
case KEY_7_PAD: strcpy(sstr, " Numpad 7"); break;
case KEY_8_PAD: strcpy(sstr, " Numpad 8"); break;
case KEY_9_PAD: strcpy(sstr, " Numpad 9"); break;
case KEY_F1: strcpy(sstr, " F1"); break;
case KEY_F2: strcpy(sstr, " F2"); break;
case KEY_F3: strcpy(sstr, " F3"); break;
case KEY_F4: strcpy(sstr, " F4"); break;
case KEY_F5: strcpy(sstr, " F5"); break;
case KEY_F6: strcpy(sstr, " F6"); break;
case KEY_F7: strcpy(sstr, " F7"); break;
case KEY_F8: strcpy(sstr, " F8"); break;
case KEY_F9: strcpy(sstr, " F9"); break;
case KEY_F10: strcpy(sstr, " F10"); break;
case KEY_F11: strcpy(sstr, " F11"); break;
case KEY_F12: strcpy(sstr, " F12"); break;
case KEY_ESC: strcpy(sstr, " Escape"); break; // invalid!
case KEY_TILDE: strcpy(sstr, " `"); break;
case KEY_MINUS: strcpy(sstr, " -"); break;
case KEY_EQUALS: strcpy(sstr, " ="); break;
case KEY_BACKSPACE: strcpy(sstr, " Backspace"); break;
case KEY_TAB: strcpy(sstr, " Tab"); break;
case KEY_OPENBRACE: strcpy(sstr, " {"); break;
case KEY_CLOSEBRACE: strcpy(sstr, " }"); break;
case KEY_ENTER: strcpy(sstr, " Enter"); break;
case KEY_COLON: strcpy(sstr, " ;"); break;
case KEY_QUOTE: strcpy(sstr, " '"); break;
case KEY_BACKSLASH: strcpy(sstr, " \\"); break;
case KEY_BACKSLASH2: strcpy(sstr, " \\"); break; // ????
case KEY_COMMA: strcpy(sstr, " ,"); break;
case KEY_STOP: strcpy(sstr, " ."); break;
case KEY_SLASH: strcpy(sstr, " /"); break;
case KEY_SPACE: strcpy(sstr, " Space"); break;
case KEY_INSERT: strcpy(sstr, " Insert"); break;
case KEY_DEL: strcpy(sstr, " Delete"); break;
case KEY_HOME: strcpy(sstr, " Home"); break;
case KEY_END: strcpy(sstr, " End"); break;
case KEY_PGUP: strcpy(sstr, " Page Up"); break;
case KEY_PGDN: strcpy(sstr, " Page Down"); break;
case KEY_LEFT: strcpy(sstr, " Left"); break;
case KEY_RIGHT: strcpy(sstr, " Right"); break;
case KEY_UP: strcpy(sstr, " Up"); break;
case KEY_DOWN: strcpy(sstr, " Down"); break;
case KEY_SLASH_PAD: strcpy(sstr, " Keypad /"); break;
case KEY_ASTERISK: strcpy(sstr, " Keypad *"); break;
case KEY_MINUS_PAD: strcpy(sstr, " Keypad -"); break;
case KEY_PLUS_PAD: strcpy(sstr, " Keypad +"); break;
case KEY_DEL_PAD: strcpy(sstr, " Keypad ."); break;
case KEY_ENTER_PAD: strcpy(sstr, " Keypad Enter"); break;
case KEY_PRTSCR: strcpy(sstr, " Print Screen"); break;
case KEY_PAUSE: strcpy(sstr, " Pause"); break;
case KEY_ABNT_C1: strcpy(sstr, " Weird Foreign Key"); break; // may as well put these in
case KEY_YEN: strcpy(sstr, " Yen"); break;
case KEY_KANA: strcpy(sstr, " Kana"); break;
case KEY_CONVERT: strcpy(sstr, " Convert"); break;
case KEY_NOCONVERT: strcpy(sstr, " NOCONVERT"); break;
case KEY_AT: strcpy(sstr, " At"); break;
case KEY_CIRCUMFLEX: strcpy(sstr, " Circumflex"); break;
case KEY_COLON2: strcpy(sstr, " 2nd Colon"); break;
case KEY_KANJI: strcpy(sstr, " Kanji"); break;
case KEY_LSHIFT: strcpy(sstr, " Left Shift"); break;
case KEY_RSHIFT: strcpy(sstr, " Right Shift"); break;
case KEY_LCONTROL: strcpy(sstr, " Left Control"); break;
case KEY_RCONTROL: strcpy(sstr, " Right Control"); break;
case KEY_ALT: strcpy(sstr, " Left Alt"); break;
case KEY_ALTGR: strcpy(sstr, " Right Alt"); break;
case KEY_LWIN: strcpy(sstr, " Left Win"); break;
case KEY_RWIN: strcpy(sstr, " Right Win"); break;
case KEY_MENU: strcpy(sstr, " Menu"); break;
case KEY_SCRLOCK: strcpy(sstr, " Scroll Lock"); break;
case KEY_NUMLOCK: strcpy(sstr, " Num Lock"); break;
case KEY_CAPSLOCK: strcpy(sstr, " Caps Lock"); break;
//case KEY_: strcpy(sstr, " "); break;

default: strcpy(sstr, " unknown key"); break;

/*

          KEY_ESC, KEY_TILDE, KEY_MINUS, KEY_EQUALS,
          KEY_BACKSPACE, KEY_TAB, KEY_OPENBRACE, KEY_CLOSEBRACE,
          KEY_ENTER, KEY_COLON, KEY_QUOTE, KEY_BACKSLASH,
          KEY_BACKSLASH2, KEY_COMMA, KEY_STOP, KEY_SLASH,
          KEY_SPACE,

*/

}


}



char scancode_to_asc(int scanc)
{

switch(scanc)
{
case KEY_A: return 'a';
case KEY_B: return 'b';
case KEY_C: return 'c';
case KEY_D: return 'd';
case KEY_E: return 'e';
case KEY_F: return 'f';
case KEY_G: return 'g';
case KEY_H: return 'h';
case KEY_I: return 'i';
case KEY_J: return 'j';
case KEY_K: return 'k';
case KEY_L: return 'l';
case KEY_M: return 'm';
case KEY_N: return 'n';
case KEY_O: return 'o';
case KEY_P: return 'p';
case KEY_Q: return 'q';
case KEY_R: return 'r';
case KEY_S: return 's';
case KEY_T: return 't';
case KEY_U: return 'u';
case KEY_V: return 'v';
case KEY_W: return 'w';
case KEY_X: return 'x';
case KEY_Y: return 'y';
case KEY_Z: return 'z';
case KEY_0: return '0';
case KEY_1: return '1';
case KEY_2: return '2';
case KEY_3: return '3';
case KEY_4: return '4';
case KEY_5: return '5';
case KEY_6: return '6';
case KEY_7: return '7';
case KEY_8: return '8';
case KEY_9: return '9';
case KEY_TILDE: return '`';
case KEY_MINUS: return '-';
case KEY_EQUALS: return '=';
case KEY_OPENBRACE: return '{';
case KEY_CLOSEBRACE: return '}';
case KEY_COLON: return ';';
case KEY_QUOTE: return '\'';
case KEY_BACKSLASH: return '\\';
case KEY_BACKSLASH2: return '\\'; // ????
case KEY_COMMA: return ',';
case KEY_STOP: return '.';
case KEY_SLASH: return '/';
case KEY_SPACE: return ' ';
case KEY_SLASH_PAD: return '/';
case KEY_ASTERISK: return '*';
case KEY_MINUS_PAD: return '-';
case KEY_PLUS_PAD: return '+';

default: return '\0';

/*

          KEY_ESC, KEY_TILDE, KEY_MINUS, KEY_EQUALS,
          KEY_BACKSPACE, KEY_TAB, KEY_OPENBRACE, KEY_CLOSEBRACE,
          KEY_ENTER, KEY_COLON, KEY_QUOTE, KEY_BACKSLASH,
          KEY_BACKSLASH2, KEY_COMMA, KEY_STOP, KEY_SLASH,
          KEY_SPACE,

*/

}

 return '\0';


}

void init_config(void)
{

// set_config is in main.c

 char miscstring [40];
 char wstring [40];
 char itstring [40];
 int i;


 options.sound_init = get_config_int("Options", "Sound_init", 1);
 options.sound_mode = get_config_int("Options", "Sound_mode", SOUNDMODE_STEREO);
 options.run_vsync = get_config_int("Options", "Run_vsync", 0);
 options.sound_volume = get_config_int("Options", "Sound_volume", 100);
 options.ambience_volume = get_config_int("Options", "Ambience_volume", 100);
 options.colour_text = get_config_int("Options", "Colour_text", 0);

 options.unlock_purple = get_config_int("Unlock", "Purple", 0);
 options.unlock_void = get_config_int("Unlock", "Void", 0);
 options.unlock_god = get_config_int("Unlock", "God", 0);
// resolution is got in main.c

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player1Keys");
  strcpy(miscstring, "Key");
//  strcpy(itstring, "");
  anum(miscstring, i);
//  strcat(miscstring, itoa(i, itstring, 10));
  player[0].keys [i] = get_config_int(wstring, miscstring, KEY_X);
 }

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player2Keys");
  strcpy(miscstring, "Key");
//  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  player[1].keys [i] = get_config_int(wstring, miscstring, KEY_SPACE);
 }

 for (i = 0; i < NO_SCORES; i ++)
 {
  strcpy(wstring, "Highscores_single");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  hs_single[i].score = get_config_int(wstring, miscstring, 100);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  hs_single[i].level = get_config_int(wstring, miscstring, 1);
  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  hs_single[i].ship = 0;//get_config_int(wstring, miscstring, SHIP_POINTY);
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  strcpy(hs_single[i].name, get_config_string(wstring, miscstring, "Nobody"));
  strcpy(wstring, "Highscores_Coop");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  hs_coop[i].score = get_config_int(wstring, miscstring, 100);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  hs_coop[i].level = get_config_int(wstring, miscstring, 1);
  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  hs_coop[i].ship = 0;//get_config_int(wstring, miscstring, SHIP_POINTY);
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  strcpy(hs_coop[i].name, get_config_string(wstring, miscstring, "Nobody"));
 }

 best_ta_time = get_config_int("Highscores_TA", "best_time", 1000);
 strcpy(best_ta_name, get_config_string("Highscores_TA", "best_name", "Nobody"));

}

void save_config(void)
{
 char miscstring [40];
 char wstring [40];
 char itstring [40];
 int i;

// options.sound_init = get_config_int("Options", "Sound_init", 1);
 set_config_int("Options", "Sound_mode", options.sound_mode);
 set_config_int("Options", "Run_vsync", options.run_vsync);
 set_config_int("Options", "Sound_volume", options.sound_volume);
 set_config_int("Options", "Ambience_volume", options.ambience_volume);
 set_config_int("Options", "Resolution", options.resolution);
 set_config_int("Options", "Colour_text", options.colour_text);

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player1Keys");
  strcpy(miscstring, "Key");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_int(wstring, miscstring, player[0].keys [i]);
 }

 for (i = 0; i < NO_CMDS; i ++)
 {
  strcpy(wstring, "Player2Keys");
  strcpy(miscstring, "Key");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_int(wstring, miscstring, player[1].keys [i]);
 }


 for (i = 0; i < NO_SCORES; i ++)
 {
  strcpy(wstring, "Highscores_single");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_int(wstring, miscstring, hs_single[i].score);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_int(wstring, miscstring, hs_single[i].level);
/*  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_single[i].ship);*/
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_string(wstring, miscstring, hs_single[i].name);
  
  strcpy(wstring, "Highscores_Coop");
  strcpy(miscstring, "Score");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_int(wstring, miscstring, hs_coop[i].score);
  strcpy(miscstring, "Level");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_int(wstring, miscstring, hs_coop[i].level);
/*  strcpy(miscstring, "Ship");
  strcpy(itstring, "");
  strcat(miscstring, itoa(i, itstring, 10));
  set_config_int(wstring, miscstring, hs_coop[i].ship);*/
  strcpy(miscstring, "Name");
  strcpy(itstring, "");
//  strcat(miscstring, itoa(i, itstring, 10));
  anum(miscstring, i);
  set_config_string(wstring, miscstring, hs_coop[i].name);
 }

 set_config_int("Highscores_TA", "best_time", best_ta_time);
 set_config_string("Highscores_TA", "best_name", best_ta_name);

}


char *ship_name_long(int ship)
{

 switch(ship)
 {
/*  case SHIP_LACEWING: return "lacewing}";
  case SHIP_CAPYBARA: return "capybara}";
  case SHIP_DESPOT: return "despot}";
  case SHIP_HOOKWORM: return "hookworm}";
  case SHIP_LENTIL: return "lentil}";
  case SHIP_PORKUPINE: return "porkupine}";
  case SHIP_PRONG: return "prong}";
  case SHIP_SCORPION: return "false} scorpion}";
  case SHIP_TORTFEASOR: return "tortfeasor}";
  case SHIP_AETHER: return "aether} squid}";
  case SHIP_RUMSFELD: return "rumsfeld's} delight}";
  case SHIP_GODBOTHERER: return "godbotherer}";
  case SHIP_BOTULUS: return "botulus}";
  case SHIP_SLIDEWINDER: return "slidewinder}";
  case SHIP_DOOM: return "doom} fork}";*/
  default: return "{random}";
 }
}

char *ship_name_short(int ship)
{

 switch(ship)
 {
/*  case SHIP_LACEWING: return "Lacewing";
  case SHIP_CAPYBARA: return "Capybara";
  case SHIP_DESPOT: return "Despot";
  case SHIP_HOOKWORM: return "Hookworm";
  case SHIP_LENTIL: return "Lentil";
  case SHIP_PORKUPINE: return "Porkupine";
  case SHIP_PRONG: return "Prong";
  case SHIP_SCORPION: return "False Scorpion";
  case SHIP_TORTFEASOR: return "Tortfeasor";
  case SHIP_AETHER: return "Aether Squid";
  case SHIP_RUMSFELD: return "Rumsfeld's Delight";
  case SHIP_GODBOTHERER: return "Godbotherer";
  case SHIP_BOTULUS: return "Botulus";
  case SHIP_SLIDEWINDER: return "Slidewinder";
  case SHIP_DOOM: return "Doom Fork";*/
  default: return "Unknown";
 }
}

void ship_description1(int ship, char dstr [200])
{
 strcpy(dstr, "");

 switch(ship)
 {
  case SHIP_POINTY: strcpy(dstr, "This vehicle uses special silver darts"); break;
  case SHIP_HORSESHOE: strcpy(dstr, "This vehicle's secondary weapon"); break;
  case SHIP_ROUND: strcpy(dstr, "This vehicle is capable of"); break;
  case SHIP_RETRO: strcpy(dstr, "This vehicle has an extra"); break;
  case SHIP_SMALL: strcpy(dstr, "This vehicle is especially small"); break;
  case SHIP_FINS: strcpy(dstr, "This vehicle has an additional set of"); break;
  case SHIP_ORBITAL: strcpy(dstr, "This vehicle carries a device which reduces"); break;
  case SHIP_CURVE: strcpy(dstr, "This vehicle causes an explosion on contact with an enemy."); break;
  default: strcpy(dstr, "Choose a random vehicle.");
 }
 

}

void ship_description2(int ship, char dstr [200])
{
 strcpy(dstr, "");
 
 switch(ship)
 {
  case SHIP_POINTY: strcpy(dstr, "which are particularly painful to your enemies."); break;
  case SHIP_HORSESHOE: strcpy(dstr, "replenishes itself at an unusually rapid rate."); break;
  case SHIP_ROUND: strcpy(dstr, "repairing damaged armour or structure."); break;
  case SHIP_RETRO: strcpy(dstr, "engine outlet instead of a brake."); break;
  case SHIP_SMALL: strcpy(dstr, "and difficult for enemies to hit."); break;
  case SHIP_FINS: strcpy(dstr, "dart-throwers mounted to face backwards"); break;
  case SHIP_ORBITAL: strcpy(dstr, "acceleration from being struck by objects or energy."); break;
  case SHIP_CURVE: strcpy(dstr, "The power of the explosion increases as you accumulate symbols."); break;
  default: return;
 }
}






void display_victory_screen(void)
{

 int county = 0;
 int county2 = 0;

 init_ms();
 init_pyro();

 do
 {
  menu_counter ++;
  make_grid_scroll();
  run_pyros();
  run_sparkles();
  make_ms_move();
  
  if (ticked == 0)
  {
   clear_bitmap(menu_bmp);
   show_grid(COLOUR_PURPLE3, COLOUR_PURPLE8);
   draw_ms();
   draw_sparkles();
   textprintf_centre_ex(menu_bmp, font2, 320, 40, COLOUR_YELLOW8 - (menu_counter / 4) % 4, -1, "{__victory__}");
   textprintf_centre_ex(menu_bmp, font, 320, 40, -1, -1, "{__victory__}");
   textprintf_centre_ex(menu_bmp, small_font, 320, 100, COLOUR_YELLOW8, -1, "You have defeated the enslavers of humanity");
   textprintf_centre_ex(menu_bmp, small_font, 320, 115, COLOUR_YELLOW8, -1, "and may now take your place among those who survive!");
//   textprintf_centre_ex(menu_bmp, small_font, 320, 230, COLOUR_YELLOW8, -1, "You may now take your place ");
   textprintf_centre_ex(menu_bmp, small_font, 320, 135, COLOUR_YELLOW8, -1, "Congratulations!");

   if (county2 > 99)
    textprintf_centre_ex(menu_bmp, small_font, 320, 420, COLOUR_YELLOW5, -1, "Press Space to continue.");
   vsync();
   menu_to_screen();
  }

  do
  {
   county ++;
  } while (ticked == 0);

  ticked --;

  county2 ++;

  if (county2 > 99 && key [KEY_SPACE] != 0)
  {
   last_key = KEY_SPACE;
   break;
  }

 }
  while(TRUE);
}


// pyrotechnics!
void init_pyro(void)
{
 int i;
 
 for (i = 0; i < NO_PYROS; i ++)
 {
  pyro[i].type = PYRO_NONE;
 }

 for (i = 0; i < NO_SPARKLES; i ++)
 {
  sparkle[i].type = SPARKLE_NONE; 
 }

 next_pyro = 10 + grand(20);
 
}

void run_pyros(void)
{

 next_pyro --;
 
 if (next_pyro <= 0)
 {
  int angle = grand(ANGLE_FULL);
  int speed = 7000 + grand(4000);
  int colours [4] = {TRANS_YELLOW, TRANS_ORANGE, TRANS_LRED, TRANS_DRED};
  colourify(colours);
  int x = 320000 + xpart(angle, ms_circle_rad * GRAIN);
  int y = 240000 + ypart(angle, ms_circle_rad * GRAIN);
  int size = 50 + grand(50);
  int type = PYRO_SPLIT1;
  if (grand(5) == 0)
   type = PYRO_ENDING;
  if (grand(10) == 0)
   type = PYRO_SPLIT2; 
  create_pyro(x, y, 
   xpart(angle, speed), ypart(angle, speed), type, colours, size, 3000 + grand(6000), 30 + grand(20));
  pyro_sound(x, size);
  size *= 3; 
  create_sparkle(x, y, 0, 0, SPARKLE_CIRCLE, colours [0], size, 12 + grand(2));
  create_sparkle(x, y, 0, 0, SPARKLE_CIRCLE, colours [1], size + 2, 10 + grand(2));
  create_sparkle(x, y, 0, 0, SPARKLE_CIRCLE, colours [2], size + 4, 8 + grand(2));
  create_sparkle(x, y, 0, 0, SPARKLE_CIRCLE, colours [3], size + 6, 6 + grand(2));
  next_pyro = 20 + grand(40);
 }

 manage_pyros();

}

void colourify(int col [4])
{

 switch(grand(16))
 {
  default:
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
   col [0] = TRANS_YELLOW;
   col [1] = TRANS_ORANGE;
   col [2] = TRANS_LRED;
   col [3] = TRANS_DRED;
   break; 
  case 6:
  case 7:
  case 8:
  case 9:
   col [0] = TRANS_WHITE;
   col [1] = TRANS_YELLOW;
   col [2] = TRANS_ORANGE;
   col [3] = TRANS_LRED;
   break; 
  case 10:
  case 11:
   col [0] = TRANS_YELLOW;
   col [1] = TRANS_LGREEN;
   col [2] = TRANS_DGREEN;
   col [3] = TRANS_DGREEN;
   break; 
  case 12:
   col [0] = TRANS_WHITE;
   col [1] = TRANS_YELLOW;
   col [2] = TRANS_LGREEN;
   col [3] = TRANS_DGREEN;
   break; 
  case 13:
   col [0] = TRANS_WHITE;
   col [1] = TRANS_LBLUE;
   col [2] = TRANS_DBLUE;
   col [3] = TRANS_DBLUE;
   break; 
  case 14:
   col [0] = TRANS_WHITE;
   col [1] = TRANS_LGREY;
   col [2] = TRANS_DGREY;
   col [3] = TRANS_DGREY;
   break; 
  case 15:
   col [0] = TRANS_WHITE;
   col [1] = TRANS_PURPLE;
   col [2] = TRANS_LBLUE;
   col [3] = TRANS_DBLUE;
   break; 
 }

}

void manage_pyros(void)
{
 int i, x, y, size;
 
 for (i = 0; i < NO_PYROS; i ++)
 {
  if (pyro[i].type == PYRO_NONE)
   continue;
  pyro[i].x += pyro[i].x_speed;
  pyro[i].y += pyro[i].y_speed;
  drag_pyro(i);
  x = pyro[i].x + grand(pyro[i].dist) - grand(pyro[i].dist);
  y = pyro[i].y + grand(pyro[i].dist) - grand(pyro[i].dist);
  size = pyro[i].size + grand(pyro[i].size);
  create_sparkle(x, y, pyro[i].x_speed, pyro[i].y_speed, SPARKLE_CIRCLE, pyro[i].colours [0], size, 12 + grand(4));
  create_sparkle(x, y, pyro[i].x_speed, pyro[i].y_speed, SPARKLE_CIRCLE, pyro[i].colours [1], size, 7 + grand(3));
  create_sparkle(x, y, pyro[i].x_speed, pyro[i].y_speed, SPARKLE_CIRCLE, pyro[i].colours [2], size, 4 + grand(2));
  create_sparkle(x, y, pyro[i].x_speed, pyro[i].y_speed, SPARKLE_CIRCLE, pyro[i].colours [3], size, 2 + grand(2));
  pyro[i].timeout --;
  if (pyro[i].timeout <= 0)
  {
   if (pyro[i].type != PYRO_ENDING)
    pyro_burst(i);
   destroy_pyro(i);
  } 
 }

}

void pyro_burst(int p)
{
 int i;
 int number = 2 + grand(5);
 int angle_dist = ANGLE_FULL / number;
 int start_angle = grand(ANGLE_FULL);
 int rand_angle = 0;
 if (grand(4) == 0)
  rand_angle = grand(ANGLE_QUARTER);
 int size = (pyro[p].size / 2) + grand(pyro[p].size / 2);
 int dist = (pyro[p].size / 2) + grand(pyro[p].size / 2);
 int timeout = 20 + grand(15);
 int rand_timeout = 0;
 if (grand(4) == 0)
  rand_timeout = grand(20);
 int speed = 5000 + grand(3000);
 int rand_speed = 0;
 if (grand(4) == 0)
  rand_speed = grand(5000); 
 int rand_colour = 0; 
 if (grand(8) == 0)
  rand_colour = 1;
  
 
 int angle = 0; 
 int type = PYRO_ENDING;
 if (pyro[p].type == PYRO_SPLIT2)
  type = PYRO_SPLIT1;

 if (grand(4) == 0)
  colourify(pyro[p].colours);
 
 for (i = 0; i < number; i ++)
 {
  angle = start_angle + (i * angle_dist) + grand(rand_angle) - grand(rand_angle);
  if (pyro[p].type == PYRO_RANDSPLIT)
  {
   type = PYRO_ENDING;
   if (grand(3) == 0)
    type = PYRO_SPLIT1;
   if (grand(19) == 0)
    type = PYRO_SPLIT2;
  }
  if (rand_colour == 1)
   colourify(pyro[p].colours);
  create_pyro(pyro[p].x, pyro[p].y, xpart(angle, speed + grand(rand_speed)), ypart(angle, speed + grand(rand_speed)), type, pyro[p].colours, size, dist, timeout + grand(rand_timeout));
 }

 pyro_sound(pyro[p].x, pyro[p].size);
 

}

void create_pyro(int x, int y, int xs, int ys, int type, int colours [4], int size, int dist, int timeout)
{
 int i;
 for (i = 0; i < NO_PYROS; i ++)
 {
  if (i == NO_PYROS - 1)
   return;
  if (pyro[i].type == PYRO_NONE)
   break;
 }
 
 pyro[i].x = x;
 pyro[i].y = y;
 pyro[i].x_speed = xs;
 pyro[i].y_speed = ys;
 pyro[i].colours [0] = colours [0];
 pyro[i].colours [1] = colours [1];
 pyro[i].colours [2] = colours [2];
 pyro[i].colours [3] = colours [3];
 pyro[i].size = size;
 pyro[i].dist = dist;
 pyro[i].timeout = timeout;
 pyro[i].type = type;

//  textprintf_centre_ex(screen, small_font, 320, 300, COLOUR_YELLOW8, -1, "new pyro %i %i %i %i %i %i", x, y, xs, ys, size, timeout);
//  rest(500);

}

void pyro_sound(int x, int size)
{
  int pan = 127;

  if (options.sound_mode == SOUNDMODE_STEREO || options.sound_mode == SOUNDMODE_REVERSED)
  {
   x = x / 640;
   x *= 255;
   x /= 1000;
//   x -= 127;
   pan = x;
   if (pan <= 0)
    pan = 0;
   if (pan >= 255)
    pan = 255;
   if (options.sound_mode != SOUNDMODE_REVERSED)
    pan = 255 - pan;
  }
  
  int pitch = 1400 + grand(100) - (size * 10);
  if (pitch <= 100)
   pitch = 100;
  
//  textprintf_centre_ex(screen, small_font, 320, 300, COLOUR_YELLOW8, -1, "%i", pan);
//  rest(300);
  play_wav2(NWAV_BURST, pitch, 150 + grand(100), pan);

}

void run_sparkles(void)
{
 int i;

 for (i = 0; i < NO_SPARKLES; i ++)
 {
  if (sparkle[i].type == SPARKLE_NONE)
   continue;
  sparkle[i].x += sparkle[i].x_speed;
  sparkle[i].y += sparkle[i].y_speed;
  drag_sparkle(i);
  sparkle[i].size -= sparkle[i].tickrate;//= 10;
  if (sparkle[i].size <= 0)
   destroy_sparkle(i);   
 }
}

void draw_sparkles(void)
{
 int i;

 drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
 
 for (i = 0; i < NO_SPARKLES; i ++)
 {
  if (sparkle[i].type == SPARKLE_NONE)
   continue;
//  circlefill(menu_bmp, sparkle[i].x / GRAIN, sparkle[i].y / GRAIN, 10, TRANS_YELLOW);
  circlefill(menu_bmp, sparkle[i].x / GRAIN, sparkle[i].y / GRAIN, sparkle[i].size / 10, sparkle[i].colour);
 }

 for (i = 0; i < NO_PYROS; i ++)
 {
  if (pyro[i].type != PYRO_NONE)
   draw_a_light(menu_bmp, (pyro[i].size / 5) + grand(15), pyro[i].x / GRAIN, pyro[i].y / GRAIN); // in display.cc
 }


 drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);

}


void create_sparkle(int x, int y, int xs, int ys, int type, int colour, int size, int tickrate)
{
 int i;
 for (i = 0; i < NO_SPARKLES; i ++)
 {
  if (i == NO_SPARKLES - 1)
   return;
  if (sparkle[i].type == SPARKLE_NONE)
   break;
 }

  sparkle[i].x = x; 
  sparkle[i].y = y; 
  sparkle[i].x_speed = xs; 
  sparkle[i].y_speed = ys; 
  sparkle[i].type = type;
  sparkle[i].colour = colour;
  sparkle[i].size = size;
  sparkle[i].tickrate = tickrate;

//  textprintf_centre_ex(screen, small_font, 320, 300, COLOUR_YELLOW8, -1, "new sparkle %i %i %i", x, y, size);
//  rest(200);
}


void destroy_pyro(int p)
{
 pyro[p].type = PYRO_NONE;
}

void destroy_sparkle(int s)
{
 sparkle[s].type = SPARKLE_NONE;
}

void drag_pyro(int p)
{
 pyro[p].x_speed *= (float) 0.95;
 pyro[p].y_speed *= (float) 0.95;
}

void drag_sparkle(int s)
{
 sparkle[s].x_speed *= (float) 0.50;
 sparkle[s].y_speed *= (float) 0.50;
}

void unlock_screen(int which)
{

 int county = 0;
 int county2 = 0;

 do
 {
  menu_counter ++;
  make_grid_scroll();

  if (ticked == 0)
  {
   clear_bitmap(menu_bmp);
   show_grid(COLOUR_PURPLE3, COLOUR_PURPLE8);
   textprintf_centre_ex(menu_bmp, font2, 320, 120, COLOUR_YELLOW8 - (menu_counter / 4) % 4, -1, "{__unlock__}");
   textprintf_centre_ex(menu_bmp, font, 320, 120, -1, -1, "{__unlock__}");
   textprintf_centre_ex(menu_bmp, small_font, 320, 200, COLOUR_YELLOW8, -1, "You have unlocked");
   switch(which)
   {
    case UNLOCK_PURPLE:
     textprintf_centre_ex(menu_bmp, small_font, 320, 230, TRANS_PURPLE, -1, "Purple Mode!");
     break;
    case UNLOCK_VOID:
     textprintf_centre_ex(menu_bmp, small_font, 320, 230, COLOUR_GREY5, -1, "Night Mode!");
     break;
    case UNLOCK_GOD:
     textprintf_centre_ex(menu_bmp, small_font, 320, 230, COLOUR_WHITE, -1, "God Mode!");
     break;
   }
   textprintf_centre_ex(menu_bmp, small_font, 320, 260, COLOUR_YELLOW8, -1, "Switch it on in the Special Menu.");
//   textprintf_centre_ex(menu_bmp, small_font, 320, 290, COLOUR_YELLOW8, "Now you can do something useful with");
//   textprintf_centre_ex(menu_bmp, small_font, 320, 305, COLOUR_YELLOW8, "your time, like ");
   if (county2 > 99)
    textprintf_centre_ex(menu_bmp, small_font, 320, 380, COLOUR_YELLOW5, -1, "Press Space to continue.");
   vsync();
   menu_to_screen();
  }

  do
  {
   county ++;
  } while (ticked == 0);

  ticked --;

  county2 ++;

  if (county2 > 99 && key [KEY_SPACE] != 0)
  {
   last_key = KEY_SPACE;
   break;
  }

 }
  while(TRUE);
}

int ms_colour(void)
{

 switch(grand(6))
 {
  case 0:
   return GC_GREY3;
  case 1:
   return GC_GREEN4;
  case 2:
   return GC_YELLOW4;
  case 3:
   return GC_RED4;
  case 4:
   return GC_BLUE4;
  case 5:
   return GC_ORANGE4;
 
 }

  return GC_ORANGE3;

}

void init_ms(void)
{
 ms_circle_rad = 30 + grand(15);
 ms_circle_col = ms_colour() + 2;
 if (game.mode_void == 1)
  ms_circle_col += 46; 
 int i;
 
 for (i = 0; i < 2; i ++)
 {
  ms_arm_col [i] = ms_colour();
  if (game.mode_void == 1)
   ms_arm_col [i] += 46; 
  ms_arm_out [i] = ms_circle_rad + (i * 40) + grand(50) + 40;
  ms_arm_angle [i] = ANGLE_1_SIXTEENTH + grand(ANGLE_1_EIGHTH);
  ms_arm_inward [i] = 15 + grand(20);
  ms_arm_rot [i] = grand(ANGLE_FULL);
  ms_arm_type [i] = grand(MS_TYPES);
//  ms_arm_type [i] = MS_SPINNER;
//  if (grand(2) == 0)
//   ms_arm_type [i] = MS_SQUAREY;
  ms_arm_number [i] = 2 + grand(4);
  ms_arm_blatter_size [i] = 8 + grand(20);
  if (ms_arm_number [i] > 3)
   ms_arm_angle [i] /= 2;
  switch(ms_arm_type [i])
  {
   case MS_SPINNER:
    ms_arm_inward [i] = 0;
    ms_arm_angle [i] = 1;
    ms_arm_angle [i] = ANGLE_1_32 + grand(ANGLE_1_SIXTEENTH);
    break;
   case MS_SPIKEY:
    ms_arm_angle [i] = ANGLE_1_SIXTEENTH + grand(ANGLE_QUARTER);
    break;
   case MS_ORBITER:
    ms_arm_inward [i] = 35 + grand(20);
    if (ms_arm_inward [i] >= ms_arm_out [i] - 10)
     ms_arm_inward [i] = ms_circle_rad;
    break;
   case MS_SQUAREY:
    ms_arm_angle [i] = 1;
    ms_arm_angle [i] = ANGLE_1_32 + grand(ANGLE_1_SIXTEENTH);
    ms_arm_inward [i] = 35 + grand(20);
    if (ms_arm_inward [i] >= ms_arm_out [i] - 10)
     ms_arm_inward [i] = ms_circle_rad;
    break;
  }
  ms_arm_rot_delta [i] = 3 + grand(5);
  if (grand(2) == 0)
   ms_arm_rot_delta [i] *= -1;   
 }
 
 if (ms_arm_rot_delta [0] == ms_arm_rot_delta [1])
  ms_arm_rot_delta [0] *= -1;
 
}

void draw_ms(void)
{
 int i, x = 320, y = 240, col1 = GC_GREY1, col2;
 
 for (i = 0; i < 2; i ++)
 {
  col2 = ms_arm_col [i];
//  col1 = col2;
  switch(ms_arm_type [i])
  {
   case MS_SPINNER:
    draw_overspinner(menu_bmp, x, y, ms_arm_rot [i], ms_arm_out [i], ms_arm_inward [i], ms_arm_angle [i], ANGLE_FULL / ms_arm_number [i], ms_arm_number [i], col1, col2);
    break;
   case MS_SPIKEY:
    draw_spikey(menu_bmp, x, y, ms_arm_rot [i], ms_arm_out [i], ms_arm_inward [i], ms_arm_angle [i], ANGLE_FULL / ms_arm_number [i], ms_arm_number [i], col1, col2, 0, 0);
    break;
   case MS_SQUAREY:
    draw_squarey(menu_bmp, x, y, ms_arm_rot [i], ms_arm_out [i], ms_arm_inward [i], ms_arm_angle [i], ANGLE_FULL / ms_arm_number [i], ms_arm_number [i], col1, col2);
    break;
   case MS_BLATTER:
    draw_blatter(menu_bmp, x, y, ms_arm_number [i], ms_arm_out [i], ms_arm_rot [i], ms_arm_blatter_size [i], col1, col2);
    break;
   case MS_ORBITER:
    draw_orbiter(menu_bmp, x, y, ms_arm_rot [i], ms_arm_out [i], ms_arm_inward [i], ms_arm_blatter_size [i], ms_arm_angle [i], ANGLE_FULL / ms_arm_number [i], ms_arm_number [i], col1, col2);
//    draw_orbiter(BITMAP *bmp, int x, int y, int attribute, int out1, int out2, int out3, int angle1, int angle2, int arms, int col1, int col2);
    break;
  
  }
  
 } 

 circlefill(menu_bmp, 320, 240, ms_circle_rad, ms_circle_col);
 circle(menu_bmp, 320, 240, ms_circle_rad, GC_GREY1);

}

void make_ms_move(void)
{
 ms_arm_rot [0] += ms_arm_rot_delta [0];
 ms_arm_rot [0] &= 1023;
 ms_arm_rot [1] += ms_arm_rot_delta [1];
 ms_arm_rot [1] &= 1023;

}

void menu_sound1(void)
{
 play_wav2(NWAV_MENU, 600, 255, 127);
}

void menu_sound2(void)
{
 play_wav2(NWAV_MENU, 300, 255, 127);
}

// appends a number to a string. Unforgivable hack needed to avoid using itoa for portability reasons, and to avoid learning how to use snprintf() for laziness reasons. Also I wrote it while hungover
void anum(char *str, int num)
{
 switch(num)
 {
  case 0: strcat(str, "0"); break;
  case 1: strcat(str, "1"); break;
  case 2: strcat(str, "2"); break;
  case 3: strcat(str, "3"); break;
  case 4: strcat(str, "4"); break;
  case 5: strcat(str, "5"); break;
  case 6: strcat(str, "6"); break;
  case 7: strcat(str, "7"); break;
  case 8: strcat(str, "8"); break;
  case 9: strcat(str, "9"); break;
  case 10: strcat(str, "10"); break;
  case 11: strcat(str, "11"); break;
  case 12: strcat(str, "12"); break;
  case 13: strcat(str, "13"); break;
  case 14: strcat(str, "14"); break;
  case 15: strcat(str, "15"); break;
  case 16: strcat(str, "16"); break;
  case 17: strcat(str, "17"); break;
  case 18: strcat(str, "18"); break;
  case 19: strcat(str, "19"); break;
  case 20: strcat(str, "20"); break;
  case 30: strcat(str, "30"); break;
  case 40: strcat(str, "40"); break;
  case 50: strcat(str, "50"); break;
  case 60: strcat(str, "60"); break;
  case 70: strcat(str, "70"); break;
  case 80: strcat(str, "80"); break;
  case 90: strcat(str, "90"); break;
  case 100: strcat(str, "100"); break;
  default: strcat(str, "num"); break;
 
 }
 
}


