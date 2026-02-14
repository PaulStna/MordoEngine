#pragma once
#include <glm/vec3.hpp>
#include "../Core/Shader/Shader.h"

class Renderer
{
public:
	virtual void Render(const Shader& shader, const glm::vec3& cameraPos) = 0;
	virtual void SetHeights(const std::vector<float>& heights, const glm::vec3& position) = 0;
	virtual ~Renderer() = default;
};
