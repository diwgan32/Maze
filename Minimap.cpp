#include "stdafx.h"

#include "Minimap.h"
#include "LoadShader.h"

#include <assert.h>

#define VERT_TOTAL 24

#define MINIMAP_V_SIZE (VERT_TOTAL * 3)
#define MINIMAP_N_SIZE (VERT_TOTAL * 3)
#define MINIMAP_T_SIZE (VERT_TOTAL * 2)
#define V_COORD_SZ 3
#define N_COORD_SZ 3
#define T_COORD_SZ 2

#define FLOAT_SZ sizeof(float)

GLuint Minimap::shader, Minimap::textureID, Minimap::normalID;
GLuint frameBufferID[1];
bool Minimap::readTexture, Minimap::readShader;

int Minimap::locAmbient, Minimap::locDiffuse, Minimap::locSpecular, Minimap::locEyeLight, Minimap::locLight, Minimap::locTexture, Minimap::locNormal;
int Minimap::locMVP, Minimap::locMV, Minimap::locNM;

Minimap::Minimap(){
	final_vert = new float[MINIMAP_V_SIZE];
	norm_final = new float[MINIMAP_N_SIZE];
	final_text = new float[MINIMAP_T_SIZE];
}

Minimap::~Minimap(){
	delete final_vert, norm_final, final_text;
}

void Minimap::onMipmap(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Minimap::offMipmap(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Minimap::onAniso(GLfloat f){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f);
}

void Minimap::offAniso(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
}

void Minimap::init(float offset[3]){
	static GLbyte * pBits;

	static int nWidth, nHeight, nComponents;
	static GLenum eFormat;

	if(readTexture == false){
		pBits = gltReadTGABits("dirt.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		//nBits = gltReadTGABits("brick_sml_grn1b_SSBump.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		readTexture = true;
	}
		//glGenFramebuffers(1, frameBufferID);
	//glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID[0]);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,textureID,1);	 
	//glReadPixels(0, 0, 128, 128, GL_RED, GL_BYTE, pBits);
	//cout << (int)pBits[0]<< endl;
	// Texture
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Normal
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &normalID);
	glBindTexture(GL_TEXTURE_2D, normalID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, nBits);

	glGenerateMipmap(GL_TEXTURE_2D);

	offMipmap();

 	gltMakeCube(batch, 2);

}

void Minimap::bind(GLenum buff_type, GLenum draw_type){
		shader = loadShaderPair("shaders/Identity.vp", "shaders/Identity.fp");

	glLinkProgram(shader);

	glBindAttribLocation(shader, 0, "vVertex");
	glBindAttribLocation(shader, 1, "vNormal");

	locMVP = glGetUniformLocation(shader, "mvpMatrix");
	locTexture = glGetUniformLocation(shader, "cubeMap");
}

void Minimap::draw(GLGeometryTransform transformPipeline){
glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glUniform1i(locTexture, 0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glUseProgram(shader);

	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());

	batch.Draw();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);


}