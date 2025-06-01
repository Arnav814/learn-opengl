#ifndef SCENEOBJECT_HPP
#define SCENEOBJECT_HPP

#include "camera.hpp"
#include "common.hpp"
#include "shaders.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

#define SCENE_GRAPH_INDENT 4

// stores info about parent nodes
struct SceneCascade {
	glm::mat4 transform;
	uint recurseDepth; // how deep we're in; first node is 0

	// recurseDepth shouldn't usually be specified
	SceneCascade(const glm::mat4& transform, uint recurseDepth) {
		this->transform = transform;
		this->recurseDepth = recurseDepth;
	}

	SceneCascade(const glm::mat4& transform) {
		this->transform = transform;
		this->recurseDepth = 0;
	}

	SceneCascade() {
		this->transform = glm::mat4(1);
		this->recurseDepth = 0;
	};

	SceneCascade operator+(const SceneCascade& other) const {
		return {other.transform * this->transform, this->recurseDepth + other.recurseDepth};
	}
};

// this will probably get more complex in the future
// only handles children
class BaseSceneGraphNode {
  private:
	std::vector<std::shared_ptr<BaseSceneGraphNode>> children;

  public:
	BaseSceneGraphNode() { this->children = {}; }

	const std::vector<std::shared_ptr<BaseSceneGraphNode>>& getChildren() const {
		return this->children;
	}

	void addChild(const std::shared_ptr<BaseSceneGraphNode> child) {
		this->children.push_back(child);
	}

	void rmChild(const uint index) { this->children.erase(this->children.begin() + index); }

	virtual SceneCascade getNodeCascade() const = 0;
	virtual void render(const Camera& camera, const SceneCascade& cascade) = 0;
	virtual void print(const SceneCascade& cascade) = 0;

	~BaseSceneGraphNode() = default;
};

// only has children, does nothing else
// initializes an empty cascade and has a passthrough render method
class SceneGraphRoot : public BaseSceneGraphNode {
  public:
	SceneGraphRoot() : BaseSceneGraphNode() {}

	virtual SceneCascade getNodeCascade() const { return {}; }

	virtual void render(const Camera& camera [[maybe_unused]],
	                    const SceneCascade& cascade [[maybe_unused]]) {}

	virtual void print(const SceneCascade& cascade) {
		std::println("{}Root:", std::string(SCENE_GRAPH_INDENT * cascade.recurseDepth, ' '));
	}

	virtual ~SceneGraphRoot() = default;
};

// a physical object, like a rock or light or something
// has to have a defined 3d position (not terrain)
class BaseSceneGraphObject : public BaseSceneGraphNode {
  protected:
	ShaderPtr shader;
	glm::mat4 transform; // position of the object

  public:
	BaseSceneGraphObject(const ShaderPtr shader, const glm::mat4& transform)
	    : BaseSceneGraphNode() {
		this->shader = shader;
		assert(transform != glm::mat4()); // can't have an empty (all 0s) matrix
		this->transform = transform;
	}

	virtual SceneCascade getNodeCascade() const { return {this->transform}; }

	virtual ~BaseSceneGraphObject() = default;
};

void recursivelyRender(const std::shared_ptr<BaseSceneGraphNode> node, const Camera& camera,
                       std::vector<SceneCascade>& stack);

void recursivelyPrint(const std::shared_ptr<BaseSceneGraphNode> node,
                      std::vector<SceneCascade>& stack);

#endif /* SCENEOBJECT_HPP */
