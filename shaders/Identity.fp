// Skybox Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;

uniform sampler2D colorMap;

varying vec3 vVaryingTexCoord;

void main(void)
{ 
    gl_FragColor = texture2D(colorMap, vVaryingTexCoord.st);
}
    