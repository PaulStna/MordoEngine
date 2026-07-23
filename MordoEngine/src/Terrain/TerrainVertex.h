#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace terrain
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 normal = glm::vec3(0.f);
		float height = 0.0f;
	};
}
