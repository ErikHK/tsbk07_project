#include "objects.h"


void create_spaceship(spaceship * s)
{
  s->body = LoadModelPlus("spaceship/spaceship_body.obj");
  s->fins[0] = LoadModelPlus("spaceship/fin.obj");
  s->fins[1] = LoadModelPlus("spaceship/fin.obj");
  s->fins[2] = LoadModelPlus("spaceship/fin.obj");
}

void create_landing_point()
{

}
