

void run_actors(void);
void gain_upgrade_points(int who, int how_many);
int hurt_actor(int a, int source, int hurty);
void init_actor(int a, int type);
void spawn_actor(int a);
void actor_new_level(int a);
int acquire_target(int a);
int closest_enemy(int x, int y, int range, int hide_invisible);
int closest_enemy_or_actor(int a, int x, int y, int range, int hide_invisible);
void gain_score(int p, int sc);
void shake_all_screens(int amount);

