
void init_sound(void);
void play_sound(int sample);
void play_sound2(int sample, int frq, int vol, int pan);
void play_soundf(int sample, int frq);
void play_sound_pos(int sample, int frq, int vol, int x2, int y2);

void play_wav(int sample);
void play_wav2(int sample, int frq, int vol, int pan);
void play_wavf(int sample, int frq);
void play_wav_pos(int sample, int frq, int vol, int x2, int y2);

void kill_drive_sounds(void);

void play_beats(void);
void calculate_beat(void);

void play_gameover_loop(int freq);
void kill_gameover_loop(void);


enum
{
SOUNDMODE_OFF,
SOUNDMODE_MONO,
SOUNDMODE_STEREO,
SOUNDMODE_REVERSED
};

enum
{
NWAV_NONE,
NWAV_BURST,
NWAV_GBLAT,
NWAV_CHIRP,
NWAV_CLICK,
NWAV_DNO,
NWAV_PPIPE,
NWAV_WARBLE,
NWAV_WARBLEB,
NWAV_WHINE,
NWAV_ZAPSTEP,
NWAV_SHORTBURST,
NWAV_CYMBAL,
NWAV_CHIME,
NWAV_CHIME2,
NWAV_BALL1,
NWAV_BANG,
NWAV_MULTI,
NWAV_SZAP,
NWAV_TWING,
NWAV_PUFF,
NWAV_DRIVE,
NWAV_DART,
NWAV_PHASE,
NWAV_SPLERK,
NWAV_LZAP,
NWAV_SEEKER,
NWAV_WORMS,
NWAV_REPAIR,
NWAV_GAMEOVER,
NWAV_BIGBANG,
NWAV_SYMBOL,
NWAV_MENU,
NWAV_EYE,
NWAV_TEETH,
NWAV_SPAWN,
NWAV_BLOCK,
NWAV_LAUNCH,
NWAV_SUCCESS,
NWAV_EXTRA,
NWAV_SHIELD,
NWAV_BURSTZ,
NWAV_BURSTZL,
NWAV_SHIELDE,
NWAV_SHADOW,
NWAV_JET,
NWAV_ALARM,
NWAV_BLAST,
NWAV_BUMP,
NWAV_BUMP2,
NWAV_MINEBANG,
NO_NWAVS
};

/*enum
{
WAV_NONE,
WAV_CANNON,
WAV_ENGINE,
WAV_WOBBLE,
WAV_LONG_WOBBLE,
WAV_WARP_IN,
WAV_BANG1,
WAV_WHINE,
WAV_HARD_ZAP,
WAV_BOSS2,
WAV_STING,
WAV_BUMP,
WAV_BOMBS,
WAV_BLORT,
WAV_STING4,
WAV_MISSILE,
WAV_ALARM,
WAV_ZAPNOTE1,
WAV_ZAPNOTE2,
WAV_BLAST,
WAV_MINE,
WAV_SHORTZAP,
WAV_BLAT,
WAV_SHORTZAP2,
WAV_SUN,
WAV_THRUM,
WAV_CROAK,
WAV_MINEBANG,
WAV_GREENSEEKER1,
WAV_GREENSEEKER2,
WAV_TORPEDO,
WAV_BOSS3_1,
WAV_BOSS3_2,
WAV_BOSS3_3,
WAV_ASPAWN,
WAV_GAME_OVER,
WAV_TUBE,
WAV_UPGRADE,
WAV_SHIELD,
WAV_REPAIR,
WAV_SEEKMINE,
WAV_PLASMA,
WAV_LEVEL_END,
WAV_MENU1,
WAV_MENU2,
WAV_EXTRA_SHIP,
WAV_ORBITAL,
WAV_PICKUP_UPGRADE,
WAV_SEEKBLOB_BANG,
WAV_BUMP2,
WAV_HIT,
WAV_HOSTILE,
WAV_MUTATE,
NO_WAVS

};
*/

