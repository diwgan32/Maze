#ifndef CUBE_H
#define CUBE_H

#include "stdafx.h"
class Cube
{

	public:

		//typdefs

		//alloc mem

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
		GLuint shader;		

		Array final_vert, final_text, norm_final;


		int total;
		int vsize; 
		int nsize;
		int tsize; 

		Cube (){
		}
		void init(){

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
				final_vert[i] = VERTS[i];
				norm_final[i] = NORM[i];
				final_text[i] = TEX[i];
			}
		}
		void getUniLoc(){
			locAmbient = glGetUniformLocation(shader, "ambientColor");
			locDiffuse = glGetUniformLocation(shader, "diffuseColor");
			locSpecular = glGetUniformLocation(shader, "specularColor");
			locEyeLight = glGetUniformLocation(shader, "vLightPosition");
			locMVP = glGetUniformLocation(shader, "mvpMatrix");
			locMV = glGetUniformLocation(shader, "mvMatrix");
			locNM = glGetUniformLocation(shader, "ambientColor");
			locTexture = glGetUniformLocation(shader, "colorMap");
		}


		void leftMult(float matrix[4][4], float vector[4], float result[4]){

			result[0] = (vector[0]*matrix[0][0])+(vector[1]*matrix[1][0])+(vector[2]*matrix[2][0])+(vector[3]*matrix[3][0]);
			result[1] = (vector[0]*matrix[0][1])+(vector[1]*matrix[1][1])+(vector[2]*matrix[2][1])+(vector[3]*matrix[3][1]);
			result[2] = (vector[0]*matrix[0][2])+(vector[1]*matrix[1][2])+(vector[2]*matrix[2][2])+(vector[3]*matrix[3][2]);
			result[3] = (vector[0]*matrix[0][3])+(vector[1]*matrix[1][3])+(vector[2]*matrix[2][3])+(vector[3]*matrix[3][3]);

		}

		void bind(GLenum buff_type, GLenum draw_type, GLuint vert_bind, GLuint norm_bind, GLuint text_bind){

			glBindBuffer(buff_type, vert_bind);
			glBufferData(buff_type, vsize, final_vert, draw_type);


			glBindBuffer(buff_type, norm_bind);
			glBufferData(buff_type, nsize, norm_final, draw_type);


			glBindBuffer(buff_type, text_bind);
			glBufferData(buff_type, tsize, final_text, draw_type);
		}



	};

#endif