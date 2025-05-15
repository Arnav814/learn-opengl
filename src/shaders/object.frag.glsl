#version 330 core
in vec3 fragPos;
in vec3 normal;

out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;

	// lighting calcs are done in world space
	// extra normal
	vec3 unitNormal = normalize(normal);
	// normalized vector from the light to the point being drawn
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 diffuse = max(dot(unitNormal, lightDir), 0.0) * lightColor;

	// camera to drawn point
	vec3 viewDir = normalize(viewPos - fragPos);
	// exit vector of the reflected light
	vec3 reflectedDir = reflect(-lightDir, unitNormal);
	float specularIntensity = pow(max(dot(reflectedDir, viewDir), 0.0), 256);
	vec3 specular = specularIntensity * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	fragColor = vec4(result, 1.0f);
} 
