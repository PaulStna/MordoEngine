#include "WaterSystem.h"
#include <glm/gtc/matrix_transform.hpp>

WaterSystem::WaterSystem() :
	m_Renderer(std::make_unique<PlaneRenderer>()),
	m_ReflectionFramebuffer(std::make_unique<Framebuffer>()),
	m_RefractionFramebuffer(std::make_unique<Framebuffer>())
{

}

void WaterSystem::Update(float deltaTime)
{
	for (WaterTile& waterTile : m_WaterTiles)
	{
		waterTile.Update(deltaTime);
	}
}

void WaterSystem::Render(
	const Shader& waterShader,
	const Texture& waterDuDvMapTexture,
	Camera& camera,
	const glm::mat4* projection,
	const glm::mat4* model,
	std::function<void(float waterY, const glm::mat4* reflectedView)> renderCallback)
{

	for (const WaterTile& waterTile : m_WaterTiles)
	{
		const WaterTileData& data = waterTile.GetData();

		//Reflection
		RenderReflection(data, camera, renderCallback);

		//Refraction
		RenderRefraction(data, renderCallback);

		//Water Plane
		glm::mat4 currentView = camera.GetViewMatrix();

		waterShader.Use();
		if (projection) waterShader.SetMat4("projection", *projection);
		waterShader.SetMat4("view", currentView);

		glm::mat4 newModel = glm::translate(glm::mat4(1.0f), data.position);
		newModel = glm::scale(newModel, data.scale);
		waterShader.SetMat4("model", newModel);
		waterShader.SetFloat("yPos", data.yPos);

		waterShader.SetFloat("waveStrength", data.waveStrength);
		waterShader.SetFloat("moveFactor", data.moveFactor);

		waterShader.SetInt("reflectionTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_ReflectionFramebuffer->GetTextureID());
		
		waterShader.SetInt("refractionTexture", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_RefractionFramebuffer->GetTextureID());

		waterShader.SetInt("dudvMap", 2);
		glActiveTexture(GL_TEXTURE2);
		waterDuDvMapTexture.Use();

		m_Renderer->Render();
	}
}

void WaterSystem::RenderRefraction(const WaterTileData& waterTile,
	std::function<void(float waterY, const glm::mat4* reflectedView)> renderCallback)
{
	m_RefractionFramebuffer->BindBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderCallback(waterTile.yPos + offSet, nullptr);
	m_RefractionFramebuffer->UnbindBuffer();
}

void WaterSystem::RenderReflection(const WaterTileData& waterTile,
	Camera& camera,
	std::function<void(float waterY, const glm::mat4* reflectedView)> renderCallback)
{
	glm::vec3 camPos = camera.GetPosition();
	glm::vec3 camForward = camera.GetForward();
	glm::vec3 camRight = camera.GetRight();

	glm::vec3 reflectedPos = glm::vec3(camPos.x, 2.0f * waterTile.yPos - camPos.y, camPos.z);
	glm::vec3 reflectedForward = glm::vec3(camForward.x, -camForward.y, camForward.z);
	glm::vec3 reflectedUp = glm::cross(camRight, reflectedForward);

	const glm::mat4 reflectedView = glm::lookAt(
		reflectedPos,
		reflectedPos + reflectedForward,
		reflectedUp
	);

	m_ReflectionFramebuffer->BindBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderCallback(waterTile.yPos + offSet, &reflectedView);
	m_ReflectionFramebuffer->UnbindBuffer();
}

void WaterSystem::AddWaterTile(const WaterTile waterTile)
{
	m_WaterTiles.push_back(waterTile);
}

void WaterSystem::AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos)
{
	m_WaterTiles.push_back(WaterTile(position, scale, yPos));
}