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
    
File: sound.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - sound initialisation and playing.
enums in sound.h

*/

#include "allegro.h"

#include <string.h>

#include "sound.h"

#include "config.h"
#include "globvars.h"

#include "math.h"
#include "stuff.h"
//DATAFILE *soundf;

//SAMPLE *sound_list [10];

//#define SOUND_OFF

#define BEAT 64
#define HALF_BEAT 32
#define QUARTER_BEAT 16
#define Q3_BEAT 48
#define SMALLEST_BEAT 16

#define MAX_PHRASES 80
#define MAX_PHRASE_LENGTH 150
#define MAX_OVERPHRASES 100

#define NO_TONES 56
#define BASE_TONE 50
// if 100, max tone = 43

int debug_sound [5];

enum
{
CHORD_MAJOR,
CHORD_MINOR,
CHORD_3,
NO_CHORDS
};

#define NO_JUMPS 7

#define NO_LONGPROGS 6
#define SIZE_LONGPROGS 25

enum
{
NOTE_0C,
NOTE_0CS,
NOTE_0D,
NOTE_0DS,
NOTE_0E,
NOTE_0F,
NOTE_0FS,
NOTE_0G,
NOTE_0GS,
NOTE_0A,
NOTE_0AS,
NOTE_0B,
NOTE_1C,
NOTE_1CS,
NOTE_1D,
NOTE_1DS,
NOTE_1E,
NOTE_1F,
NOTE_1FS,
NOTE_1G,
NOTE_1GS,
NOTE_1A,
NOTE_1AS,
NOTE_1B,
NOTE_2C,
NOTE_2CS,
NOTE_2D,
NOTE_2DS,
NOTE_2E,
NOTE_2F,
NOTE_2FS,
NOTE_2G,
NOTE_2GS,
NOTE_2A,
NOTE_2AS,
NOTE_2B,
NOTE_ENDNOTE

};

int long_prog [NO_LONGPROGS] [SIZE_LONGPROGS] = 
{
{NOTE_1C, NOTE_1E, NOTE_1G, NOTE_2C, NOTE_1G, NOTE_1E, NOTE_0AS, NOTE_1D, NOTE_1F, NOTE_1AS, NOTE_1F, NOTE_0AS, NOTE_ENDNOTE},
{NOTE_1C, NOTE_1DS, NOTE_1C, NOTE_1F, NOTE_1C, NOTE_1FS, NOTE_1C, NOTE_1G, NOTE_1C, NOTE_1AS, NOTE_1C, NOTE_2C, NOTE_ENDNOTE}, 
{NOTE_1C, NOTE_2C, NOTE_1AS, NOTE_1G, NOTE_1FS, NOTE_1F, NOTE_1DS, NOTE_ENDNOTE}, 
{NOTE_1C, NOTE_1G, NOTE_2C, NOTE_1C, NOTE_1G, NOTE_2C, NOTE_1C, NOTE_2C, NOTE_ENDNOTE}, 
{NOTE_1C, NOTE_1E, NOTE_1G, NOTE_2C, NOTE_2E, NOTE_1G, NOTE_2C, NOTE_2E, NOTE_ENDNOTE}, 
{NOTE_1DS, NOTE_0G, NOTE_0AS, NOTE_0FS, NOTE_0G, NOTE_1C, NOTE_1FS, NOTE_1F, NOTE_ENDNOTE}
//{NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, 
//{NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, 
//{NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, NOTE_1, 
};

int chord_jumps [NO_CHORDS] [NO_JUMPS] =
{
{NOTE_1C, NOTE_1D, NOTE_1E, NOTE_1F, NOTE_1G, NOTE_1GS, NOTE_2C},
{NOTE_1C, NOTE_1D, NOTE_1DS, NOTE_1G, NOTE_1GS, NOTE_1B, NOTE_2C},
{NOTE_1C, NOTE_1D, NOTE_1E, NOTE_1F, NOTE_1G, NOTE_1GS, NOTE_2C}
};

int success_note [3];
int success_samp;
int success_step;
int success_base;


int tone [NO_TONES];

#define PROG_LENGTH 40
#define NO_PROGS 4

int progression [NO_PROGS] [PROG_LENGTH];
int prog_pos [NO_PROGS];
int prog_base [NO_PROGS];
int prog_length [NO_PROGS];

enum
{
BEAT_DRUM1,
BEAT_DRUM2,
BEAT_LOWDRUM,
BEAT_STICK,
BEAT_BELL_S,
BEAT_BASS,
BEAT_BASS2,
BEAT_STRING,

BEAT_TWIRM, // here and above: common beats. Below: rarer beats
BEAT_ZAPDRUM1,
BEAT_ZAPDRUM2,
BEAT_PPIPE,
BEAT_SPARKLE,
BEAT_BELL_L,
BEAT_MOSQ,
BEAT_SPACE,
BEAT_SWEEP,
BEAT_TING,
//BEAT_OBOE,
BEAT_REVERSE,
BEAT_SAW,
//BEAT_BASSOON,
BEAT_BRASS,
BEAT_CHOIR,
BEAT_FLUTE,
BEAT_ODD,
BEAT_REVERSE2,
BEAT_ACCORD,


NO_BEATS,
BEAT_EMPTY
};


struct beat_phrase_struct
{
 int samp;
 int pitch;
 int rand_pitch;
 int pan;
 int rand_pan;
 int melody;
 int vol;
};

struct beat_phrase_struct beat_phrase [MAX_PHRASES] [MAX_PHRASE_LENGTH];

int over_beat_phrase [MAX_OVERPHRASES];

int bpos;
int over_bpos;

int phrase_length;
int over_phrase_length;
int over_phrase_loop_point;

#define MAX_MELODIES 5
#define MAX_MELODY_LENGTH 400
#define MAX_OVERMELODIES 20

enum
{
OVERMELODY_REPEAT,
OVERMELODY_FILL
}; // for overmelody_mode

struct overmelody_struct 
{
 int mel;
 int key;
 int start;
};


struct overmelody_struct overmelody [MAX_OVERMELODIES];

struct beat_phrase_struct melody [MAX_MELODIES] [MAX_MELODY_LENGTH];

int melody_active [MAX_MELODIES];
int mpos [MAX_MELODIES];
int over_mpos;
int over_mpos2;
int overmelodies;
int overmelody_mode;
int overmelody_fill_length;
int melodies_played;
int melody_length [MAX_MELODIES];
int mdelay [MAX_MELODIES];
//int total_melody_delay;

extern struct optionstruct options;


SAMPLE *beat [NO_BEATS];



//SAMPLE *sounds [NO_WAVS];
SAMPLE *new_sounds [NO_NWAVS];

char sound_active;

void check_sound(SAMPLE *samp);
void load_sample_in(int samp, const char *sfile);
void load_new_sample_in(int samp, const char *sfile);
void copy_phrase(int target, int source);
//void play_beat(int samp);
//void play_beatf(int samp, int freq);
void play_beatfvp(int samp, int freq, int vol, int pan);
void play_successfvp(int samp, int freq, int vol, int pan);
void init_beat(void);
void load_beat_sample_in(int samp, const char *sfile);
void calculate_beat(void);
void calculate_melody(int active);
void play_melody(void);
void copy_melody(int t, int s);
void add_note(int beat, int bp1, int bp2, int pitch, int rand_pitch, int pan, int vol, int mel);
void add_over_phrase(int phrase, int start, int finish);
void add_melody_note(int samp, int melody, int mp2, int pitch, int rand_pitch, int pan, int vol);
void add_melody_string1(int samp, int mp1, int length, int chords);
//void assign_beat(int index, int beaty, int subtype);
void create_progression(int which, int length, int key);
void play_success_sound(void);

//int ppos;
//int plength;



void init_sound(void)
{
#ifdef SOUND_OFF
 return;
#endif


   int i, j;
   float t;
   
   for (i = 0; i < NO_TONES; i ++)
   { 
    t = (float) BASE_TONE;
    for (j = 0; j < i; j ++)
    {
     t *= (float) (1000 + (1000 / 13)) / 1000;
    }
    tone [i] = t;
    // saves me from having to remember how to use the pow function
   }
   
   sound_active = 1;

   if (options.sound_init == 0)
   {
//    cprintf("\n\r\n\rSound disabled in proj.cfg.");
//    allegro_message("\n\r\n\rSound disabled in config file.\n\r");
//    allegro_message("\n\r\n\rSound disabled in config file.\n\r");
//    rest(500);
    sound_active = 0;
    return;
   }

   reserve_voices(16, 0);
   if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) == -1)
   {
//    allegro_message("\n\r\n\rSound autodetect failed.");
    sound_active = 0;
//    rest(300);
//    do
//    {
//    } while (keypressed() == 0);
   }
   set_volume(255, 0);

   load_new_sample_in(NWAV_ALARM, "alarm");
   load_new_sample_in(NWAV_BANG, "bang");
   load_new_sample_in(NWAV_BIGBANG, "bigbang");
   load_new_sample_in(NWAV_BLAST, "blast");
   load_new_sample_in(NWAV_BLOCK, "block");
   load_new_sample_in(NWAV_BUMP, "bump");
   load_new_sample_in(NWAV_BUMP2, "bump2");
   load_new_sample_in(NWAV_BURST, "burst");
   load_new_sample_in(NWAV_SHORTBURST, "bursts");
   load_new_sample_in(NWAV_BURSTZ, "burstz");
   load_new_sample_in(NWAV_BURSTZL, "burstzl");
   load_new_sample_in(NWAV_CHIME, "chime");
   load_new_sample_in(NWAV_CHIME2, "chime2");
   load_new_sample_in(NWAV_CHIRP, "chirp2");
   load_new_sample_in(NWAV_CLICK, "click");
   load_new_sample_in(NWAV_CYMBAL, "cymbal");
   load_new_sample_in(NWAV_DART, "dart");
   load_new_sample_in(NWAV_DNO, "dno");
   load_new_sample_in(NWAV_DRIVE, "drive");
//   load_sample_in(WAV_ENGINE, "engine");
   load_new_sample_in(NWAV_EXTRA, "extra");
   load_new_sample_in(NWAV_EYE, "eye");
   load_new_sample_in(NWAV_GAMEOVER, "gameover");
   load_new_sample_in(NWAV_GBLAT, "gblat");
   load_new_sample_in(NWAV_JET, "jet");
   load_new_sample_in(NWAV_LAUNCH, "launch");
   load_new_sample_in(NWAV_LZAP, "longzap");
   load_new_sample_in(NWAV_MENU, "menu");
   load_new_sample_in(NWAV_MINEBANG, "minebang"); // not mines - pulser arms
   load_new_sample_in(NWAV_PHASE, "phase");
   load_new_sample_in(NWAV_PPIPE, "ppipe");
   load_new_sample_in(NWAV_PUFF, "puff");
   load_new_sample_in(NWAV_REPAIR, "repair");
   load_new_sample_in(NWAV_SEEKER, "seeker");
   load_new_sample_in(NWAV_SHADOW, "shadow");
   load_new_sample_in(NWAV_SZAP, "sharpzap");//szap");
   load_new_sample_in(NWAV_SHIELD, "shield");
   load_new_sample_in(NWAV_SHIELDE, "shielde");
   load_new_sample_in(NWAV_SPAWN, "spawn");
   load_new_sample_in(NWAV_SPLERK, "splerk");
   load_new_sample_in(NWAV_BALL1, "squelch1");
   load_new_sample_in(NWAV_MULTI, "squelch2");
   load_new_sample_in(NWAV_SUCCESS, "success");
   load_new_sample_in(NWAV_SYMBOL, "symbol");
   load_new_sample_in(NWAV_TEETH, "teeth");
   load_new_sample_in(NWAV_TWING, "twing"); // ouch sound
   load_new_sample_in(NWAV_WARBLE, "warble");
   load_new_sample_in(NWAV_WARBLEB, "warbleb");
   load_new_sample_in(NWAV_WHINE, "whine4");
   load_new_sample_in(NWAV_WORMS, "worms");
   load_new_sample_in(NWAV_ZAPSTEP, "zapstep");

   

//   load_new_sample_in(NWAV_, "");

/*
   load_sample_in(WAV_CANNON, "zap");
   load_sample_in(WAV_WOBBLE, "wobble");
   load_sample_in(WAV_LONG_WOBBLE, "longwob");
   load_sample_in(WAV_WARP_IN, "warp_in");
   load_sample_in(WAV_BANG1, "bangs");
   load_sample_in(WAV_WHINE, "whine");
   load_sample_in(WAV_BUMP, "bump");
   load_sample_in(WAV_BOSS2, "boss2");//_2");
   load_sample_in(WAV_STING, "sting");
   load_sample_in(WAV_HARD_ZAP, "hardzap");
   load_sample_in(WAV_BLORT, "blort");
   load_sample_in(WAV_BLAST, "blast");
   load_sample_in(WAV_ALARM, "alarm");
   load_sample_in(WAV_MISSILE, "missile");
   load_sample_in(WAV_ZAPNOTE1, "zapnote1");
   load_sample_in(WAV_ZAPNOTE2, "zapnote2");
   load_sample_in(WAV_MINE, "mine");
   load_sample_in(WAV_SHORTZAP, "shortzap");
   load_sample_in(WAV_BLAT, "blat");
   load_sample_in(WAV_SHORTZAP2, "shortz2");
   load_sample_in(WAV_SUN, "sun");
   load_sample_in(WAV_THRUM, "thrum");
   load_sample_in(WAV_CROAK, "croak");
   load_sample_in(WAV_MINEBANG, "minebang");
   load_sample_in(WAV_ASPAWN, "aspawn");
   load_sample_in(WAV_GAME_OVER, "gover2");
   load_sample_in(WAV_TUBE, "tube");
   load_sample_in(WAV_UPGRADE, "upgr");
   load_sample_in(WAV_SHIELD, "shield");
   load_sample_in(WAV_REPAIR, "repair");
   load_sample_in(WAV_SEEKMINE, "seekmine");
   load_sample_in(WAV_PLASMA, "plasma");

   load_sample_in(WAV_LEVEL_END, "finish");
   load_sample_in(WAV_MENU1, "menu1");
   load_sample_in(WAV_MENU2, "menu2");
   load_sample_in(WAV_EXTRA_SHIP, "extra");
   load_sample_in(WAV_ORBITAL, "orbital");
   load_sample_in(WAV_PICKUP_UPGRADE, "pickup");
   load_sample_in(WAV_SEEKBLOB_BANG, "seekb");
   load_sample_in(WAV_BUMP2, "bump2");
   load_sample_in(WAV_HIT, "hit");
   load_sample_in(WAV_HOSTILE, "buzzzap");
//   load_sample_in(WAV_MUTATE, "mutate");
//   load_sample_in(WAV_, "");
*/
   init_beat();

//   priority->sound_list[WAV_POP] = 1;

}

void load_new_sample_in(int samp, const char *sfile)
{

 char sfile_name [50];

 strcpy(sfile_name, ".//wavs//");
 strcat(sfile_name, sfile);
 strcat(sfile_name, ".wav");

 new_sounds [samp] = load_sample(sfile_name);

 if (new_sounds [samp] == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load sound file: %s", sfile_name);
      exit(1);
 }
}

/*void load_sample_in(int samp, const char *sfile)
{

 char sfile_name [50];

 strcpy(sfile_name, ".//sound//");
 strcat(sfile_name, sfile);
 strcat(sfile_name, ".wav");

 sounds [samp] = load_sample(sfile_name);

 if (sounds [samp] == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load sound file: %s", sfile_name);
      exit(1);
 }
}

*/
/*
void load_sample_in(SAMPLE *samp)
{
 if (samp == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load sound file");
      exit(1);
 }
}
*/
/*
void play_sound(int sample)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;
 play_sample(sounds [sample], (int) (250 * options.sound_volume) / 100, 127, 1000, 0);

}
*/
/*
void play_sound2(int sample, int frq, int vol, int pan)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;
 play_sample(sounds [sample], (vol * options.sound_volume) / 100, pan, frq, 0);


}

void play_soundf(int sample, int frq)
{

#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;
 play_sample(sounds [sample], (int) (255 * options.sound_volume) / 100, 127, frq, 0);


}

void play_sound_pos(int sample, int frq, int vol, int x2, int y2)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;

 int pan = 127;

 int vol2 = vol;
 int distance;

 int x1 = actor[player[game.single_player].actor_controlled].x;
 int y1 = actor[player[game.single_player].actor_controlled].y;

 distance = hypot(abs(x2 - x1), abs(y2 - y1)) / GRAIN;
 if (distance > 1000)
  return;
 if (distance > 300)
 {
  distance -= 300;
  distance = 1000 - distance;
  vol2 *= distance; //(800 - (distance - 300));
  vol2 /= 1000;
 }



 if (options.sound_mode == SOUNDMODE_MONO || game.users == 2)
  pan = 127;
   else
   {
    if (x1 == x2)
    {
     pan = 127;
    }
     else
     {
      if (x1 < x2 - (300 * GRAIN))
       pan = 0;
        else
         {
          if (x1 > x2 + (300 * GRAIN))
           pan = 255;
            else
            {
             if (x1 > x2)
             {
              pan = 127 + ((x1 - x2) * 125) / (300 * GRAIN);
             }
              else
              {
               pan = 127 - ((x2 - x1) * 125) / (300 * GRAIN);
              }
            }
         }
    }
   if (options.sound_mode == SOUNDMODE_REVERSED)
    pan = 255 - pan;
  }  



// stop_sample(sounds [sample]);

 play_sample(sounds [sample], (vol2 * options.sound_volume) / 100, pan, frq, 0);

}

*/

void set_engine_sound(int a, int drive, int setting)
{

#ifdef SOUND_OFF
 return;
#endif

}

void kill_drive_sounds(void)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0)
  return;

// if (actor[a].drive_sound [DRIVE_THRUST] > 0)
 {
//  stop_sample(sounds [WAV_ENGINE]);
 }
}



/*
************************

Beat functions

************************
*/


void play_beats(void)
{

 if (arena.level_finished == 1 && sound_active != 0)
 {
  play_success_sound();
  return;
 } // this is in play_beats, but it goes through whichever volume control is higher



 if (sound_active == 0 || options.ambience_volume == 0)
  return;
// if (options.ambience_volume == 0)
//  return;


 play_melody();

  
 bpos ++;
 if (bpos >= phrase_length)
 {
  bpos = 0;
  over_bpos ++;
  if (over_bpos >= over_phrase_length)
   over_bpos = over_phrase_loop_point;
 } 

 if (beat_phrase [over_beat_phrase [over_bpos]] [bpos].samp == BEAT_EMPTY)
  return;

 
 int tone_play = beat_phrase [over_beat_phrase [over_bpos]] [bpos].pitch;
 
 if (beat_phrase [over_beat_phrase [over_bpos]] [bpos].rand_pitch != -1)
 {
  if (grand(5) != 0)
   tone_play += chord_jumps [beat_phrase [over_beat_phrase [over_bpos]] [bpos].rand_pitch] [grand(NO_JUMPS)];
    else 
     tone_play -= chord_jumps [beat_phrase [over_beat_phrase [over_bpos]] [bpos].rand_pitch] [grand(NO_JUMPS)];
 }

/* int mel = beat_phrase [over_beat_phrase [over_bpos]] [bpos].melody;

 if (mel != -1)
 {
  tone_play = prog_base [mel] + progression [mel] [prog_pos [mel]];
  prog_pos [mel] ++;
  if (prog_pos [mel] >= prog_length [mel])
   prog_pos [mel] = 0;
 }*/
 
 if (beat_phrase [over_beat_phrase [over_bpos]] [bpos].samp != BEAT_EMPTY)
// if (FALSE)
 {
   play_beatfvp(beat_phrase [over_beat_phrase [over_bpos]] [bpos].samp, 
//    beat_phrase [over_beat_phrase [over_bpos]] [bpos].pitch + grand(beat_phrase [over_beat_phrase [over_bpos]] [bpos].rand_pitch),
    tone [tone_play],
//    note [beat_phrase [over_beat_phrase [over_bpos]] [bpos].pitch],
    beat_phrase [over_beat_phrase [over_bpos]] [bpos].vol,
    beat_phrase [over_beat_phrase [over_bpos]] [bpos].pan);
 
 }
/*  
 if (ppos == 12 || ppos == 24)
  play_beat(BEAT_LOWDRUM);

 if (ppos == 17)
  play_beat(BEAT_ZAPDRUM1);
*/

 
}


void calculate_beat(void)
{

 if (sound_active == 0)
  return;

 int success_chord = grand(NO_CHORDS);
 
 success_base = 1;

 success_note [0] = chord_jumps [success_chord] [0];
 success_note [1] = chord_jumps [success_chord] [grand(NO_JUMPS)];
 success_note [2] = chord_jumps [success_chord] [grand(NO_JUMPS)];

 success_samp = BEAT_STRING;
 
 switch(grand(6))
 {
  case 0: success_samp = BEAT_STRING; break;
  case 1: success_samp = BEAT_BELL_S; break;
  case 2: success_samp = BEAT_BELL_L; break;
  case 3: success_samp = BEAT_STRING; break;
  case 4: success_samp = BEAT_SPARKLE; break;
  case 5: success_samp = BEAT_ZAPDRUM2; break;
 }

 success_step = 1 + grand(3);

 if (options.ambience_volume == 0)
  return;
 
 int i, j;
 int basic_beat [5];
 
 int split_phrase = 0;
 if (grand(3) != 0)
  split_phrase = 1;
 if (grand(4) == 0)
  split_phrase = 2;


 for (i = 0; i < MAX_PHRASES; i ++)
 {
  for (j = 0; j < MAX_PHRASE_LENGTH; j ++)
  {
   beat_phrase[i] [j].samp = BEAT_EMPTY;
   beat_phrase[i] [j].pitch = 1000;
   beat_phrase[i] [j].rand_pitch = 0;
   beat_phrase[i] [j].pan = 127;
   beat_phrase[i] [j].rand_pan = 0;
   beat_phrase[i] [j].melody = 0;
   beat_phrase[i] [j].vol = 200;
  } 
 }

// phrase_length = (20 + grand(25)) * (split_phrase + 1);
 phrase_length = (15 + grand(20)) * (split_phrase + 1);
// phrase_length = (12 + grand(10)) * (split_phrase + 1);
 if (grand(5) == 0)
  phrase_length += 15;
// phrase_length = (10) * (split_phrase + 1);
 
/* basic_beat [0] = grand(BEAT_TWIRM + 1);
 basic_beat [1] = grand(BEAT_TWIRM + 1);
 basic_beat [2] = grand(BEAT_TWIRM + 1);
 basic_beat [3] = grand(BEAT_TWIRM + 1);
 basic_beat [4] = grand(BEAT_TWIRM + 1);*/

 basic_beat [0] = grand(BEAT_TWIRM + 1);
 basic_beat [1] = grand(BEAT_TWIRM + 1);  

 if (grand(4) == 0)
  basic_beat [0] = grand(NO_BEATS);
 if (grand(4) == 0)
  basic_beat [1] = grand(NO_BEATS);
 basic_beat [2] = grand(NO_BEATS);
 basic_beat [3] = grand(NO_BEATS);
 basic_beat [4] = grand(NO_BEATS);

 int rand_note = -1; 
// int prog_note = -1;
//nt no_progs = 0;
// int prog_active [NO_PROGS] = {0,0,0,0};
 int base_phrase_length = phrase_length;
 int num_phrases = 0;
// int mel_delay = 0;
// int mel_length;
// int mel_start;
// int mel_pos;
// int mel_delay_rand;
 
 if (split_phrase == 1)
  base_phrase_length /= 2;
 if (split_phrase == 3)
  base_phrase_length /= 3;
  
 int basenote = 25;
 int randnote = 27; 

  add_note(basic_beat [0], 0, grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  add_note(basic_beat [0], 0, grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  if (split_phrase > 0)
  {
   add_note(basic_beat [0], 0, base_phrase_length + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
   add_note(basic_beat [0], 0, base_phrase_length + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  }
  if (split_phrase > 1)
  {
   add_note(basic_beat [0], 0, (base_phrase_length * 2) + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
   add_note(basic_beat [0], 0, (base_phrase_length * 2) + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  }
  num_phrases ++;

  copy_phrase(1, 0);
  add_note(basic_beat [1], 1, grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  add_note(basic_beat [1], 1, grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  if (split_phrase > 0)
  {
   add_note(basic_beat [1], 1, base_phrase_length + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
   add_note(basic_beat [1], 1, base_phrase_length + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  }
  if (split_phrase > 1)
  {
   add_note(basic_beat [1], 0, (base_phrase_length * 2) + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
   add_note(basic_beat [1], 0, (base_phrase_length * 2) + grand(base_phrase_length), basenote + grand(randnote), -1, 127, 200, 0);
  }
  num_phrases ++;

  copy_phrase(2, 1);
  rand_note = grand(NO_CHORDS);
  if (grand(15) != 0)
   rand_note = -1;
  add_note(basic_beat [2], 2, grand(base_phrase_length), basenote + grand(randnote), rand_note, 127, 200, -1);
  if (split_phrase > 0)
   add_note(basic_beat [2], 2, base_phrase_length + grand(base_phrase_length), basenote + grand(randnote), rand_note, 127, 200, -1);
  if (split_phrase > 1)
   add_note(basic_beat [2], 2, (base_phrase_length * 2) + grand(base_phrase_length), basenote + grand(randnote), rand_note, 127, 200, -1);
  num_phrases ++;

//  prog_note = -1;
  if (grand(2) == 0)
  {
    calculate_melody(1);  
  }
   else
   {
    calculate_melody(0);
    copy_phrase(3, 2);
    rand_note = grand(NO_CHORDS);
    if (grand(15) != 0)
     rand_note = -1;
    add_note(basic_beat [3], 3, grand(base_phrase_length), basenote + grand(randnote), rand_note, 127, 200, -1);
    if (split_phrase > 0)
     add_note(basic_beat [3], 3, base_phrase_length + grand(base_phrase_length), basenote + grand(randnote), rand_note, 127, 200, -1);
    if (split_phrase > 1)
     add_note(basic_beat [3], 3, (base_phrase_length * 2) + grand(base_phrase_length), basenote + grand(randnote), rand_note, 127, 200, -1);
// add_note(basic_beat [0], 0, grand(phrase_length), 15 + grand(10), 0, 127, 200);
   }

 if (grand(2) == 0)
   num_phrases ++;

/*
 prog_note = -1;
 copy_phrase(4, 3);
 rand_note = grand(NO_CHORDS);
 if (grand(3) != 0)
  rand_note = -1;
 if (grand(3) != 0)
 {
  prog_note = 2;
  prog_active [2] = 1;
 } 
// if (prog_note == 2)
//  no_progs ++; 
 add_note(basic_beat [4], 4, grand(base_phrase_length), 15 + grand(10), rand_note, 127, 200, prog_note);
 if (split_phrase > 0)
  add_note(basic_beat [4], 4, base_phrase_length + grand(base_phrase_length), 15 + grand(10), rand_note, 127, 200, prog_note);
 if (split_phrase > 1)
  add_note(basic_beat [4], 4, (base_phrase_length * 2) + grand(base_phrase_length), 15 + grand(10), rand_note, 127, 200, prog_note);
// add_note(basic_beat [1], 0, grand(phrase_length), 15 + grand(10), 0, 127, 200);
*/

 int oplength = 7 + grand(5);
 int pause_length = grand(3);
 int up_to = 0, next_op = 0;

 add_over_phrase(0, 0, MAX_OVERPHRASES);


 if (split_phrase == 1)
 {
//  if (grand(3) != 0)
//   no_progs *= 2;
  oplength = 4 + grand(3); 
 }
 if (split_phrase == 2)
 {
//  if (grand(3) != 0)
//   no_progs *= 3;
  oplength = 2 + grand(3); 
 }
 
// if (grand(8) == 0)
//  no_progs = 4 + grand(3) - grand(3);
/* prog_length [0] = 8 + grand(3) - grand(3);
 prog_length [1] = prog_length [0];
 prog_length [2] = prog_length [0];
 prog_length [3] = prog_length [0];
 
 create_progression(0, prog_length [0], grand(NO_CHORDS));
 create_progression(1, prog_length [1], grand(NO_CHORDS));
 create_progression(2, prog_length [2], grand(NO_CHORDS));
 create_progression(3, prog_length [3], grand(NO_CHORDS));
*/ 
// prog_length = no_progs;

 for (i = 0; i < num_phrases; i ++)
 {
  next_op = up_to + oplength;
  add_over_phrase(i, up_to + pause_length, next_op);
  up_to = next_op;
 }
 
/* add_over_phrase(0, 0, 9);
 add_over_phrase(1, 10, 20);
 add_over_phrase(2, 21, 30);
 add_over_phrase(3, 31, 40);
 add_over_phrase(4, 41, 50);*/

 over_phrase_length = up_to;
 over_phrase_loop_point = oplength * (1 + grand(3));

 bpos = 0;
 over_bpos = 0;
 
/* for (i = 0; i < NO_PROGS; i ++)
 {
  prog_pos [i] = 0;
  prog_base [i] = 10 + grand(15);
 } */
 

}

void add_note(int beat, int bp1, int bp2, int pitch, int rand_pitch, int pan, int vol, int mel)
{
 if (bp1 >= MAX_PHRASES || bp2 >= MAX_PHRASE_LENGTH)
  return; 
 if (pitch <= 0 || pitch >= NO_TONES)
  return; 
 beat_phrase[bp1] [bp2].samp = beat;
 beat_phrase[bp1] [bp2].pitch = pitch;
 beat_phrase[bp1] [bp2].rand_pitch = rand_pitch;
 if (grand(3) == 0)
  beat_phrase[bp1] [bp2].pan = grand(255);
   else
    beat_phrase[bp1] [bp2].pan = pan;
 if (options.sound_mode == SOUNDMODE_MONO)
  beat_phrase[bp1] [bp2].pan = 127; // don't worry about reversing stereo; it doesn't matter
 beat_phrase[bp1] [bp2].vol = vol; 
 beat_phrase[bp1] [bp2].melody = mel; 
}

void add_over_phrase(int phrase, int start, int finish)
{
 int i;
 for (i = start; i < finish + 1; i ++)
 {
  if (i >= MAX_OVERPHRASES)
   return;
  over_beat_phrase [i] = phrase;
 }

}

void copy_phrase(int target, int source)
{
 int j;
 if (source >= MAX_PHRASES || target >= MAX_PHRASES)
  return; 

 for (j = 0; j < MAX_PHRASE_LENGTH; j ++)
 {
  beat_phrase[target] [j].samp = beat_phrase[source] [j].samp;
  beat_phrase[target] [j].pitch = beat_phrase[source] [j].pitch;
  beat_phrase[target] [j].rand_pitch = beat_phrase[source] [j].rand_pitch;
  beat_phrase[target] [j].pan = beat_phrase[source] [j].pan;
  beat_phrase[target] [j].rand_pan = beat_phrase[source] [j].rand_pan;
  beat_phrase[target] [j].melody = beat_phrase[source] [j].melody;
 } 
}

void create_progression(int which, int length, int key)
{
 int i;
 int n = grand(NO_JUMPS);
 int longprogpos = 0;
 
 if (FALSE) //grand(4) == 0)
 { 
  for (i = 0; i < length; i ++)
  {
   progression [which] [i] = chord_jumps [key] [n];
   n += grand(4);
   if (grand(3) == 0)
    n -= grand(4);
   n %= NO_JUMPS;
  }
  return;
 } 
 
  for (i = 0; i < length; i ++)
  {
   progression [which] [i] = long_prog [0] [longprogpos];
   longprogpos ++;
   if (long_prog [0] [longprogpos] == NOTE_ENDNOTE)
    longprogpos = 0;
  }

}

void play_melody(void)
{
//return;

 int i = 0;
 
if (overmelody_mode == OVERMELODY_REPEAT)
{
 for (i = 0; i < MAX_MELODIES; i ++)
 {
  if (melody_active [i] == 0)
   continue;
  if (mdelay [i] > 0)
  {
   mdelay [i] --;
   continue;
  }
  mpos [i] ++;
  if (mpos [i] >= melody_length [i])
  {
   mpos [i] = 0;
   over_mpos ++;
   if (over_mpos >= overmelodies)
    over_mpos = 0;
   melodies_played ++; 
  } 
// } 
 if (melody [i] [mpos [i]].samp != BEAT_EMPTY
    && overmelody [over_mpos].start <= melodies_played)
  {

    debug_sound [0] = over_mpos;
    debug_sound [1] = over_mpos2; //mpos [i];
    debug_sound [2] = melody [i] [mpos [i]].pitch + overmelody [over_mpos].key;
    debug_sound [3] = tone [melody [i] [mpos [i]].pitch + overmelody [over_mpos].key]; 
   if (melody [i] [mpos [i]].pitch + overmelody [over_mpos].key <= 0
       || melody [i] [mpos [i]].pitch + overmelody [over_mpos].key >= NO_TONES)
    return;
    play_beatfvp(melody  [i] [mpos [i]].samp, 
//     melody [i] [mpos [i]].pitch + overmelody [over_mpos], //grand(melody [i] [mpos [i]].rand_pitch),
     tone [melody [i] [mpos [i]].pitch + overmelody [over_mpos].key], //grand(melody [i] [mpos [i]].rand_pitch),
     melody [i] [mpos [i]].vol,
     melody [i] [mpos [i]].pan);

  }
 } 


}

  if (overmelody_mode == OVERMELODY_FILL)
  {
    mpos [0] ++;
   if (mpos [0] >= melody_length [0])
   {
    over_mpos2 ++;
    mpos [0] = 0;
    if (over_mpos2 >= overmelody_fill_length)
    {
     over_mpos2 = 0;     
     over_mpos ++;
     if (over_mpos >= overmelodies)
      over_mpos = 1;
    }
     if (mdelay [0] > 0)
      mdelay [0] --;
   } 

    if (mdelay [0] > 0)
    {
     return;
    }


  if (melody [over_mpos] [mpos [0]].samp != BEAT_EMPTY)
   {

    debug_sound [0] = mpos [0];
    debug_sound [1] = over_mpos; //mpos [i];
    debug_sound [2] = over_mpos2;
    debug_sound [3] = tone [melody [over_mpos] [mpos [0]].pitch + overmelody [over_mpos].key]; 
   if (melody [over_mpos] [mpos [0]].pitch + overmelody [over_mpos].key <= 0
       || melody [over_mpos] [mpos [0]].pitch + overmelody [over_mpos].key >= NO_TONES)
    return;
    play_beatfvp(melody  [over_mpos] [mpos [0]].samp, 
//     melody [i] [mpos [i]].pitch + overmelody [over_mpos], //grand(melody [i] [mpos [i]].rand_pitch),
     tone [melody [over_mpos] [mpos [0]].pitch + overmelody [over_mpos].key], //grand(melody [i] [mpos [i]].rand_pitch),
     melody [over_mpos] [mpos [0]].vol,
     melody [over_mpos] [mpos [0]].pan);

   }

  }  



}



void calculate_melody(int active)
{
// now, we assume that the beat is fully set up, so we can use all of the beat-related globals.

 int i, j;
 int overlength = 1 + grand(3);
 int melody_note = BEAT_ZAPDRUM1;
// if (grand(2) == 0)
  melody_note = grand(NO_BEATS - BEAT_TWIRM) + BEAT_TWIRM;//BEAT_OBOE;
 
 j = 0;

 melody_active [0] = 0;
 melody_active [1] = 0;
 melody_active [2] = 0;
 melody_active [3] = 0;
 
  for (i = 0; i < MAX_OVERMELODIES; i ++)
  {
   overmelody [i].mel = 0;
   overmelody [i].key = NOTE_1C;
   overmelody [i].start = 0;
  }

 
  for (i = 0; i < MAX_MELODIES; i ++)
  {
   melody_length [i] = phrase_length * overlength;
   for (j = 0; j < MAX_MELODY_LENGTH; j ++)
   {
    melody [i] [j].samp = BEAT_EMPTY;
   }
  }

 if (!active)
  return; // just clear it

 int chord = CHORD_MAJOR; //grand(NO_CHORDS);

 j = 0;





/* add_melody_string1(melody_note, 1, melody_length [1], chord);
 add_melody_string1(melody_note, 2, melody_length [2], chord);
 add_melody_string1(melody_note, 3, melody_length [3], chord);
*/
/* add_melody_note(melody_note, j, 8, 0, 0, 127, 200);
 add_melody_note(melody_note, j, 16, 5, 0, 127, 200);
 add_melody_note(melody_note, j, 24, 8, 0, 127, 200);
 add_melody_note(melody_note, j, 32, 11, 0, 127, 200);
 add_melody_note(melody_note, j, 40, 8, 0, 127, 200);*/
/* add_melody_note(melody_note, j, grand(melody_length [j]), 5, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 10, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 15, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 20, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 25, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 30, 0, 127, 200);*/
/*
 add_melody_note(melody_note, j, grand(melody_length [j]), 0, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 5, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 10, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 15, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 20, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 25, 0, 127, 200);
 add_melody_note(melody_note, j, grand(melody_length [j]), 30, 0, 127, 200);
*/
 switch(grand(2))
 {
  case 0:
   overmelody_mode = OVERMELODY_REPEAT;
   melody_active [0] = 1;
   overmelodies = 4;
   add_melody_string1(melody_note, 0, melody_length [0], chord);
   overmelody [0].mel = 0;
   overmelody [0].key = 0;
   overmelody [0].start = 3;
   overmelody [1].mel = 0;
   overmelody [1].key = chord_jumps [chord] [grand(4)];
   overmelody [1].start = 5;
   overmelody [2].mel = 0;
   overmelody [2].key = chord_jumps [chord] [grand(4)];
   overmelody [2].start = 7;
   overmelody [3].mel = 0;
   overmelody [3].key = chord_jumps [chord] [grand(4)];
   overmelody [3].start = 9;
   break;
  case 1:
   overmelody_mode = OVERMELODY_FILL;
   overmelodies = 3;
   overmelody_fill_length = 4;
   if (melody_length [0] < 120)
    overmelody_fill_length = 6 + grand(3);
   if (melody_length [0] < 80)
    overmelody_fill_length = 8 + grand(3);
   if (melody_length [0] < 50)
    overmelody_fill_length = 10 + grand(3);
   melody_active [0] = 1;
   melody_active [1] = 1;
   melody_active [2] = 1;
   melody_length [2] = melody_length [0];
   add_melody_string1(melody_note, 2, melody_length [0], chord);
   copy_melody(1, 2);
   j = 0;
   for (i = 0; i < MAX_MELODY_LENGTH; i ++)
   {
    if (melody [1] [i].samp != BEAT_EMPTY)
    {
     j ++;
     if (j % 2 != 0)
      melody [1] [i].samp = BEAT_EMPTY;
    }
   }
   copy_melody(0, 1);
   j = 0;
   for (i = 0; i < MAX_MELODY_LENGTH; i ++)
   {
    if (melody [0] [i].samp != BEAT_EMPTY)
    {
     j ++;
     if (j % 3 == 0)
      melody [0] [i].samp = BEAT_EMPTY;
    }
   }
   overmelody [0].mel = 0;
   overmelody [0].key = 0;
   overmelody [0].start = 4;
   overmelody [1].mel = 1;
   overmelody [1].key = 0;
   overmelody [1].start = 7;
   overmelody [2].mel = 2;
   overmelody [2].key = 0;
   overmelody [2].start = 10;
   mdelay [0] = 4 + grand(4);
   break;
 } 


mpos [0] = 0;
over_mpos = 0;
over_mpos2 = 0;
melodies_played = 0;

int k;

  for (i = 0; i < MAX_MELODIES; i ++)
  {
   for (j = 0; j < melody_length [i]; j ++)
   {
    for (k = 0; k < MAX_OVERMELODIES; k ++)
    {
     if (melody [i] [j].pitch + overmelody [k].key <= 0 
         || melody [i] [j].pitch + overmelody [k].key >= NO_TONES)
          melody [i] [j].samp = BEAT_EMPTY;
    } 
   }
  }


}

void copy_melody(int t, int s)
{
 int i;
 
 for (i = 0; i < MAX_MELODY_LENGTH; i ++)
 {
  melody [t] [i].samp = melody [s] [i].samp;
  melody [t] [i].pitch = melody [s] [i].pitch;
  melody [t] [i].rand_pitch = melody [s] [i].rand_pitch;
  melody [t] [i].pan = melody [s] [i].pan;
  melody [t] [i].vol = melody [s] [i].vol; 
 }

 melody_length [t] = melody_length [s];
 mdelay [t] = mdelay [s];

}


void add_melody_note(int samp, int mp1, int mp2, int pitch, int rand_pitch, int pan, int vol)
{
 if (options.sound_mode == SOUNDMODE_MONO)
  pan = 127;
 melody [mp1] [mp2].samp = samp;
 melody [mp1] [mp2].pitch = pitch;
 melody [mp1] [mp2].rand_pitch = rand_pitch;
 melody [mp1] [mp2].pan = pan;
 melody [mp1] [mp2].vol = vol; 

}


void add_melody_string1(int samp, int mp1, int length, int chords)
{
 int mstring [100];
 int nm = 0;
 int i;
 int disp = 0;
/// int jump = 0;

 for (i = 0; i < 100; i ++)
 {
  mstring [i] = -1;
 }

 int basenote = 0 + grand(13);
 
/* nm = 3 + grand(6);
 if (length < 35)
  nm = 2 + grand(3); */
  
 nm = length / (10 + grand(6)); 

 if (nm >= 100)
  nm = 99;

 mstring [0] = BEAT_EMPTY; //basenote;

 int which_longprog = grand(NO_LONGPROGS);
 int longprogpos = 0;
 int lplength = 0;
 
 for (i = 0; i < SIZE_LONGPROGS; i ++)
 {
  lplength = i;
  if (long_prog [which_longprog] [i] == NOTE_ENDNOTE)
   break;
 }

 longprogpos = grand(lplength);

 for (i = 0; i < nm; i ++)
// i = 1, not 0
 {
   mstring [i] = basenote + long_prog [which_longprog] [longprogpos];
//   mstring [i] = 40 + i;
   longprogpos ++;
   if (long_prog [which_longprog] [longprogpos] == NOTE_ENDNOTE)
    longprogpos = 0;
 
//  jump = chord_jumps [chords] [grand(NO_JUMPS)];
/*  switch(grand(6))
  {
   case 0: jump = 0; break;
   case 1: jump = 5; break;
   case 2: jump = 8; break;
   case 3: jump = 11; break;
   case 4: jump = 13; break;
   case 5: jump = 9; break;
  }*/
/*  if (grand(2) == 0)
   jump *= -1;
  if (grand(3) == 0)
   mstring [i] = mstring [i - 1] + jump;
    else
     mstring [i] = basenote + jump;
  if (mstring [i] < 0 || mstring [i] >= NO_TONES
      || (mstring [i - 1] != basenote && grand(3) == 0))
   mstring [i] = basenote;
  if (grand(10) == 0)
   mstring [i] = -1; */
 } 

 disp = length / nm;
 if (disp < 5)
  disp = 5;
 int j = 0;
 int base_disp = disp / 2;
 int rand_disp = grand(base_disp);
 
 if (grand(2) == 0)
 {
  base_disp = disp;
  rand_disp = 0;
 }
 
 for (i = 0; i < nm; i ++)
 {
  if (mstring [i] != -1)
   add_melody_note(samp, mp1, j, mstring [i], 0, 127, 200);
//   add_melody_note(samp, mp1, mstring [i], j, 0, 127, 200);
  j += base_disp + grand(rand_disp); 
  if (j >= MAX_MELODY_LENGTH)
   break;
 }


}


/*
void play_beat(int samp)
{

 play_sample(beat [samp], (255 * options.sound_volume) / 100, 127, 1000, 0);
}

void play_beatf(int samp, int freq)
{

 play_sample(beat [samp], (255 * options.sound_volume) / 100, 127, freq, 0);
}
*/
void play_beatfvp(int samp, int freq, int vol, int pan)
{
 play_sample(beat [samp], (vol * options.ambience_volume) / 100, pan, freq, 0);
}

void play_successfvp(int samp, int freq, int vol, int pan)
{
 int volum = options.ambience_volume;
 if (options.sound_volume > volum)
  volum = options.sound_volume;
 if (volum == 0)
  return;
 play_sample(beat [samp], (vol * volum) / 100, pan, freq, 0);
}



void play_success_sound(void)
{
 if (arena.game_over != 0)
  return;
 int ss = 166 - arena.time_left;
 
// if (ss < 40 || ss >= 160)
 if (ss >= 160)
  return;
 
 if (ss % 10 != 0)
  return;
  
 ss /= 10; 
 
 ss += 2;
 
 int freq = success_base;//NOTE_1C;  

 freq += (ss / 3) * success_step;

 if (ss % 3 == 0)
  freq += success_note [0];
 if (ss % 3 == 1)
  freq += success_note [1];
 if (ss % 3 == 2)
  freq += success_note [2];
  
// play_beatfvp(BEAT_BASS, tone [freq], 200, 127);
 play_successfvp(success_samp, tone [freq], 200, 127);
 
}



void init_beat(void)
{

 if (sound_active == 0)
  return;

 load_beat_sample_in(BEAT_DRUM1, "drum1");
 load_beat_sample_in(BEAT_DRUM2, "drum2");
 load_beat_sample_in(BEAT_LOWDRUM, "lowdrum");
 load_beat_sample_in(BEAT_STICK, "stick");
 load_beat_sample_in(BEAT_TWIRM, "twirm");
 load_beat_sample_in(BEAT_ZAPDRUM1, "zapdrum1");
 load_beat_sample_in(BEAT_ZAPDRUM2, "zapdrum2");
 load_beat_sample_in(BEAT_PPIPE, "ppipe");
 load_beat_sample_in(BEAT_BELL_S, "bell_s");
 load_beat_sample_in(BEAT_BELL_L, "bell_l");
 load_beat_sample_in(BEAT_SPARKLE, "sparkle");
 load_beat_sample_in(BEAT_MOSQ, "mosq");
 load_beat_sample_in(BEAT_SPACE, "space");
 load_beat_sample_in(BEAT_SWEEP, "sweep");
 load_beat_sample_in(BEAT_TING, "ting");
 load_beat_sample_in(BEAT_REVERSE, "reverse");
 load_beat_sample_in(BEAT_SAW, "saw");
 load_beat_sample_in(BEAT_BASS, "bass");
// load_beat_sample_in(BEAT_OBOE, "oboe");
 load_beat_sample_in(BEAT_BASS2, "bass2");
// load_beat_sample_in(BEAT_BASSOON, "bassoon");
 load_beat_sample_in(BEAT_BRASS, "brass");
 load_beat_sample_in(BEAT_CHOIR, "choir");
 load_beat_sample_in(BEAT_FLUTE, "flute");
 load_beat_sample_in(BEAT_ODD, "odd");
 load_beat_sample_in(BEAT_REVERSE2, "reverse2");
 load_beat_sample_in(BEAT_ACCORD, "accord");
 load_beat_sample_in(BEAT_STRING, "string");



// load_beat_sample_in(BEAT_, "");

// ppos = 0;
// plength = 0;


}




void load_beat_sample_in(int samp, const char *sfile)
{

 char sfile_name [50];

 strcpy(sfile_name, ".//wavs//beat//");
 strcat(sfile_name, sfile);
 strcat(sfile_name, ".wav");

 beat [samp] = load_sample(sfile_name);

 if (beat [samp] == NULL)
 {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Fatal Error: Unable to load sound file: %s", sfile_name);
      exit(1);
 }
}








void play_wav(int sample)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;

 play_sample(new_sounds [sample], (int) (250 * options.sound_volume) / 100, 127, 1000, 0);

}

void play_wav2(int sample, int frq, int vol, int pan)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;
/*
 stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);*/
// stop_sample(sounds [sample]);
 play_sample(new_sounds [sample], (vol * options.sound_volume) / 100, pan, frq, 0);


}

void play_wavf(int sample, int frq)
{

#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;
 play_sample(new_sounds [sample], (int) (255 * options.sound_volume) / 100, 127, frq, 0);


}

/*
Positional sound for stereo effects.
y2 currently unused, but may be in future for some kind of
triangulation.
*/
void play_wav_pos(int sample, int frq, int vol, int x2, int y2)
{
#ifdef SOUND_OFF
 return;
#endif

 if (sound_active == 0 || options.sound_volume == 0) return;

/* stop_sample(soundf[WAV_POP].dat);
 stop_sample(soundf[WAV_RICOCHET].dat);
 if (sample == WAV_S_PULSE)
  stop_sample(soundf[WAV_S_PULSE].dat);*/

/* if (options[0].positional_sound == 0 || game.users > 1)
 {
  play_sample(soundf[sample].dat, vol, 127, frq, 0);
 }*/


 int pan = 127;

 int vol2 = vol;
 int distance;

 int x1 = actor[player[game.single_player].actor_controlled].x;
 int y1 = actor[player[game.single_player].actor_controlled].y;

 distance = hypot(abs(x2 - x1), abs(y2 - y1)) / GRAIN;
 if (distance > 1000)
  return;
 if (distance > 300)
 {
  distance -= 300;
  distance = 1000 - distance;
  vol2 *= distance; //(800 - (distance - 300));
  vol2 /= 1000;
 }



 if (options.sound_mode == SOUNDMODE_MONO || game.users == 2)
  pan = 127;
   else
   {
    if (x1 == x2)
    {
     pan = 127;
    }
     else
     {
      if (x1 < x2 - (300 * GRAIN))
       pan = 0;
        else
         {
          if (x1 > x2 + (300 * GRAIN))
           pan = 255;
            else
            {
             if (x1 > x2)
             {
              pan = 127 + ((x1 - x2) * 125) / (300 * GRAIN);
             }
              else
              {
               pan = 127 - ((x2 - x1) * 125) / (300 * GRAIN);
              }
            }
         }
    }
   if (options.sound_mode == SOUNDMODE_REVERSED)
    pan = 255 - pan;
  }  

 play_sample(new_sounds [sample], (vol2 * options.sound_volume) / 100, pan, frq, 0);

}

void play_gameover_loop(int freq)
{

 play_sample(new_sounds [NWAV_GAMEOVER], (200 * options.sound_volume) / 100, 127, freq, 1);

}

void kill_gameover_loop(void)
{
 stop_sample(new_sounds [NWAV_GAMEOVER]);
}


