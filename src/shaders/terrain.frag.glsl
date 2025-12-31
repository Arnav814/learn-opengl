#version 330 core
#include "lighting.glsl"
in vec3 fragPos;
in vec3 inputNormal;
in vec3 vertDiffuse;
in vec3 vertSpecular;

out vec4 fragColor;

struct TerrMaterial {
	float shininess; // specular exponent
};

#define POINT_LIGHT_COUNT 4

uniform vec3 viewPos;
uniform SpotLight spotLight;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[POINT_LIGHT_COUNT];
uniform TerrMaterial material;
uniform bool displayNormals;

void main() {
	// basic properties
	vec3 normal = normalize(inputNormal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 diffVal = vertDiffuse;
	vec3 specVal = vertSpecular;

	// directional light
	vec3 result = calcDirLight(dirLight, material.shininess, normal, viewDir, diffVal, specVal);

	// point lights
	for (int i = 0; i < POINT_LIGHT_COUNT; i++) {
		result += calcPointLight(pointLights[i], material.shininess, normal, fragPos, viewDir, diffVal, specVal);
	}

	// spot light
	result += calcSpotLight(spotLight, material.shininess, normal, fragPos, viewDir, diffVal, specVal);

	if (displayNormals)
		result = (normal + vec3(1)) / 2.;

	fragColor = vec4(result, 1.0f);
} 

