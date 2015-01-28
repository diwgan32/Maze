#ifndef CUBE_H
#define CUBE_H

#include "stdafx.h"
using namespace std;
class Cube
{

public:

	typedef float* Array;
	int locAmbient;
	int locDiffuse;
	int locSpecular;
	int locEyeLight;
	int locMVP;
	int locMV;
	int locNM;
	int locTexture;
	int numFrame;
	int locLight;
	GLuint shader;		
	GLuint textureID;
	Array final_vert, final_text, norm_final;

	GLuint vertbuffID[1], normbuffID[1], texbuffID[1];

	int total;
	int vsize; 
	int nsize;
	int tsize; 

	Cube (){
	}
	
GLuint loadShaderPair( char * vertsrc, char * fragsrc){
	GLuint program;
	GLuint vert;
	GLuint frag;
	GLint testVal;
	vert = glCreateShader(GL_VERTEX_SHADER);
	frag = glCreateShader(GL_FRAGMENT_SHADER);

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

	void init(float offset[3]){
		glActiveTexture(GL_TEXTURE1);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		GLbyte *pBits;
		int nWidth, nHeight, nComponents;
		GLenum eFormat;
		pBits = gltReadTGABits("CoolTexture.tga", &nWidth, &nHeight, &nComponents, &eFormat);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
		glGenBuffers(1, vertbuffID);
		glGenBuffers(1, normbuffID);
		glGenBuffers(1, texbuffID);


		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		total = 36;
		float VERTS[] = {0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 
			1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 
			0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1,
			1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 
			1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 
			1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1};
		float TEX[]= {1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 
			1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,
			1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
			0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 
			0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
		float NORM[]= {0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, -1
			, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, 0, 1, 0, 0, 1, 
			0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
			1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
			0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1};

		vsize = sizeof(float) * total * 4 + 1;
		nsize = sizeof(float) * total * 3;
		tsize = sizeof(float) * total * 3;

		final_vert = new float[108];
		norm_final = new float[108];
		final_text = new float[108];

		for(int i = 0; i<108; i++){
			final_vert[i] = VERTS[i]+offset[i%3];
			norm_final[i] = NORM[i];
			final_text[i] = TEX[i];
		}
	}

	void leftMult(float matrix[4][4], float vector[4], float result[4]){

		result[0] = (vector[0]*matrix[0][0])+(vector[1]*matrix[1][0])+(vector[2]*matrix[2][0])+(vector[3]*matrix[3][0]);
		result[1] = (vector[0]*matrix[0][1])+(vector[1]*matrix[1][1])+(vector[2]*matrix[2][1])+(vector[3]*matrix[3][1]);
		result[2] = (vector[0]*matrix[0][2])+(vector[1]*matrix[1][2])+(vector[2]*matrix[2][2])+(vector[3]*matrix[3][2]);
		result[3] = (vector[0]*matrix[0][3])+(vector[1]*matrix[1][3])+(vector[2]*matrix[2][3])+(vector[3]*matrix[3][3]);

	}

	void bind(GLenum buff_type, GLenum draw_type){

		glBindBuffer(buff_type, vertbuffID[0]);
		glBufferData(buff_type, vsize, final_vert, draw_type);


		glBindBuffer(buff_type, normbuffID[0]);
		glBufferData(buff_type, nsize, norm_final, draw_type);


		glBindBuffer(buff_type, texbuffID[0]);
		glBufferData(buff_type, tsize, final_text, draw_type);


		shader = loadShaderPair("ADSTexture.vp", "ADSTexture.fp");

		glBindAttribLocation(shader, 0, "vVertex");
		glBindAttribLocation(shader, 1, "vNormal");
		glBindAttribLocation(shader, 2, "vTexture0");

		glLinkProgram(shader);
		int testVal;
		glGetProgramiv(shader, GL_LINK_STATUS, &testVal);


		locAmbient = glGetUniformLocation(shader, "ambientColor");
		locDiffuse = glGetUniformLocation(shader, "diffuseColor");
		locSpecular = glGetUniformLocation(shader, "specularColor");
		locLight = glGetUniformLocation(shader, "vLightPosition");
		locMVP = glGetUniformLocation(shader, "mvpMatrix");
		locMV  = glGetUniformLocation(shader, "mvMatrix");
		locNM  = glGetUniformLocation(shader, "normalMatrix");
		locTexture = glGetUniformLocation(shader, "colorMap");

	}

	void draw(GLGeometryTransform transformPipeline){
		
	static GLfloat vLightPos [] = { 2.0f, 2.0f, 0.0f };
	GLfloat vWhite[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLfloat vEyeLight[] = {-100.0f, -100.0f, -100.0f};
	GLfloat vAmbientColor[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat vDiffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glBindTexture(GL_TEXTURE_2D, textureID);
	glUseProgram(shader);

	glUniform4fv(locAmbient, 1, vAmbientColor);
	glUniform4fv(locDiffuse, 1, vDiffuseColor);
	glUniform4fv(locSpecular, 1, vSpecularColor);
	glUniform3fv(locLight, 1, vEyeLight);
	glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
	glUniform1i(locTexture, 1);
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertbuffID[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,NULL);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normbuffID[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffID[0]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);


	glDrawArrays(GL_TRIANGLES, 0, total);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	}
};

#endif