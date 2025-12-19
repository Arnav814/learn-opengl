#include "model.hpp"

#include "assimp2glm.hpp"
#include "object.hpp"

void loadMaterialTextures(std::vector<Texture>& textures, const filesystem::path& dirPath,
                          const aiMaterial* mat, const aiTextureType type,
                          const TextureType textureType) {
	static std::unordered_map<filesystem::path, Texture> textureCache{};

	for (uint i = 0; i < mat->GetTextureCount(type); i++) {
		aiString aiPath;
		mat->GetTexture(type, i, &aiPath);
		filesystem::path asFsPath{std::string(aiPath.C_Str())};
		filesystem::path fullPath = dirPath / asFsPath;
		Texture texture;

		auto cachedTexture = textureCache.find(fullPath);
		if (cachedTexture != textureCache.end()) { // already loaded
			texture = cachedTexture->second;
		} else { // load texture
			texture.id = loadTexture(fullPath);
			texture.type = textureType;
			textureCache[fullPath] = texture;
		}
		textures.push_back(texture);
	}
}

void Model::loadModel(const filesystem::path& path) {
	Assimp::Importer importer{};
	const aiScene* scene = importer.ReadFile(
	    path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
	              | aiProcess_ValidateDataStructure | aiProcess_GenSmoothNormals);
	// check for errors
	if (not scene or scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE or not scene->mRootNode) {
		throw std::runtime_error(std::format("Failed to load model at {}: {}", path.string(),
		                                     importer.GetErrorString()));
	}

	// recursively process all nodes
	processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode* node, const aiScene* scene) {
	// process this node's meshes
	for (uint i = 0; i < node->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->addChild(
		    std::make_shared<Mesh<TexVertex, Shaders::Object>>(this->processMesh(mesh, scene)));
	}

	// recurse into child nodes
	for (uint i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh<TexVertex, Shaders::Object> Model::processMesh(const aiMesh* mesh, const aiScene* scene) {
	std::vector<TexVertex> verticies;
	std::vector<uint> indicies;
	std::vector<Texture> textures;
	float shininess = 0;

	verticies.reserve(mesh->mNumVertices);
	for (uint i = 0; i < mesh->mNumVertices; i++) {
		// setup vertex
		TexVertex vertex{};
		vertex.position = toGlm(mesh->mVertices[i]);
		vertex.normal = toGlm(mesh->mNormals[i]);
		if (mesh->mTextureCoords[0]) vertex.texCoords = toGlm(mesh->mTextureCoords[0][i]);
		else vertex.texCoords = glm::vec2(0);
		verticies.push_back(vertex);
	}

	// process indicies
	for (uint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; j++) {
			indicies.push_back(face.mIndices[j]);
		}
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		loadMaterialTextures(textures, this->modelPath.parent_path(), material,
		                     aiTextureType_DIFFUSE, TextureType::textureDiffuse);
		loadMaterialTextures(textures, this->modelPath.parent_path(), material,
		                     aiTextureType_SPECULAR, TextureType::textureSpecular);
		auto statusCode = aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);
		if (statusCode != AI_SUCCESS) shininess = 32; // default value
	}

	return Mesh(verticies, indicies, textures, shininess, this->shader);
}
