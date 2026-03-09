#pragma once
#include <glm/vec3.hpp>

struct WaterTileData
{
	glm::vec3 position;
	glm::vec3 scale;
	float yPos;
	float waveStrength = 0.02f;
	float moveFactor = 0.0f;
};