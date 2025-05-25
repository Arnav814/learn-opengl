#ifndef MODEL_HPP
#define MODEL_HPP

#include "assimp2glm.hpp"
#include "common.hpp"
#include "loadTexture.hpp"
#include "mesh.hpp"
#include "shaders.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>

// adds to the passed in textures vector
void loadMaterialTextures(std::vector<Texture>& textures, const filesystem::path& dirPath,
                          const aiMaterial* mat, const aiTextureType type,
                          const TextureType textureType);

class Model {
  private:
	std::vector<Mesh> meshes;
	filesystem::path directory;

	void loadModel(const filesystem::path& path);

	void processNode(const aiNode* node, const aiScene* scene);

	Mesh processMesh(const aiMesh* mesh, const aiScene* scene);

  public:
	Model(const filesystem::path& path) {
		this->directory = path.parent_path();
		this->loadModel(path);
	}

	void draw(ShaderProgram& shader) {
		for (Mesh& mesh : this->meshes) {
			mesh.draw(shader);
		}
	}
};

#endif /* MODEL_HPP */
