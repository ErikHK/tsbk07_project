#include "objects.h"
#include <time.h>
#include <stdlib.h>

#define VK_SPACE 32

void create_spaceship(spaceship * s)
{
	//init to zero
	for (int i = 0; i < 3; i++)
	{
		s->acc[i] = 0;
		s->speed[i] = 0;
	}
	s->angle_acc[0] = 0.0008;
	s->angle_acc[1] = 0.0008;

	s->angle_speed[0] = 0;
	s->angle_speed[1] = 0;

	s->angle[0] = 0;
	s->angle[1] = 0;

	s->thrust = 0.02;

	s->pos[0] = 80;
	s->pos[1] = 20;
	s->pos[2] = 80;
	s->gravity = -0.006;

	s->acc[1] = s->gravity;	//y acc, gravity

	s->body = LoadModelPlus("spaceship/spaceship_body.obj");
	s->fins[0] = LoadModelPlus("spaceship/fin.obj");
	s->fins[1] = LoadModelPlus("spaceship/fin.obj");
	s->fins[2] = LoadModelPlus("spaceship/fin.obj");

	//s->body_matrix = Mult(Rz(2), Mult(T(s->pos[0], s->pos[1], s->pos[2]), S(0.1, 0.1, 0.1)));
	
	s->fins_matrix[0] = Mult(T(0, -1, -2), s->body_matrix);
	s->fins_matrix[1] = Mult(T(1, -1, -2), s->body_matrix);
	s->fins_matrix[2] = Mult(T(2, -1, -2), s->body_matrix);

	LoadTGATextureSimple("spaceship/spaceship_uvw_body.tga", &(s->body_tex));
}

void draw_spaceship(spaceship * s, GLuint program)
{
	s->fins_matrix[0] = Mult(s->body_matrix, T(0, -10, -15));
	s->fins_matrix[1] = Mult(s->body_matrix, T(0, -10, -35));
	s->fins_matrix[2] = Mult(s->body_matrix, T(0, -10, -15));

  mat4 total = s->body_matrix;
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
  glBindTexture(GL_TEXTURE_2D, s->body_tex);
  DrawModel(s->body, program, "inPosition", "inNormal", "inTexCoord");
  
  //draw fins
  
  for (int i = 0; i < 3;i++)
  {
	  total = s->fins_matrix[i];
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
	s->body_matrix = Mult(T(s->pos[0], s->pos[1], s->pos[2]), Mult(Mult(Rx(s->angle[0]), Ry(2)), S(0.1, 0.1, 0.1)));

	if (keyIsDown(VK_SPACE))
	{
		s->acc[2] = s->thrust*sin(s->angle[0]);
		s->acc[1] = s->thrust*cos(s->angle[0])+s->gravity;
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
		return;
	//*cam_matrix = lookAt(cam_pos->x, cam_pos->y, cam_pos->z,
	//	s->pos[0], s->pos[1], s->pos[2],
	//	0.0, 1.0, 0.0);

	*cam_matrix = lookAt(cam_pos->x, cam_pos->y, cam_pos->z,
		s->pos[0], s->pos[1], s->pos[2],
		0.0, 1.0, 0.0);


}


void create_landing_point()
{

}

double random()
{
	return (double)rand() / (double)RAND_MAX;
}



void create_cloud(cloud * c, vec3 init_pos)
{
	c->spheres[0] = LoadModelPlus("sphere.obj");
	memcpy(&(c->spheres[1]), &(c->spheres[0]), sizeof(c->spheres[0]));
	memcpy(&(c->spheres[2]), &(c->spheres[0]), sizeof(c->spheres[0]));
	//vec3 tmp = { 1, 1.5, 1 };
	//memcpy(&(c->sphere_scales[0]), &tmp, sizeof(tmp));
	//memcpy(&(c->sphere_scales[1]), &tmp, sizeof(tmp));
	c->num_spheres = 10;

	for (int i = 0; i < c->num_spheres; i++)
	{
		if (i>0)
			memcpy(&(c->spheres[i]), &(c->spheres[0]), sizeof(c->spheres[0]));
		c->matrix[i] = Mult(T(init_pos.x+(random()-.5)*i/3, init_pos.y+(random()-.5)*i/2, init_pos.z+(random()-.5)*i*2), 
			S(.5+(random()-.2), .5+(random()-.3), .6+(random()-.3)));
	}
	
}


void draw_cloud(cloud *c, GLuint program)
{
	for (int i = 0; i < c->num_spheres; i++)
	{
		mat4 total = c->matrix[i];
		glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
		//glBindTexture(GL_TEXTURE_2D, s->body_tex);
		DrawModel(c->spheres[i], program, "inPosition", "inNormal", "inTexCoord");
	}

}
