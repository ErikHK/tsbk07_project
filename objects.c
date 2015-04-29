#include "objects.h"

void create_spaceship(spaceship * s)
{
  s->body = LoadModelPlus("spaceship/spaceship_body.obj");
  s->fins[0] = LoadModelPlus("spaceship/fin.obj");
  s->fins[1] = LoadModelPlus("spaceship/fin.obj");
  s->fins[2] = LoadModelPlus("spaceship/fin.obj");

  s->body_matrix = S(0.02,0.02,0.02);

  s->fins_matrix[0] = T(1,0,0);
  s->fins_matrix[1] = T(0,1,0);
  s->fins_matrix[2] = T(2,1,0);

  LoadTGATextureSimple("spaceship/spaceship_uvw_body.tga", &(s->body_tex));
}

void draw_spaceship(spaceship * s, mat4 * cam_matrix, GLuint program)
{
  mat4 total = Mult(*cam_matrix, s->body_matrix);
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
  glBindTexture(GL_TEXTURE_2D, s->body_tex);
  DrawModel(s->body, program, "inPosition", "inNormal", "inTexCoord");
  
}


void create_landing_point()
{

}
