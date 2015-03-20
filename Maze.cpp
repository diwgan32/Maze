// Triangle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Maze.h"
#include "Cube.h"
#include "Floor.h"
#include "SkyBox.h"
#include "Minimap.h"
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

#define SPEED_MULTIPLIER 1.01f
#define START_SPEED 0.02f
#define MAX_SPEED 0.05f
#define ROTATE_SPEED 0.1f

#define PROPORTION 300
#define START_WIDTH 940
#define START_HEIGHT 800

using namespace std;

Cube * model;
Floor * floorModel;
SkyBox * skyBox;
Minimap * mmap;

GLFrustum           viewFrustum;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;

GLfloat fLargest;

GLGeometryTransform transformPipeline;

float camera_position[] = {0, 0, 0};
float rot[] = {0, 0, 0};

GLFrame viewFrame;

int numBlocks = 0;

bool keys[256];
bool collide = false;
bool escDown, isMipmap, isAniso, canAniso;

int width, height;
int map[41][41];
int frame=0,time,timebase=0;
float fps;

float speed = 0;

void changeSize(int w, int h)
{
	if(h == 0)
		h = 1;

	width = w;
	height = h;

	glViewport(0, 0, w, h);

	viewFrustum.SetPerspective(30.0f, float(w)/float(h), .2f, 1000.0f);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	modelViewMatrix.LoadIdentity();

}

void setupRC(void/*HINSTANCE hInstance*/)
{
	glEnable(GL_DEPTH_TEST);

	if(gltIsExtSupported("GL_EXT_texture_filter_anisotropic")){
		canAniso = true;

		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	}

	BMP maze;
	maze.ReadFromFile("easybmp1.bmp");

	floorModel = new Floor();
	floorModel->init();
	floorModel->bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);

	skyBox = new SkyBox();
	skyBox->init();
	skyBox->bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);

	int size = 41;

	for(int i = 0; i<size; i++){
		for(int j = 0; j<size; j++){
			if(maze(i, j)->Red == 0){
				map[i][j] = 1;
				numBlocks++;
			}else{
				map[i][j] = 0;
			}
		}
	}
	numBlocks *= 2;




	model = new Cube[numBlocks];

	viewFrame.MoveForward(3.0f);
	viewFrame.MoveRight(.0f);
	viewFrame.MoveUp(0.0f);

	viewFrame.RotateLocalY(m3dDegToRad(180));

	int count = 0;

	for(int i = 0; i<size; i++){
		for(int j = 0; j<size; j++){
			if(map[i][j] == 1){
				for(int k = 0; k<2; k++){
					float offset[] = {i, k, j};
					model[count].init(offset);
					model[count].bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
					count++;
				}
			}
		}
	}

	float offset[] = {-3, 0, -3};
	mmap = new Minimap();
	mmap->init(offset);
	mmap->bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
}

void renderScene(void)
{
	collide = false;
	processSceneInfo();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	modelViewMatrix.PushMatrix();

	modelViewMatrix.Rotate(rot[0], 1.0, 0.0, 0.0);
	modelViewMatrix.Rotate(rot[2], 0.0, 1.0, 0.0);	

	float * camera_vector = new float[3];
	for(int i = 0; i<numBlocks; i++){
		if(keys['W'] || keys['w']){
			if(keys['d'] || keys['D']){
				if(-(camera_position[0] -speed*PROPORTION*sin((3.1415/180)*rot[2])*speed -speed*PROPORTION*cos((3.14/180)*rot[2])*speed) < model[i].hitBox.greatestX && -(camera_position[0]-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed-speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastX &&
					-(camera_position[2]+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestY && -(camera_position[2]+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastY
					)
				{
					camera_position[0] += sin((3.1415/180)*rot[2])*speed;
					camera_position[2] += -cos((3.1415/180)*rot[2])*speed;
					camera_position[0] += cos((3.14/180)*rot[2])*speed;
					camera_position[2] += sin((3.14/180)*rot[2])*speed;
					modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);

					collide = true;

				}
			}else if(keys['A'] || keys['a']){	
				if(-(camera_position[0] -speed*PROPORTION*sin((3.1415/180)*rot[2])*speed +speed*PROPORTION*cos((3.14/180)*rot[2])*speed) < model[i].hitBox.greatestX && -(camera_position[0]-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastX &&
					-(camera_position[2]+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed+speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestY && -(camera_position[2]+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed+speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastY
					)
				{
					camera_position[0] += sin((3.1415/180)*rot[2])*speed;
					camera_position[2] += -cos((3.1415/180)*rot[2])*speed;
					camera_position[0] -= cos((3.14/180)*rot[2])*speed;
					camera_position[2] -= sin((3.14/180)*rot[2])*speed;
					modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);

					collide = true;

				}
			}else{

				if(-(camera_position[0]-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestX && -(camera_position[0]-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastX &&
					-(camera_position[2]+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestY && -(camera_position[2]+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastY
					)
				{
					camera_position[0] += sin((3.1415/180)*rot[2])*speed;
					camera_position[2] += -cos((3.1415/180)*rot[2])*speed;

					modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);

					collide = true;

				}
			}

		}
		if(keys['s'] || keys['S']){
			if(-(camera_position[0]+speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestX && -(camera_position[0]-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastX &&
				-(camera_position[2]-speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestY && -(camera_position[2]-speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastY
				)
			{
				camera_position[0] += -sin((3.1415/180)*rot[2])*speed;
				camera_position[2] += cos((3.1415/180)*rot[2])*speed;


				modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
				collide = true;
				break;
			}
		}
		if(keys['A'] || keys['a']){
			if(-(camera_position[0]+speed*PROPORTION*cos((3.14/180)*rot[2])*speed) < model[i].hitBox.greatestX && -(camera_position[0]+speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastX &&
				-(camera_position[2]+speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestY && -(camera_position[2]+speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastY
				)
			{
				camera_position[0] += -cos((3.14/180)*rot[2])*speed;
				camera_position[2] += -sin((3.14/180)*rot[2])*speed;


				modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
				collide = true;

			}
		}
		if(keys['d'] || keys['D']){
			//	cout << "D" << endl;
			if(-(camera_position[0]-speed*PROPORTION*cos((3.14/180)*rot[2])*speed) < model[i].hitBox.greatestX && -(camera_position[0]-speed*PROPORTION*cos((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastX &&
				-(camera_position[2]-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) < model[i].hitBox.greatestY && -(camera_position[2]-speed*PROPORTION*sin((3.1415/180)*rot[2])*speed) > model[i].hitBox.leastY
				)
			{
				camera_position[0] += cos((3.14/180)*rot[2])*speed;
				camera_position[2] += sin((3.14/180)*rot[2])*speed;
				modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
				collide = true;

			}
		}
		if(collide) break;
	}
	if(!collide)
		modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);



	skyBox->draw(transformPipeline);
	floorModel->draw(transformPipeline);
	mmap->draw(transformPipeline);
	for(int i = 0; i<numBlocks; i++){
		model[i].draw(transformPipeline);
	}
	char topText[256];
	sprintf(topText, "FPS: %d", (int) fps);

	renderText(topText, 10, height - 20);

	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}

void renderText(char * p, int x, int y){
	double old_modelview[16];

	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX, old_modelview);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE); 

	glColor3f(0.0f, 1.0f, 0.0f); // Green
	glRasterPos2i(x, y);

	string s = p;
	void * font = GLUT_BITMAP_9_BY_15;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		//cout << c;
		glutBitmapCharacter(font, c);
	}

	//cout << endl;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(old_modelview);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void processSceneInfo(void){
	if(keyIn("wWsSaAdD")){
		if(speed == 0)
			speed = START_SPEED;
		else{
			speed *= SPEED_MULTIPLIER;
			if(speed > MAX_SPEED)
				speed = MAX_SPEED;
		}
		mmap->moveTexture(speed*100);
	}else{
		speed = 0;
	}
	if(keyIn("wW")){
		camera_position[0] += -sin((3.14/180)*rot[2])*speed;
		camera_position[2] += cos((3.14/180)*rot[2])*speed;
	}
	if(keyIn("sS")){
		camera_position[0] += sin((3.14/180)*rot[2])*speed;
		camera_position[2] += -cos((3.14/180)*rot[2])*speed;
	}
	if(keyIn("aA")){
		camera_position[0] += cos((3.14/180)*rot[2])*speed;
		camera_position[2] += sin((3.14/180)*rot[2])*speed;
	}
	if(keyIn("dD")){
		camera_position[0] += -cos((3.14/180)*rot[2])*speed;
		camera_position[2] += -sin((3.14/180)*rot[2])*speed;
	}

	if(keyIn("eE")){
		camera_position[1] -= speed;
	}
	if(keyIn("cC")){
		camera_position[1] += speed;
	}

	if(!escDown)
		glutWarpPointer(width / 2, height / 2);

	if(keyIn("mM"))
		if(isMipmap){
			for(int i = 0; i < 20*20; i++)
				model[i].onMipmap();
			floorModel->onMipmap();
		}else{
			for(int i = 0; i < 20*20; i++)
				model[i].offMipmap();
			floorModel->offMipmap();
		}

		if(keyIn("nN"))
			if(isAniso){
				for(int i = 0; i < 20*20; i++)
					model[i].onAniso(fLargest);
				floorModel->onAniso(fLargest);
			}else{
				for(int i = 0; i < 20*20; i++)
					model[i].offAniso();
				floorModel->offAniso();
			}
}

void downKeys(unsigned char key, int x, int y){
	keys[key] = true;

	if(key == 27){
		escDown = !escDown;
		glutSetCursor(escDown ? GLUT_CURSOR_LEFT_ARROW : GLUT_CURSOR_CROSSHAIR);
	}
	if(key == 'm')
		isMipmap = !isMipmap;
	if(key == 'n' && canAniso)
		isAniso = !isAniso;
}

void upKeys(unsigned char key, int x, int y){
	keys[key] = false;
}

bool keyIn(const char * string){
	for(int i = 0; i < strlen(string); i++)
		if(keys[string[i]]) return true;
	return false;
}

void mouseFunction(int x, int y){
	if(escDown){
		renderText("PAUSED", width/2, height/2);
		return;
	}

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

	glutCreateWindow("Maze");

	glutIdleFunc(countFPS);
	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(downKeys);
	glutKeyboardUpFunc(upKeys);
	glutPassiveMotionFunc(mouseFunction);

	glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	GLenum err = glewInit();

	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	setupRC();

	glutMainLoop();
	return 0;
}