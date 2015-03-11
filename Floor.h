#ifndef FLOOR_H
#define FLOOR_H

#include "stdafx.h"

using namespace std;

class Floor
{
public:
	Floor(void);
	~Floor(void);

	void init();
	void bind(GLenum, GLenum);
	void draw(GLGeometryTransform);

	void onMipmap();
	void offMipmap();

	void onAniso(GLfloat);
	void offAniso();

private:
	typedef float * Array;

	static int locAmbient, locDiffuse, locSpecular, locEyeLight, locLight, locTexture, locNormal;
	static int locMVP, locMV, locNM;

	int numFrame;

	static GLuint shader, textureID, normalID;

	static bool readTexture, readShader;

	Array final_vert, final_text, norm_final;

	GLuint vertbuffID[1], normbuffID[1], texbuffID[1];

	int total, vsize, nsize, tsize; 
};

#endif