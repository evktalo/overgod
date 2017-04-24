
void init_enemies(void);

int create_enemy(int type, int subtype, int x, int y,
int x_speed, int y_speed, int angle, int carrying_pickup, int special [10],
unsigned char target);

void run_enemies(void);

int hurt_enemy(int e, int hurty, int source, char pulse);

int spawn_enemy(int type, int subtype, int carrying_pickup, int target);

int enemy_sound_pitch(int ec);

