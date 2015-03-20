#ifndef MINIMAP_H
#define MINIMAP_H

#include "stdafx.h"

using namespace std;

class Minimap
{
public:
	Minimap(void);
	~Minimap(void);

	void init(float[]);
	void bind(GLenum, GLenum);
	void draw(GLGeometryTransform);

	void onMipmap();
	void offMipmap();

	void onAniso(GLfloat);
	void offAniso();
	void moveTexture(int);

private:
	static int locAmbient, locDiffuse, locSpecular, locEyeLight, locLight, locTexture, locNormal;
	static int locMVP, locMV, locNM;
	static GLbyte * pBits;
	typedef float* Array;

	int numFrame;

		static GLuint shader, textureID, normalID;
	static int nWidth, nHeight, nComponents;
	
	static GLenum eFormat;
	static bool readTexture, readShader;

	Array final_vert, final_text, norm_final;

	GLuint vertbuffID[1], normbuffID[1], texbuffID[1];

	int total, vsize, nsize, tsize; 
	GLBatch batch;
};

#endif