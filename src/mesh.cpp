#include "mesh.hpp"

void Mesh::setupMesh() {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, VECTOR_SIZE_BYTES(this->verticies), this->verticies.data(),
	             GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, VECTOR_SIZE_BYTES(this->indicies), this->indicies.data(),
	             GL_STATIC_DRAW);

	STRUCT_MEMBER_ATTRIB(0, Vertex, position);
	STRUCT_MEMBER_ATTRIB(1, Vertex, normal);
	STRUCT_MEMBER_ATTRIB(2, Vertex, texCoords);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw(ShaderProgram& shader) {
	// counters for number of diffuse/specular textures processed
	uint diffuseN = 1;
	uint specularN = 1;

	// loop through textures and set uniforms
	for (uint i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number; // texture to use
		TextureType texType = textures[i].type;
		if (texType == TextureType::textureDiffuse) number = std::to_string(diffuseN++);
		else if (texType == TextureType::textureSpecular) number = std::to_string(specularN++);

		shader.setUniform("material." + std::string(magic_enum::enum_name(texType)) + number,
		                  (int)i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// actually draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indicies.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
