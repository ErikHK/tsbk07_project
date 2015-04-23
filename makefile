all : project

project: project.c GL_utilities.c VectorUtils3.c loadobj.c LoadTGA.c MicroGlut.c
	gcc -DGL_GLEXT_PROTOTYPES project.c GL_utilities.c VectorUtils3.c loadobj.c LoadTGA.c MicroGlut.c -std=c99 -lGL -lglut -lX11 -lm -o project

clean :
	rm -f project
