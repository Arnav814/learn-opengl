#version 330 core
in vec3 fragPos;
in vec3 normal;

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// lighting calcs are done in world space
	// extra normal
	vec3 unitNormal = normalize(normal);
	// normalized vector from the light to the point being drawn
	vec3 lightToFrag = normalize(lightPos - fragPos);
	vec3 diffuse = dot(unitNormal, lightToFrag) * lightColor;

	vec3 result = (ambient + diffuse) * objectColor;
	fragColor = vec4(result, 1.0f);
} 
