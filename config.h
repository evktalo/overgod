//Platform:

//#define OVERGOD_DOS

#ifdef ALLEGRO_WINDOWS
#define OVERGOD_WINDOWS
#endif

#if defined (ALLEGRO_LINUX) || defined(ALLEGRO_UNIX)
#define OVERGOD_LINUX
#endif

#ifdef ALLEGRO_MACOSX
#define OVERGOD_MAC
#endif


//#define SANITY_CHECK


#define NO_CLOUDS 1000
#define NO_ACTORS 25
#define NO_BULLETS 1000
#define NO_PLAYERS 2
#define GRAIN 1000
//#define NO_CMDS 12
#define NO_STARS 100
#define NO_PICKUPS 20

#define MAX_SCORE 9999999
#define GRID_WIDTH 50

#define NO_WEAPONS 2
#define NO_SHIP_TYPES 17

#define NO_SYSTEM_TYPES 3

#define NO_UPGRADES 9

#define FULL_UPGRADE 5

#define NO_ENEMY_ATTRIBUTES 10
#define NO_ENEMIES 40
#define MAX_TURRETS 4
//#define NO_ENEMY_TYPES 50

#define ANGLE_FULL_HEX 0x400
#define ANGLE_FULL 1024
#define ANGLE_HALF 512
#define ANGLE_QUARTER 256
#define ANGLE_1_EIGHTH 128
#define ANGLE_1_SIXTEENTH 64
#define ANGLE_3_EIGHTHS 384
#define ANGLE_3_SIXTEENTHS 192
#define ANGLE_1_32 32
#define ANGLE_TO_FIXED 4

#define OWNER_ENEMY -1

#define ATTACK_NONE -1

#define MAX_EYES 8

#ifndef PI
#define PI 3.141592
#endif

#define ARMOUR_UPGRADE 200


struct armoury
{
 char *name;
 int type;
 int speed;
 int timeout;
 int damage;
 int mass;

};

enum
{
TARGET_PRIMARY, // need to kill to advance
TARGET_NO, // turrets etc
TARGET_EXTRA,
TARGET_CRAWLY // crawlies...
};

enum
{
CMD_THRUST,
CMD_LEFT,
CMD_RIGHT,
CMD_FIRE1,
CMD_FIRE2,
CMD_BRAKE,
CMD_LEFT1,
CMD_RIGHT1,
CMD_UPGRADE,
CMD_LINK,
NO_CMDS
};

enum
{
GAME_SINGLE,
GAME_COOP,
GAME_DUEL,
GAME_TIME_ATTACK,
GAME_TIME_ATTACK_COOP
};

enum
{
SECONDARY_NONE,
SECONDARY_MISSILE,
SECONDARY_BOMB
};

enum
{
ACTORTYPE_NONE,
ACTORTYPE_SHIP,
ACTORTYPE_REDSHIP

};

/*
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

enum
{
SHIP_POINTY,
SHIP_HORSESHOE,
SHIP_ROUND,
SHIP_RETRO,
SHIP_SMALL,
SHIP_FINS,
SHIP_ORBITAL,
SHIP_CURVE

/*SHIP_LACEWING,
SHIP_CAPYBARA,
SHIP_DESPOT,
SHIP_HOOKWORM,
SHIP_LENTIL,
SHIP_PORKUPINE,
SHIP_PRONG,
SHIP_SCORPION,
SHIP_TORTFEASOR,
SHIP_AETHER,
SHIP_RUMSFELD,
SHIP_GODBOTHERER,
SHIP_BOTULUS,
SHIP_SLIDEWINDER,
SHIP_DOOM*/

};

/*

Ships:

Lacewing - pointy: fighter
Aether Squid - spiny: multifire
Lentil - rounded: heavy bomber
Rumsfeld's Delight - triangle: silly
Hookworm - small round wing: many homing missiles
Tortfeasor - 2 prongs: light bomber (+ slide)
False Scorpion: - bracketed: agile
Porkupine - large round wing: multi missiles

Despot - wide lacewing - just 2 homing missiles
Capybara - blobby - medium bomber
Prong - pointy2 - super cannons

Godbotherer - fat triangle - heavy fighter - multi, autof, heavy cannons, tubes?
Botulus - backwards curve - fast rocketed missiles - rocket, loader, multi, power,
Slidewinder - three triangles - fast seeker missiles - seek, loader, auto, slide
Doom Fork - two prongs forwards - drifter - auto, power, rocket, whead
 
*/

/*
Aether Squid
Lentil
*/

enum
{
PICKUP_NONE,
PICKUP_REPAIR,
PICKUP_GRACE,
PICKUP_SHIP,
PICKUP_SQUARE,
PICKUP_CIRCLE,
PICKUP_TRIANGLE,
PICKUP_PRESYMBOL,
PICKUP_SECONDARY,
PICKUP_PRESECONDARY
};

enum
{
SYMBOL_SQUARE,
SYMBOL_CIRCLE,
SYMBOL_TRIANGLE,
SYMBOL_CROSS
};

#define SYMBOL_TIMEOUT 64

enum
{
ABILITY_PRIMARY,
ABILITY_DRIVE,
ABILITY_SECONDARY,
ABILITY_DEFENCE
};

enum
{
SUBTYPE_NONE, // for creepy-crawlies
SUBTYPE_GREEN,
SUBTYPE_YELLOW,
SUBTYPE_ORANGE,
SUBTYPE_RED,
SUBTYPE_BLUE,
SUBTYPE_PURPLE,
SUBTYPE_WHITE
};


enum
{
FIGHTER_ATTACK,
FIGHTER_MOVE,
BEE_RESTING
};

enum
{
BULLET_NONE,
BULLET_ZAP,
BULLET_POWERED,
BULLET_MISSILE,
BULLET_MISSILE_MINI,
BULLET_BOMB,
BULLET_BOMB_SIDE,
BULLET_BOMBLET, // also used for cluster missiles
BULLET_STING,
BULLET_HOSTILE,
BULLET_PREMINE,
BULLET_MINE,
BULLET_BLUE_BLOB,
BULLET_RED_BLOB,
BULLET_YELLOW_BLOB,
BULLET_ORANGE_BLOB,
BULLET_SEEKER_BLOB,
BULLET_PLASMA,
BULLET_PRESEEKMINE,
BULLET_SEEKMINE,
BULLET_SEEKER_BLOB2,
BULLET_LINES,
BULLET_CURVE,
BULLET_PRONG,
BULLET_ORBITAL,
BULLET_FLAK,
BULLET_SEEKER_BLOB3,
BULLET_ZZZ_BOLT,
BULLET_TORPEDO,
BULLET_SUPERSTING,
BULLET_NICE_ORBITAL,
BULLET_STING2,
BULLET_PURPLE_BLOB,
BULLET_BOSS4,
BULLET_TORPEDO2,

BULLET_BALL1,
BULLET_BALL2,

BULLET_WOODEN_DART,
BULLET_ICE_DART,
BULLET_SILVER_TOOTH,
BULLET_SNOW_DART,
BULLET_FROZEN_BREATH,
BULLET_SHOCK_PATH,
BULLET_BRASS_TOOTH,
BULLET_CRYSTAL_TOOTH,
BULLET_GOLDEN_NEEDLE,
BULLET_PARTICLE_SPITTER,
BULLET_CRYSTAL_SPINE,
BULLET_GOLDEN_PATH,
BULLET_NUMEROUS_DART,
BULLET_NUMEROUS_BLADE,
BULLET_FAR_SPITTER,
//BULLET_ICE_TOOTH,
BULLET_BURNING_SPIRIT,

BULLET_WOODEN_DART_SMALL,
BULLET_ICE_DART_SMALL,
BULLET_SILVER_TOOTH_SMALL,
BULLET_SNOW_DART_SMALL,
BULLET_BRASS_TOOTH_SMALL,
BULLET_SHOCK_PATH_SMALL,
BULLET_GOLDEN_PATH_SMALL,
BULLET_GOLDEN_NEEDLE_SMALL,

BULLET_FURIOUS_ORB,
BULLET_BURNING_EYE,
BULLET_LARVA_NEST,
BULLET_MANIFOLD_ORB,
BULLET_EYE_DESOLATION,
BULLET_TERROR_EEL,
BULLET_PANIC_EEL,
BULLET_WORM_SORROW,
BULLET_WORM_AGONY,
BULLET_FROZEN_STAR,
BULLET_FROZEN_TOOTH,
BULLET_ORB_RECTIFICATION,

BULLET_FURIOUS_ORB_SMALL,
BULLET_BURNING_EYE_SMALL,
BULLET_EYE_DESOLATION_SMALL,
BULLET_LARVA_NEST_SMALL,
BULLET_MANIFOLD_ORB_SMALL,

BULLET_GREEN_BLAT,
BULLET_EVIL_EEL,
BULLET_YELLOW_PULSE,
BULLET_CIRCLES,
BULLET_SHOCK,
BULLET_SQUIRMY,
BULLET_BURST,
BULLET_BURNING_DRAGGED,
BULLET_SEEKER1,
BULLET_SEEKER2,
BULLET_SEEKER3,
BULLET_SEEKER4,
BULLET_BLUE_BLAT,
BULLET_BLAST,
BULLET_E_BOMB,
BULLET_E_BOMB2,
BULLET_TWIRLY1,
BULLET_TWIRLY2,
BULLET_WINGS1,
BULLET_WINGS2,
BULLET_WINGS3,
BULLET_BLOCKS,
BULLET_THICK_SHOCK,
BULLET_WORM_BOMB,
BULLET_CIRCLES2,
BULLET_EVIL_WORM,
BULLET_FORK1,
BULLET_FORK2,
BULLET_FORK3,
BULLET_MINE1,
BULLET_MINE2,
BULLET_MINE3,
BULLET_MINE4,
BULLET_BOLT,
BULLET_CIRCLER,
BULLET_YELLOW_BLAT,
BULLET_BFLAK,
BULLET_YELLOW_FLAK,
BULLET_PUFFY3,
BULLET_PUFFY4,
BULLET_EVIL_STAR,
BULLET_PULSE1,
BULLET_PULSE2,
BULLET_TOXIC_SUN,
BULLET_TOXIC_FLARE,

BULLET_FLOWER,
BULLET_PETAL1,
BULLET_PETAL2,
BULLET_BIGBALL1,
BULLET_BIGSEEKER,
BULLET_BIGWINGS1,
BULLET_BIGWINGS2,
BULLET_BIGWINGS3,
BULLET_BIGCIRCLES,
BULLET_ORBIT,
BULLET_ATTRACTOR_LINE,
BULLET_DISRUPT1,
BULLET_DISRUPT1_DROP,
BULLET_DISRUPT2,
BULLET_DISRUPT3,
BULLET_SPORE,
BULLET_FLOWER2,
BULLET_TRI1,
BULLET_TRI2,
BULLET_TRI3,
BULLET_OVERTRI,
BULLET_CLAW,
BULLET_TURN_WORM,
BULLET_ZAP_DRAG,
BULLET_JET,
BULLET_TWISTY,
BULLET_TWIRLY3,
BULLET_CHARGE_LINE,
BULLET_CHARGE,
BULLET_OVERPULSE,
BULLET_SLIVER,
BULLET_NOVA,
BULLET_HOLE,
BULLET_HOLE_LINE,
BULLET_ZIGZAG1,
BULLET_MBOMB,
BULLET_CRYSTAL1,
BULLET_CRYSTAL2,
BULLET_SWIRL1,
BULLET_SWIRL2,
BULLET_BFLAK2,
BULLET_CURVEY,
BULLET_LINE_PULSE,
BULLET_OVERBLOCKS,
BULLET_ZIGZAG2,
BULLET_FLAME
};

enum
{
WEAPON_WOODEN_DARTS,
WEAPON_SNOW_DARTS,
WEAPON_SILVER_TEETH,
WEAPON_ICE_DARTS,
WEAPON_FROZEN_BREATH,
WEAPON_BRASS_TEETH,
WEAPON_SHOCK_PATH,
WEAPON_CRYSTAL_TEETH,
WEAPON_GOLDEN_NEEDLES,
WEAPON_PARTICLE_SPITTER,
WEAPON_CRYSTAL_SPINES,
WEAPON_GOLDEN_PATH,
WEAPON_NUMEROUS_DARTS,
WEAPON_NUMEROUS_BLADES,
WEAPON_FAR_SPITTER,
WEAPON_BURNING_SPIRIT
};

enum
{
WPN_DARTS, // like wooden darts
WPN_BURST, // like numerous darts (all equal)
WPN_SPINES, // high-speed
WPN_TEETH // long-range

};

enum
{
SECOND_NONE,
SECOND_FURIOUS_ORB,
SECOND_BURNING_EYE,
SECOND_LARVA_NEST,
SECOND_MANIFOLD_ORB,
SECOND_EYE_DESOLATION,
SECOND_TERROR_EELS,
SECOND_PANIC_EELS,
SECOND_WORMS_SORROW,
SECOND_WORMS_AGONY,
SECOND_FROZEN_STARS,
SECOND_FROZEN_TEETH,
SECOND_TOXIC_SUN,
SECOND_FLOWER,
SECOND_SPORES,
SECOND_CLAWS,
TOTAL_SECONDS
};


enum
{
ENEMY_NONE,
ENEMY_GUARDIAN1,
ENEMY_PUFFER1,
ENEMY_BRACKET1,
ENEMY_WORMER1,
ENEMY_HEAD1,
ENEMY_HEAD1_EYE1,
ENEMY_HEAD1_EYE2,
ENEMY_HEAD1_EYE3,
ENEMY_PUFFER2,
ENEMY_SPINNER1,
ENEMY_SPINNER2,
ENEMY_SPINNER3,
ENEMY_SPINNER4,
ENEMY_SPINNER5,
ENEMY_BRACKET2,
ENEMY_BRACKET3,
ENEMY_BRACKET4,
ENEMY_BRACKET4_TURRET,
ENEMY_FIGHTER1,
ENEMY_FIGHTER2,
ENEMY_FIGHTER3,
ENEMY_FIGHTER4,
ENEMY_FIGHTER5,
ENEMY_BOMBER1,
ENEMY_BOMBER2,
ENEMY_BOMBER3,
ENEMY_CRUISER1,
ENEMY_CRUISER1_TURRET,
ENEMY_GUARDIAN2,
ENEMY_GUARDIAN3,
ENEMY_SPIKEY1,
ENEMY_SPIKEY2,
ENEMY_SPIKEY3,
ENEMY_SPIKEY4,
ENEMY_SPIKEY5,
ENEMY_WORMER2,
ENEMY_WORMER3,
ENEMY_FORKER1,
ENEMY_FORKER2,
ENEMY_MINER1,
ENEMY_MINER2,
ENEMY_MINER3,
ENEMY_MINER3_TURRET,
ENEMY_CRUISER2,
ENEMY_CRUISER2_TURRET,
ENEMY_CRUISER3,
ENEMY_CRUISER3_TURRET,
ENEMY_CRUISER4,
ENEMY_CRUISER4_TURRET,
ENEMY_BOSS1_1,
ENEMY_BOSS1_2,
ENEMY_BOSS1_3,
ENEMY_BOSS1_TURRET1,
ENEMY_BOSS1_TURRET2,
ENEMY_BOSS1_TURRET3,
ENEMY_CIRCLER1,
ENEMY_BLATTER1,
ENEMY_BLATTER2,
ENEMY_BLATTER3,
ENEMY_BLATTER4,
ENEMY_BLATTER5,
ENEMY_MINEFIELDER1,
ENEMY_BLOATER1,
ENEMY_BLOATER2,
ENEMY_BOSS2,
ENEMY_BOSS2_TURRET1,
ENEMY_BOSS2_TURRET2,
ENEMY_BOSS2_TURRET3,
ENEMY_BOSS2_TURRET4,
ENEMY_SHIELDER1,
ENEMY_PUFFER3,
ENEMY_PUFFER4,
ENEMY_PULSER1,
ENEMY_PULSER2,
ENEMY_ZAPPER1,
ENEMY_MULTI1,
ENEMY_MULTI2,
ENEMY_MULTI3,
ENEMY_DEFENDER1,
ENEMY_DEFENDER1_TURRET1,
ENEMY_DEFENDER1_TURRET2,
ENEMY_DEFENDER1_TURRET3,
ENEMY_OVERSPINNER,
ENEMY_OVERSPIKEY,
ENEMY_UNDERSPIKEY,
ENEMY_OVERBLATTER,
ENEMY_DEFENDER2,
ENEMY_DEFENDER2_TURRET1,
ENEMY_DEFENDER2_TURRET2,
ENEMY_DEFENDER2_TURRET3,
ENEMY_DEFENDER3,
ENEMY_DEFENDER3_TURRET1,
ENEMY_DEFENDER3_TURRET2,
ENEMY_DEFENDER3_TURRET3,
ENEMY_DEFENDER3_TURRET4,
ENEMY_DEFENDER3_TURRET5,
ENEMY_DEFENDER3_TURRET6,
ENEMY_ORBITER1,
ENEMY_ORBITER2,
ENEMY_ORBITER3,
ENEMY_ATTRACTOR,
ENEMY_DISRUPTER1,
ENEMY_DISRUPTER2,
ENEMY_DISRUPTER3,
ENEMY_TRIANGLER1,
ENEMY_TRIANGLER2,
ENEMY_TRIANGLER3,
ENEMY_OVERTRIANGLER,
ENEMY_OVERTRIANGLER_TURRET,
ENEMY_LEAPER1,
ENEMY_LEAPER2,
ENEMY_WORMER4,
ENEMY_BEAMER1,
ENEMY_BEAMER2,
ENEMY_OVERBLATTER2,
ENEMY_OVERDISRUPTER,
ENEMY_GUARDIAN4,
ENEMY_GUARDIAN5,
ENEMY_OVERZAPPER,
ENEMY_BRACKET5,
ENEMY_BOSS2_2,
ENEMY_BOSS2_3,
ENEMY_BOSS3_1,
ENEMY_BOSS3_2,
ENEMY_BOSS3_3,
ENEMY_MESSENGER,
ENEMY_SHADOW1,
ENEMY_SHADOW2,
ENEMY_BOSS3_TURRET1,
ENEMY_BOSS3_TURRET2,
ENEMY_BOSS3_TURRET3,
ENEMY_WAVER1,
ENEMY_WAVER2,
ENEMY_CURVE1,
ENEMY_CURVE2,
ENEMY_CURVE3,
ENEMY_UNDERSPIKEY2,
ENEMY_UNDERSPIKEY3,
ENEMY_OVERSPIKEY2,
ENEMY_DEAD_TRI1,
ENEMY_DEAD_TRI2,
ENEMY_DEAD_TRI3,
/*
ENEMY_BOSS3,
ENEMY_BOSS3_TURRET1,
ENEMY_BOSS3_TURRET2,
ENEMY_BOSS3_TURRET3,
ENEMY_BOSS3_TURRET4,*/
NO_ENEMY_TYPES
};
// can't have entries above without eclass entries or the whole world will collapse

enum
{
AI_NONE,
AI_DRIFTER, // chooses a random location and goes there, then chooses another. etc
AI_STINGER, // darts randomly around
AI_PERIODIC_STINGER, // darts around at specified intervals
AI_TURRET, // just sits there and rotates
AI_FIGHTER, // turns to face target and flies by, then chooses a random dest, then returns
AI_BOMBER, // a lot like fighter
AI_CRUISER, // like drifter, but turns to face direction moved
AI_AUTO, // orbitals
AI_LEAPER, // cross between fighter and periodic stinger
AI_FOLLOWER, // like drifter, but destination is player's location at the time
AI_BEAMER // drifter, but special firing code
};

enum
{
ROLE_TARGET,
ROLE_TURRET,
ROLE_EXTRA,
ROLE_DEAD,
ROLE_BOSS,
ROLE_MINIBOSS,
};

enum
{
WPN_NONE,
WPN_ZAPPER
};

enum
{
SYST_NONE,
SYST_WPN_ZAPPER
};

enum
{
BRAKES_NONE,
BRAKES_RETRO,
BRAKES_DRAG
};

enum
{
SHIELD_NONE,
SHIELD_LOWFREQ,
SHIELD_MEDIUMFREQ,
SHIELD_HIGHFREQ
};

enum
{
SLIDE_NO,
SLIDE_YES
};

enum
{
CLOUD_NONE,
CLOUD_SPECK,
CLOUD_TRAIL1,
CLOUD_SMALL_CIRCLE,
CLOUD_SMALL_TRANS_CIRCLE,
CLOUD_MED_CIRCLE,
CLOUD_MED_TRANS_CIRCLE,
CLOUD_LARGE_CIRCLE,
CLOUD_LARGE_TRANS_CIRCLE,
CLOUD_FADING_LINE,
CLOUD_SHRINKING_LINE,
CLOUD_SHOCKWAVE,
CLOUD_BURSTLET,
CLOUD_GROWING_CIRCLE,
CLOUD_LINE_SHADOW,
CLOUD_SHRINKING_CIRCLE,
CLOUD_SPAWNER,
CLOUD_LIGHT,
CLOUD_BANG,
CLOUD_BLUE_BLOB,
CLOUD_BLUE_BLOB2,
CLOUD_FLECK,
CLOUD_SMALL_FADING_CIRCLE,
CLOUD_ORANGE_BLOB,
CLOUD_ORANGE_BLOB2,
CLOUD_GREEN_BLOB,
CLOUD_GREEN_BLOB2,
CLOUD_LARGE_FADING_CIRCLE,
CLOUD_TRANS_FADING_CIRCLE,
CLOUD_BLAST_CIRCLE,
CLOUD_MED_TRANS_FADING_CIRCLE,
CLOUD_SMALL_SHRINKING_CIRCLE,
CLOUD_BLOCK1,
CLOUD_BLOCK2,
CLOUD_BLOCK3,
CLOUD_BLOCK4,
CLOUD_THICK_SHOCK_CIRCLE,
CLOUD_CIRCLER_IN,
CLOUD_FLAK_BURST,
CLOUD_SHIELD_LINE,
CLOUD_SQUAREY,
CLOUD_PULSER1_V,
CLOUD_PULSER1_H,
CLOUD_PULSER2_V,
CLOUD_PULSER2_H,
CLOUD_SEEKER_CIRCLE,
CLOUD_ATTRACT,
CLOUD_TRANS_FADING_LINE,
CLOUD_DRIVE_CIRCLE,
CLOUD_LARGE_TRANS_FADING_CIRCLE,
CLOUD_BIG_BLAST_CIRCLE,
CLOUD_DISTORT,
CLOUD_JET_CIRCLE1,
CLOUD_JET_CIRCLE2,
CLOUD_TWISTY_CIRCLE,
CLOUD_TWIRLY_CIRCLE,
CLOUD_SHATTER1,
CLOUD_SHATTER2,
CLOUD_BLOCK5,
CLOUD_TRI1, // not used
CLOUD_TRI2,
CLOUD_TRI3,
CLOUD_OVERTRI, // to here.
CLOUD_FLAME_CIRCLE,
CLOUD_SHRINKING_FADING_CIRCLE


//CLOUD_BLUE_BLOB,
//CLOUD_BLUE_BLOB2



};

enum
{
BANG_NONE,
BANG_RED,
BANG_ORANGE,
BANG_YELLOW,
BANG_BLUE,
BANG_GREEN,
BANG_GREY
};

enum
{
UPG_NONE,
UPG_SPEED,
// increases speed + turning
// special: aerodynamics (may not be a benefit, really)
UPG_ARMOUR,
// increases armour
// special: shield, then increase shield
UPG_POWER,
// increases cannon damage
// special: plasma bolt
UPG_AUTOFIRE,
// increases cannon fire rate + scatter
// special: sidekick
UPG_MULTIFIRE,
// increases: cannon shot spread
// special: backwards fire
UPG_PROJECT,
// increases cannon speed + range
// special: lightning bolt
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
UPG_SLIDE,
// gives, then increases, slide rockets
// special: retro rockets (replaces drag field)
UPG_BOMB,
// increases bomb damage
// special: dispersion (explosion size)
UPG_LAUNCHER,
// adds more bombs (side-bombs are smaller)
// special: cluster bombs (main one only?)
UPG_LOADER,
// increases missile firing rate

UPG_SPECIAL,

UPG_TURRET,

UPG_SIDEKICK,

UPG_HEAVY,

UPG_ORBITAL,

UPG_BACKFIRE,

UPG_SHIELD,

UPG_RETRO,

UPG_ROAMER,

UPG_MINIMISSILES,

UPG_DRIFT,

/*
REMEMBER: When adding something which upgrades a secondary weapon,
must make sure that the secondary weapon is switched on when the upgrade
is gained.
*/

MAX_UPGRADES
};


enum
{
DRIVE_THRUST,
DRIVE_SLIDE,
DRIVE_RETRO

};

enum
{
DUEL_10_POINTS,
DUEL_30_POINTS,
DUEL_3_MINUTES,
DUEL_10_MINUTES
};

#define LOCK_ACTOR0 -3
#define LOCK_ACTOR1 -2

/*
enum
{
SPECIAL_SHIELD,
SPECIAL_
};
*/
struct optionstruct
{
 int sound_init; // if 0, sound isn't initialised at all. Changed in config file only.
 int sound_mode; // mono, stereo, reversed, off
 int run_vsync; // on or off
 int sound_volume; // sound fx volume; if 0 sound effects not played
 int ambience_volume; // if 0 ambience not played
 int resolution;
 int unlock_purple;
 int unlock_void;
 int unlock_god;
 int colour_text;
};

struct starstruct
{
 int colour [2];
 int x [2];
 int y [2];
 char distance [2];

};

struct gamestruct
{
 int users;
 int type;
 int duel_mode;
 int duel_crawlies;
 int duel_handicap [2];
 int duel_level;
 int duel_leader_handicap;
 int duel_winner;
 int symbols_given; // ta and duel
 int ta_enemies [15];
 int ta_extra_list [15];
 int ta_symbol_count;
 long ta_total_time; // probably doesn't need to be long
 int ta_level;
 int ta_enemy_index;
 int single_player;
 int ships_left;
 float drag;
 int mode_purple;
 int mode_void;
 int mode_god;
};

struct arenastruct
{
  int max_x;
  int max_y;
  int level;
  int colour1;
  int colour2;
  int colour3;
  int next_target;
//  int target [3];
//  int targets_left [3];
  
  int enemy_list [50];
  int symbol_list [50];
  int list_index;
  int symbol_index;
  int total_targets;
  int targets_left_total;
  int next_target_upgrade;
  int max_targets_in_level;
  int battle_type;

  int next_non_target;
  int max_non_targets_in_level;

  int time_left;
  int level_finished;
  int seconds_left_when_finished;
  int time_bonus;
  int game_over;
  int new_level_sign;
  
  int max_crawlies_in_level;
  int next_crawly;
  int next_crawly_upgrade;
  int crawlies_created;

  int between_target_upgrades;
  int between_crawly_upgrades;

  int hostile;
  
  int eye_x [MAX_EYES];
  int eye_y [MAX_EYES];
  int eye_recycle [MAX_EYES];
  int eye_angle [MAX_EYES];
  int eye_colour1;
  int eye_colour2;
  int eye_colour3;
  int eyes_on_level;
  
  int waver_on_level;
};

#define ENEMY_LIST_END -1
#define ENEMY_LIST_WAIT -2
#define ENEMY_LIST_EMPTY -3

enum
{
BATTLE_NORMAL,
BATTLE_BOSS,
BATTLE_VARIOUS
};

struct playerstruct
{
 int actor_controlled;
 int user;
// int ships_left;
 char player_cmd [NO_CMDS];
 char link_fire;
 int link_toggle_delay;

 int ship;
 int score;

 int duel_score;

 int keys [NO_CMDS];
 
 int screen_shake_x;
 int screen_shake_y;
// _time is in actor 
};


struct actorstruct
{
 int x, y, x_speed, y_speed, angle, base_angle;
 int moving_angle; // the angle at which it's moving. Set in drag_actor,
 // so may not be very reliable but is good enough for friction burn and
 // ai routines etc.

 int type;
 int ship;
 int controller;
 char actor_cmd [NO_CMDS];
 int drag_amount; // amount that actor is accelerated by drag
 int radius;
 int edge_radius;
 char in_play;

 int turning;
 int thrust;
 int slide;
 int mass;
 int recharge;

 int armour;
 int max_armour;

 int upgrade_slot;
 int upgrades [NO_UPGRADES]; // referred to by slot
 int upgrade_specials [NO_UPGRADES];
 int upgrade_structure [NO_UPGRADES];
 int upgraded_system [MAX_UPGRADES]; // referred to by UPG_ index
 int just_upgraded;
 int just_upgraded_timeout;
 int shield_pulse;
 int recycle1;
 int recycle2;
 int cannon_status;
 int bomb_status;
 int shield;
 int max_shield;
 int shield_recharge;
/* int shield_type;
 int shield_strength1;
 int shield_strength2;
 int shield_pulse;
 int shield_angle;
 int system [3];
 int system_status1 [3];
 int system_status2 [3];
 int max_energy;
 int energy;*/
 int brakes;
 int brake_strength;
 char dragging; // is 1 if drag field active
 int just_collided;
 int spawn_delay;
 int grace_period;
 int repairing;
 int secondary_weapon;

 int seek_x;
 int seek_y;
 int lock;
 int new_lock;
 
 int sidekick_x [5];
 int sidekick_y [5];
 int sidekick_x_speed [5];
 int sidekick_y_speed [5];
 int sidekick_angle [5];
 int sidekick_recycle;
 int sidekicks;

 int turret_lock;
 int turret_recycle;

 int heavy_recycle;

 int backfire_recycle;

 int exhaust_distance_x;
 int exhaust_distance_y;
// int exhaust_displacement;
 int retro_distance_x;
 int retro_distance_y;
// int retro_displacement;
 int slide_distance_x;
 int slide_distance_y;
// int slide_displacement;
 int flash_distance;

 int engine_demand;

 int orbital_angle;
 int orbital_spin;

 int hurt_pulse;

 int drive_sound [3];
 
 int ability [4] [4];
 
 int primary;
 int secondary;
 
 int secondary_burst;
 int secondary_burst_recycle;
 
 int screen_shake_time; 
// shake_x/_y are in player

 int total_power;
 int next_spawn_x;
 int next_spawn_y;
};

enum
{
PRIMARY_WOODEN_DARTS,
PRIMARY_ICE_DARTS,
PRIMARY_SILVER_TEETH,
PRIMARY_SNOW_DARTS,
PRIMARY_FROZEN_BREATH,
PRIMARY_BRONZE_DARTS,
PRIMARY_BRASS_TEETH,
PRIMARY_CRYSTAL_TEETH,
PRIMARY_GOLDEN_NEEDLES,
PRIMARY_PARTICLE_SPIT,
PRIMARY_CRYSTAL_SPINES,
PRIMARY_GOLDEN_TEETH
};

struct bulletstruct
{
 int x, y, x_speed, y_speed;
 int x2, y2;
 int type;
 int owner;
 int timeout;
 int size;
 int damage;
 int left_owner;
 int mass;
 int angle;
 int colours [4];
 int special1;
 // missiles: acceleration
 // bombs: explosion size
 // enemy seeker blobs: actor target
 // enemy prong: spin direction (applied in display.c)
 int special2;
 // missiles: number of clusters
 // bombs: number of clusters
 int special3;
 // missiles: seeker target
 int special4;
 // missiles: seeker turn rate
 int special5;
 // missiles: seeker guidance system

};


struct cloudstruct
{
 int x, y, x_speed, y_speed;
 int x2, y2;
 int type;
 unsigned char seed;
 int angle;
 int timeout;
 int tickrate;
 int delta_tickrate;
 int status;
 int colour [5];
};

struct pickupstruct
{
 int x, y, x_speed, y_speed;
 int counter;
 int type;
 int subtype;
 int timeout;
 int radius;
};

struct enemystruct
{

 int x, y, x_speed, y_speed, angle, base_angle;
 int moving_angle;
 int type;
 int subtype;
 int attacking; // which actor is it attacking (-1 for none)
 int drag_amount; // amount that enemy is accelerated by drag
 char drag_affected; // is it affected by drag?
 int radius;
 int edge_radius;
 int recycle;
// char in_play;
 int armour;
 int hurt_pulse;
 int shield;
 int max_shield;
 int attribute [NO_ENEMY_ATTRIBUTES];
 int colours [3];
 int hurt_pulse_colour1;
 int hurt_pulse_colour2;
 unsigned char counter;
 int just_collided;
 int mass;
 int carrying_pickup;
 int burst_fire;
 int target;
 int turret [MAX_TURRETS];
 int turret_main; // whose turret it it?
 int turret_index; // which turret is it?
 int x_dest;
 int y_dest;
 int slew_dir;
 int next_impulse;
 int ta_time_left; // time attack - time before it warps out
};

struct enemy_classification
{
 int max_armour;
 int level;
 int common;
 int score;
 int radius; // for hit calculation
 int edge_radius; // for edge-of-map collision
 float drag_amount;
 int mass;
// int default_attributes [NO_ENEMY_ATTRIBUTES];
 int ai_type;
 int generous;
 int colour1;
 int colour2;
 int turrets;
 int pitch; // sound effects - hit and explodes
 int acceleration;
 int impulse_delay;
 int range;
 int slew;
 int role;
};



enum
{
PITCH_GUARDIAN,
PITCH_FIGHTER,
PITCH_SMALL_SPINNER,
PITCH_LARGE_SPINNER,
PITCH_HEAVY_SPINNER,
PITCH_BOSS1,
PITCH_BOSS2,
PITCH_BOSS3,
PITCH_MINIBOSS1,
PITCH_MINIBOSS2,
PITCH_TURRET1,
PITCH_TURRET2,
PITCH_BRACKET,
PITCH_MEDIUM,
PITCH_HEAVY
};

