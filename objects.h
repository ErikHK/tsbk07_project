//Different objects as structs, for example spaceship, landing point etc
#include "loadobj.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "LoadTGA.h"

float random();

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
  float world_angle[2];
  int landed;
  float fuel;

  Model *body;
  Model *fins[3];
  Model *exhaust;
  Model *fire;

  mat4 exhaust_pos;
  mat4 fire_pos;
  int fire_visible;
  mat4 body_matrix;
  mat4 fins_matrix[3];
  GLuint body_tex;


} spaceship;


typedef struct landing_point_t
{
	Model * landing_point_model;
	vec3 pos;

} landing_point;


typedef struct hud_t
{
	Model *fuel_bar;
	Model *game_over_sign;
	Model *you_win_sign;
	GLuint game_over_sign_tex;

} hud;

typedef struct tree_t
{
	Model * spheres[6];
	Model * cones[6];
	int num_spheres;
	int num_cones;
	vec3 sphere_scales[6];
	mat4 matrix[10];

} tree;

typedef struct moon_t
{
	Model * top_half;
	Model * bottom_half;
	float angle[2];
	mat4 matrix[2];
} moon;

typedef struct cloud_t
{
	Model * spheres[10];
	int num_spheres;
	vec3 sphere_scales[10];
	mat4 matrix[10];

} cloud;

typedef struct spark_t
{
  Model * model;
  float speed[3];
  float acc[3];
  float pos[3];
  mat4 matrix;

} spark;

void create_cloud();
void create_spaceship();
void create_landing_point();
void create_spark();
//void draw_spaceship(spaceship * s, mat4 * cam_matrix, GLuint program);
void draw_spaceship();
void move_spaceship();
void draw_cloud();
void update_cam_matrix();
void create_hud();
void draw_hud();
void draw_landing_point();
void draw_game_over();
void draw_you_win();
void set_landing_point();
float spaceship_total_speed();
float distance_to_target();

void create_moon();
void move_moon();
void draw_moon();