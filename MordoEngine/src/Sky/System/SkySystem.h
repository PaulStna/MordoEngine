#pragma once
#include "../../Core/Shader/Shader.h"
#include "../../Core/Texture/Texture.h"
#include "../../Renderer/Skybox/SkyBoxRenderer.h"
#include <memory>

class SkySystem 
{
private:
	std::unique_ptr<SkyBoxRenderer> m_Renderer;

public:
	SkySystem();
	void Update(float deltaTime);
	void Render(const Shader& shader,
		        const Texture& texture,
				const glm::mat4* projection,
				const glm::mat4* view,
				const glm::mat4* model);
};