// Skybox Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;

uniform sampler2D colorMap;

smooth in vec2 vVaryingTexCoord;

void main(void)
{ 
    gl_FragColor = texture2D(colorMap, vVaryingTexCoord);
}
    