#include "WaterSystem.h"
#include <glm/gtc/matrix_transform.hpp>

WaterSystem::WaterSystem() : 
	m_Renderer(std::make_unique<PlaneRenderer>()),
	m_Framebuffer(std::make_unique<Framebuffer>())
{

}

void WaterSystem::Update(float deltaTime)
{
	for (WaterTile& waterTile : m_WaterTiles)
	{
		waterTile.Update(deltaTime);
	}
}

void WaterSystem::Render(const Shader& shader,
						const glm::vec3& cameraPos,
						const glm::mat4* projection,
						const glm::mat4* view,
						const glm::mat4* model)
{
	m_Framebuffer->BindBuffer();
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_Framebuffer->UnbindBuffer();

	shader.Use();
	shader.SetInt("uTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetTextureID());

	if (projection) {
		shader.SetMat4("projection", *projection);
	}
	if (view) {
		shader.SetMat4("view", *view);
	}
	for (const WaterTile& waterTile : m_WaterTiles)
	{
		const WaterTileData& waterTileData = waterTile.GetData();
		glm::mat4 newModel = glm::mat4(1.0f);
		newModel = glm::translate(newModel, waterTileData.position);
		newModel = glm::scale(newModel, waterTileData.scale);
		shader.SetMat4("model", newModel);
		shader.SetFloat("yPos", waterTileData.yPos);
		m_Renderer->Render();
	}
}

void WaterSystem::AddWaterTile(const WaterTile waterTile)
{
	m_WaterTiles.push_back(waterTile);
}

void WaterSystem::AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos)
{
	m_WaterTiles.push_back(WaterTile(position, scale, yPos));
}