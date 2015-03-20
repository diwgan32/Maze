// OpenGL SuperBible
// Demonstrates applying a cube map to an object (sphere) using
// and using the same map for the skybox.
// Program by Richard S. Wright Jr.

#include <GLTools.h>	// OpenGL toolkit
#include <GLMatrixStack.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <GLGeometryTransform.h>

#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

#include "stdafx.h"

#include "SkyBox.h"
#include "LoadShader.h"

#define SIZE 500

#define FLOAT_SZ sizeof(float)

int SkyBox::locMVP;
int SkyBox::locTexture;

GLuint SkyBox::shader, SkyBox::textureID;

// Six sides of a cube map
const char *szCubeFaces[6] = { "sky_texture/pos_x.tga", "sky_texture/neg_x.tga", "sky_texture/pos_y.tga", "sky_texture/neg_y.tga", "sky_texture/pos_z.tga", "sky_texture/neg_z.tga" };

GLenum  cube[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

SkyBox::SkyBox(){
}

SkyBox::~SkyBox(){
}

//////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering
// context. 
void SkyBox::init(void)
{
	GLbyte *pBytes;
	GLint iWidth, iHeight, iComponents;
	GLenum eFormat;
	int i;

	glEnable(GL_TEXTURE_CUBE_MAP);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// Set up texture maps        
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);       

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load Cube Map images
	for(i = 0; i < 6; i++)
	{
		pBytes = gltReadTGABits(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
		glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); 

	gltMakeCube(batch, SIZE);
}

void SkyBox::bind(GLenum buff_type, GLenum draw_type)
{
	shader = loadShaderPair("shaders/SkyBox.vp", "shaders/SkyBox.fp");

	glLinkProgram(shader);

	glBindAttribLocation(shader, 0, "vVertex");
	glBindAttribLocation(shader, 1, "vNormal");

	locMVP = glGetUniformLocation(shader, "mvpMatrix");
	locTexture = glGetUniformLocation(shader, "cubeMap");
}

// Called to draw scene
void SkyBox::draw(GLGeometryTransform transformPipeline)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glUniform1i(locTexture, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glUseProgram(shader);

	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());

	batch.Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);
}