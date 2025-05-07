#version 330 core
in vec3 pxColor;
in vec3 vertPos;

out vec4 fragColor;

uniform float greenColor;

void main() {
	// fragColor = vec4(pxColor.r, greenColor, pxColor.b, 1.0f);
	fragColor = vec4(
		vertPos.x / 2.0f + 0.5f,
		vertPos.y / 2.0f + 0.5f,
		vertPos.z / 2.0f + 0.5f,
		1.0f
	);
} 
