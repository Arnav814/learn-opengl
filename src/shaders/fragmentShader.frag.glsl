#version 330 core
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D containerTexture;
uniform sampler2D faceTexture;

void main() {
	fragColor = mix(texture(containerTexture, texCoord), texture(faceTexture, texCoord), 0.2);
} 
