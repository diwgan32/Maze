// Triangle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
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

using namespace std;


Cube *model;
GLFrustum           viewFrustum;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
float camera_position[] = {0, 0, 0};
float rot = 0;
GLFrame viewFrame;

	int numBlocks = 0;
void ChangeSize(int w, int h)
{
	
	if(h == 0)
		h = 1;

	
	glViewport(0, 0, w, h);

	viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 1000.0f);

	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	modelViewMatrix.LoadIdentity();
	
}





void SetupRC(/*HINSTANCE hInstance*/)
{
	


	glEnable(GL_DEPTH_TEST);

	BMP maze;
	maze.ReadFromFile("easybmp.bmp");
	int map[20][20];
	int size = 20;
	for(int i = 0; i<size; i++){
		for(int j = 0; j<size; j++){
			if(maze(i, j)->Red == 0){
				map[i][j] = 1;
				numBlocks ++;
			}else{
				map[i][j] = 0;
			}
		}
	}
	numBlocks *= 5;
	model = new Cube[numBlocks];
	viewFrame.MoveForward(3.0f);
	viewFrame.MoveRight(.0f);
	viewFrame.MoveUp(0.0f);
	viewFrame.RotateLocalY(m3dDegToRad(180));
	int count = 0;
	cout << numBlocks << endl;
	for(int i = 0; i<size; i++){
		for(int j = 0; j<size; j++){
			if(map[i][j] == 1){
				for(int k = 0; k<5; k++){
				float offset[] = {i, k, j};
				model[count].init(offset);
				model[count].bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
				count++;
				}
			}
		}
	}

}

void RenderScene(void)
{
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glEnable(GL_DEPTH_TEST);

	modelViewMatrix.PushMatrix(viewFrame);
	modelViewMatrix.Translate(camera_position[0]-15, camera_position[1]-5, camera_position[2]-15);
	modelViewMatrix.Rotate(rot, 0.0, 1.0, 0.0);
	for(int i = 0; i<numBlocks; i++){
	model[i].draw(transformPipeline);
	}
	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
{
	if(key == GLUT_KEY_UP){
		
		camera_position[2]+=.5;
	}

	if(key == GLUT_KEY_DOWN){
		camera_position[2]-=.5;
	}
	if(key == GLUT_KEY_LEFT){
		camera_position[0]+=.5;
	}
	if(key == GLUT_KEY_RIGHT){
			camera_position[0]-=.5;
	}
	glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(940, 800);
	glutCreateWindow("Triangle");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKeys);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
	}
	
	SetupRC();
	
	glutMainLoop();
	return 0;
}

