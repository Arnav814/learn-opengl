#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aDiffuse;
layout (location = 3) in vec3 aSpecular;

out vec3 fragPos;
out vec3 inputNormal; // it's an input for the fragment shader
out vec3 vertDiffuse;
out vec3 vertSpecular;

uniform mat4 obj2world;
uniform mat3 obj2normal;
uniform mat4 world2cam;
uniform mat4 projection;

void main() {
	gl_Position = projection * world2cam * obj2world * vec4(aPos, 1.0);
	fragPos = vec3(obj2world * vec4(aPos, 1.0));
	inputNormal = obj2normal * aNormal;
	vertDiffuse = aDiffuse;
	vertSpecular = aSpecular;
}

