#ifndef SCENEOBJECT_CPP
#define SCENEOBJECT_CPP

#include "sceneObject.hpp"

void recursivelyRender(const std::shared_ptr<BaseSceneGraphNode> node, const Camera& camera,
                       std::vector<SceneCascade>& stack) {
	// render this node
	SceneCascade lastCascade;
	if (stack.size() > 0) lastCascade = stack.back();
	else lastCascade = {};
	node->render(camera, lastCascade);

	// add this node's cascade to the stack
	SceneCascade newCascade = lastCascade + node->getNodeCascade();
	stack.push_back(newCascade);

	// recurse into child nodes
	for (auto child : node->getChildren()) {
		recursivelyRender(child, camera, stack);
	}

	// remove this node's stack entry
	stack.pop_back();
}

void recursivelyPrint(const std::shared_ptr<BaseSceneGraphNode> node,
                      std::vector<SceneCascade>& stack) {
	// render this node
	SceneCascade lastCascade;
	if (stack.size() > 0) lastCascade = stack.back();
	else lastCascade = {};
	node->print(lastCascade);

	// add this node's cascade to the stack
	SceneCascade newCascade = lastCascade + node->getNodeCascade();
	newCascade.recurseDepth++;
	stack.push_back(newCascade);

	// recurse into child nodes
	for (auto child : node->getChildren()) {
		recursivelyPrint(child, stack);
	}

	// remove this node's stack entry
	stack.pop_back();
}

#endif /* SCENEOBJECT_CPP */
