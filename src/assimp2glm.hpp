#ifndef ASSIMP2GLM_HPP
#define ASSIMP2GLM_HPP

#include "common.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <assimp/vector3.h>
#include <assimp/vector2.h>

inline glm::vec2 toGlm(const aiVector2D& aiVector) {
	glm::vec2 glmVector;
	glmVector.x = aiVector.x;
	glmVector.y = aiVector.y;
	return glmVector;
}

inline glm::vec3 toGlm(const aiVector3D& aiVector) {
	glm::vec3 glmVector;
	glmVector.x = aiVector.x;
	glmVector.y = aiVector.y;
	glmVector.z = aiVector.z;
	return glmVector;
}

#endif /* ASSIMP2GLM_HPP */
