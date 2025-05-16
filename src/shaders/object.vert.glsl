#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 obj2world;
uniform mat3 obj2normal;
uniform mat4 world2cam;
uniform mat4 projection;

void main() {
	gl_Position = projection * world2cam * obj2world * vec4(aPos, 1.0);
	fragPos = vec3(obj2world * vec4(aPos, 1.0));
	normal = obj2normal * aNormal;
	texCoord = aTexCoord;
}

