#ifndef MODEL_HPP
#define MODEL_HPP

#include "common.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "sceneObject.hpp"
#include "shaders.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>

// adds to the passed in textures vector
void loadMaterialTextures(std::vector<Texture>& textures, const filesystem::path& dirPath,
                          const aiMaterial* mat, const aiTextureType type,
                          const TextureType textureType);

class Model : public BaseSceneGraphObject {
  private:
	filesystem::path modelPath;
	Shaders::Object shader;

	void loadModel(const filesystem::path& path);

	void processNode(const aiNode* node, const aiScene* scene);

	Mesh<TexVertex, Shaders::Object> processMesh(const aiMesh* mesh, const aiScene* scene);

  public:
	Model(const filesystem::path& path, const Shaders::Object shader)
	    : BaseSceneGraphObject(glm::mat4(1)) {
		this->modelPath = path;
		this->shader = shader;
		this->loadModel(path);
	}

	// meshes do the actual drawing
	virtual void render(const Camera& camera [[maybe_unused]],
	                    const SceneCascade& cascade [[maybe_unused]]) {}

	virtual void print(const SceneCascade& cascade) {
		std::println("{}Model: {}", std::string(SCENE_GRAPH_INDENT * cascade.recurseDepth, ' '),
		             this->modelPath.string());
	}

	virtual ~Model() = default;
};

#endif /* MODEL_HPP */
