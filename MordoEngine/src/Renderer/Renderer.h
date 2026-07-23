#pragma once
#include "../Core/Shader/Shader.h"
#include "RenderContext.h"

class Renderer
{
protected:
	Shader& p_Shader;

public:
	Renderer(Shader& shader) : p_Shader(shader) {}

	virtual void Render(const RenderContext& renderContext) = 0;

	virtual ~Renderer() = default;
};
