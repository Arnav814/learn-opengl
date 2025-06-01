#include "mesh.hpp"

#include "shaderStructs.hpp"

template <typename Vertex> void Mesh<Vertex>::setupMesh() {
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
	if constexpr (requires { Vertex::texCoords; }) {
		STRUCT_MEMBER_ATTRIB(2, Vertex, texCoords);
	} else if constexpr (requires { Vertex::color; }) {
		STRUCT_MEMBER_ATTRIB(2, Vertex, color);
	} else {
		static_assert(false);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename Vertex>
void Mesh<Vertex>::render(const Camera& camera, const SceneCascade& cascade) {
	// also include this node
	SceneCascade combinedCascade = cascade + this->getNodeCascade();

	this->shader->use();
	this->shader->setUniform("obj2world", combinedCascade.transform);
	this->shader->setUniform("obj2normal",
	                         glm::mat3(glm::transpose(glm::inverse(combinedCascade.transform))));
	this->shader->setUniform("world2cam", camera.toCamSpace());
	this->shader->setUniform("projection", camera.projectionMat());
	this->shader->setUniform("viewPos", camera.getPosition());

	this->draw();

	this->shader->stopUsing();
}

template <typename Vertex> void Mesh<Vertex>::draw() {
	if constexpr (requires { Vertex::texCoords; }) {
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

			this->shader->setUniform(
			    "material." + std::string(magic_enum::enum_name(texType)) + number, (int)i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);
	}

	this->shader->setUniform("material.shininess", this->shininess);

	// actually draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, (uint)this->indicies.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

uint loadTexture(const filesystem::path& path) {
	stbi_set_flip_vertically_on_load(true);

	int width;
	int height;
	int nChannels;
	uchar* data = stbi_load(path.string().c_str(), &width, &height, &nChannels, 0);

	if (not data) {
		throw std::runtime_error(std::format("Error loading image at {}.", path.string()));
	}

	uint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 3 ? GL_RGB : GL_RGBA,
	             GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

template class Mesh<TexVertex>;
template class Mesh<ColorVertex>;
