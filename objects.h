//Different objects as structs, for example spaceship, landing point etc
#include "loadobj.h"
#include "VectorUtils3.h"

typedef struct spaceship_
{
  Model *body;
  Model *fins[3];
  mat4 body_matrix;
  mat4 fins_matrix[3];

} spaceship;

typedef struct landing_point
{
  Model *landing_point_model;

} landing_point;

void create_spaceship();
void create_landing_point();
