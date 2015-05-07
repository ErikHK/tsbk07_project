#if defined(_WIN32)
    #include "windows.h"
#endif

#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "objects.h"
#include "LoadTGA.h"
#include <math.h>

//scale for terrain, higher means higher mountains 
#define SCALE 0.1

mat4 projectionMatrix;
mat4 total, modelView, camMatrix;
//cam position in world
vec3 cam = { 0, 30, 90 };

// Reference to shader program
GLuint program;

//textures
GLuint ground_tex;
TextureData ttex; // terrain

GLfloat *vertexArray;
int texwidth;

// terrain object
Model *tm;
//spaceship object
spaceship s;
//test cloud object
cloud c;

Point3D lightSourcesColorsArr[] = { { 1.0f, 1.0f, 1.0f },
{ 1.0f, 1.0f, 1.0f },
{ 1.0f, 1.0f, 1.0f },
{ 1.0f, 1.0f, 1.0f } };

GLfloat specularExponent[] = { 1.0, 20.0, 60.0, 5.0 };
GLint isDirectional[] = { 0, 0, 0, 1 };

Point3D lightSourcesDirectionsPositions[] = { { 0.0f, 5.0f, 0.0f }, // Red light, positional
{ 0.0f, 50.0f, 10.0f }, // Green light, positional
{ -1.0f, 2.0f, 0.0f }, // Blue light along X
{ 0.0f, 1.0f, -1.0f } }; // White light along Z


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
	Point3D vec1, vec2;

	if(x > 0 && z > 0 && x < width && z < width)
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

		
		*normal = Normalize(CrossProduct(vec1, vec2));

	}
}


Model* GenerateTerrain(TextureData *tex)
{
	texwidth = tex->width;
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;
	
	vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);
	Point3D tmp_normal;


	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
		// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] * SCALE;
			vertexArray[(x + z * tex->width)*3 + 2] = z;
		// Normal vectors. You need to calculate these.

			calc_normal(vertexArray, x, z, tex->width, &tmp_normal);
			//printf("%f %f %f\n", tmp_normal.x, tmp_normal.y, tmp_normal.z);


			normalArray[(x + z * tex->width)*3 + 0] = tmp_normal.x;
			normalArray[(x + z * tex->width)*3 + 1] = tmp_normal.y;
			normalArray[(x + z * tex->width)*3 + 2] = tmp_normal.z;
		// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
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

void init(void)
{
	//init spaceship
	create_spaceship(&s);
	//init test cloud
	create_cloud(&c);

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 750.0);

	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("maskros512.tga", &ground_tex);
	
	// Load terrain data
	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniform1i(glGetUniformLocation(program, "skybox"), 0);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
	
	printError("pre display");
	
	glUseProgram(program);
	
	//upload spaceship position to shader for shadow calculation
	glUniform3f(glGetUniformLocation(program, "spaceship_pos"), s.pos[0], s.pos[1], s.pos[2]);

	//upload cam matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);

	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	
	//draw terrain with water etc
	glUniform1i(glGetUniformLocation(program, "water"), 1);
	glBindTexture(GL_TEXTURE_2D, ground_tex);		// Bind Our Texture
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
	glUniform1i(glGetUniformLocation(program, "water"), 0);

	//takes care of button presses and movement of spaceship
	move_spaceship(&s);

	//draw the spaceship
	draw_spaceship(&s, &camMatrix, program);

	//draw cloud c
	draw_cloud(&c, &camMatrix, program);

	//dunno
	printError("display 2");
	
	glutSwapBuffers();
}

void timer(int i)
{
	//will be collision detection in the future
	float h = calc_height(vertexArray, s.pos[0], s.pos[2], texwidth);
	if (s.pos[1] <= h)
		s.pos[1] = h;

	//looks at the spaceship
	update_cam_matrix(&s, &camMatrix, &cam);

	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	//init with antialiasing (multisample)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 - Project");
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