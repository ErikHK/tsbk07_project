//Different objects as structs, for example spaceship, landing point etc
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"

typedef struct spaceship_t
{
  float speed[3];
  float acc[3];
  float pos[3];
  float thrust;
  float gravity;
  float angle_acc[2];
  float angle_speed[2];
  float angle[2];
  int landed;

  Model *body;
  Model *fins[3];
  mat4 body_matrix;
  mat4 fins_matrix[3];
  GLuint body_tex;

} spaceship;

typedef struct cloud_t
{
	Model * spheres[10];
	int num_spheres;
	vec3 sphere_scales[10];
	mat4 matrix[10];

} cloud;

typedef struct landing_point
{
  Model *landing_point_model;

} landing_point;

void create_cloud();
void create_spaceship();
void create_landing_point();
//void draw_spaceship(spaceship * s, mat4 * cam_matrix, GLuint program);
void draw_spaceship();
void move_spaceship();
void draw_cloud();
void update_cam_matrix();