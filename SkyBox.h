#ifndef SKYBOX_H
#define SKYBOX_H

#include "stdafx.h"

using namespace std;

class SkyBox
{
public:
	SkyBox(void);
	~SkyBox(void);

	void init();
	void bind(GLenum, GLenum);
	void draw(GLGeometryTransform);

	void onMipmap();
	void offMipmap();

	void onAniso(GLfloat);
	void offAniso();
private:
	static int locTexture;
	static int locMVP;

	typedef float* Array;

	static GLuint shader, textureID;

	Array final_vert, final_text;

	GLuint vertbuffID[1], texbuffID[1];
};

#endif