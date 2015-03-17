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
		pBits = gltReadTGABits("CoolTexture.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		//nBits = gltReadTGABits("brick_sml_grn1b_SSBump.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		readTexture = true;
	}
	//	glGenFramebuffers(1, frameBufferID);
//	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID[0]);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,textureID,1);	 
	//glReadPixels(0, 0, 400, 400, GL_RED, GL_BYTE, pBits);
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

 	glGenBuffers(1, vertbuffID);
	glGenBuffers(1, normbuffID);
	glGenBuffers(1, texbuffID);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		const float VERTS[] = {0, 0, 0, 0, 2, 0, 2, 2, 0, 2, 0, 0, 0, 0, 2, 0, 2
		, 2, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 
		0, 2, 2, 2, 2, 2, 2, 0, 2, 0, 0, 0, 2, 0, 0, 2, 2, 2, 
		2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0};

	const float TEX[]= {0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0};

	const float NORM[]= {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 
		-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
		0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
		1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
		0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0};
	for(int i = 0; i<MINIMAP_V_SIZE; i++){
		final_vert[i] = VERTS[i] + offset[i % 3];

		norm_final[i] = NORM[i];
	}
	for(int i = 0; i<MINIMAP_T_SIZE; i++)
		final_text[i] = TEX[i];

}

void Minimap::bind(GLenum buff_type, GLenum draw_type){
	glBindBuffer(buff_type, vertbuffID[0]);
	glBufferData(buff_type, MINIMAP_V_SIZE*FLOAT_SZ, final_vert, draw_type);

	glBindBuffer(buff_type, normbuffID[0]);
	glBufferData(buff_type, MINIMAP_N_SIZE*FLOAT_SZ, norm_final, draw_type);

	glBindBuffer(buff_type, texbuffID[0]);
	glBufferData(buff_type, MINIMAP_T_SIZE*FLOAT_SZ, final_text, draw_type);

	if(readShader == false){
		shader = loadShaderPair("shaders/Identity.vp", "shaders/Identity.fp");

		glBindAttribLocation(shader, 0, "vVertex");
		glBindAttribLocation(shader, 1, "vNormal");
		glBindAttribLocation(shader, 2, "vTexture0");

		glLinkProgram(shader);

		int testVal;
		glGetProgramiv(shader, GL_LINK_STATUS, &testVal);

		locAmbient = glGetUniformLocation(shader, "ambientColor");
		locDiffuse = glGetUniformLocation(shader, "diffuseColor");
		locSpecular = glGetUniformLocation(shader, "specularColor");
		locTexture = glGetUniformLocation(shader, "colorMap");
		locNormal = glGetUniformLocation(shader, "normalMap");
		locLight = glGetUniformLocation(shader, "vLightPosition");

		locMVP = glGetUniformLocation(shader, "mvpMatrix");
		locMV  = glGetUniformLocation(shader, "mvMatrix");
		locNM  = glGetUniformLocation(shader, "normalMatrix");

		readShader = true;
	}
}

void Minimap::draw(GLGeometryTransform transformPipeline){
	static GLfloat vLightPos[] = { final_vert[0] - 0.5f, final_vert[1], final_vert[2] - 0.5f };
	const GLfloat vWhite[] = { 0.0f, 0.0f, 1.0f, 0.0f };

	GLfloat vEyeLight[] = {1.0f, -1.0f, 1.0f};
	GLfloat vAmbientColor[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat vDiffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glUniform1i(locTexture, 0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glUniform1i(locNormal, 1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glUseProgram(shader);

	glUniform4fv(locAmbient, 1, vAmbientColor);
	glUniform4fv(locDiffuse, 1, vDiffuseColor);
	glUniform4fv(locSpecular, 1, vSpecularColor);
	glUniform3fv(locLight, 1, vEyeLight);

	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertbuffID[0]);
	glVertexAttribPointer(0, V_COORD_SZ, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normbuffID[0]);
	glVertexAttribPointer(1, N_COORD_SZ, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffID[0]);
	glVertexAttribPointer(2, T_COORD_SZ, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_QUADS, 0, VERT_TOTAL);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}