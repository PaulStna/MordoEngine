#include "World.h"
#include "../Core/Shader/Shader.h"
#include "../Core/Texture/Texture.h"
#include "../Core/FileSystem/FileSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>
#include <iostream>

// Namespace for the test animation (fox) and model (lantern)
namespace
{
	// glTF Separate (.gltf + .bin + textures), never .glb: Texture only builds
	// from a file path, so embedded textures cannot be read.
	// Swap these for your own Blender exports; see docs/loading-blender-models.md.
	const char* FOX_MODEL_PATH = "res/models/fox/Fox.gltf";
	const glm::vec2 FOX_WORLD_XZ{ 650.0f, 718.0f };
	const float FOX_SCALE = 1.5f;
	const float FOX_YAW = 160.f;

	const char* LANTERN_MODEL_PATH = "res/models/lantern/Lantern.gltf";
	const glm::vec2 LANTERN_WORLD_XZ{ 524.0f, 700.0f };
	const float LANTERN_SCALE = 10.0f;
	const float LANTERN_YAW = 60.f;
}

World::World(unsigned int screenWidth, unsigned int screenHeight,
	ResourceLibrary<Shader>& shaders, ResourceLibrary<Texture>& textures)
	: m_Terrain(textures),
	m_Camera(m_Terrain.GetMiddleTerrainPosition(), screenWidth, screenHeight),
	m_Lights(),
	m_Water(),
	m_Sky(),
	m_TerrainShader(shaders.Get("terrain")),
	m_CubeLightShader(shaders.Get("lightCube")),
	m_SkyShader(shaders.Get("skyBox")),
	m_WaterShader(shaders.Get("water")),
	m_UnderwaterShader(shaders.Get("underwater")),
	m_ModelShader(shaders.Get("model")),
	m_SkyTexture(textures.Get("skyBox")),
	m_WaterDuDv(textures.Get("dudvMap")),
	m_SceneBuffer(std::make_unique<Framebuffer>()),
	m_ScreenQuad(std::make_unique<PlaneRenderer>()),
	m_ScreenWidth(screenWidth),
	m_ScreenHeight(screenHeight)
{
	// Lights
	glm::vec3 center = m_Terrain.GetMiddleTerrainPosition();
	float yOffset = 0.2f;
	float scale = m_Terrain.GetTerrainWorldScale();

	auto addLight = [&](float offsetX, float offsetZ)
		{
			float x = center.x + offsetX * scale;
			float z = center.z + offsetZ * scale;
			float y = m_Terrain.GetTerrainInterpolatedHeightAt(x, z, yOffset);
			m_Lights.AddPointLight(PointLight(glm::vec3(x, y, z)));
		};

	addLight(50.0f, 50.0f);
	addLight(-100.0f, -100.0f);

	// Water
	m_WaterLevel = m_Terrain.GetTerrainHeightScale() * 0.3f;
	m_Water.SetWaterLevel(m_WaterLevel);
	m_Water.AddWaterTile(
		glm::vec3(center.x, 0.0f, center.z),
		glm::vec3(700.0f, 1.0f, 700.0f),
		m_WaterLevel);

	// Models. All optional: a file that is not there just means the scene renders
	// without it.
	//
	// The clip name is taken here rather than returned to the caller on purpose.
	// Add invalidates any reference it handed out earlier, so nothing that points
	// into the vector is allowed to outlive the next call.
	auto dropOnTerrain = [&](const char* relativePath, const glm::vec2& xz,
		float uniformScale, float yawDegrees, const char* clip = nullptr)
		{
			const std::string path = FileSystem::getPath(relativePath);
			if (!std::filesystem::exists(path))
			{
				std::cout << "No model at " << path << ", skipping.\n";
				return;
			}

			Model& model = m_Models.Add(path, textures);

			// Both models have their pivot at the base, so the terrain height is
			// the position with no vertical correction.
			Transform& transform = model.GetTransform();
			transform.SetPosition({ xz.x,
				m_Terrain.GetTerrainInterpolatedHeightAt(xz.x, xz.y),
				xz.y });
			transform.SetScale(uniformScale);
			transform.SetYaw(yawDegrees);

			std::cout << relativePath << " at y=" << transform.GetPosition().y
				<< ", scale " << uniformScale;

			if (const Animator* animator = model.GetAnimator())
			{
				std::cout << ", clips:";
				for (size_t i = 0; i < animator->GetAnimationCount(); i++)
				{
					std::cout << " " << animator->GetAnimationName(i);
				}

				if (clip && !model.PlayAnimation(clip))
				{
					std::cout << " (no clip named '" << clip << "')";
				}
			}

			std::cout << "\n";
		};

	std::cout << "Water level " << m_WaterLevel << ".\n";
	dropOnTerrain(LANTERN_MODEL_PATH, LANTERN_WORLD_XZ, LANTERN_SCALE, LANTERN_YAW);
	dropOnTerrain(FOX_MODEL_PATH, FOX_WORLD_XZ, FOX_SCALE, FOX_YAW, "Walk");
}

void World::Update(float deltaTime)
{
	m_Time += deltaTime;
	m_Terrain.Update(deltaTime);
	m_Lights.Update(deltaTime);
	m_Sky.Update(deltaTime);
	m_Water.Update(deltaTime, m_Camera.GetPosition());
	m_Models.Update(deltaTime);
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

void World::RenderOpaque(const RenderContext& renderContext)
{
	m_TerrainShader.Use();
	m_TerrainShader.SetVec4("plane", renderContext.clipPlane);

	m_Lights.Render(m_TerrainShader, m_CubeLightShader, renderContext);
	m_Terrain.Render(m_TerrainShader, renderContext);

	if (!m_Models.IsEmpty())
	{
		m_Lights.ApplyUniforms(m_ModelShader, renderContext);
		m_Models.Render(m_ModelShader, renderContext);
	}

	m_Sky.Render(m_SkyShader, m_SkyTexture, renderContext);
}

void World::Render()
{
	// 1) Capture the reflection/refraction textures. 
	// WaterSystem derives the context for each pass and manages GL_CLIP_DISTANCE0 around them.
	RenderContext renderContext = MakeRenderContext();
	m_Water.CaptureReflectionRefraction(
		renderContext,
		m_Camera,
		[this](const RenderContext& pass) { RenderOpaque(pass); });

	// 2) Render the opaque scene plus the water surface. 
	// Only when the camera is submerged do we capture it into the offscreen 
	// buffer for the underwater post-process; otherwise we draw straight to the screen.
	const bool underwater = m_Water.GetIsUnderwater();

	// Above water we draw straight to the screen, 
	if (underwater)
	{
		m_SceneBuffer->BindBuffer();
		RenderSceneAndWater(renderContext);
		m_SceneBuffer->UnbindBuffer();
		m_Water.RenderUnderwater(
			m_UnderwaterShader,
			m_WaterDuDv,
			renderContext,
			m_SceneBuffer->GetTextureID(),
			m_Time,
			m_ScreenWidth,
			m_ScreenHeight
		);
	}
	else
	{
		RenderSceneAndWater(renderContext);
	}
}

void World::RenderSceneAndWater(const RenderContext& renderContext)
{
	RenderOpaque(renderContext);
	m_Water.RenderSurface(m_WaterShader, m_WaterDuDv, renderContext);
}
