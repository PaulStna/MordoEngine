#include "World.h"
#include "../Core/Managers/Manager.h"
#include "../Core/Shader/Shader.h"
#include "../Core/Texture/Texture.h"
#include <glad/glad.h>

World::World(unsigned int screenWidth, unsigned int screenHeight)
	: m_TerrainSystem(),
	m_Camera(m_TerrainSystem.GetMiddleTerrainPosition(), screenWidth, screenHeight),
	m_LightSystem(),
	m_WaterSystem(),
	m_SkySystem()
{
	glm::vec3 center = m_TerrainSystem.GetMiddleTerrainPosition();
	float yOffset = 0.2f;
	float scale = m_TerrainSystem.GetTerrainWorldScale();

	auto addLight = [&](float offsetX, float offsetZ)
		{
			float x = center.x + offsetX * scale;
			float z = center.z + offsetZ * scale;
			float y = m_TerrainSystem.GetTerrainInterpolatedHeightAt(x, z, yOffset);
			m_LightSystem.AddPointLight(PointLight(glm::vec3(x, y, z)));
		};

	addLight(50.0f, 50.0f);
	addLight(-100.0f, -100.0f);

	float waterLevel = m_TerrainSystem.GetTerrainHeightScale() * 0.3f;
	m_WaterSystem.AddWaterTile(
		glm::vec3(center.x, 0.0f, center.z),
		glm::vec3(700.0f, 1.0f, 700.0f),
		waterLevel);
}

void World::Update(float deltaTime)
{
	m_TerrainSystem.Update(deltaTime);
	m_LightSystem.Update(deltaTime);
	m_SkySystem.Update(deltaTime);
	m_WaterSystem.Update(deltaTime);
}

RenderContext World::MakeRenderContext() const
{
	RenderContext ctx;
	ctx.projection = m_Camera.GetProjectionMatrix();
	ctx.view = m_Camera.GetViewMatrix();
	ctx.model = glm::mat4(1.0f);
	ctx.cameraPos = m_Camera.GetPosition();
	ctx.clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	return ctx;
}

void World::RenderOpaque(const RenderContext& ctx)
{
	Shader& terrainShader = Manager<Shader>::Get("terrain");
	Shader& cubeLightShader = Manager<Shader>::Get("lightCube");
	Shader& skyShader = Manager<Shader>::Get("skyBox");
	Texture& skyTexture = Manager<Texture>::Get("skyBox");

	terrainShader.Use();
	terrainShader.SetVec4("plane", ctx.clipPlane);

	m_LightSystem.Render(terrainShader, cubeLightShader, ctx.cameraPos,
		&ctx.projection, &ctx.view, &ctx.model);
	m_TerrainSystem.Render(terrainShader, ctx.cameraPos,
		&ctx.projection, &ctx.view, &ctx.model);
	m_SkySystem.Render(skyShader, skyTexture,
		&ctx.projection, &ctx.view, &ctx.model);
}

void World::Render(const RenderContext& ctx)
{
	Shader& waterShader = Manager<Shader>::Get("water");
	Texture& waterDuDv = Manager<Texture>::Get("dudvMap");

	glm::mat4 projection = ctx.projection;

	glEnable(GL_CLIP_DISTANCE0);
	m_WaterSystem.Render(
		waterShader, waterDuDv, m_Camera, &projection, nullptr,
		[&](float waterY, const glm::mat4* reflectedView)
		{
			RenderContext pass = ctx;
			if (reflectedView) {
				pass.view = *reflectedView;
				pass.cameraPos.y = 2.0f * waterY - ctx.cameraPos.y;
				pass.clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, -waterY);
			}
			else {
				pass.clipPlane = glm::vec4(0.0f, -1.0f, 0.0f, waterY);
			}
			RenderOpaque(pass);
		});

	RenderContext mainPass = ctx;
	mainPass.clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	RenderOpaque(mainPass);
}