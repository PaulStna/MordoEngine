#pragma once
#include <glm/vec3.hpp>

/// One square patch of water as plain data: where it sits, how big it is, and
/// the state its ripples are animated with.
struct WaterTileData
{
	// Centre of the tile. The Y here is not what the surface sits at; yPos is.
	glm::vec3 position;

	glm::vec3 scale;

	// World-space height of the surface, and the value the camera is compared
	// against to decide whether the underwater post-process runs.
	float yPos;

	// How far the dudv map pushes the sampled coordinates. Higher is choppier.
	float waveStrength = 0.02f;

	// Scrolls with time to drive the distortion. Wraps at 1, so it never grows
	// large enough to lose precision.
	float moveFactor = 0.0f;
};
