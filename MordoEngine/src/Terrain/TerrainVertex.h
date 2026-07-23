#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

// The terrain vertex layout. Produced by whichever meshing technique is in use
// and consumed by the GL renderer, so it belongs to neither.
struct TerrainVertex
{
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal = glm::vec3(0.f);
	float height = 0.0f;
};
