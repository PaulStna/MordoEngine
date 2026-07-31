#include "World.h"
#include "../Core/Shader/Shader.h"
#include "../Core/Texture/Texture.h"
#include "../Core/Model/Model.h"
#include "../Actor/ActorContext.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Namespace for the test animation (fox) and model (lantern)
namespace
{
	// Ids as ResourceLoader registered them, not paths: the files are loaded once
	// at startup and everything here just borrows what is already resident.
	// Swap these for your own Blender exports; see docs/loading-blender-models.md.
	const char* FOX_MODEL_ID = "fox";
	const glm::vec2 FOX_WORLD_XZ{ 650.0f, 718.0f };
	const float FOX_SCALE = 1.5f;
	const float FOX_YAW = 160.f;

	const char* LANTERN_MODEL_ID = "lantern";
	const glm::vec2 LANTERN_WORLD_XZ{ 524.0f, 700.0f };
	const float LANTERN_SCALE = 10.0f;
	const float LANTERN_YAW = 60.f;

	// Where the lamp head sits inside the model, taken from the LanternPole_Lantern
	// node in the glTF. Model space, so the actor's scale and yaw carry it.
	const glm::vec3 LANTERN_LIGHT_OFFSET{ -9.582f, 18.009f, 0.0f };
}

World::World(unsigned int screenWidth, unsigned int screenHeight,
	ResourceLibrary<Shader>& shaders,
	ResourceLibrary<Texture>& textures,
	ResourceLibrary<Model>& models)
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
	// Bare lights, no body: they keep the debug cube so they can still be found
	// by eye while placing things.
	glm::vec3 center = m_Terrain.GetMiddleTerrainPosition();
	float yOffset = 0.2f;
	float scale = m_Terrain.GetTerrainWorldScale();

	/*
	
	auto addLight = [&](float offsetX, float offsetZ)
		{
			LightActor& light = m_Actors.Spawn<LightActor>(m_Lights, glm::vec3(0.0f), true);
			light.PlaceOnTerrain(m_Terrain,
				{ center.x + offsetX * scale, center.z + offsetZ * scale }, yOffset);
		};

	addLight(50.0f, 50.0f);
	addLight(-100.0f, -100.0f);
	*/

	// Water
	m_WaterLevel = m_Terrain.GetTerrainHeightScale() * 0.3f;
	m_Water.SetWaterLevel(m_WaterLevel);
	m_Water.AddWaterTile(
		glm::vec3(center.x, 0.0f, center.z),
		glm::vec3(700.0f, 1.0f, 700.0f),
		m_WaterLevel);

	// Bodies. All optional: a model that is not there just means the scene
	// renders without it, so this hands back null rather than throwing.
	auto findModel = [&](const char* id) -> Model*
		{
			if (!models.Exists(id))
			{
				std::cout << "No model '" << id << "', skipping.\n";
				return nullptr;
			}
			return &models.Get(id);
		};

	// Everything in the scene is an actor from here on. What each one is comes
	// from its type, not from a pile of setup living in this constructor: the
	// lantern is a light that happens to have a body, the fox is a creature that
	// switches clips. A new object is a new type, and this stays a list.
	//
	// The player carries no model: the camera is the view, so a body would only
	// hang in the air below it. SetModel is all it takes to give it one.
	m_Player = &m_Actors.Spawn<PlayerActor>();

	std::cout << "Water level " << m_WaterLevel << ".\n";

	if (Model* model = findModel(LANTERN_MODEL_ID))
	{
		LightActor& lantern = m_Actors.Spawn<LightActor>(m_Lights, LANTERN_LIGHT_OFFSET);
		lantern.SetName("lantern");
		lantern.SetModel(model);

		// The model's pivot is at its base, so terrain height needs no correction.
		lantern.PlaceOnTerrain(m_Terrain, LANTERN_WORLD_XZ);
		lantern.GetTransform().SetScale(LANTERN_SCALE);
		lantern.GetTransform().SetYaw(LANTERN_YAW);

		std::cout << "lantern at y=" << lantern.GetTransform().GetPosition().y
			<< ", light at y=" << lantern.GetLightPosition().y << "\n";
	}

	if (Model* model = findModel(FOX_MODEL_ID))
	{
		// Three foxes on one Model: one set of GPU buffers, one copy of the
		// textures and of the keyframes between them. All that makes them
		// different is a Body each, so they can stand apart, run different clips
		// and start at different points in them.
		struct FoxSetup
		{
			glm::vec2         offset;
			AnimalActor::State state;
			float             phase;   // seconds into the clip
		};

		const FoxSetup setups[] = {
			{ {   0.0f,  0.0f }, AnimalActor::State::Walking, 0.0f },
			{ {  60.0f, 25.0f }, AnimalActor::State::Running, 0.4f },
			{ { -45.0f, 55.0f }, AnimalActor::State::Idle,    0.8f },
		};

		for (const FoxSetup& setup : setups)
		{
			AnimalActor& fox = m_Actors.Spawn<AnimalActor>(
				AnimalActor::Clips{ "Survey", "Walk", "Run" });
			fox.SetName("fox");
			fox.SetModel(model);
			fox.PlaceOnTerrain(m_Terrain, FOX_WORLD_XZ + setup.offset);
			fox.GetTransform().SetScale(FOX_SCALE);
			fox.GetTransform().SetYaw(FOX_YAW);

			const bool playing = fox.SetState(setup.state);
			fox.GetBody().SetAnimationTime(setup.phase);

			std::cout << "fox at y=" << fox.GetTransform().GetPosition().y
				<< ", clip playing: " << std::boolalpha << playing << "\n";
		}
	}

	// One zero-length tick so every actor has settled its light and its pose
	// before the first frame is drawn.
	ActorContext actorContext{ m_Terrain, m_Camera, m_Actors };
	m_Actors.Update(0.0f, actorContext);
}

void World::Update(float deltaTime)
{
	m_Time += deltaTime;
	m_Terrain.Update(deltaTime);
	m_Lights.Update(deltaTime);
	m_Sky.Update(deltaTime);
	m_Water.Update(deltaTime, m_Camera.GetPosition());

	// Actors first: they decide where things are, and the models follow.
	ActorContext actorContext{ m_Terrain, m_Camera, m_Actors };
	m_Actors.Update(deltaTime, actorContext);
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

	if (!m_Actors.IsEmpty())
	{
		m_Lights.ApplyUniforms(m_ModelShader, renderContext);
		m_Actors.Render(m_Models, m_ModelShader, renderContext);
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
