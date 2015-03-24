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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

#define SPEED_MULTIPLIER 1.01f
#define START_SPEED 1.5f
#define MAX_SPEED 3.5f
#define ROTATE_SPEED 0.1f
#define START_JUMP_SPEED 5.0f
#define GRAVITY -8.0f
#define CAMERA_FLOOR -0.5f

// in ms
#define TICK 20

#define PROPORTION 30
#define START_WIDTH 1024
#define START_HEIGHT 1024
#define DEG_TO_RAD (3.1415/180)
#define MAZE_SIZE 41

using namespace Maze;

namespace Maze{

	Cube * model;
	Floor * floorModel;
	SkyBox * skyBox;
	Minimap * mmap;
	float minimapOffsetX = .4;
	float minimapOffsetY = .9;
	GLFrustum           viewFrustum;
	GLMatrixStack       modelViewMatrix;
	GLMatrixStack       projectionMatrix;
	float matrix1[16];
	GLMatrixStack       modelViewMatrix_minimap;
	GLfloat fLargest;

	GLGeometryTransform transformPipeline, transformPipeline_minimap;

	float camera_position[] = {-18, CAMERA_FLOOR, 6};
	float rot[] = {-6.4, 0, 177};

	GLFrame viewFrame, viewFrame_minimap;

	int numBlocks = 0;

	bool keys[256];
	bool collide = false;
	bool escDown, isMipmap, isAniso, canAniso;
	bool chMipmap, chAniso;
	bool running;

	int width, height;
	int map[MAZE_SIZE][MAZE_SIZE];
	int frame=0,time,timebase=0;
	float fps;

	float vxz = 0, vy = 0;

	boost::thread * psiThread;

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
		transformPipeline_minimap.SetMatrixStacks(modelViewMatrix_minimap, projectionMatrix);
		modelViewMatrix_minimap.LoadIdentity();
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

		for(int i = 0; i<MAZE_SIZE; i++){
			for(int j = 0; j<MAZE_SIZE; j++){
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

		//viewFrame.TranslateLocal(18, 0, 1);

		//viewFrame.RotateLocalY(m3dDegToRad(180));

		int count = 0;

		for(int i = 0; i<MAZE_SIZE; i++){
			for(int j = 0; j<MAZE_SIZE; j++){
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

//			viewFrame.TranslateLocal(camera_position[0], camera_position[1], camera_position[2]-5);
				viewFrame.TranslateLocal(19.935, -1.435, -4.75);
			//cout << -camera_position[0] << endl;
		mmap = new Minimap();
		mmap->init(Minimap::offset, .3);
		mmap->bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);

	}

	void renderScene(void)
	{
		collide = false;
		//processSceneInfo();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		modelViewMatrix.PushMatrix();
		modelViewMatrix.Rotate(rot[0], 1.0, 0.0, 0.0);
		modelViewMatrix.Rotate(rot[2], 0.0, 1.0, 0.0);	

		float * camera_vector = new float[3];

		//	cout << rot[0] << " " << rot[1] <<" " << rot[2]<< endl;
		modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);

		// Toggle mipmaps/anisotropic filtering if changed
		if(chMipmap){
			if(isMipmap){
				for(int i = 0; i < numBlocks; i++)
					model[i].onMipmap();
				floorModel->onMipmap();
			}else{
				for(int i = 0; i < numBlocks; i++)
					model[i].offMipmap();
				floorModel->offMipmap();
			}
			chMipmap = false;
		}
		if(chAniso){
			if(isAniso){
				for(int i = 0; i < numBlocks; i++)
					model[i].onAniso(fLargest);
				floorModel->onAniso(fLargest);
			}else{
				for(int i = 0; i < numBlocks; i++)
					model[i].offAniso();
				floorModel->offAniso();
			}
			chAniso = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		floorModel->draw(transformPipeline);

		for(int i = 0; i<numBlocks; i++){
			model[i].draw(transformPipeline);
		}
		char topText[256];
		sprintf(topText, "FPS: %d", (int) fps);

		renderText(topText, 10, height - 20);


		modelViewMatrix.PopMatrix();
	
		modelViewMatrix.PushMatrix(viewFrame);
			
			modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]-1);
			mmap->draw(transformPipeline, minimapOffsetX, minimapOffsetY);
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

	void procThread(void){

		SLEEP(1000);

		while(running){
			processSceneInfo();
			SLEEP(TICK);
		}
	}

	void processSceneInfo(void){
		float dxz = 0, dy = 0;

		if(keyIn("wWsSaAdDeEcC")){
			if(vxz == 0){
				vxz = START_SPEED;
			}else{
				vxz *= SPEED_MULTIPLIER;

				if(vxz > MAX_SPEED)
					vxz = MAX_SPEED;
			}
			dxz = (1.0f/TICK) * vxz;

		//	mmap->moveTexture(dxz*100);
			//mmap->draw(transformPipeline);
		}else{
			vxz = 0;
		}

		// For the very, very strange condition that up is -y
		if(keys[' '] && vy == 0)
			vy = -START_JUMP_SPEED;

		vy += -((1.0f/TICK) * GRAVITY);
		dy = (1.0f/TICK) * vy;

		if(keyIn("wW")){
			camera_position[0] += -sin(DEG_TO_RAD*rot[2])*dxz;
			camera_position[2] += cos(DEG_TO_RAD*rot[2])*dxz;
			minimapOffsetX += sin(DEG_TO_RAD*rot[2])*dxz*.02;
			minimapOffsetY += cos(DEG_TO_RAD*rot[2])*dxz*.02;
		
			viewFrame.TranslateLocal(sin(DEG_TO_RAD*rot[2])*dxz, 0, -cos(DEG_TO_RAD*rot[2])*dxz);
		}
		if(keyIn("sS")){
			camera_position[0] += sin(DEG_TO_RAD*rot[2])*dxz;
			camera_position[2] += -cos(DEG_TO_RAD*rot[2])*dxz;
			minimapOffsetX += -sin(DEG_TO_RAD*rot[2])*dxz*.02;
			minimapOffsetY += -cos(DEG_TO_RAD*rot[2])*dxz*.02;
			viewFrame.TranslateLocal(-sin(DEG_TO_RAD*rot[2])*dxz, 0, cos(DEG_TO_RAD*rot[2])*dxz);
		}
		if(keyIn("aA")){
			camera_position[0] += cos(DEG_TO_RAD*rot[2])*dxz;
			camera_position[2] += sin(DEG_TO_RAD*rot[2])*dxz;
			minimapOffsetX += -cos(DEG_TO_RAD*rot[2])*dxz*.02;
			minimapOffsetY += sin(DEG_TO_RAD*rot[2])*dxz*.02;
			
			viewFrame.TranslateLocal(-cos(DEG_TO_RAD*rot[2])*dxz, 0, -sin(DEG_TO_RAD*rot[2])*dxz);
		}
		if(keyIn("dD")){
			camera_position[0] += -cos(DEG_TO_RAD*rot[2])*dxz;
			camera_position[2] += -sin(DEG_TO_RAD*rot[2])*dxz;
			minimapOffsetX += cos(DEG_TO_RAD*rot[2])*dxz*.02;
			minimapOffsetY += -sin(DEG_TO_RAD*rot[2])*dxz*.02;
			
			viewFrame.TranslateLocal(cos(DEG_TO_RAD*rot[2])*dxz, 0, sin(DEG_TO_RAD*rot[2])*dxz);
		}

		if(camera_position[1] + dy <= CAMERA_FLOOR)
			camera_position[1] += dy;
		else{
			if(!keys[' '])
				vy = 0;
			camera_position[1] = CAMERA_FLOOR;
		}

		if(!escDown)
			glutWarpPointer(width / 2, height / 2);

		for(int i = 0; i<numBlocks; i++){
			if(keyIn("wW")){
				if(keyIn("dD")){
					if(-(camera_position[0] -dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz -dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
						-(camera_position[2]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
					{
						float tempX = camera_position[0];
						float tempY = camera_position[2];
						camera_position[0] += (float) (sin(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) (-cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[0] += (float) (cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) (sin(DEG_TO_RAD*rot[2])*dxz);
						minimapOffsetX -= (float) (sin(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) (cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetX -= (float) (cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) (-sin(DEG_TO_RAD*rot[2])*dxz*.02);
						viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						//	modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);

						collide = true;
						break;

					}
				}else if(keyIn("aA")){	
					if(-(camera_position[0] -dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz +dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
						-(camera_position[2]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz+dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz+dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
					{
						float tempX = camera_position[0];
						float tempY = camera_position[2];
						camera_position[0] += (float) (sin(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) (-cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[0] += -(float) (cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += -(float) (sin(DEG_TO_RAD*rot[2])*dxz);
						minimapOffsetX -= (float) (sin(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) (cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetX -= (float) -(cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) (sin(DEG_TO_RAD*rot[2])*dxz*.02);
						//modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
						viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						collide = true;
						break;
					}
				}else{

					if(-(camera_position[0]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
						-(camera_position[2]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
					{
						float tempX = camera_position[0];
						float tempY = camera_position[2];
						camera_position[0] += (float) (sin(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) (-cos(DEG_TO_RAD*rot[2])*dxz);
						minimapOffsetX -= (float) (sin(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) (cos(DEG_TO_RAD*rot[2])*dxz*.02);
						viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						
						//modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);

						collide = true;
						break;
					}
				}
			}
			if(keyIn("sS")){
				if(keyIn("dD")){
					if(-(camera_position[0] +dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz -dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
						-(camera_position[2]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
					{
						float tempX = camera_position[0];
						float tempY = camera_position[2];
						camera_position[0] += -(float) (sin(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += -(float) (-cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[0] += (float) (cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) (sin(DEG_TO_RAD*rot[2])*dxz);
						minimapOffsetX -= -(float) -(sin(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= -(float) -(cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetX -= (float) (cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) (-sin(DEG_TO_RAD*rot[2])*dxz*.02);
						//	modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
						viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						
						collide = true;
						break;

					}
				}else if(keyIn("aA")){	
					if(-(camera_position[0] +dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz +dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
						-(camera_position[2]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz+dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz+dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
					{
						float tempX = camera_position[0];
						float tempY = camera_position[2];
						camera_position[0] += (float) -(sin(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) -(-cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[0] += (float) -(cos(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) -(sin(DEG_TO_RAD*rot[2])*dxz);
						minimapOffsetX -= (float) -(sin(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) -(cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetX -= (float) -(cos(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) (sin(DEG_TO_RAD*rot[2])*dxz*.02);
						//modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
						viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						
						collide = true;
						break;
					}
				}else{

					if(-(camera_position[0]+dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
						-(camera_position[2]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
					{
						float tempX = camera_position[0];
						float tempY = camera_position[2];
						camera_position[0] += (float) (-sin(DEG_TO_RAD*rot[2])*dxz);
						camera_position[2] += (float) (cos(DEG_TO_RAD*rot[2])*dxz);
						minimapOffsetX -= (float) -(sin(DEG_TO_RAD*rot[2])*dxz*.02);
						minimapOffsetY -= (float) -(cos(DEG_TO_RAD*rot[2])*dxz*.02);

						viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						
						//modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
						collide = true;
						break;
					}
				}

			}
			if(keyIn("aA")){
				if(-(camera_position[0]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]+dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
					-(camera_position[2]+dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]+dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
				{
					float tempX = camera_position[0];
						float tempY = camera_position[2];
					camera_position[0] += (float) (-cos(DEG_TO_RAD*rot[2])*dxz);
					camera_position[2] += (float) (-sin(DEG_TO_RAD*rot[2])*dxz);
					minimapOffsetX -= (float) -(cos(DEG_TO_RAD*rot[2])*dxz*.02);
					minimapOffsetY -= (float) -(sin(DEG_TO_RAD*rot[2])*dxz*.02);
					viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						
					//modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
					collide = true;
					break;
				}
			}
			if(keyIn("dD")){
				//	cout << "D" << endl;
				if(-(camera_position[0]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestX && -(camera_position[0]-dxz*PROPORTION*cos(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastX &&
					-(camera_position[2]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) < model[i].hitBox.greatestY && -(camera_position[2]-dxz*PROPORTION*sin(DEG_TO_RAD*rot[2])*dxz) > model[i].hitBox.leastY)
				{
					float tempX = camera_position[0];
						float tempY = camera_position[2];
					camera_position[0] += (float) (cos(DEG_TO_RAD*rot[2])*dxz);
					camera_position[2] += (float) (sin(DEG_TO_RAD*rot[2])*dxz);
					minimapOffsetX -= (float) (cos(DEG_TO_RAD*rot[2])*dxz*.02);
					minimapOffsetY -= (float) (sin(DEG_TO_RAD*rot[2])*dxz*.02);
					//modelViewMatrix.Translate(camera_position[0], camera_position[1], camera_position[2]);
					viewFrame.TranslateLocal(-(camera_position[0]-tempX), 0, -(camera_position[2]-tempY));
						
					collide = true;

				}
			}
			if(collide) break;
		}
			//
	}

	void downKeys(unsigned char key, int x, int y){
		keys[key] = true;

		if(key == 27){
			escDown = !escDown;
			glutSetCursor(escDown ? GLUT_CURSOR_LEFT_ARROW : GLUT_CURSOR_CROSSHAIR);
		}
		if(keyIn("mM")){
			chMipmap = true;
			isMipmap = !isMipmap;
		}
		if(keyIn("nN") && canAniso){
			chAniso = true;
			isAniso = !isAniso;
		}
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

	void shutdown(void){
		running = false;
		psiThread->join();
		SAFE_DELETE(psiThread);
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

	atexit(Maze::shutdown);

	GLenum err = glewInit();

	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}

	running = true;
	psiThread = new boost::thread(procThread);

	setupRC();

	glutMainLoop();
	return 0;
}