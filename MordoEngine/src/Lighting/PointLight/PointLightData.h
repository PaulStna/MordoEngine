#pragma once
#include <glm/vec3.hpp>

/// One point light as the shaders see it, matching the PointLight struct
/// declared in GLSL.
struct PointLightData
{
	glm::vec3 position;    // world space
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	// Attenuation, applied as 1 / (constant + linear*d + quadratic*d*d). The
	// quadratic term is what makes the falloff physical; the other two keep it
	// from blowing up close to the source.
	float constant;
	float linear;
	float quadratic;
};
