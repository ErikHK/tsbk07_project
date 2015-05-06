#include "objects.h"

void create_spaceship(spaceship * s)
{
	//init to zero
	for (int i = 0; i < 3; i++)
	{
		//s->pos[i] = 0;
		s->acc[i] = 0;
		s->speed[i] = 0;
	}
	s->angle_acc[0] = 0.0008;
	s->angle_acc[1] = 0.0008;

	s->angle_speed[0] = 0;
	s->angle_speed[1] = 0;

	s->angle[0] = 0;
	s->angle[1] = 0;

	s->thrust = 0.01;

	s->pos[0] = 80;
	s->pos[1] = 20;
	s->pos[2] = 80;
	s->gravity = -0.006;

	s->acc[1] = s->gravity;	//y acc, gravity

	s->body = LoadModelPlus("spaceship/spaceship_body.obj");
	s->fins[0] = LoadModelPlus("spaceship/fin.obj");
	s->fins[1] = LoadModelPlus("spaceship/fin.obj");
	s->fins[2] = LoadModelPlus("spaceship/fin.obj");

	s->body_matrix = Mult(Rz(2), Mult(T(s->pos[0], s->pos[1], s->pos[2]), S(0.2, 0.2, 0.2)));
	
	s->fins_matrix[0] = Mult(T(0, -1, -2), s->body_matrix);
	s->fins_matrix[1] = Mult(T(1, -1, -2), s->body_matrix);
	s->fins_matrix[2] = Mult(T(2, -1, -2), s->body_matrix);

	LoadTGATextureSimple("spaceship/spaceship_uvw_body.tga", &(s->body_tex));
}

void draw_spaceship(spaceship * s, mat4 * cam_matrix, GLuint program)
{
	s->fins_matrix[0] = Mult(s->body_matrix, T(0, -10, -15));
	s->fins_matrix[1] = Mult(s->body_matrix, T(0, -10, -35));
	s->fins_matrix[2] = Mult(s->body_matrix, T(0, -10, -15));

  mat4 total = Mult(*cam_matrix, s->body_matrix);
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
  glBindTexture(GL_TEXTURE_2D, s->body_tex);
  DrawModel(s->body, program, "inPosition", "inNormal", "inTexCoord");
  
  //draw fins
  
  for (int i = 0; i < 3;i++)
  {
	  total = Mult(*cam_matrix, s->fins_matrix[i]);
	  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	  glBindTexture(GL_TEXTURE_2D, s->body_tex);
	  DrawModel(s->fins[i], program, "inPosition", "inNormal", "inTexCoord");

  }
  
  
}

void move_spaceship(spaceship * s)
{
	for (int i = 0; i < 3; i++)
	{
		if (fabs(s->speed[i]) <= 0.005);
		s->speed[i] += s->acc[i];

		s->pos[i] += s->speed[i];
	}
	s->body_matrix = Mult(T(s->pos[0], s->pos[1], s->pos[2]), Mult(Rx(s->angle[0]), S(0.2, 0.2, 0.2)));

	if (keyIsDown(VK_SPACE))
	{
		s->acc[2] = s->thrust*sin(s->angle[0]);
		s->acc[1] = s->thrust*cos(s->angle[0]);
	}
	else
		s->acc[1] = s->gravity;

	if (keyIsDown('a'))
		s->angle_speed[0] -= s->angle_acc[0];
	else
		s->angle_speed[0] *= 0.99;

	if (keyIsDown('d'))
		s->angle_speed[0] += s->angle_acc[0];
	else
		s->angle_speed[0] *= 0.99;

	s->angle[0] += s->angle_speed[0];

	//stabilize!
	if (fabs(s->angle[0]) <= M_PI/12)
		s->angle[0] *= .98;
	else
		s->angle[0] *= .995;

	s->speed[2] *= 0.99;
}

void update_cam_matrix(spaceship * s, mat4 * cam_matrix, vec3 * cam_pos)
{
	if (!(s && cam_matrix && cam_pos))
		return NULL;
	*cam_matrix = lookAt(cam_pos->x, cam_pos->y, cam_pos->z,
		s->pos[0], s->pos[1], s->pos[2],
		0.0, 1.0, 0.0);


}


void create_landing_point()
{

}
