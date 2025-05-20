#version 330 core
in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

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

struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// for attenuation
	float constant;
	float linear;
	float quadratic;

	// cos of the covered angle
	float cutoff;
};

struct Material {
	sampler2D diffuseMap; // shared for ambient and diffuse
	sampler2D specularMap;
	float shininess; // specular exponent
};

uniform vec3 viewPos;
uniform SpotLight light;
uniform Material material;

void main() {
	vec3 diffuseVal = vec3(texture(material.diffuseMap, texCoord));
	vec3 specularVal = vec3(texture(material.specularMap, texCoord));

	// calculate attenuation
	float lightDistance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * lightDistance
							   + light.quadratic * pow(lightDistance, 2));

	// we always need ambient
	vec3 ambient = light.ambient * diffuseVal;
	ambient *= attenuation;
	vec3 result = ambient;

	// lighting calcs are done in world space

	// normalized vector from the light to the point being drawn
	vec3 lightDir = normalize(light.position - fragPos);
	// angle between the light's direction and the light to current point vector
	float theta = dot(normalize(lightDir), normalize(-light.direction));

	// > not < because cosines
	if (theta > light.cutoff) {
		// extra normal
		vec3 unitNormal = normalize(normal);
		// vec3 lightDir = normalize(-light.direction);
		float diffuseIntensity = max(dot(unitNormal, lightDir), 0.0);
		vec3 diffuse = light.diffuse * (diffuseIntensity * diffuseVal);

		// camera to drawn point
		vec3 viewDir = normalize(viewPos - fragPos);
		// exit vector of the reflected light
		vec3 reflectedDir = reflect(-lightDir, unitNormal);
		float specularIntensity = pow(max(dot(reflectedDir, viewDir), 0.0), material.shininess);
		vec3 specular = light.specular * (specularIntensity * specularVal);

		diffuse *= attenuation;
		specular *= attenuation;
		result += diffuse + specular;
	}

	fragColor = vec4(result, 1.0f);
} 
