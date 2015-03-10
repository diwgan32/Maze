#include "stdafx.h"
#include "Floor.h"

#include <assert.h>

#define SIZE 41

#define VERT_TOTAL 4

#define FLOOR_V_SIZE (VERT_TOTAL * 3)
#define FLOOR_N_SIZE (VERT_TOTAL * 3)
#define FLOOR_T_SIZE (VERT_TOTAL * 2)

#define V_COORD_SZ 3
#define N_COORD_SZ 3
#define T_COORD_SZ 2

#define FLOAT_SZ sizeof(float)

GLuint Floor::shader, Floor::textureID, Floor::normalID;

bool Floor::readTexture, Floor::readShader;

int Floor::locAmbient, Floor::locDiffuse, Floor::locSpecular, Floor::locEyeLight, Floor::locLight, Floor::locTexture, Floor::locNormal;
int Floor::locMVP, Floor::locMV, Floor::locNM;

Floor::Floor(){
	final_vert = new float[FLOOR_V_SIZE];
	norm_final = new float[FLOOR_N_SIZE];
	final_text = new float[FLOOR_T_SIZE];
}

Floor::~Floor(){
	delete final_vert, norm_final, final_text;
}

GLuint Floor::loadShaderPair(char * vertsrc, char * fragsrc){
	GLuint program;
	GLuint vert;
	GLuint frag;
	GLint testVal;

	vert = glCreateShader(GL_VERTEX_SHADER);
	frag = glCreateShader(GL_FRAGMENT_SHADER);

	// Vertex Program
	if(gltLoadShaderFile(vertsrc, vert) == false)
	{
		glDeleteShader(vert);
		glDeleteShader(frag);

		cout << "The shader at" << vert
			<< "could not be found.\n";

		return (GLuint)NULL;
	}

	// Fragment Program
	if(gltLoadShaderFile(fragsrc, frag) == false)
	{
		glDeleteShader(vert);
		glDeleteShader(frag);

		cout << "The shader at " << frag
			<< "could not be found.\n";

		return (GLuint)NULL;
	}

	glCompileShader(vert);
	glCompileShader(frag);

	glGetShaderiv(vert, GL_COMPILE_STATUS, &testVal);

	if(testVal == GL_FALSE)
	{
		char infoLog[1024];

		glGetShaderInfoLog(vert, 1024, NULL, infoLog);

		cout << "The shader at " << vert
			<< "failed to compile with the following error:\n"
			<< infoLog << "\n";

		glDeleteShader(vert);
		glDeleteShader(frag);

		return (GLuint)NULL;
	}

	// Check for errors in fragment shader
	glGetShaderiv(frag, GL_COMPILE_STATUS, &testVal);

	if(testVal == GL_FALSE)
	{
		char infoLog[1024];

		glGetShaderInfoLog(frag, 1024, NULL, infoLog);

		cout << "The shader at "<< frag
			<< "failed to compile with the following error:\n"
			<< infoLog << "\n";

		glDeleteShader(vert);
		glDeleteShader(frag);

		return (GLuint)NULL;
	}
	//	cout << vertsrc << endl;

	program = glCreateProgram();

	glAttachShader(program, vert);
	glAttachShader(program, frag);

	return program;
}

void Floor::onMipmap(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Floor::offMipmap(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Floor::onAniso(GLfloat f){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f);
}

void Floor::offAniso(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
}

void Floor::init(){
	static GLbyte * pBits, * nBits;

	static int nWidth, nHeight, nComponents;
	static GLenum eFormat;

	if(readTexture == false){
		pBits = gltReadTGABits("dirt.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		nBits = gltReadTGABits("dirt_SSBump.tga", &nWidth, &nHeight, &nComponents, &eFormat);
		readTexture = true;
	}

	// Texture Map
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Normal Map
	glActiveTexture(GL_TEXTURE1);

	glGenTextures(1, &normalID);
	glBindTexture(GL_TEXTURE_2D, normalID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, nBits);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Other settings

	offMipmap();

	glGenBuffers(1, vertbuffID);
	glGenBuffers(1, normbuffID);
	glGenBuffers(1, texbuffID);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	const float VERTS[] = {0, 0, 0, 0, 0, SIZE, SIZE, 0, SIZE, SIZE, 0, 0};

	const float TEX[]= {0, 0, 0, SIZE, SIZE, SIZE, SIZE, 0};

	const float NORM[]= {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};

	for(int i = 0; i<FLOOR_V_SIZE; i++){
		final_vert[i] = VERTS[i];
		norm_final[i] = NORM[i];
	}
	for(int i = 0; i<FLOOR_T_SIZE; i++)
		final_text[i] = TEX[i];
}

void Floor::bind(GLenum buff_type, GLenum draw_type){
	glBindBuffer(buff_type, vertbuffID[0]);
	glBufferData(buff_type, FLOOR_V_SIZE*FLOAT_SZ, final_vert, draw_type);

	glBindBuffer(buff_type, normbuffID[0]);
	glBufferData(buff_type, FLOOR_N_SIZE*FLOAT_SZ, norm_final, draw_type);

	glBindBuffer(buff_type, texbuffID[0]);
	glBufferData(buff_type, FLOOR_T_SIZE*FLOAT_SZ, final_text, draw_type);

	if(readShader == false){
		shader = Floor::loadShaderPair("shaders/ADSPhong.vp", "shaders/ADSPhong.fp");

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

void Floor::draw(GLGeometryTransform transformPipeline){
	static GLfloat vLightPos[] = { 0.0f, 10.0f, 0.0f };
	const GLfloat vWhite[] = { 0.0f, 0.0f, 1.0f, 0.0f };

	GLfloat vEyeLight[] = {-1.0f, -1.0f, -1.0f};
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
	glVertexAttribPointer(0, V_COORD_SZ, GL_FLOAT, GL_FALSE, 0,NULL);

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