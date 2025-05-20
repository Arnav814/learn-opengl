#version 330 core
in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// for attenuation
	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	sampler2D diffuseMap; // shared for ambient and diffuse
	sampler2D specularMap;
	float shininess; // specular exponent
};

uniform vec3 viewPos;
uniform PointLight light;
uniform Material material;

void main() {
	vec3 diffuseVal = vec3(texture(material.diffuseMap, texCoord));
	vec3 specularVal = vec3(texture(material.specularMap, texCoord));

	vec3 ambient = light.ambient * diffuseVal;

	// lighting calcs are done in world space
	// extra normal
	vec3 unitNormal = normalize(normal);
	// normalized vector from the light to the point being drawn
	vec3 lightDir = normalize(light.position - fragPos);
	// vec3 lightDir = normalize(-light.direction);
	float diffuseIntensity = max(dot(unitNormal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diffuseIntensity * diffuseVal);

	// camera to drawn point
	vec3 viewDir = normalize(viewPos - fragPos);
	// exit vector of the reflected light
	vec3 reflectedDir = reflect(-lightDir, unitNormal);
	float specularIntensity = pow(max(dot(reflectedDir, viewDir), 0.0), material.shininess);
	vec3 specular = light.specular * (specularIntensity * specularVal);

	// calculate attenuation
	float lightDistance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * lightDistance
	                           + light.quadratic * pow(lightDistance, 2));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	fragColor = vec4(result, 1.0f);
} 
