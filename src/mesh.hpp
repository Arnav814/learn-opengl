#ifndef MESH_HPP
#define MESH_HPP

#include "common.hpp"
#include "sceneObject.hpp"
#include "shaders.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <stb_image.h>

#include <string>
#include <vector>

#pragma pack(push, 1)

// has texture coordinates
struct TexVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

// has a color
struct ColorVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

enum class TextureType { textureDiffuse, textureSpecular };

struct Texture {
	uint id;
	TextureType type;
};

#pragma pack(pop)

// loads the file at runtime, so the path should be absolute or relative to the final binary
[[nodiscard]] uint loadTexture(const filesystem::path& path);

template <typename Vertex> class Mesh : public BaseSceneGraphObject {
  private:
	std::vector<Vertex> verticies;
	std::vector<uint> indicies;
	std::vector<Texture> textures; // TODO: this is useless if we aren't using textured verticies
	float shininess;
	uint VAO, VBO, EBO;

	// setup VAO, VB0, and EBO
	void setupMesh();

  public:
	Mesh<TexVertex>(const std::vector<Vertex>& verticies, const std::vector<uint>& indicies,
	                const std::vector<Texture>& textures, const float shininess,
	                const std::shared_ptr<ShaderProgram> shader)
	    : BaseSceneGraphObject(shader, glm::mat4(1)) {
		this->verticies = verticies;
		this->indicies = indicies;
		this->textures = textures;
		this->shininess = shininess;

		this->setupMesh();
	}

	Mesh<ColorVertex>(const std::vector<Vertex>& verticies, const std::vector<uint>& indicies,
	                  const float shininess, const std::shared_ptr<ShaderProgram> shader)
	    : BaseSceneGraphObject(shader, glm::mat4(1)) {
		this->verticies = verticies;
		this->indicies = indicies;
		this->shininess = shininess;
		this->shader = shader;

		this->setupMesh();
	}

	virtual void print(const SceneCascade& cascade) {
		std::println("{}Mesh:", std::string(SCENE_GRAPH_INDENT * cascade.recurseDepth, ' '));
	}

	// sets uniforms and stuff
	virtual void render(const Camera& camera, const SceneCascade& cascade);

	// actually draws the object; can assume the shader is correctly set
	void draw();

	virtual ~Mesh() = default;
};

#endif /* MESH_HPP */
