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
GLenum Minimap::eFormat;
int Minimap::nWidth, Minimap::nHeight, Minimap::nComponents;
GLbyte *Minimap::pBits;
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

void Minimap::moveTexture(int speed){
	//cout << speed << endl;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, speed, speed, nWidth, nHeight, eFormat, GL_UNSIGNED_BYTE, pBits);
//	batch.Draw();
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
	

	

	if(readTexture == false){
		pBits = gltReadTGABits("easybmp1.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		//nBits = gltReadTGABits("brick_sml_grn1b_SSBump.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		readTexture = true;
	}
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, NULL);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenerateMipmap(GL_TEXTURE_2D);

	offMipmap();

 gltMakeCube(batch, 1);

}

void Minimap::bind(GLenum buff_type, GLenum draw_type){
	

	if(readShader == false){
		shader = loadShaderPair("shaders/Identity.vp", "shaders/Identity.fp");

		glLinkProgram(shader);

	glBindAttribLocation(shader, 0, "vVertex");
	glBindAttribLocation(shader, 1, "vNormal");

	locMVP = glGetUniformLocation(shader, "mvpMatrix");
	locTexture = glGetUniformLocation(shader, "colorMap");
	}
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