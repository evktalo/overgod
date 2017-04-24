int create_bullet(int type, int x, int y,
int x_speed, int y_speed, int owner,
int damage, int timer, int mass, int angle,
int status, int size, int colours [4], int speed_div,
int special1, int special2, int special3, int special4, int special5);

void init_bullets(void);
void run_bullets(void);

void blast(int x, int y, int radius, int damage, int force, int owner);

int closest_target(int x, int y);
int nearby_target(int range, int x, int y);

void shield_line(int e, int bx, int by);
void shielder_pulse(int e, int dam);
int check_shielder(int e);
void enemy_beam(int x, int y, int angle, int power, int type, int e);
void line_blast(int x, int y, int type, int target);

