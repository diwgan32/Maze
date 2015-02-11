#ifndef CUBE_H
#define CUBE_H

#include "stdafx.h"

using namespace std;

struct box{
	float greatestX;
	float leastX;
	float greatestY;
	float leastY;
};
class Cube
{
public:
	Cube(void);
	~Cube(void);

	void init(float[]);
	void bind(GLenum, GLenum);
	void draw(GLGeometryTransform);

	void onMipmap();
	void offMipmap();

	void onAniso(GLfloat);
	void offAniso();
	box hitBox;
private:
	static GLuint loadShaderPair(char *, char *);

	typedef float * Array;

	static int locAmbient, locDiffuse, locSpecular, locEyeLight, locLight, locTexture;
	static int locMVP, locMV, locNM;

	int numFrame;

	static GLuint shader, textureID;

	static bool readTexture, readShader;

	Array final_vert, final_text, norm_final;

	GLuint vertbuffID[1], normbuffID[1], texbuffID[1];

	int total, vsize, nsize, tsize; 
};

#endif