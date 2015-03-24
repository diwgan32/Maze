#include "stdafx.h"

#include "Minimap.h"
#include "LoadShader.h"

#include <assert.h>

#define VERT_TOTAL 6

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
int Minimap::nWidth, Minimap::nHeight;
GLenum Minimap::eFormat;
GLbyte* Minimap::pBits;
float * Minimap::offset;
int Minimap::locTexture, Minimap::locOffset;
int Minimap::locP, Minimap::locMV, Minimap::locNM;

Minimap::Minimap(){
	final_vert = new float[MINIMAP_V_SIZE];
	//norm_final = new float[MINIMAP_N_SIZE];
	final_text = new float[MINIMAP_T_SIZE]; 
		offset = new float[3];
	offset[0] = -2;
	offset[1] = -2;
	offset[2] = 0;
}
Minimap::~Minimap(){
	delete final_vert, norm_final, final_text;
}


void Minimap::moveTexture(float speed){
	//cout << speed << endl;
	//offset+=speed;
	//float offset1[] = {0, 0, 0};
	//init(offset1, offset);
	//	bind(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	//	batch.Draw();
	float TEX[]= {0, 0, 0, 0, speed, 0, speed, 0,
		0, 0, 0, 0, speed, 0, speed, 0,
		0, 0, 0, 0, speed, 0, speed, 0,
		0, 0, 0, 0, speed, 0, speed, 0,
		0, 0, 0, 0, speed, 0, speed, 0,
		0, 0, 0, 0, speed, 0, speed, 0};

	for(int i = 0; i<MINIMAP_T_SIZE; i++)
		final_text[i] = TEX[i];
	glBindBuffer(GL_ARRAY_BUFFER, texbuffID[0]);
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, MINIMAP_T_SIZE*FLOAT_SZ, final_text);
}
void Minimap::init(float offset1[3], float speed){


	static GLbyte * pBits, * nBits;

	static int nWidth, nHeight, nComponents;
	static GLenum eFormat;

	if(readTexture == false){
		pBits = gltReadTGABits("easybmp1.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		//nBits = gltReadTGABits("brick_sml_grn1b_SSBump.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		readTexture = true;
	}
	//cout << speed << endl;
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
	glGenBuffers(1, vertbuffID);
	//glGenBuffers(1, normbuffID);
	glGenBuffers(1, texbuffID);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
const float VERTS[] = {0, 0, 0, 0, .03, 0, .03, .03, 0, .03, 0, 0, 0, 0, .03, 0, .03
		, .03, 0, .03, 0, 0, 0, 0, 0, 0, .03, 0, .03, .03, .03, .03, .03, .03, 0, .03, .03, 
		0, .03, .03, .03, .03, .03, .03, 0, .03, 0, 0, 0, .03, 0, 0, .03, .03, .03, 
		.03, .03, .03, .03, 0, 0, 0, 0, 0, 0, .03, .03, 0, .03, .03, 0, 0};

	float TEX[]= {0, 0, 0, speed, speed, speed, speed, 0,
		0, 0, 0, speed, speed, speed, speed, 0,
		0, 0, 0, speed, speed, speed, speed, 0,
		0, 0, 0, speed, speed, speed, speed, 0,
		0, 0, 0, speed, speed, speed, speed, 0,
		0, 0, 0, speed, speed, speed, speed, 0};


	const float NORM[]= {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 
		-1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
		0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
		1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
		0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
		0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0};
	for(int i = 0; i<MINIMAP_V_SIZE; i++){
		final_vert[i] = VERTS[i] + offset1[i % 3];

		//	norm_final[i] = NORM[i];
	}
	for(int i = 0; i<MINIMAP_T_SIZE; i++)
		final_text[i] = TEX[i];

}

void Minimap::bind(GLenum buff_type, GLenum draw_type){
	glBindBuffer(buff_type, vertbuffID[0]);
	glBufferData(buff_type, MINIMAP_V_SIZE*FLOAT_SZ, final_vert, GL_STATIC_DRAW);

	//glBindBuffer(buff_type, normbuffID[0]);
	//glBufferData(buff_type, MINIMAP_N_SIZE*FLOAT_SZ, norm_final, draw_type);

	glBindBuffer(buff_type, texbuffID[0]);
	glBufferData(buff_type, MINIMAP_T_SIZE*FLOAT_SZ, final_text, draw_type);

	if(readShader == false){
		shader = loadShaderPair("shaders/Identity.vp", "shaders/Identity.fp");

		glBindAttribLocation(shader, 0, "vVertex");
		//glBindAttribLocation(shader, 1, "vNormal");
		glBindAttribLocation(shader, 1, "vTexture0");

		glLinkProgram(shader);

		int testVal;
		glGetProgramiv(shader, GL_LINK_STATUS, &testVal);


		locTexture = glGetUniformLocation(shader, "colorMap");
		locOffset = glGetUniformLocation(shader, "textureOffset");
		locP = glGetUniformLocation(shader, "pMatrix");
		locMV = glGetUniformLocation(shader,"mvMatrix");

		readShader = true;
	}
}

void Minimap::draw(GLGeometryTransform transformPipeline, float xOffset, float yOffset){
	
	
	//glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glUniform1i(locTexture, 0);
	glBindTexture(GL_TEXTURE_2D, textureID);
//	cout << xOffset << " " << yOffset << endl;
	glUseProgram(shader);

	glUniform2f(locOffset, xOffset, yOffset);
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniformMatrix4fv(locP, 1, GL_FALSE, transformPipeline.GetProjectionMatrix());
	glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertbuffID[0]);
	glVertexAttribPointer(0, V_COORD_SZ, GL_FLOAT, GL_FALSE, 0, NULL);

	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, normbuffID[0]);
	//glVertexAttribPointer(1, N_COORD_SZ, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffID[0]);
	glVertexAttribPointer(1, T_COORD_SZ, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(GL_QUADS, 0, VERT_TOTAL);
	// Set the list of draw buffers.

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);


}