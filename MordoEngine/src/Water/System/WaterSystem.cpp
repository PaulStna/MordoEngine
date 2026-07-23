#include "WaterSystem.h"
#include <glm/gtc/matrix_transform.hpp>

WaterSystem::WaterSystem() :
	m_Renderer(std::make_unique<PlaneRenderer>()),
	m_ReflectionFramebuffer(std::make_unique<Framebuffer>()),
	m_RefractionFramebuffer(std::make_unique<Framebuffer>())
{

}

void WaterSystem::Update(float deltaTime, const glm::vec3& cameraPosition)
{
	for (WaterTile& waterTile : m_WaterTiles)
	{
		waterTile.Update(deltaTime);
	}
	m_IsUnderwater = cameraPosition.y <= m_WaterLevel;
}

void WaterSystem::CaptureReflectionRefraction(
	const RenderContext& renderContext,
	const Camera& camera,
	const RenderPass& renderPass)
{
	// Clipping is only needed while capturing the reflection/refraction halves.
	glEnable(GL_CLIP_DISTANCE0);

	for (const WaterTile& waterTile : m_WaterTiles)
	{
		const WaterTileData& data = waterTile.GetData();

		//Reflection
		RenderReflection(data, renderContext, camera, renderPass);

		//Refraction
		RenderRefraction(data, renderContext, renderPass);
	}

	// The main scene and the water plane itself must not be clipped.
	glDisable(GL_CLIP_DISTANCE0);
}

void WaterSystem::RenderSurface(
	const Shader& waterShader,
	const Texture& waterDuDvMapTexture,
	const RenderContext& renderContext)
{
	for (const WaterTile& waterTile : m_WaterTiles)
	{
		const WaterTileData& data = waterTile.GetData();

		// Whether the camera is below the water surface.
		const bool underwater = renderContext.cameraPos.y <= data.yPos;

		waterShader.Use();
		waterShader.SetMat4("projection", renderContext.projection);
		waterShader.SetMat4("view", renderContext.view);

		glm::mat4 newModel = glm::translate(glm::mat4(1.0f), data.position);
		newModel = glm::scale(newModel, data.scale);
		waterShader.SetMat4("model", newModel);
		waterShader.SetFloat("yPos", data.yPos);

		waterShader.SetFloat("waveStrength", data.waveStrength);
		waterShader.SetFloat("moveFactor", data.moveFactor);
		waterShader.SetVec3("cameraPosition", renderContext.cameraPos);
		waterShader.SetInt("underwater", underwater ? 1 : 0);

		waterShader.SetInt("reflectionTexture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_ReflectionFramebuffer->GetTextureID());

		waterShader.SetInt("refractionTexture", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_RefractionFramebuffer->GetTextureID());

		waterShader.SetInt("dudvMap", 2);
		glActiveTexture(GL_TEXTURE2);
		waterDuDvMapTexture.Use();

		if (underwater)
		{
			// From below, draw the surface as a translucent sheet that does not
			// write depth, so the submerged terrain stays visible through and
			// around it instead of being hidden behind the reflected sky.
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);

			m_Renderer->Render();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
		else
		{
			m_Renderer->Render();
		}
	}
}

void WaterSystem::RenderRefraction(const WaterTileData& waterTile,
	const RenderContext& renderContext,
	const RenderPass& renderPass)
{
	const float clipY = waterTile.yPos + offSet;

	// Keep only what is below the surface.
	RenderContext pass = renderContext;
	pass.clipPlane = glm::vec4(0.0f, -1.0f, 0.0f, clipY);

	m_RefractionFramebuffer->BindBuffer();
	renderPass(pass);
	m_RefractionFramebuffer->UnbindBuffer();
}

void WaterSystem::RenderReflection(const WaterTileData& waterTile,
	const RenderContext& renderContext,
	const Camera& camera,
	const RenderPass& renderPass)
{
	const glm::vec3 camPos = camera.GetPosition();
	const glm::vec3 camForward = camera.GetForward();
	const glm::vec3 camRight = camera.GetRight();

	const glm::vec3 reflectedPos = glm::vec3(camPos.x, 2.0f * waterTile.yPos - camPos.y, camPos.z);
	const glm::vec3 reflectedForward = glm::vec3(camForward.x, -camForward.y, camForward.z);
	const glm::vec3 reflectedUp = glm::cross(camRight, reflectedForward);

	const float clipY = waterTile.yPos + offSet;

	// Mirror the camera across the surface and keep only what is above it.
	RenderContext pass = renderContext;
	pass.view = glm::lookAt(reflectedPos, reflectedPos + reflectedForward, reflectedUp);
	pass.cameraPos.y = 2.0f * clipY - renderContext.cameraPos.y;
	pass.clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, -clipY);

	m_ReflectionFramebuffer->BindBuffer();
	renderPass(pass);
	m_ReflectionFramebuffer->UnbindBuffer();
}

void WaterSystem::RenderUnderwater(
	const Shader& underwaterShader,
	const Texture& waterDuDvMapTexture,
	const RenderContext& renderContext,
	const GLuint textureId,
	const float deltaTime,
	const unsigned int screenWidth, const unsigned int screenHeight)
{
	float submergence = 0.0f;
	if (m_IsUnderwater)
		submergence = glm::clamp((m_WaterLevel - renderContext.cameraPos.y) / 300.0f, 0.0f, 1.0f);

	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	underwaterShader.Use();
	underwaterShader.SetInt("sceneTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	underwaterShader.SetInt("dudvMap", 1);
	glActiveTexture(GL_TEXTURE1);
	waterDuDvMapTexture.Use();

	underwaterShader.SetInt("underwater", m_IsUnderwater ? 1 : 0);
	underwaterShader.SetFloat("time", deltaTime);
	underwaterShader.SetFloat("submergence", submergence);

	m_Renderer->Render();
	glEnable(GL_DEPTH_TEST);
}

void WaterSystem::AddWaterTile(const WaterTile waterTile)
{
	m_WaterTiles.push_back(waterTile);
}

void WaterSystem::AddWaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos)
{
	m_WaterTiles.push_back(WaterTile(position, scale, yPos));
}
