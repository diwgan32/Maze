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

#define VERT_TOTAL 24

#define SKYBOX_V_SIZE (VERT_TOTAL * 3)

#define V_COORD_SZ 3

#define FLOAT_SZ sizeof(float)

int SkyBox::locMVP;
int SkyBox::locTexture;

GLuint SkyBox::shader, SkyBox::textureID;

// Six sides of a cube map
const char *szCubeFaces[6] = { "pos_x.tga", "neg_x.tga", "pos_y.tga", "neg_y.tga", "pos_z.tga", "neg_z.tga" };

GLenum  cube[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

SkyBox::SkyBox(){
	final_vert = new float[SKYBOX_V_SIZE];
}

SkyBox::~SkyBox(){
	delete final_vert;
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

	glActiveTexture(GL_TEXTURE2);
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
		// Load this texture map
		pBytes = gltReadTGABits(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
		glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
		free(pBytes);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); 

	glGenBuffers(1, vertbuffID);

	const float VERTS[] = {0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1
		, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 
		0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 
		1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0};

	copy_n(VERTS, sizeof(VERTS)/FLOAT_SZ, final_vert);
}

void SkyBox::bind(GLenum buff_type, GLenum draw_type)
{
	glBindBuffer(buff_type, vertbuffID[0]);
	glBufferData(buff_type, SKYBOX_V_SIZE*FLOAT_SZ, final_vert, draw_type);

	shader = gltLoadShaderPairWithAttributes("shaders/SkyBox.vp", "shaders/SkyBox.fp", 2, 
		GLT_ATTRIBUTE_VERTEX, "vVertex",
		GLT_ATTRIBUTE_NORMAL, "vNormal");

	locMVP = glGetUniformLocation(shader, "mvpMatrix");
}

// Called to draw scene
void SkyBox::draw(GLGeometryTransform transformPipeline)
{
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glUniform1i(locTexture, 0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glUseProgram(shader);

	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertbuffID[0]);
	glVertexAttribPointer(0, V_COORD_SZ, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_QUADS, 0, VERT_TOTAL);

	glDisableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}