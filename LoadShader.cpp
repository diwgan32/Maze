#include <GLTools.h>	// OpenGL toolkit

#include "stdafx.h"

GLuint loadShaderPair(char * vertsrc, char * fragsrc){
	GLuint program;
	GLuint vert;
	GLuint frag;
	GLint testVal;

	vert = glCreateShader(GL_VERTEX_SHADER);
	frag = glCreateShader(GL_FRAGMENT_SHADER);

	// Vertex Program
	if(gltLoadShaderFile(vertsrc, vert) == false)
	{
		glDeleteShader(vert);
		glDeleteShader(frag);

		std::cout << "The shader at" << vert
			<< "could not be found.\n";

		return (GLuint)NULL;
	}

	// Fragment Program
	if(gltLoadShaderFile(fragsrc, frag) == false)
	{
		glDeleteShader(vert);
		glDeleteShader(frag);

		std::cout << "The shader at " << frag
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

		std::cout << "The shader at " << vert
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

		std::cout << "The shader at "<< frag
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