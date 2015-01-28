#ifndef CUBE_H
#define CUBE_H

#include "stdafx.h"

using namespace std;

class Cube
{
public:
	typedef float * Array;

	//static 
	int locAmbient, locDiffuse, locSpecular, locEyeLight, locLight, locTexture;
	//static 
	int locMVP, locMV, locNM;

	int numFrame;

	GLuint shader, textureID;

	static bool readTexture;//, readShader;

	Array final_vert, final_text, norm_final;

	GLuint vertbuffID[1], normbuffID[1], texbuffID[1];

	int total, vsize, nsize, tsize; 

	Cube(void);

	static GLuint loadShaderPair(char *, char *);

	void init(float[]);
	void bind(GLenum, GLenum);
	void draw(GLGeometryTransform);
};

#endif