#define NO_LIGHTS 100

struct lightstruct
{
 int x;
 int y;
 int size;
 int type;

};

enum
{
LIGHT_NONE,
LIGHT_NORMAL

};

void reset_lights(void);
int add_light(int ltype, int lsize, int lx, int ly);


