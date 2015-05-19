#if defined(_WIN32)
    #include "windows.h"
#endif

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "objects.h"
#include "LoadTGA.h"
#include <math.h>
#include <time.h>
#include "perlin.h"

#define NUM_CLOUDS 8
#define VK_RETURN 0x0D

#define CWIDTH 384
#define CHEIGHT 384

//scale for terrain, higher means higher mountains 
#define SCALE 0.1

mat4 projectionMatrix;
mat4 total, modelView, camMatrix;
//cam position in world
//vec3 cam = { 20, 20, 100 };
vec3 cam = { 0, 60, 128 };

int game_over = 0;
int finished = 0;
int start = 0;

// Reference to shader program
GLuint program;

//highest point on the map
//float highest[3];
vec3 highest = {0,0,0};

//textures
GLuint ground_tex, tex1, water_tex;
TextureData ttex; // terrain

GLfloat *vertexArray;
int texwidth;

// terrain object
Model *tm;
Model *skybox;
Model *ground;
//spaceship object
spaceship s;
//test cloud object
cloud c[NUM_CLOUDS];
//hud object
hud h;
//landing point
landing_point lp;

Point3D lightSourcesColorsArr[] = { { 1.0f, 0.0f, 1.0f },
{ 1.0f, 1.0f, 1.0f },
{ 1.0f, 1.0f, 1.0f },
{ 1.0f, 1.0f, 1.0f } };

GLfloat specularExponent[] = { 1.0, 5.0, 30.0, 5.0 };
GLint isDirectional[] = { 1,1,1,1};

Point3D lightSourcesDirectionsPositions[] = { { 0.0f, 30.0f, 0.0f }, // Red light, positional
{ 3.0f, 5.0f, 3.0f }, // Green light, positional
{ 10.0f, 12.0f, 0.0f }, // Blue light along X
{ 10.0f, 5.0f, 10.0f } }; // White light along Z

/*
GLfloat vertices[] =
{
	-0.5f, -0.5f, 0, //1
	-0.5f, 0.5f, 0,
	0.5f, 0.5f, 0,
	-0.5f, -0.5f, 0, //2
	0.5f, 0.5f, 0,
	0.5f, -0.5f, 0
};
*/

//calculate height of terrain
float calc_height(GLfloat *vertexArray, float x, float z, int width)
{
	int quad = (floor(x) + floor(z)*width)*3;
	int choose_upper = 0;
	
	if (x < 0 || x > width || z < 0 || z > width)
		return 0;


	Point3D corners[3];

	if( (x-floor(x)) + (z-floor(z)) > 1)
		choose_upper = 1;


	if(choose_upper)
	{
		corners[0].x = vertexArray[quad + (1 + 1*width)*3 + 0];
		corners[0].y = vertexArray[quad + (1 + 1*width)*3 + 1];
		corners[0].z = vertexArray[quad + (1 + 1*width)*3 + 2];

		corners[1].x = vertexArray[quad + (0 + 1*width)*3 + 0];
		corners[1].y = vertexArray[quad + (0 + 1*width)*3 + 1];
		corners[1].z = vertexArray[quad + (0 + 1*width)*3 + 2];

		corners[2].x = vertexArray[quad + (1 + 0*width)*3 + 0];
		corners[2].y = vertexArray[quad + (1 + 0*width)*3 + 1];
		corners[2].z = vertexArray[quad + (1 + 0*width)*3 + 2];

	}else{

		corners[0].x = vertexArray[quad + (0 + 0*width)*3 + 0];
		corners[0].y = vertexArray[quad + (0 + 0*width)*3 + 1];
		corners[0].z = vertexArray[quad + (0 + 0*width)*3 + 2];

		corners[1].x = vertexArray[quad + (1 + 0*width)*3 + 0];
		corners[1].y = vertexArray[quad + (1 + 0*width)*3 + 1];
		corners[1].z = vertexArray[quad + (1 + 0*width)*3 + 2];

		corners[2].x = vertexArray[quad + (0 + 1*width)*3 + 0];
		corners[2].y = vertexArray[quad + (0 + 1*width)*3 + 1];
		corners[2].z = vertexArray[quad + (0 + 1*width)*3 + 2];
	}

	Point3D vec1, vec2, normal;
	//Plane equation is given as Ax + By + Cz + D = 0
	float A,B,C,D;
	vec1 = VectorSub(corners[1], corners[0]);
	vec2 = VectorSub(corners[2], corners[0]);
	normal = Normalize(CrossProduct(vec2, vec1));
	A = normal.x;
	B = normal.y;
	C = normal.z;
	D = -(A*corners[0].x + B*corners[0].y + C*corners[0].z);

	float y = (-D-C*z-A*x)/B;
	return y;

}

//calculate normals of terrain
void calc_normal(GLfloat *vertexArray, int x, int z, int width, Point3D *normal)
{
	//Point3D vec1, vec2;
	vec3 vec1, vec2;

	if(x > 0 && z > 0 && x < width-1 && z < width-1)
	{
		vec1.x = vertexArray[(x-1 + z * width)*3 + 0] - 
		vertexArray[(x + z * width)*3 + 0];

		vec1.y = vertexArray[(x-1 + z * width)*3 + 1] - 
		vertexArray[(x + z * width)*3 + 1];

		vec1.z = vertexArray[(x-1 + z * width)*3 + 2] - 
		vertexArray[(x + z * width)*3 + 2];


		vec2.x = vertexArray[(x + (z+1) * width)*3 + 0] - 
		vertexArray[(x + z * width)*3 + 0];

		vec2.y = vertexArray[(x + (z+1) * width)*3 + 1] - 
		vertexArray[(x + z * width)*3 + 1];

		vec2.z = vertexArray[(x + (z+1) * width)*3 + 2] - 
		vertexArray[(x + z * width)*3 + 2];

		
		*normal = Normalize(CrossProduct(vec2, vec1));

	}
}

Model* GenerateTerrain(TextureData *tex)
{
	//init_perlin();
	//tex->width *= 1.5;
	//tex->height *= 1.5;
	texwidth = tex->width;
	
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;
	
	vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);
	//Point3D tmp_normal;
	vec3 tmp_normal;


	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			float height = 100 * OctavePerlin(x / (256.0 * 16 * 4 * 4), z / (256.0 * 16 * 4 * 4), 0, 10, 10.0) - 50;

			if (height > highest.y)
			{
				highest.x = x;
				highest.z = z;
				highest.y = height;
			}
			// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x;
			vertexArray[(x + z * tex->width) * 3 + 1] = height;
			vertexArray[(x + z * tex->width)*3 + 2] = z;

			// Normal vectors. You need to calculate these.
			calc_normal(vertexArray, x, z, tex->width, &tmp_normal);
			//printf("%f %f %f\n", tmp_normal.x, tmp_normal.y, tmp_normal.z);

			normalArray[(x + z * tex->width)*3 + 0] = tmp_normal.x;
			normalArray[(x + z * tex->width)*3 + 1] = tmp_normal.y;
			normalArray[(x + z * tex->width)*3 + 2] = tmp_normal.z;
			
			/*
			vec3 up = { 0, -1, 0 };
			float dot;
			
			dot = DotProduct(tmp_normal, up);
			if (fabs(dot) < .1 && height > 0.0 && x > 2 && z > 2 && x < tex->width-2 && z < tex->width-2)
			//if (fabs(dot) < .1 && fabs(dot) <= 1)
			{
				highest.x = x;
				highest.z = z;
				highest.y = height;
			}*/
			

		// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x/20.0; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z/20.0; // (float)z / tex->height;
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}
	
	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}

void draw_skybox()
{

	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform1i(glGetUniformLocation(program, "skybox"), 1);

	//curr_trans = &trans1;
	//glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE, myMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, look.m);

	GLfloat skyboxmat[16];
	memcpy(skyboxmat, camMatrix.m, sizeof(skyboxmat));
	skyboxmat[3] = 0;
	skyboxmat[7] = 0;
	skyboxmat[11] = 0;
	skyboxmat[15] = 1;

	//glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, skyboxmat);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, IdentityMatrix().m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, skyboxmat);
	glBindTexture(GL_TEXTURE_2D, tex1);
	DrawModel(skybox, program, "inPosition", "inNormal", "inTexCoord");

	glEnable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform1i(glGetUniformLocation(program, "skybox"), 0);
}

void randomize_landing_point()
{
	vec3 rand_pos = { 0, 0, 0 };
	//vec3 rand_pos = {random()*WIDTH, 0, random()*WIDTH};
	rand_pos.x = random()*CWIDTH;
	rand_pos.z = random()*CWIDTH;
	rand_pos.y = calc_height(vertexArray, rand_pos.x, rand_pos.z, CWIDTH) + 8;
	float dist_to_target = distance_to_target(&s, &rand_pos);
	while (!(rand_pos.x > 20 && rand_pos.z > 20 && rand_pos.z < CWIDTH - 20 && rand_pos.x < CWIDTH - 20 && rand_pos.y > 1 &&
		dist_to_target > 60))
	{
		rand_pos.x = random() * CWIDTH;
		rand_pos.z = random() * CWIDTH;
		rand_pos.y = calc_height(vertexArray, rand_pos.x, rand_pos.z, texwidth) + 8;
		dist_to_target = distance_to_target(&s, &rand_pos);
	}
	
	
	s.fuel_use = 3 / sqrt(dist_to_target);
	
	float test[3] = { rand_pos.x, rand_pos.y, rand_pos.z };
	set_landing_point(&lp, test);
}

void restart()
{
	game_over = 0;
	finished = 0;
	init_perlin();
	tm = GenerateTerrain(&ttex);
	create_spaceship(&s);
	randomize_landing_point();
	glutPostRedisplay();
}

void init(void)
{
	//LoadTGATextureSimple("SkyBox512.tga", &tex1);
	//init spaceship
	create_spaceship(&s);
	//init hud;
	create_hud(&h);
	//init landing point
	create_landing_point(&lp);

	//init clouds
	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		vec3 init_pos = { random()*CWIDTH, 70+random()*15, random()*CWIDTH };
		create_cloud(&c[i], init_pos);
	}

	//ground = LoadModelPlus("ground.obj");
	skybox = LoadModelPlus("sphere.obj");

	// GL inits
	//glClearColor(0.2,0.2,0.5,0);
	glClearColor(0.9,  1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_BLEND);
	printError("GL inits");

	float w = glutGet(GLUT_SCREEN_WIDTH);
	float h = glutGet(GLUT_SCREEN_HEIGHT);
	float ratio = w / h;
	projectionMatrix = frustum(-0.1*ratio, 0.1*ratio, -0.1, 0.1, 0.2, 750.0);

	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("sand.tga", &ground_tex);
	//LoadTGATextureSimple("water_text.tga", &water_tex);
	
	// Load terrain data
	LoadTGATextureData("fft-terrain.tga", &ttex);
	ttex.width *= 1.5;
	ttex.height *= 1.5;
	//LoadTGATextureData("SkyBox512.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

	
	randomize_landing_point();
	
	//upload highest point to shader
	glUniform3f(glGetUniformLocation(program, "highest"), highest.x, highest.y, highest.z);
}

void display(void)
{
	glUniform3f(glGetUniformLocation(program, "cam_vector"), s.pos[0] - cam.x, s.pos[1] - cam.y, s.pos[2] - cam.z);
	
	draw_skybox();

	// clear the screen
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform1i(glGetUniformLocation(program, "skybox"), 0);
	//glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	//glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	//glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	//glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
	
	printError("pre display");
	
	glUseProgram(program);
	
	//upload spaceship position to shader for shadow calculation
	glUniform3f(glGetUniformLocation(program, "spaceship_pos"), s.pos[0], s.pos[1], s.pos[2]);
	glUniform3f(glGetUniformLocation(program, "landing_point_pos"), lp.pos[0], lp.pos[1], (float)lp.pos[2]);

	//upload cam matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);

	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	//glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, IdentityMatrix().m);

	//draw terrain with water etc
	glUniform1i(glGetUniformLocation(program, "water"), 1);
	glBindTexture(GL_TEXTURE_2D, ground_tex);		// Bind Our Texture
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
	//glBindTexture(GL_TEXTURE_2D, ground_tex);		// Bind Our Texture
	//DrawModel(ground, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "water"), 0);

	
	//draw the landing point
	draw_landing_point(&lp, program);

	//draw the spaceship
	draw_spaceship(&s, program);

	draw_fuel_bar(&h, &s.fuel, program);

	if (game_over)
		draw_game_over(&h, program);

	if (finished)
		draw_you_win(&h, program);

	//draw clouds c
	for (int i = 0; i < NUM_CLOUDS; i++)
	{
		draw_cloud(&c[i], program);
	}
	//draw_cloud(&c[0], program);
	//draw_cloud(&c[1], program);
	//draw_cloud(&c[2], program);

	//dunno
	printError("display 2");
	
	glutSwapBuffers();
}

void handle_collisions()
{

	float h = calc_height(vertexArray, s.pos[0], s.pos[2], texwidth);
	if (s.pos[1] < 0.0)
		game_over = 1;


	if (s.pos[1] - 1.5 <= h)
	{
		s.pos[1] = h + 1.5;

		float tot_speed = fabs(spaceship_total_speed(&s));
		if (tot_speed > .03)
		{
			game_over = 1;
			return;
		}
	}
	if (s.pos[1] <= lp.pos[1]+2)
	{
		//s.gravity = 0;
		

		if (distance_to_target(&s, &lp.pos) < 15)
		{
			//check if the spaceship is not straight!
			if (fabs(s.angle[0]) > .15 && fabs(s.angle[1]) > .15)
			{
				//s.pos[1] = lp.pos.y;
				//if (fabs(spaceship_total_speed(&s)) < .03)
				//	finished = 1;
				game_over = 1;
				return;
			}
			if (fabs(spaceship_total_speed(&s)) > .2)
			{
				game_over = 1;
				return;
			}
			//too skewed
			//game_over = 1;
			finished = 1;
			return;

		}
		
		//else, game over!
		//finished = 1;
		/*
		s.speed[0] = 0;
		s.speed[1] = 0;
		s.speed[2] = 0;
		s.angle_speed[0] = 0;
		s.angle_speed[1] = 0;
		s.landed = 1;
		*/
	}
	else{
		s.landed = 0;
	}

}

void draw_welcome_screen()
{
	//draw_you_win(&h, program);
	camMatrix = T(0, 0, 0);
	s.fire_visible = 1;
	s.body_matrix = Mult(T(110, -80, -300), Mult(Mult(Rx(.5),Rz(-.5)),Ry(.5)));
	draw_spaceship(&s, program);
	s.fire_visible = 0;


	glUniform1i(glGetUniformLocation(program, "hud"), 1);
	//draw lunar lander sign
	mat4 total = Mult(Mult(T(0, 2, -8), S(.01, .01, .003)), Rx(.3));

	glUniform1i(glGetUniformLocation(program, "lunar_lander_sign"), 1);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	//glBindTexture(GL_TEXTURE_2D, h->game_over_sign_tex);
	DrawModel(h.lunar_lander_sign, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "lunar_lander_sign"), 0);

	total = Mult(Mult(T(0, -1, -8), S(.1, .1, .003)), Rx(-.1));
	glUniform1i(glGetUniformLocation(program, "enter_to_start_sign"), 1);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	//glBindTexture(GL_TEXTURE_2D, h->game_over_sign_tex);
	DrawModel(h.enter_to_start_sign, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "enter_to_start_sign"), 0);

	glUniform1i(glGetUniformLocation(program, "hud"), 0);
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	//upload time to shaders
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	glUniform1f(glGetUniformLocation(program, "time"), t);

	if (keyIsDown(VK_RETURN))
		start = 1;

	if (keyIsDown(VK_ESCAPE))
		glutExit();

	if (!start)
	{
		draw_welcome_screen();
		glutSwapBuffers();
		//glutPostRedisplay();
		
		return;
	}

	if (game_over)
	{
		freeze_spaceship(&s);
		glutPostRedisplay();

		//run game over handler here!
		if (keyIsDown(VK_RETURN))
			restart();
		return;
	}

	if (finished)
	{
		freeze_spaceship(&s);
		glutPostRedisplay();

		//run finished handler here!
		if (keyIsDown(VK_RETURN))
			restart();
		return;
	}

	//takes care of button presses and movement of spaceship
	move_spaceship(&s, program);

	//upload random variable for water noise
	//glUniform1f(glGetUniformLocation(program, "randwater"), (GLfloat)random());
	glUniform1f(glGetUniformLocation(program, "randwater"), random());


	//will be collision detection in the future!
	handle_collisions();

	//looks at the spaceship
	update_cam_matrix(&s, &camMatrix, &cam);

	
	glutPostRedisplay();

	
}


int main(int argc, char **argv)
{
	srand(time(NULL));
	init_perlin();
	glutInit(&argc, argv);
	//init with antialiasing (multisample)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GL_MULTISAMPLE);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (900, 600);
	glutCreateWindow ("TSBK07 - Project");
	//glutFullScreen();
	glutDisplayFunc(display);

	//init GLEW if windows
	#if defined(_WIN32)
		glewExperimental = GL_TRUE;

		GLenum err = glewInit();

		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
	#endif
	

	init ();
	initKeymapManager();
	glutTimerFunc(20, &timer, 0);
	
	glutMainLoop();
	exit(0);
}