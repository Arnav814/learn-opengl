#ifndef VERTEXDATA_HPP
#define VERTEXDATA_HPP
#include "shaderStructs.hpp"
#include <glm/ext/vector_float3.hpp>

#include <vector>

std::vector<float> getVertexData();

std::vector<glm::vec3> getCubePositions();

std::vector<PointLight> getPointLights();

#endif /* VERTEXDATA_HPP */
