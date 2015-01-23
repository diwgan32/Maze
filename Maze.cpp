// Triangle.cpp
// Our first OpenGL program that will just draw a triangle on the screen.
#include "stdafx.h"
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
GLBatch	triangleBatch;
GLShaderManager	shaderManager;

GLint	myIdentityShader;
GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f, 
		                  0.5f, 0.0f, 0.0f,
						  0.0f, 0.5f, 0.0f };
///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h)
    {
	glViewport(0, 0, w, h);
    }


///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context. 
// This is the first opportunity to do any OpenGL related tasks.
GLuint loadShaderPair(/*GLuint * vert, GLuint * frag,*/ char * vertsrc, char * fragsrc){
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
	cout << vertsrc << endl;
	

	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);

	

	return program;
}
void SetupRC()
	{
	// Blue background
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f );
    
	shaderManager.InitializeStockShaders();

	myIdentityShader = loadShaderPair("shaders\\Identity.vp", "shaders\\Identity.fp");
	}


///////////////////////////////////////////////////////////////////////////////
// Cleanup
void ShutdownRC()
   {
   glDeleteProgram(myIdentityShader);

   }


///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVerts);
	glBindAttribLocation(myIdentityShader, 0, "vVertex");
	glLinkProgram(myIdentityShader);
	glUseProgram(myIdentityShader);

	GLint iColorLocation = glGetUniformLocation(myIdentityShader, "vColor");
	GLfloat vColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	glUniform4fv(iColorLocation, 1, vColor);
	glDrawArrays(GL_TRIANGLES, 0, 3);


	// Perform the buffer swap to display back buffer
	glutSwapBuffers();
	}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[])
	{
	gltSetWorkingDirectory(argv[0]);
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Triangle");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return 1;
		}
	
	SetupRC();

	glutMainLoop();

	ShutdownRC();

	return 0;
	}
