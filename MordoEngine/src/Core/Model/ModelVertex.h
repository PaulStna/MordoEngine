#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/// How many bones may influence a single vertex. Four is what hardware skinning
/// normally assumes, and the loader asks Assimp to reduce down to this.
inline constexpr int MAX_BONE_INFLUENCES = 4;

/// How many bones a single model may have. Must match MAX_BONES in model.vs;
/// raising one without the other overruns the shader's uniform array.
inline constexpr int MAX_BONES = 100;

/// The model vertex layout, produced by the Assimp loader and consumed by the
/// GL renderer. The two must agree on this struct and on nothing else.
struct ModelVertex
{
	glm::vec3 pos;
	glm::vec2 texCoord;
	glm::vec3 normal = glm::vec3(0.f);

	// Skinning. Unused slots hold bone -1 with weight 0, so a vertex that no
	// bone touches costs nothing and needs no special case in the shader.
	glm::ivec4 boneIds = glm::ivec4(-1);
	glm::vec4  weights = glm::vec4(0.0f);
};
