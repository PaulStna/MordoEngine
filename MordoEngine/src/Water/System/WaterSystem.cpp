#include "WaterSystem.h"
#include <glm/gtc/matrix_transform.hpp>

WaterSystem::WaterSystem() :
	m_Renderer(std::make_unique<PlaneRenderer>()),
	m_ReflectionFramebuffer(std::make_unique<Framebuffer>())
{

}

void WaterSystem::Update(float deltaTime)
{
	for (WaterTile& waterTile : m_WaterTiles)
	{
		waterTile.Update(deltaTime);
	}
}

void WaterSystem::Render(const Shader& terrainShader,
	const Shader& waterShader,
	Camera& camera,
	const glm::mat4* projection,
	const glm::mat4* model,
	std::function<void(float waterY, const glm::mat4& reflectedView)> renderCallback)
{
	//Reflection
	for (const WaterTile& waterTile : m_WaterTiles)
	{
		const WaterTileData& data = waterTile.GetData();
		glm::vec3 camPos = camera.GetPosition();
		glm::vec3 camForward = camera.GetForward();
		glm::vec3 camRight = camera.GetRight();

		glm::vec3 reflectedPos = glm::vec3(camPos.x, 2.0f * data.yPos - camPos.y, camPos.z);
		glm::vec3 reflectedForward = glm::vec3(camForward.x, -camForward.y, camForward.z);
		glm::vec3 reflectedUp = glm::cross(camRight, reflectedForward);

		glm::mat4 reflectedView = glm::lookAt(
			reflectedPos,
			reflectedPos + reflectedForward,
			reflectedUp
		);

		m_ReflectionFramebuffer->BindBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		terrainShader.Use();
		renderCallback(data.yPos, reflectedView);

		m_ReflectionFramebuffer->UnbindBuffer();

		glm::mat4 currentView = camera.GetViewMatrix();

		waterShader.Use();
		if (projection) waterShader.SetMat4("projection", *projection);
		waterShader.SetMat4("view", currentView);

		glm::mat4 newModel = glm::translate(glm::mat4(1.0f), data.position);
		newModel = glm::scale(newModel, data.scale);
		waterShader.SetMat4("model", newModel);
		waterShader.SetFloat("yPos", data.yPos);
		waterShader.SetInt("uTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_ReflectionFramebuffer->GetTextureID());

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