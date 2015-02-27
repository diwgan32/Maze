// ADS Point lighting Shader
// Fragment Shader
// Richard S. Wright Jr.
// OpenGL SuperBible
#version 130

out vec4 vFragColor;

uniform vec4    ambientColor;
uniform vec4    diffuseColor;   
uniform vec4    specularColor;

uniform sampler2D colorMap;
uniform sampler2D normalMap;

smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec2 vTexCoords;

void main(void)
    {
	const float maxVariance = 256.0; // Mess around with this value to increase/decrease normal perturbation
	const float minVariance = maxVariance / 2.0;
	
	// Create a normal which is our standard normal + the normal map perturbation (which is going to be either positive or negative)
	vec3 normalAdjusted = vVaryingNormal + normalize(texture2D(normalMap, vTexCoords.st).rgb * maxVariance - minVariance);
	
    // Dot product gives us diffuse intensity
	float diff = max(0.0, dot(normalize(normalAdjusted), normalize(vVaryingLightDir)));
	
    // Multiply intensity by diffuse color, force alpha to 1.0
    vFragColor = diff * diffuseColor;

    // Add in ambient light
    vFragColor += ambientColor;

	// Modulate in the texture
    vFragColor *= texture2D(colorMap, vTexCoords);

    // Specular Light
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir), normalize(vVaryingNormal)));
	float spec = max(0.0, dot(normalize(normalAdjusted), vReflection));
    
	if(diff != 0) {
        float fSpec = pow(spec, 128.0);
        vFragColor.rgb += vec3(fSpec, fSpec, fSpec);
        }
    }
    