#include "objects.h"
#include <time.h>
#include <stdlib.h>

#define VK_SPACE 0x20
#define VK_RETURN 0x0D

void create_spaceship(spaceship * s)
{
	//has not yet landed
	s->landed = 0;
	s->fuel = 100.0;

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

	s->pos[0] = 90;
	s->pos[1] = 40;
	s->pos[2] = 80;
	s->gravity = -0.006;
	
	s->acc[1] = s->gravity;	//y acc, gravity

	s->body = LoadModelPlus("spaceship/spaceship_body.obj");
	s->exhaust = LoadModelPlus("spaceship/exhaust.obj");
	s->fire = LoadModelPlus("cone.obj");
	
	s->fins[0] = LoadModelPlus("spaceship/fin.obj");
	s->fins[1] = LoadModelPlus("spaceship/fin.obj");
	s->fins[2] = LoadModelPlus("spaceship/fin.obj");

	//s->body_matrix = Mult(Rz(2), Mult(T(s->pos[0], s->pos[1], s->pos[2]), S(0.1, 0.1, 0.1)));
	
	s->fins_matrix[0] = Mult(T(0, -1, -2), s->body_matrix);
	s->fins_matrix[1] = Mult(T(1, -1, -2), s->body_matrix);
	s->fins_matrix[2] = Mult(T(2, -1, -2), s->body_matrix);

	LoadTGATextureSimple("spaceship/spaceship_uvw_body.tga", &(s->body_tex));
}

void create_hud(hud *h)
{
	h->fuel_bar = LoadModelPlus("cube.obj");
	h->game_over_sign = LoadModelPlus("game_over.obj");
	h->you_win_sign = LoadModelPlus("you_win.obj");
	LoadTGATextureSimple("spaceship/spaceship_uvw_body.tga", &(h->game_over_sign_tex));
}

void draw_spaceship(spaceship * s, GLuint program)
{
	s->fins_matrix[0] = Mult(s->body_matrix, T(0, -10, -15));
	s->fins_matrix[1] = Mult(s->body_matrix, Mult(Ry(2.094),T(0, -10, -15)));
	s->fins_matrix[2] = Mult(s->body_matrix, Mult(Ry(2.094*2), T(0, -10, -15)));

	s->exhaust_pos = Mult(s->body_matrix, S(1.3,1.3,1.3));
	s->fire_pos = Mult(s->body_matrix, Mult(Rx(M_PI), T(0,15,0)));

  mat4 total = s->body_matrix;
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
  glBindTexture(GL_TEXTURE_2D, s->body_tex);
  DrawModel(s->body, program, "inPosition", "inNormal", "inTexCoord");
  
  //draw exhaust
  total = s->exhaust_pos;
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
  glUniform1i(glGetUniformLocation(program, "exhaust"), 1);
  glBindTexture(GL_TEXTURE_2D, s->body_tex);
  DrawModel(s->exhaust, program, "inPosition", "inNormal", "inTexCoord");
  glUniform1i(glGetUniformLocation(program, "exhaust"), 0);

  //draw fire
  total = s->fire_pos;
  if(s->fire_visible)
  {
  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
  glUniform1i(glGetUniformLocation(program, "fire"), 1);
  glBindTexture(GL_TEXTURE_2D, s->body_tex);
  DrawModel(s->fire, program, "inPosition", "inNormal", "inTexCoord");
  glUniform1i(glGetUniformLocation(program, "fire"), 0);
  }
  
  

  //draw fins
  for (int i = 0; i < 3;i++)
  {
	  total = s->fins_matrix[i];
	  glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	  glBindTexture(GL_TEXTURE_2D, s->body_tex);
	  DrawModel(s->fins[i], program, "inPosition", "inNormal", "inTexCoord");

  }
  
  
}

void draw_fuel_bar(hud * h, float * fuel, GLuint program)
{
	float fuel_scale = *fuel / 1000.0;
	mat4 scale = S(fuel_scale*2, .04, .05);
	mat4 trans = T(1, .9, -2);
	mat4 total = Mult(trans, Mult(scale, T(1,0,0)));
	glUniform1i(glGetUniformLocation(program, "hud"), 1);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	//glBindTexture(GL_TEXTURE_2D, s->body_tex);
	if (*fuel >= 1)
		DrawModel(h->fuel_bar, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "fuel_full"), 1);

	scale = S(.2, .04, .05);
	trans = T(1, .9, -2);
	total = Mult(trans, Mult(scale, T(1, 0, 0)));
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	//glBindTexture(GL_TEXTURE_2D, s->body_tex);
	DrawModel(h->fuel_bar, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "fuel_full"), 0);
	
	glUniform1i(glGetUniformLocation(program, "hud"), 0);
}

void draw_game_over(hud * h, GLuint program)
{
	
	glUniform1i(glGetUniformLocation(program, "hud"), 1);

	
	mat4 total = Mult(Mult(T(0, 1, -8),S(.1, .1, .1)), Ry(.3));
	
	glUniform1i(glGetUniformLocation(program, "game_over_sign"), 1);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glBindTexture(GL_TEXTURE_2D, h->game_over_sign_tex);
	DrawModel(h->game_over_sign, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "game_over_sign"), 0);
	glUniform1i(glGetUniformLocation(program, "hud"), 0);

	
}



void draw_you_win(hud * h, GLuint program)
{

	glUniform1i(glGetUniformLocation(program, "hud"), 1);


	mat4 total = Mult(Mult(T(0, 1, -8), S(.1, .1, .1)), Ry(.3));

	glUniform1i(glGetUniformLocation(program, "game_over_sign"), 1);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glBindTexture(GL_TEXTURE_2D, h->game_over_sign_tex);
	DrawModel(h->you_win_sign, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "game_over_sign"), 0);
	glUniform1i(glGetUniformLocation(program, "hud"), 0);


}


void move_spaceship(spaceship * s, GLuint program)
{
	for (int i = 0; i < 3; i++)
	{
		if (fabs(s->speed[i]) <= 0.05);
			s->speed[i] += s->acc[i];

		s->pos[i] += s->speed[i];
	}
	s->body_matrix = Mult(T(s->pos[0], s->pos[1], s->pos[2]), 
		Mult(Mult( Mult( Rx(s->angle[0]), Rz(s->angle[1])), Ry(2)), S(0.1, 0.1, 0.1)));

	if (keyIsDown(VK_SPACE) && s->fuel >= 0)
	{
		s->fuel -= .2;
		glUniform1i(glGetUniformLocation(program, "fire"), 1);
		s->fire_visible = 1;
		s->acc[2] = s->thrust*sin(s->angle[0]);

		s->acc[1] = s->thrust*cos(s->angle[0])*cos(s->angle[1]) + s->gravity;

		s->acc[0] = -s->thrust*sin(s->angle[1]);
		glUniform1i(glGetUniformLocation(program, "fire"), 0);
	}
	else
	{
		s->acc[1] = s->gravity;
		s->fire_visible = 0;
	}

	if (!s->landed)
	{
		if (keyIsDown('a'))
			s->angle_speed[0] -= s->angle_acc[0];
		else
			s->angle_speed[0] *= 0.99;

		if (keyIsDown('d'))
			s->angle_speed[0] += s->angle_acc[0];
		else
			s->angle_speed[0] *= 0.99;


		if (keyIsDown('w'))
			s->angle_speed[1] -= s->angle_acc[1];
		else
			s->angle_speed[1] *= 0.99;

		if (keyIsDown('s'))
			s->angle_speed[1] += s->angle_acc[1];
		else
			s->angle_speed[1] *= 0.99;
	}

	s->angle[0] += s->angle_speed[0];
	s->angle[1] += s->angle_speed[1];

	//stabilize!
	if (!s->landed)
	{
		if (fabs(s->angle[0]) <= M_PI / 12)
			s->angle[0] *= .96;
		else
			s->angle[0] *= .995;

		if (fabs(s->angle[1]) <= M_PI / 12)
			s->angle[1] *= .96;
		else
			s->angle[1] *= .995;
	}
	s->speed[0] *= 0.96;
	//s->speed[1] *= 0.9;
	s->speed[2] *= 0.96;
	
}

void update_cam_matrix(spaceship * s, mat4 * cam_matrix, vec3 * cam_pos)
{
	if (!(s && cam_matrix && cam_pos))
		return;

	

	if (keyIsDown('c') || keyIsDown('C'))
	{

		*cam_matrix = lookAt(0, 300, 0,
			128, 0, 128,
			0.0, 1.0, 0.0);
		return;
	}

	//*cam_matrix = lookAt(cam_pos->x, cam_pos->y, cam_pos->z,
	//	s->pos[0], s->pos[1], s->pos[2],
	//	0.0, 1.0, 0.0);
	//cam.x = s.pos[0] - 80;
	//*cam_matrix = lookAt(cam_pos->x, cam_pos->y, cam_pos->z,
	//	s->pos[0], s->pos[1], s->pos[2],
	//	0.0, 1.0, 0.0);

	*cam_matrix = lookAt(s->pos[0]-80, cam_pos->y, s->pos[2],
		s->pos[0], s->pos[1], s->pos[2],
		0.0, 1.0, 0.0);


}


void create_landing_point(landing_point * lp)
{
	lp->landing_point_model = LoadModelPlus("landing_point.obj");

}

void set_landing_point(landing_point * lp, vec3 pos)
{
	lp->pos = pos;
}

void draw_landing_point(landing_point * lp, GLuint program)
{
	glUniform1i(glGetUniformLocation(program, "landing_point"), 1);
	mat4 total = Mult(T(lp->pos.x, lp->pos.y, lp->pos.z), S(.2,.2,.2));
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	//glBindTexture(GL_TEXTURE_2D, s->body_tex);
	DrawModel(lp->landing_point_model, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "landing_point"), 0);


}

float random()
{
	return (float)rand() / (float)RAND_MAX;
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
			S(.7+(random()/3-.2), .7+(random()/3-.3), .8+(random()/2.6-.3)));
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


void create_tree(tree *t, vec3 init_pos)
{
	t->cones[0] = LoadModelPlus("cone.obj");
}

void draw_tree(tree *t, GLuint program)
{
	mat4 total = T(90, 20, 80);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	DrawModel(t->cones[0], program, "inPosition", "inNormal", "inTexCoord");
}

void create_spark(spark *s, GLuint program)
{
	s->model = LoadModelPlus("spark.obj");
	
}

float spaceship_total_speed(spaceship * s)
{
	float xx = pow(s->speed[0], 2);
	float yy = pow(s->speed[1], 2);
	float zz = pow(s->speed[2], 2);

	return sqrt(xx + yy + zz);

}

float distance_to_target(spaceship *s, vec3 *target_pos)
{
	float xx = pow(s->pos[0] - target_pos->x, 2);
	float zz = pow(s->pos[2] - target_pos->z, 2);
	return sqrt(xx + zz);
}