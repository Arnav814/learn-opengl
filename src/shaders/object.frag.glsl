#version 330 core
in vec3 fragPos;
in vec3 normal;

out vec4 fragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess; // specular exponent
};

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform vec3 lightColor;

void main() {
	vec3 ambient = material.ambient;

	// lighting calcs are done in world space
	// extra normal
	vec3 unitNormal = normalize(normal);
	// normalized vector from the light to the point being drawn
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuseIntensity = max(dot(unitNormal, lightDir), 0.0);
	vec3 diffuse = lightColor * (diffuseIntensity * material.diffuse);

	// camera to drawn point
	vec3 viewDir = normalize(viewPos - fragPos);
	// exit vector of the reflected light
	vec3 reflectedDir = reflect(-lightDir, unitNormal);
	float specularIntensity = pow(max(dot(reflectedDir, viewDir), 0.0), material.shininess);
	vec3 specular = lightColor * (specularIntensity * material.specular);

	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0f);
} 
