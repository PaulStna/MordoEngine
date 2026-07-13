#pragma once
#include <glm/glm.hpp>

struct RenderContext
{
	glm::mat4 projection{ 1.0f };
	glm::mat4 view{ 1.0f };
	glm::mat4 model{ 1.0f };
	glm::vec3 cameraPos{ 0.0f };
	glm::vec4 clipPlane{ 0.0f, 1.0f, 0.0f, 0.0f };
};
