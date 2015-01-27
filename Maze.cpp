// Triangle.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Cube.h"
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


Cube model;
GLFrustum           viewFrustum;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLuint vertbuffTorusID[1], normbuffTorusID[1], textbuffTorusID[1];

GLuint textureID;


GLuint shaderTorus;
GLint	locAmbientTorus;			
GLint   locDiffuseTorus;			
GLint   locSpecularTorus;		
GLint	locLightTorus;			
GLint	locMVPTorus;				
GLint	locMVTorus;				
GLint	locNMTorus;				
GLint   locTextureTorus;

GLFrame viewFrame;


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



GLuint loadShaderPair( char * vertsrc, char * fragsrc){
	GLuint program;
	GLuint vert;
	GLuint frag;
	GLint testVal;
	vert = glCreateShader(GL_VERTEX_SHADER);
	frag = glCreateShader(GL_FRAGMENT_SHADER);

	if(gltLoadShaderFile(vertsrc, vert) == false)
	{
		glDeleteShader(vert);
		glDeleteShader(frag);
		cout << "The shader at" << vert
			<< "could not be found.\n";
		return (GLuint)NULL;
	}
	// Fragment Program
	if(gltLoadShaderFile(fragsrc, frag) == false)
	{
		glDeleteShader(vert);
		glDeleteShader(frag);
		cout << "The shader at " << frag
			<< "could not be found.\n";
		return (GLuint)NULL;
	}
	glCompileShader(vert);
	glCompileShader(frag);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(vert, 1024, NULL, infoLog);
		cout << "The shader at " << vert
			<< "failed to compile with the following error:\n"
			<< infoLog << "\n";
		glDeleteShader(vert);
		glDeleteShader(frag);
		return (GLuint)NULL;
	}
	// Check for errors in fragment shader
	glGetShaderiv(frag, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(frag, 1024, NULL, infoLog);
		cout << "The shader at "<< frag
			<< "failed to compile with the following error:\n"
			<< infoLog << "\n";
		glDeleteShader(vert);
		glDeleteShader(frag);
		return (GLuint)NULL;
	}
	//	cout << vertsrc << endl;


	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);



	return program;
}



void SetupRC(/*HINSTANCE hInstance*/)
{
	
	glActiveTexture(GL_TEXTURE1);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	pBits = gltReadTGABits("CoolTexture.tga", &nWidth, &nHeight, &nComponents, &eFormat);
	 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
	glGenBuffers(1, vertbuffTorusID);
	glGenBuffers(1, normbuffTorusID);
	glGenBuffers(1, textbuffTorusID);


	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glEnable(GL_DEPTH_TEST);


	viewFrame.MoveForward(15.0f);
	viewFrame.MoveRight(.0f);
	viewFrame.MoveUp(0.0f);
	viewFrame.RotateLocalY(m3dDegToRad(180));

	
	model.init();
	model.bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, vertbuffTorusID[0], normbuffTorusID[0], textbuffTorusID[0]);
	

	shaderTorus = loadShaderPair("ADSTexture.vp", "ADSTexture.fp");

	glBindAttribLocation(shaderTorus, 0, "vVertex");
	glBindAttribLocation(shaderTorus, 1, "vNormal");
	glBindAttribLocation(shaderTorus, 2, "vTexture0");
	
	glLinkProgram(shaderTorus);
	int testVal;
	glGetProgramiv(shaderTorus, GL_LINK_STATUS, &testVal);


	locAmbientTorus = glGetUniformLocation(shaderTorus, "ambientColor");
	locDiffuseTorus = glGetUniformLocation(shaderTorus, "diffuseColor");
	locSpecularTorus = glGetUniformLocation(shaderTorus, "specularColor");
	locLightTorus = glGetUniformLocation(shaderTorus, "vLightPosition");
	locMVPTorus = glGetUniformLocation(shaderTorus, "mvpMatrix");
	locMVTorus  = glGetUniformLocation(shaderTorus, "mvMatrix");
	locNMTorus  = glGetUniformLocation(shaderTorus, "normalMatrix");
	locTextureTorus = glGetUniformLocation(shaderTorus, "colorMap");

	

}

void RenderScene(void)
{
	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	glEnable(GL_DEPTH_TEST);

	modelViewMatrix.PushMatrix(viewFrame);

	static GLfloat vLightPos [] = { 2.0f, 2.0f, 0.0f };
	GLfloat vWhite[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLfloat vEyeLight[] = {-100.0f, -100.0f, -100.0f};
	GLfloat vAmbientColor[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat vDiffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindTexture(GL_TEXTURE_2D, textureID);
	glUseProgram(shaderTorus);

	glUniform4fv(locAmbientTorus, 1, vAmbientColor);
	glUniform4fv(locDiffuseTorus, 1, vDiffuseColor);
	glUniform4fv(locSpecularTorus, 1, vSpecularColor);
	glUniform3fv(locLightTorus, 1, vEyeLight);
	glUniformMatrix4fv(locMVPTorus, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMVTorus, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNMTorus, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
	glUniform1i(locTextureTorus, 1);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertbuffTorusID[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,NULL);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normbuffTorusID[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, textbuffTorusID[0]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);


	glDrawArrays(GL_TRIANGLES, 0, model.total);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	modelViewMatrix.PopMatrix();

	glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
{
	if(key == GLUT_KEY_UP)
		
		viewFrame.MoveForward(.50f);
	if (key == GLUT_KEY_F1){
		viewFrame.MoveUp(.5f);
	}
	if (key == GLUT_KEY_F2){
		viewFrame.MoveUp(-.5f);
	}

	if(key == GLUT_KEY_F3){
		viewFrame.RotateLocal(m3dDegToRad(3.0f), 1.0f, .0f, 0.0f);
	}
	if(key == GLUT_KEY_DOWN)
		
		viewFrame.MoveForward(-.5f);

	if(key == GLUT_KEY_LEFT)
		viewFrame.RotateLocal(m3dDegToRad(3.0), 0.0f, 0.0f, 1.0f);
	
	if(key == GLUT_KEY_RIGHT)
		viewFrame.RotateLocal(m3dDegToRad(-3.0), 0.0f, 0.0f, 1.0f);
	
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


