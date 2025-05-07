#version 330 core
in vec3 pxColor;

out vec4 fragColor;

uniform float greenColor;

void main() {
	fragColor = vec4(pxColor.r, greenColor, pxColor.b, 1.0f);
} 
