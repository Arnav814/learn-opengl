#ifndef MESH_HPP
#define MESH_HPP

#include "common.hpp"
#include "shaders.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <string>
#include <vector>

#define STRUCT_MEMBER_ATTRIB(attrNum, structName, member) \
	do { \
		glEnableVertexAttribArray(attrNum); \
		glVertexAttribPointer(attrNum, sizeof(structName::member), GL_FLOAT, GL_FALSE, \
		                      sizeof(structName), (void*)offsetof(structName, member)); \
	} while (false)

#pragma pack(push, 1)

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

enum class TextureType { textureDiffuse, textureSpecular };

struct Texture {
	uint id;
	TextureType type;
};

#pragma pack(pop)

class Mesh {
  private:
	std::vector<Vertex> verticies;
	std::vector<uint> indicies;
	std::vector<Texture> textures;
	uint VAO, VBO, EBO;

	// setup VAO, VB0, and EBO
	void setupMesh();

  public:
	Mesh(std::vector<Vertex> verticies, std::vector<uint> indicies, std::vector<Texture> textures) {
		this->verticies = verticies;
		this->indicies = indicies;
		this->textures = textures;

		this->setupMesh();
	}

	void draw(ShaderProgram& shader);
};

#endif /* MESH_HPP */
