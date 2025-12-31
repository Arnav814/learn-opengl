#version 330 core
#include "lighting.glsl"
in vec3 fragPos;
in vec3 inputNormal;
in vec2 texCoord;

out vec4 fragColor;

#define POINT_LIGHT_COUNT 4

uniform vec3 viewPos;
uniform SpotLight spotLight;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[POINT_LIGHT_COUNT];
uniform Material material;
uniform bool displayNormals;

// #define MAX_LIGHTS_PER_TYPE 10
// layout (std140) uniform Lights {
// 	SpotLight[MAX_LIGHTS_PER_TYPE] spotLights;
// 	DirectionalLight[MAX_LIGHTS_PER_TYPE] dirLights;
// 	PointLight[MAX_LIGHTS_PER_TYPE] pointLights;
// };

void main() {
	// basic properties
	vec3 normal = normalize(inputNormal);
	vec3 viewDir = normalize(viewPos - fragPos);

	// texture data
	vec3 diffVal = vec3(texture(material.textureDiffuse1, texCoord));
	vec3 specVal = vec3(texture(material.textureSpecular1, texCoord));

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

