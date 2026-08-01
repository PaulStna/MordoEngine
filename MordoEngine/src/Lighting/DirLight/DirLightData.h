#pragma once
#include <glm/vec3.hpp>

/// The sun as the shaders see it: one direction and the two colour terms it
/// contributes, matching the DirLight struct declared in GLSL.
struct DirLightData
{
	// Direction the light travels, normalized, not the direction towards it.
	glm::vec3 direction;

	// Floor colour applied everywhere, so surfaces facing away are lit rather
	// than black.
	glm::vec3 ambient;

	// Colour of the directional term itself, scaled by the surface normal.
	glm::vec3 diffuse;
};
