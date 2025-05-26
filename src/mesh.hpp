#ifndef MESH_HPP
#define MESH_HPP

#include "common.hpp"
#include "shaders.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <stb_image.h>

#include <string>
#include <vector>

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

// loads the file at runtime, so the path should be absolute or relative to the final binary
[[nodiscard]] uint loadTexture(const filesystem::path& path);

class Mesh {
  private:
	std::vector<Vertex> verticies;
	std::vector<uint> indicies;
	std::vector<Texture> textures;
	float shininess;
	uint VAO, VBO, EBO;

	// setup VAO, VB0, and EBO
	void setupMesh();

  public:
	Mesh(std::vector<Vertex> verticies, std::vector<uint> indicies, std::vector<Texture> textures,
	     float shininess) {
		this->verticies = verticies;
		this->indicies = indicies;
		this->textures = textures;
		this->shininess = shininess;

		this->setupMesh();
	}

	void draw(ShaderProgram& shader);
};

#endif /* MESH_HPP */
