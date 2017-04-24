void init_pickups(void);
void destroy_pickup(int p);
void pickup_explodes(int p, int taken);
void run_pickups(void);
int create_pickup(int type, int subtype, int x, int y, int timeout);
void explode_all_pickups(void);

