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
    
File: eclass.c
History:
11/9/2005 - Version 1.0 finalised

This file contains:
 - the enemy structure

*/

#include "config.h"

#include "globvars.h"
#include "palette.h"

#define HIGH_DRAG 0.06
#define MED_DRAG 0.03
#define LOW_DRAG 0.01
#define VERY_LOW_DRAG 0.005

#define GEN_LOW 3
#define GEN_MED 5
#define GEN_HIGH 8

struct enemy_classification eclass [NO_ENEMY_TYPES] =
{
{
0, // int max_armour;
0, // level
0, // common
0, // score
0, // int radius;
0, // int edge_radius;
0, // drag
0, // mass
AI_NONE, // ai_type
0, // acceleration
0, // colour1
0, // colour2
0, // turrets
0, // attributes
0, // generous
0, // range
0 // slew
},
{ // guardian1
350, // int max_armour;
1, // level
3, // common
10, // score
19000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_GUARDIAN, // attributes
30, // acceleration
0, // impulse delay
900000, // range
3, // slew
ROLE_TARGET
},
{ // puffer
300, // int max_armour;
1, // level
3, // common
20, // score
15000, // int radius;
23000, // int edge_radius;
LOW_DRAG, // drag
150, // mass
AI_PERIODIC_STINGER, // AI
GEN_LOW, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
2500, // acceleration
99, // impulse delay
900000, // range
5, // slew
ROLE_TARGET
},
{ // Bracket1
500, // int max_armour;
5, // level
2, // common
30, // score
11000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
250, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_BRACKET, // pitch
30, // acceleration
0, // impulse delay
900000, // range
5, // slew
ROLE_TARGET
},
{ // wormer1
500, // int max_armour;
8, // level
2, // common
100, // score
10000, // int radius;
23000, // int edge_radius;
LOW_DRAG, // drag
400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
15, // acceleration
99, // impulse delay
900000, // range
0, // slew
ROLE_TARGET
},
{ // head1
800, // int max_armour;
4, // level
0, // common
100, // score
13000, // int radius;
81000, // int edge_radius;
HIGH_DRAG, // drag
2400, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
2, // turrets
PITCH_MINIBOSS1, // pitch
35, // acceleration
99, // impulse delay
900000, // range
4, // slew
ROLE_MINIBOSS
},
{ // head1_eye1
300, // int max_armour;
0, // level
0, // common
100, // score
8000, // int radius;
0, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
900000, // range
7, // slew
ROLE_TURRET
},
{ // head1_eye2
300, // int max_armour;
0, // level
0, // common
100, // score
8000, // int radius;
0, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
900000, // range
7, // slew
ROLE_TURRET
},
{ // head1_eye3
300, // int max_armour;
0, // level
0, // common
100, // score
8000, // int radius;
0, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
900000, // range
7, // slew
ROLE_TURRET
},
{ // puffer2
600, // int max_armour;
6, // level
2, // common
200, // score
18000, // int radius;
27000, // int edge_radius;
LOW_DRAG, // drag
300, // mass
AI_PERIODIC_STINGER, // AI
GEN_LOW, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
2500, // acceleration
99, // impulse delay
900000, // range
2, // slew
ROLE_TARGET
},
{ // spinner1
350, // int max_armour;
4, // level
3, // common
200, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
1, // slew
ROLE_EXTRA
},
// NOTE: Spinners have same stats as spikeys
{ // spinner2
500, // int max_armour;
8, // level
3, // common
400, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
1, // slew
ROLE_EXTRA
},
{ // spinner3
600, // int max_armour;
10, // level
3, // common
800, // score
15000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
1, // slew
ROLE_EXTRA
},
{ // spinner4
700, // int max_armour;
12, // level
3, // common
1200, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_RED6, // col1
COLOUR_RED2, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
1, // slew
ROLE_EXTRA
},
{ // spinner5
800, // int max_armour;
14, // level
3, // common
1500, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_GREY5, // col1
COLOUR_GREY2, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
16, // slew
ROLE_EXTRA
},
{ // Bracket2
800, // int max_armour;
7, // level
2, // common
500, // score
10000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
350, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_BRACKET, // pitch
30, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // Bracket3
900, // int max_armour;
10, // level
2, // common
800, // score
11000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
350, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_BRACKET, // pitch
30, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // Bracket4
900, // int max_armour;
12, // level
2, // common
800, // score
10000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
350, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
2, // turrets
PITCH_BRACKET, // pitch
15, // acceleration
0, // impulse delay
1900000, // range
4, // slew
ROLE_TARGET
},
{ // Bracket4 turret
400, // int max_armour;
12, // level
0, // common
200, // score
7000, // int radius;
10000, // int edge_radius;
MED_DRAG, // drag
100, // mass
AI_TURRET, // AI
0, // generous
COLOUR_BLUE5, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
900000, // range
2, // slew
ROLE_TURRET
},
{ // Fighter1
200, // int max_armour;
2, // level
2, // common
50, // score
8000, // int radius;
10000, // int edge_radius;
LOW_DRAG, // drag
150, // mass
AI_FIGHTER, // AI
GEN_LOW, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
50, // acceleration
0, // impulse delay
500000, // range
16, // slew
ROLE_EXTRA
},
{ // Fighter2 note fighter2 is higher-level than 3
300, // int max_armour;
8, // level
2, // common
50, // score
8000, // int radius;
10000, // int edge_radius;
LOW_DRAG, // drag
150, // mass
AI_FIGHTER, // AI
GEN_LOW, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
60, // acceleration
0, // impulse delay
500000, // range
16, // slew
ROLE_EXTRA
},
{ // Fighter3
250, // int max_armour;
5, // level
2, // common
50, // score
8000, // int radius;
10000, // int edge_radius;
LOW_DRAG, // drag
150, // mass
AI_FIGHTER, // AI
GEN_LOW, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
55, // acceleration
0, // impulse delay
500000, // range
16, // slew
ROLE_EXTRA
},
{ // Fighter4
400, // int max_armour;
10, // level
2, // common
50, // score
8000, // int radius;
10000, // int edge_radius;
LOW_DRAG, // drag
150, // mass
AI_FIGHTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
70, // acceleration
0, // impulse delay
500000, // range
18, // slew
ROLE_EXTRA
},
{ // Fighter5
450, // int max_armour;
13, // level
2, // common
50, // score
8000, // int radius;
10000, // int edge_radius;
LOW_DRAG, // drag
150, // mass
AI_FIGHTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
75, // acceleration
0, // impulse delay
500000, // range
20, // slew
ROLE_EXTRA
},
{ // Bomber1
400, // int max_armour;
4, // level
2, // common
150, // score
10000, // int radius;
12000, // int edge_radius;
MED_DRAG, // drag
250, // mass
AI_BOMBER, // AI
GEN_MED, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
80, // acceleration
0, // impulse delay
500000, // range
8, // slew
ROLE_EXTRA
},
{ // Bomber2
500, // int max_armour;
8, // level
2, // common
150, // score
10000, // int radius;
12000, // int edge_radius;
MED_DRAG, // drag
250, // mass
AI_BOMBER, // AI
GEN_MED, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
100, // acceleration
0, // impulse delay
500000, // range
8, // slew
ROLE_EXTRA
},
{ // Bomber3
600, // int max_armour;
12, // level
2, // common
250, // score
10000, // int radius;
12000, // int edge_radius;
MED_DRAG, // drag
250, // mass
AI_BOMBER, // AI
GEN_MED, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
110, // acceleration
0, // impulse delay
500000, // range
8, // slew
ROLE_EXTRA
},
{ // Cruiser1
900, // int max_armour;
6, // level
2, // common
500, // score
19000, // int radius;
17000, // int edge_radius;
MED_DRAG, // drag
500, // mass
AI_CRUISER, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
1, // turrets
PITCH_GUARDIAN, // pitch
80, // acceleration
0, // impulse delay
400000, // range
5, // slew
ROLE_TARGET
},
{ // cruiser1 turret
400, // int max_armour;
6, // level
0, // common
100, // score
9000, // int radius;
12000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
400000, // range
7, // slew
ROLE_TURRET
},
{ // guardian2
600, // int max_armour;
7, // level
3, // common
150, // score
19000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
35, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // guardian3
800, // int max_armour;
10, // level
3, // common
500, // score
19000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
37, // acceleration
0, // impulse delay
900000, // range
3, // slew
ROLE_TARGET
},
{ // spikey1
300, // int max_armour;
3, // level
3, // common
200, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
6, // slew
ROLE_EXTRA
},
{ // spikey2
400, // int max_armour;
7, // level
3, // common
400, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
4, // slew
ROLE_EXTRA
},
{ // spikey3
500, // int max_armour;
9, // level
3, // common
800, // score
15000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
4, // slew
ROLE_EXTRA
},
{ // spikey4
600, // int max_armour;
12, // level
3, // common
1200, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_RED6, // col1
COLOUR_RED2, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
4, // slew
ROLE_EXTRA
},
{ // spikey5
750, // int max_armour;
14, // level
3, // common
1500, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE2, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
1, // slew
ROLE_EXTRA
},
{ // wormer2
800, // int max_armour;
10, // level
2, // common
300, // score
10000, // int radius;
23000, // int edge_radius;
LOW_DRAG, // drag
400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
18, // acceleration
99, // impulse delay
900000, // range
0, // slew
ROLE_TARGET
},
{ // wormer3
1000, // int max_armour;
13, // level
2, // common
600, // score
10000, // int radius;
23000, // int edge_radius;
LOW_DRAG, // drag
400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_GREY5, // col1
COLOUR_GREY2, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
15, // acceleration
99, // impulse delay
900000, // range
0, // slew
ROLE_TARGET
},
{ // forker1
600, // int max_armour;
9, // level
2, // common
1200, // score
13000, // int radius;
27000, // int edge_radius;
LOW_DRAG, // drag
400, // mass
AI_STINGER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
2200, // acceleration
40, // impulse delay
900000, // range
3, // slew
ROLE_TARGET
},
{ // forker2
800, // int max_armour;
13, // level
2, // common
1500, // score
13000, // int radius;
27000, // int edge_radius;
LOW_DRAG, // drag
500, // mass
AI_STINGER, // AI
GEN_LOW, // generous
COLOUR_GREY5, // col1
COLOUR_GREY1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
2000, // acceleration
35, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // miner1
450, // int max_armour;
5, // level
2, // common
20, // score
14000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_HEAVY, // pitch
50, // acceleration
0, // impulse delay
2900000, // range
0, // slew
ROLE_TARGET
},
{ // miner2
650, // int max_armour;
8, // level
2, // common
120, // score
14000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_HEAVY, // pitch
55, // acceleration
0, // impulse delay
2900000, // range
0, // slew
ROLE_TARGET
},
{ // miner3
800, // int max_armour;
11, // level
2, // common
450, // score
14000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
600, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
1, // turrets
PITCH_HEAVY, // pitch
50, // acceleration
0, // impulse delay
2900000, // range
0, // slew
ROLE_TARGET
},
{ // Miner3 turret
400, // int max_armour;
11, // level
0, // common
200, // score
7000, // int radius;
10000, // int edge_radius;
MED_DRAG, // drag
100, // mass
AI_TURRET, // AI
0, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
900000, // range
5, // slew
ROLE_TURRET
},
{ // Cruiser2
1000, // int max_armour;
9, // level
2, // common
1000, // score
19000, // int radius;
17000, // int edge_radius;
MED_DRAG, // drag
500, // mass
AI_CRUISER, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
1, // turrets
PITCH_GUARDIAN, // pitch
85, // acceleration
0, // impulse delay
400000, // range
6, // slew
ROLE_TARGET
},
{ // cruiser2 turret
350, // int max_armour;
9, // level
0, // common
150, // score
9000, // int radius;
12000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
400000, // range
6, // slew
ROLE_TURRET
},
{ // Cruiser3
1200, // int max_armour;
12, // level
2, // common
1300, // score
19000, // int radius;
17000, // int edge_radius;
MED_DRAG, // drag
500, // mass
AI_CRUISER, // AI
0, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
1, // turrets
PITCH_GUARDIAN, // pitch
80, // acceleration
0, // impulse delay
400000, // range
6, // slew
ROLE_TARGET
},
{ // cruiser3 turret
400, // int max_armour;
12, // level
0, // common
200, // score
9000, // int radius;
12000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
200000, // range
4, // slew
ROLE_TURRET
},
{ // Cruiser4
1300, // int max_armour;
15, // level
2, // common
1500, // score
19000, // int radius;
17000, // int edge_radius;
MED_DRAG, // drag
500, // mass
AI_CRUISER, // AI
0, // generous
COLOUR_GREY5, // col1
COLOUR_GREY1, // col2
1, // turrets
PITCH_GUARDIAN, // pitch
78, // acceleration
0, // impulse delay
400000, // range
6, // slew
ROLE_TARGET
},
{ // cruiser4 turret
550, // int max_armour;
15, // level
0, // common
250, // score
9000, // int radius;
12000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_GREY5, // col1
COLOUR_GREY1, // col2
0, // turrets
PITCH_TURRET1, // pitch
0, // acceleration
0, // impulse delay
400000, // range
3, // slew
ROLE_TURRET
},
{ // Boss1_1
2000, // int max_armour;
5, // level
10, // common
500, // score
55000, // int radius;
118000, // int edge_radius;
HIGH_DRAG, // drag
3000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
4, // turrets
PITCH_BOSS1, // pitch
35, // acceleration
99, // impulse delay
900000, // range
4, // slew
ROLE_BOSS
},
{ // Boss1_2
2000, // int max_armour;
5, // level
10, // common
500, // score
40000, // int radius;
168000, // int edge_radius;
HIGH_DRAG, // drag
3000, // mass
AI_STINGER, // AI
GEN_HIGH, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
3, // turrets
PITCH_BOSS1, // pitch
3800, // acceleration
60, // impulse delay
1900000, // range
4, // slew
ROLE_BOSS
},
{ // Boss1_3
2000, // int max_armour;
5, // level
10, // common
500, // score
40000, // int radius;
118000, // int edge_radius;
HIGH_DRAG, // drag
3000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
3, // turrets
PITCH_BOSS1, // pitch
43, // acceleration
99, // impulse delay
900000, // range
3, // slew
ROLE_BOSS
},
{ // Boss1_turret1
500, // int max_armour;
5, // level
0, // common
150, // score
15000, // int radius;
18000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
6, // slew
ROLE_TURRET
},
{ // Boss1_turret2
500, // int max_armour;
5, // level
0, // common
150, // score
15000, // int radius;
18000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
6, // slew
ROLE_TURRET
},
{ // Boss1_turret3
500, // int max_armour;
5, // level
0, // common
150, // score
15000, // int radius;
18000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
6, // slew
ROLE_TURRET
},
{ // circler1
1000, // int max_armour;
10, // level
2, // 2, // common
500, // score
10000, // int radius;
39000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_BRACKET, // pitch
37, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_TARGET
},
{ // blatter1
350, // int max_armour;
4, // level
3, // common
200, // score
16000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
300000, // range
8, // slew
ROLE_EXTRA
},
{ // blatter2
500, // int max_armour;
6, // level
3, // common
400, // score
16000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
300000, // range
6, // slew
ROLE_EXTRA
},
{ // blatter3
600, // int max_armour;
9, // level
3, // common
600, // score
16000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
300000, // range
3, // slew
ROLE_EXTRA
},
{ // blatter4
700, // int max_armour;
11, // level
3, // common
800, // score
16000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_RED6, // col1
COLOUR_RED2, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
300000, // range
8, // slew
ROLE_EXTRA
},
{ // blatter5
800, // int max_armour;
14, // level
3, // common
1000, // score
11000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE2, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
400000, // range
4, // slew
ROLE_EXTRA
},
{ // minefielder1
800, // int max_armour;
12, // level
5, // common
1000, // score
15000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
800, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_RED6, // col1
COLOUR_RED2, // col2
0, // turrets
PITCH_HEAVY_SPINNER, // pitch
0, // acceleration
0, // impulse delay
2900000, // range
3, // slew
ROLE_EXTRA
},
{ // bloater1
100, // int max_armour;
8, // level
2, // common
1000, // score
16000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
100, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE2, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
100, // range
0, // slew
ROLE_EXTRA
},
{ // bloater2
100, // int max_armour;
13, // level
2, // common
1000, // score
16000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
100, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE2, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
100, // range
0, // slew
ROLE_EXTRA
},
{ // Boss2
3000, // int max_armour;
10, // level
0, // common
500, // score
35000, // int radius;
125000, // int edge_radius;
HIGH_DRAG, // drag
5000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
4, // turrets
PITCH_BOSS2, // pitch
65, // acceleration
99, // impulse delay
1900000, // range
0, // slew
ROLE_BOSS
},
{ // Boss2_turret1
600, // int max_armour;
10, // level
0, // common
400, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
6, // slew
ROLE_TURRET
},
{ // Boss2_turret2
600, // int max_armour;
10, // level
0, // common
400, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TURRET
},
{ // Boss2_turret3
600, // int max_armour;
10, // level
0, // common
400, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TURRET
},
{ // Boss2_turret4
600, // int max_armour;
10, // level
0, // common
400, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TURRET
},
{ // shielder1
750, // int max_armour;
11, // level
3, // common
1000, // score
15000, // int radius;
25000, // int edge_radius;
MED_DRAG, // drag
500, // mass
AI_DRIFTER, // AI
GEN_MED, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE2, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
20, // acceleration
0, // impulse delay
1000000, // range
4, // slew
ROLE_EXTRA
},
{ // puffer3
900, // int max_armour;
11, // level
2, // common
800, // score
14000, // int radius;
24000, // int edge_radius;
LOW_DRAG, // drag
300, // mass
AI_PERIODIC_STINGER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
3300, // acceleration
66, // impulse delay
1400000, // range
2, // slew
ROLE_TARGET
},
{ // puffer4
1200, // int max_armour;
15, // level
2, // common
1300, // score
21000, // int radius;
29000, // int edge_radius;
LOW_DRAG, // drag
500, // mass
AI_PERIODIC_STINGER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
2600, // acceleration
66, // impulse delay
1400000, // range
2, // slew
ROLE_TARGET
},
{ // pulser1
800, // int max_armour;
6, // level
3, // common
90, // score
17000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
30, // acceleration
0, // impulse delay
9000000, // range
3, // slew
ROLE_TARGET
},
{ // pulser2
1200, // int max_armour;
11, // level
3, // common
500, // score
17000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
25, // acceleration
0, // impulse delay
9000000, // range
3, // slew
ROLE_TARGET
},
{ // zapper1
600, // int max_armour;
11, // level
3, // common
500, // score
13000, // int radius;
25000, // int edge_radius;
LOW_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
200000, // range
16, // slew
ROLE_EXTRA
},
{ // multi1
250, // int max_armour;
1, // level
3, // common
10, // score
15000, // int radius;
29000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
24, // acceleration
0, // impulse delay
900000, // range
2, // slew
ROLE_TARGET
},
{ // multi2
600, // int max_armour;
8, // level
3, // common
150, // score
13000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
30, // acceleration
0, // impulse delay
900000, // range
2, // slew
ROLE_TARGET
},
{ // multi3
1000, // int max_armour;
12, // level
3, // common
500, // score
13000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
32, // acceleration
0, // impulse delay
900000, // range
2, // slew
ROLE_TARGET
},
{ // defender1
1200, // int max_armour;
4, // level
3, // common
100, // score
25000, // int radius;
131000, // int edge_radius;
HIGH_DRAG, // drag
3000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
2, // turrets
PITCH_MINIBOSS1, // pitch
35, // acceleration
0, // impulse delay
2000000, // range
3, // slew
ROLE_MINIBOSS
},
{ // defender1_turret1
500, // int max_armour;
4, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
3, // slew
ROLE_TURRET
},
{ // defender1_turret2
500, // int max_armour;
4, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
3, // slew
ROLE_TURRET
},
{ // defender1_turret3
500, // int max_armour;
4, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_TURRET
},
{ // overspinner
3500, // int max_armour;
13, // level
3, // common
2500, // score
30000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2500, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_MINIBOSS2, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
16, // slew
ROLE_MINIBOSS
},
{ // overspikey
3500, // int max_armour;
13, // level
3, // common
2500, // score
30000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2500, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_MINIBOSS2, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
16, // slew
ROLE_MINIBOSS
},
{ // underspikey
1500, // int max_armour;
5, // level
3, // common
1000, // score
26000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_MINIBOSS1, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
3, // slew
ROLE_MINIBOSS
},
{ // overblatter
3000, // int max_armour;
7, // level
3, // common
1000, // score
26000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_MINIBOSS1, // pitch
0, // acceleration
0, // impulse delay
400000, // range
2, // slew
ROLE_MINIBOSS
}
,
{ // defender2
1600, // int max_armour;
10, // level
3, // common
900, // score
25000, // int radius;
131000, // int edge_radius;
HIGH_DRAG, // drag
3000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
2, // turrets
PITCH_MINIBOSS2, // pitch
45, // acceleration
0, // impulse delay
2000000, // range
4, // slew
ROLE_MINIBOSS
},
{ // defender2_turret1
700, // int max_armour;
10, // level
0, // common
100, // score
9000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
5, // slew
ROLE_TURRET
},
{ // defender2_turret2
700, // int max_armour;
10, // level
0, // common
100, // score
9000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
5, // slew
ROLE_TURRET
},
{ // defender2_turret3
700, // int max_armour;
10, // level
0, // common
100, // score
9000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
3, // slew
ROLE_TURRET
},
{ // defender3
2500, // int max_armour;
13, // level
3, // common
900, // score
25000, // int radius;
131000, // int edge_radius;
HIGH_DRAG, // drag
3000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
2, // turrets
PITCH_MINIBOSS2, // pitch
50, // acceleration
0, // impulse delay
2000000, // range
5, // slew
ROLE_MINIBOSS
},
{ // defender3_turret1 - shielder
500, // int max_armour;
12, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_TURRET
},
{ // defender3_turret2 - orbital
700, // int max_armour;
12, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_TURRET
},
{ // defender3_turret3 - slivers
700, // int max_armour;
12, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
400000, // range
8, // slew
ROLE_TURRET
},
{ // defender3_turret4 - ball2
700, // int max_armour;
12, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
600000, // range
3, // slew
ROLE_TURRET
},
{ // defender3_turret5
700, // int max_armour;
12, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_TURRET
},
{ // defender3_turret6
700, // int max_armour;
12, // level
0, // common
100, // score
12000, // int radius;
15000, // int edge_radius;
0, // drag
400, // mass
AI_TURRET, // AI
0, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TURRET
},
{ // orbiter1
800, // int max_armour;
8, // level
3, // common
800, // score
13000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_AUTO, // AI
GEN_MED, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
1, // slew
ROLE_EXTRA
},
{ // orbiter2
1000, // int max_armour;
10, // level
3, // common
1200, // score
10000, // int radius;
25000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_AUTO, // AI
GEN_MED, // generous
COLOUR_RED6, // col1
COLOUR_RED2, // col2
0, // turrets
PITCH_SMALL_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
1, // slew
ROLE_EXTRA
},
{ // orbiter3
1100, // int max_armour;
13, // level
3, // common
1500, // score
15000, // int radius;
29000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_AUTO, // AI
GEN_MED, // generous
COLOUR_BLUE5, // col1
COLOUR_BLUE2, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
16, // slew
ROLE_EXTRA
},
{ // attractor
900, // int max_armour;
12, // level
3, // common
1500, // score
15000, // int radius;
29000, // int edge_radius;
HIGH_DRAG, // drag
1500, // mass
AI_AUTO, // AI
GEN_MED, // generous
COLOUR_BLUE5, // col1
COLOUR_BLUE2, // col2
0, // turrets
PITCH_HEAVY_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_EXTRA
},
{ // Disrupter1
900, // int max_armour;
9, // level
2, // common
800, // score
15000, // int radius;
54000, // int edge_radius;
LOW_DRAG, // drag
350, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
15, // acceleration
0, // impulse delay
1500000, // range
4, // slew
ROLE_TARGET
},
{ // Disrupter2
1000, // int max_armour;
12, // level
2, // common
1200, // score
15000, // int radius;
54000, // int edge_radius;
LOW_DRAG, // drag
450, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
15, // acceleration
0, // impulse delay
1500000, // range
4, // slew
ROLE_TARGET
},
{ // Disrupter3
1200, // int max_armour;
14, // level
2, // common
800, // score
15000, // int radius;
54000, // int edge_radius;
LOW_DRAG, // drag
550, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_GREY5, // col1
COLOUR_GREY1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
15, // acceleration
0, // impulse delay
1500000, // range
4, // slew
ROLE_TARGET
},
{ // triangler1
750, // int max_armour;
9, // level
3, // common
200, // score
16000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
600, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_HEAVY, // pitch
30, // acceleration
0, // impulse delay
2000000, // range
2, // slew
ROLE_TARGET
},
{ // triangler2
1250, // int max_armour;
12, // level
3, // common
500, // score
18000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
800, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_HEAVY, // pitch
30, // acceleration
0, // impulse delay
3000000, // range
3, // slew
ROLE_TARGET
},
{ // triangler3
1350, // int max_armour;
15, // level
3, // common
500, // score
15000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
800, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_HEAVY, // pitch
30, // acceleration
0, // impulse delay
2000000, // range
4, // slew
ROLE_TARGET
},
{ // overtriangler
2800, // int max_armour;
15, // level
3, // common
500, // score
330000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
1400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
1, // turrets
PITCH_MINIBOSS2, // pitch
23, // acceleration
0, // impulse delay
2000000, // range
2, // slew
ROLE_MINIBOSS
},
{ // overtriangler_turret
600, // int max_armour;
12, // level
0, // common
400, // score
10000, // int radius;
13000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
400000, // range
6, // slew
ROLE_TURRET
},
{ // leaper1
300, // int max_armour;
7, // level
2, // common
250, // score
9000, // int radius;
10000, // int edge_radius;
MED_DRAG, // drag
150, // mass
AI_LEAPER, // AI
GEN_LOW, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
5000, // acceleration
60, // impulse delay
500000, // range
12, // slew
ROLE_EXTRA
},
{ // leaper2
400, // int max_armour;
11, // level
2, // common
500, // score
9000, // int radius;
10000, // int edge_radius;
MED_DRAG, // drag
190, // mass
AI_LEAPER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
7000, // acceleration
50, // impulse delay
500000, // range
12, // slew
ROLE_EXTRA
},
{ // wormer4
900, // int max_armour;
12, // level
2, // common
500, // score
10000, // int radius;
23000, // int edge_radius;
LOW_DRAG, // drag
400, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE5, // col1
COLOUR_BLUE2, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
15, // acceleration
99, // impulse delay
900000, // range
0, // slew
ROLE_TARGET
},
{ // beamer1
2500, // int max_armour;
12, // level
2, // common
900, // score
35000, // int radius;
131000, // int edge_radius;
HIGH_DRAG, // drag
4000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
2, // turrets
PITCH_MINIBOSS1, // pitch
40, // acceleration
0, // impulse delay
1000000, // range
4, // slew
ROLE_MINIBOSS
},
{ // beamer2
3500, // int max_armour;
14, // level
2, // common
1200, // score
35000, // int radius;
131000, // int edge_radius;
HIGH_DRAG, // drag
5000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
2, // turrets
PITCH_MINIBOSS2, // pitch
40, // acceleration
0, // impulse delay
1000000, // range
5, // slew
ROLE_MINIBOSS
},
{ // overblatter2
3000, // int max_armour;
13, // level
3, // common
2000, // score
26000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2500, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_MINIBOSS2, // pitch
0, // acceleration
0, // impulse delay
400000, // range
2, // slew
ROLE_MINIBOSS
},
{ // overdisrupter
2500, // int max_armour;
14, // level
2, // common
900, // score
25000, // int radius;
131000, // int edge_radius;
HIGH_DRAG, // drag
3000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
2, // turrets
PITCH_MINIBOSS2, // pitch
40, // acceleration
0, // impulse delay
2000000, // range
5, // slew
ROLE_MINIBOSS
},
{ // guardian4
1000, // int max_armour;
13, // level
3, // common
800, // score
19000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
250, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED5, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
36, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // guardian5
1100, // int max_armour;
15, // level
3, // common
900, // score
19000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
250, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE5, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_GUARDIAN, // pitch
32, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // overzapper
3500, // int max_armour;
13, // level
3, // common
2500, // score
30000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2500, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
1, // turrets
PITCH_MINIBOSS2, // pitch
0, // acceleration
0, // impulse delay
2000000, // range
2, // slew
ROLE_MINIBOSS
},
{ // Bracket5
1000, // int max_armour;
13, // level
2, // common
800, // score
11000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
350, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_BRACKET, // pitch
32, // acceleration
0, // impulse delay
700000, // range
2, // slew
ROLE_TARGET
},
{ // Boss2_2
3000, // int max_armour;
10, // level
0, // common
500, // score
35000, // int radius;
125000, // int edge_radius;
HIGH_DRAG, // drag
5000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
3, // turrets
PITCH_BOSS2, // pitch
65, // acceleration
99, // impulse delay
1900000, // range
3, // slew
ROLE_BOSS
},
{ // Boss2_3
3000, // int max_armour;
10, // level
0, // common
500, // score
35000, // int radius;
125000, // int edge_radius;
HIGH_DRAG, // drag
5000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
4, // turrets
PITCH_BOSS3, // pitch
65, // acceleration
99, // impulse delay
1900000, // range
3, // slew
ROLE_BOSS
},
{ // Boss3_1
4000, // int max_armour;
15, // level
0, // common
500, // score
35000, // int radius;
125000, // int edge_radius;
HIGH_DRAG, // drag
5000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_BLUE5, // col1
COLOUR_BLUE1, // col2
4, // turrets
PITCH_BOSS3, // pitch
65, // acceleration
99, // impulse delay
1900000, // range
3, // slew
ROLE_BOSS
},
{ // Boss3_2
4000, // int max_armour;
15, // level
0, // common
500, // score
35000, // int radius;
125000, // int edge_radius;
HIGH_DRAG, // drag
5000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_BLUE5, // col1
COLOUR_BLUE1, // col2
3, // turrets
PITCH_BOSS3, // pitch
65, // acceleration
99, // impulse delay
1900000, // range
3, // slew
ROLE_BOSS
},
{ // Boss3_3
4000, // int max_armour;
15, // level
0, // common
500, // score
35000, // int radius;
125000, // int edge_radius;
HIGH_DRAG, // drag
5000, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
4, // turrets
PITCH_BOSS3, // pitch
65, // acceleration
99, // impulse delay
1900000, // range
2, // slew
ROLE_BOSS
},
{ // Messenger
200, // int max_armour;
15, // level
0, // common
50, // score
7000, // int radius;
10000, // int edge_radius;
LOW_DRAG, // drag
150, // mass
AI_FIGHTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_FIGHTER, // pitch
65, // acceleration
0, // impulse delay
500000, // range
12, // slew
ROLE_EXTRA
},
{ // shadow1
400, // int max_armour;
9, // level
2, // common
10, // score
12000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
26, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // shadow2
500, // int max_armour;
13, // level
2, // common
900, // score
12000, // int radius;
60000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_GREY6, // col1
COLOUR_GREY1, // col2
0, // turrets
PITCH_MEDIUM, // pitch
30, // acceleration
0, // impulse delay
900000, // range
4, // slew
ROLE_TARGET
},
{ // Boss3_turret1
700, // int max_armour;
15, // level
0, // common
400, // score
14000, // int radius;
17000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
3, // slew
ROLE_TURRET
},
{ // Boss3_turret2
700, // int max_armour;
15, // level
0, // common
400, // score
14000, // int radius;
17000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
6, // slew
ROLE_TURRET
},
{ // Boss3_turret3
700, // int max_armour;
15, // level
0, // common
400, // score
14000, // int radius;
17000, // int edge_radius;
0, // drag
200, // mass
AI_TURRET, // AI
0, // generous
COLOUR_GREY5, // col1
COLOUR_GREY1, // col2
0, // turrets
PITCH_TURRET2, // pitch
0, // acceleration
0, // impulse delay
900000, // range
6, // slew
ROLE_TURRET
},
{ // waver1
800, // int max_armour;
7, // level
2, // common
800, // score
15000, // int radius;
55000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_AUTO, // AI
GEN_MED, // generous
COLOUR_ORANGE6, // col1
COLOUR_ORANGE1, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_EXTRA
},
{ // waver2
1000, // int max_armour;
11, // level
2, // common
1200, // score
15000, // int radius;
60000, // int edge_radius;
HIGH_DRAG, // drag
500, // mass
AI_AUTO, // AI
GEN_MED, // generous
COLOUR_RED6, // col1
COLOUR_RED2, // col2
0, // turrets
PITCH_LARGE_SPINNER, // pitch
0, // acceleration
0, // impulse delay
900000, // range
0, // slew
ROLE_EXTRA
},
{ // Curve1
250, // int max_armour;
1, // level
2, // common
10, // score
13000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
200, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_GREEN6, // col1
COLOUR_GREEN1, // col2
0, // turrets
PITCH_BRACKET, // pitch
17, // acceleration
0, // impulse delay
900000, // range
1, // slew
ROLE_TARGET
},
{ // Curve2
1050, // int max_armour;
5, // level
2, // common
10, // score
13000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
300, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_RED6, // col1
COLOUR_RED1, // col2
0, // turrets
PITCH_BRACKET, // pitch
17, // acceleration
0, // impulse delay
1500000, // range
1, // slew
ROLE_TARGET
},
{ // Curve3
1250, // int max_armour;
8, // level
2, // common
10, // score
13000, // int radius;
54000, // int edge_radius;
MED_DRAG, // drag
250, // mass
AI_DRIFTER, // AI
GEN_LOW, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_BRACKET, // pitch
17, // acceleration
0, // impulse delay
1500000, // range
1, // slew
ROLE_TARGET
},
{ // underspikey2
2000, // int max_armour;
7, // level
2, // common
1000, // score
26000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2500, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_MINIBOSS1, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
2, // slew
ROLE_MINIBOSS
},
{ // underspikey3
1700, // int max_armour;
6, // level
2, // common
1000, // score
26000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2500, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_MINIBOSS1, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
3, // slew
ROLE_MINIBOSS
},
{ // overspikey2
3500, // int max_armour;
12, // level
2, // common
2500, // score
27000, // int radius;
120000, // int edge_radius;
HIGH_DRAG, // drag
2500, // mass
AI_DRIFTER, // AI
GEN_HIGH, // generous
COLOUR_BLUE6, // col1
COLOUR_BLUE1, // col2
0, // turrets
PITCH_MINIBOSS2, // pitch
0, // acceleration
0, // impulse delay
9000000, // range
4, // slew
ROLE_MINIBOSS
},
{ // dead_tri1
1, // int max_armour;
0, // level
0, // common
200, // score
0, // int radius;
60000, // int edge_radius;
LOW_DRAG, // drag
1, // mass
AI_DRIFTER, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_HEAVY, // pitch
0, // acceleration
0, // impulse delay
0, // range
0, // slew
ROLE_DEAD
},
{ // dead_tri2
1, // int max_armour;
0, // level
0, // common
200, // score
0, // int radius;
60000, // int edge_radius;
LOW_DRAG, // drag
1, // mass
AI_DRIFTER, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_HEAVY, // pitch
0, // acceleration
0, // impulse delay
0, // range
0, // slew
ROLE_DEAD
},
{ // dead_tri3
1, // int max_armour;
0, // level
0, // common
200, // score
0, // int radius;
60000, // int edge_radius;
LOW_DRAG, // drag
1, // mass
AI_DRIFTER, // AI
0, // generous
COLOUR_YELLOW6, // col1
COLOUR_YELLOW1, // col2
0, // turrets
PITCH_HEAVY, // pitch
0, // acceleration
0, // impulse delay
0, // range
0, // slew
ROLE_DEAD
}

};
// black enemy?

