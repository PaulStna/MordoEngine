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
						const glm::mat4* projection,
						const glm::mat4* view,
						const glm::mat4* model)
{
	shader.Use();
	if (view) {
		glm::mat4 skyboxView = glm::mat4(glm::mat3(*view));
		shader.SetMat4("view", skyboxView);
	}
	if (projection) {
		shader.SetMat4("projection", *projection);
	}
	
	shader.SetInt("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	texture.UseCubeMap();

	m_Renderer->Render();
}