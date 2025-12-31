#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

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

	// cos of the inside cone's covered angle
	float inCutoff;
	// cos of the outside cone's covered angle
	float outCutoff;
};

struct Material {
	// TODO: support multiple textures
	// shared for ambient and diffuse
	sampler2D textureDiffuse1;
	// sampler2D textureDiffuse2;
	// sampler2D textureDiffuse3;
	sampler2D textureSpecular1;
	// sampler2D textureSpecular2;
	// sampler2D textureSpecular3;
	float shininess; // specular exponent
};

// FIXME: these functions have a lot of duplicated code

vec3 calcDirLight(DirectionalLight light, float shininess, vec3 normal, vec3 viewDir, vec3 diffVal, vec3 specVal) {
	vec3 lightDir = normalize(-light.direction);

	// diffuse component
	float diff = max(dot(normal, lightDir), 0.0);

	// specular component
	vec3 reflectedRay = reflect(-lightDir, normal); // light exit ray
	float spec = pow(max(dot(reflectedRay, viewDir), 0.0), shininess);

	// final result calculation
	vec3 ambient = light.ambient * diffVal;
	vec3 diffuse = light.diffuse * diff * diffVal;
	vec3 specular = light.specular * spec * specVal;

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, float shininess, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffVal, vec3 specVal) {
	// light to drawn point vector
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse component
	float diff = max(dot(normal, lightDir), 0.0);

	// specular component
	vec3 reflectedRay = reflect(-lightDir, normal); // light exit ray
	float spec = pow(max(dot(viewDir, reflectedRay), 0.0), shininess);

	// find components as vec3s
	vec3 ambient = light.ambient * diffVal;
	vec3 diffuse = light.diffuse * diff * diffVal;
	vec3 specular = light.specular * spec * specVal;

	// attenuation calculation
	float lightDistance = distance(light.position, fragPos);
	float attenuation = 1.0 / (light.constant +
	                           light.linear * lightDistance +
	                           light.quadratic * pow(lightDistance, 2));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, float shininess, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffVal, vec3 specVal) {
	// light to drawn point vector
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse component
	float diff = max(dot(normal, lightDir), 0.0);

	// specular component
	vec3 reflectedRay = reflect(-lightDir, normal); // light exit ray
	float spec = pow(max(dot(reflectedRay, viewDir), 0.0), shininess);

	// find components as vec3s
	vec3 ambient = light.ambient * diffVal;
	vec3 diffuse = light.diffuse * diff * diffVal;
	vec3 specular = light.specular * spec * specVal;

	// attenuation calculation
	float lightDistance = distance(light.position, fragPos);
	float attenuation = 1.0 / (light.constant +
	                           light.linear * lightDistance +
	                           light.quadratic * pow(lightDistance, 2));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// calculate the cone of the spotlight
	// angle between the light's direction and the light to current point vector
	float theta = dot(lightDir /* already normalized */, normalize(-light.direction));
	// difference between the inside and outside cones
	float epilson = light.inCutoff - light.outCutoff;
	// final intensity value
	float intensity = clamp((theta - light.outCutoff) / epilson, 0.0, 1.0);

	// leave ambient unaffected so we have some light
	diffuse *= intensity;
	specular *= intensity;

	return (ambient + diffuse + specular);
}


#endif /* LIGHTING_GLSL */
