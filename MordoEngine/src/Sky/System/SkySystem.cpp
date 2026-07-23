#include "SkySystem.h"

SkySystem::SkySystem() : m_Renderer(std::make_unique<SkyBoxRenderer>())
{

}

void SkySystem::Update(float deltaTime)
{
	//
}

void SkySystem::Render(const Shader& shader,
						const Texture& texture,
						const RenderContext& renderContext)
{
	shader.Use();

	// Strip the translation so the skybox stays centred on the camera.
	glm::mat4 skyboxView = glm::mat4(glm::mat3(renderContext.view));
	shader.SetMat4("view", skyboxView);
	shader.SetMat4("projection", renderContext.projection);

	shader.SetInt("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	texture.UseCubeMap();

	m_Renderer->Render();
}