//Different objects as structs, for example spaceship, landing point etc
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"

typedef struct spaceship_
{
  Model *body;
  Model *fins[3];
  mat4 body_matrix;
  mat4 fins_matrix[3];
  GLuint body_tex;

} spaceship;

typedef struct landing_point
{
  Model *landing_point_model;

} landing_point;

void create_spaceship();
void create_landing_point();
//void draw_spaceship(spaceship * s, mat4 * cam_matrix, GLuint program);
void draw_spaceship();
