#ifndef SCENEOBJECT_CPP
#define SCENEOBJECT_CPP

#include "sceneObject.hpp"

void recursivelyDo(const std::shared_ptr<BaseSceneGraphNode> node, std::vector<SceneCascade>& stack,
                   const std::function<void(BaseSceneGraphNode&, const SceneCascade&)>& operation) {
	// render this node
	SceneCascade lastCascade;
	if (stack.size() > 0) lastCascade = stack.back();
	else lastCascade = {};
	operation(*node, lastCascade);

	// add this node's cascade to the stack
	SceneCascade newCascade = lastCascade + node->getNodeCascade();
	newCascade.recurseDepth++;
	stack.push_back(newCascade);

	// recurse into child nodes
	for (auto child : node->getChildren()) {
		recursivelyDo(child, stack, operation);
	}

	// remove this node's stack entry
	stack.pop_back();
}

void recursivelyRender(const std::shared_ptr<BaseSceneGraphNode> node, const Camera& camera,
                       std::vector<SceneCascade>& stack) {
	auto renderOp = [&camera](BaseSceneGraphNode& currentNode, const SceneCascade& cascade) {
		currentNode.render(camera, cascade);
	};
	recursivelyDo(node, stack, renderOp);
}

void recursivelyPrint(const std::shared_ptr<BaseSceneGraphNode> node,
                      std::vector<SceneCascade>& stack) {
	auto printOp = [](BaseSceneGraphNode& currentNode, const SceneCascade& cascade) {
		currentNode.print(cascade);
	};
	recursivelyDo(node, stack, printOp);
}

#endif /* SCENEOBJECT_CPP */
