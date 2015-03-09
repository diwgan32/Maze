// Skybox Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 120

//out vec4 vFragColor;

uniform samplerCube  cubeMap;

varying vec3 vVaryingTexCoord;

void main(void)
    { 
    gl_FragColor = texture1D(cubeMap, vVaryingTexCoord);
    }
    