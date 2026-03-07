#include "WaterSystem.h"
#include <glm/gtc/matrix_transform.hpp>

WaterSystem::WaterSystem() : m_Renderer(std::make_unique<PlaneRenderer>())
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
	shader.Use();
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