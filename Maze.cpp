// Triangle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Maze.h"
#include "Cube.h"

#include "EasyBMP\EasyBMP.h"

#include <GLTools.h>            // OpenGL toolkit
#include <GLShaderManager.h>    // Shader Manager Class

#include <iostream>

#ifdef __APPLE__
#include <glut/glut.h>          // OS X version of GLUT
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>            // Windows FreeGlut equivalent
#endif

#define MOVE_SPEED 0.02f
#define ROTATE_SPEED 0.1f

#define START_WIDTH 940
#define START_HEIGHT 800

using namespace std;

Cube * model;

GLFrustum           viewFrustum;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;

GLGeometryTransform transformPipeline;

float camera_position[] = {0, 0, 0};
float rot[] = {0, 0, 0};

GLFrame viewFrame;

int numBlocks = 0;

bool keys[256];

bool escDown, isMipmap;

int width, height;

int frame=0,time,timebase=0;
float fps;

void changeSize(int w, int h)
{
	if(h == 0)
		h = 1;

	width = w;
	height = h;

	glViewport(0, 0, w, h);

	viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 1000.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	modelViewMatrix.LoadIdentity();

}

void setupRC(void/*HINSTANCE hInstance*/)
{
	glEnable(GL_DEPTH_TEST);

	BMP maze;
	maze.ReadFromFile("easybmp.bmp");

	int map[20][20];
	int size = 20;

	for(int i = 0; i<size; i++)
		for(int j = 0; j<size; j++)
			if(maze(i, j)->Red == 0){
				map[i][j] = 1;
				numBlocks++;
			}else
				map[i][j] = 0;

			numBlocks *= 5;

			model = new Cube[numBlocks];

			viewFrame.MoveForward(3.0f);
			viewFrame.MoveRight(.0f);
			viewFrame.MoveUp(0.0f);

			viewFrame.RotateLocalY(m3dDegToRad(180));

			int count = 0;

			// cout << numBlocks << endl;

			for(int i = 0; i<size; i++)
				for(int j = 0; j<size; j++)
					if(map[i][j] == 1)
						for(int k = 0; k<5; k++){
							float offset[] = {i, k, j};
							model[count].init(offset);
							model[count].bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
							count++;
						}
}

void renderScene(void)
{
	processSceneInfo();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	modelViewMatrix.PushMatrix();

	modelViewMatrix.Rotate(rot[0], 1.0, 0.0, 0.0);
	modelViewMatrix.Rotate(rot[2], 0.0, 1.0, 0.0);

	modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
	//modelViewMatrix.Translate(camera_position[0]-15, camera_position[1]-5, camera_position[2]-15);

	for(int i = 0; i<numBlocks; i++)
		model[i].draw(transformPipeline);

	renderText("FPS: ");

	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
}

void renderText(char * p){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(0.0, 1.0, 0.0); // Green

	glRasterPos2i(10, 10);

	string s = p;
	void * font = GLUT_BITMAP_9_BY_15;

	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(font, c);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void processSceneInfo(void){
	if(keys['w'] || keys['W']){
		camera_position[0] += -sin((3.14/180)*rot[2])*MOVE_SPEED;
		camera_position[2] += cos((3.14/180)*rot[2])*MOVE_SPEED;
	}
	if(keys['s'] || keys['S']){
		camera_position[0] += sin((3.14/180)*rot[2])*MOVE_SPEED;
		camera_position[2] += -cos((3.14/180)*rot[2])*MOVE_SPEED;
	}
	if(keys['a'] || keys['A']){
	camera_position[0] += cos((3.14/180)*rot[2])*MOVE_SPEED;
		camera_position[2] += sin((3.14/180)*rot[2])*MOVE_SPEED;
	}
	if(keys['d'] || keys['D']){
		camera_position[0] += -cos((3.14/180)*rot[2])*MOVE_SPEED;
		camera_position[2] += -sin((3.14/180)*rot[2])*MOVE_SPEED;
	}
	if(keys['a'] || keys['A'])
		camera_position[0] += MOVE_SPEED;
	if(keys['d'] || keys['D'])
		camera_position[0] -= MOVE_SPEED;
	if(keys['e'] || keys['E'])
		camera_position[1] -= MOVE_SPEED;
	if(keys['c'] || keys['C'])
		camera_position[1] += MOVE_SPEED;

	if(!escDown)
		glutWarpPointer(width / 2, height / 2);

	if(isMipmap)
		for(int i = 0; i < 20*20; i++)
			model[i].onMipmap();
	else
		for(int i = 0; i < 20*20; i++)
			model[i].offMipmap();
	num++;
	num%=10;
	glutPostRedisplay();

}

void downKeys(unsigned char key, int x, int y){
	keys[key] = true;

	if(key == 27)
		escDown = !escDown;
	if(key == 'm')
		isMipmap = !isMipmap;
}

void upKeys(unsigned char key, int x, int y){
	keys[key] = false;
}

void mouseFuction(int x, int y){
	if(escDown)
		return;

	float new_x = x-(width/2);
	float new_y = y-(height/2);

	//if (abs(new_x) < 100 && abs(new_y) < 100){
	//	rot[0] = 0;
	//	rot[2] = 0;
	//}else{

	rot[0] += new_y*ROTATE_SPEED;
	rot[2] += new_x*ROTATE_SPEED;
	//}

}

void countFPS(){
	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		fps = (float)(frame*1000.0/(time-timebase));
	 	timebase = time;
		frame = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char * argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(START_WIDTH, START_HEIGHT);

	glutCreateWindow("Triangle");

	glutIdleFunc(countFPS);
	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(downKeys);
	glutKeyboardUpFunc(upKeys);
	glutPassiveMotionFunc(mouseFuction);

	GLenum err = glewInit();

	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	setupRC();

	glutMainLoop();
	return 0;
}