#pragma once
#include <glm/vec3.hpp>

struct DirLightData
{
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
};
