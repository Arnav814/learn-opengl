#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 obj2world;
uniform mat4 world2cam;
uniform mat4 projection;

void main() {
	gl_Position = projection * world2cam * obj2world * vec4(aPos, 1.0);
}

