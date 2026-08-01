#pragma once
#include "../Camera/Camera.h"
#include "../Terrain/System/TerrainSystem.h"
#include "../Water/System/WaterSystem.h"
#include "../Lighting/LightSystem.h"
#include "../Sky/System/SkySystem.h"
#include "../Core/Model/System/ModelSystem.h"
#include "../Actor/ActorContext.h"
#include "../Actor/System/ActorSystem.h"
#include "../Actor/Player/PlayerActor.h"
#include "../Actor/Light/LightActor.h"
#include "../Actor/Animal/AnimalActor.h"
#include "../Renderer/RenderContext.h"
#include "../Renderer/Framebuffer/Framebuffer.h"
#include "../Renderer/Plane/PlaneRenderer.h"
#include "../Renderer/Debug/CollisionDebugRenderer.h"
#include "../Core/Resources/ResourceLibrary.h"
#include <memory>

class Shader;
class Texture;

class World
{
public:
	World(unsigned int screenWidth, unsigned int screenHeight,
		ResourceLibrary<Shader>& shaders, 
		ResourceLibrary<Texture>& textures,
		ResourceLibrary<Model>& models
	);

	// The input defaults to nothing pressed, so a scene that does not read a
	// device at all still drives the world.
	void Update(float deltaTime, const ActorInput& input = ActorInput{});
	void RenderOpaque(const RenderContext& renderContext);
	void Render();
	RenderContext MakeRenderContext() const;

	Camera& GetCamera() { return m_Camera; }
	TerrainSystem& GetTerrain() { return m_Terrain; }

	PlayerActor& GetPlayer() { return *m_Player; }

	// Off by default. Draws every collider and the player's aim ray over the
	// finished frame; costs nothing while it is off, since the renderer is only
	// asked for anything inside the guard.
	void SetCollisionDebugVisible(bool visible) { m_CollisionDebugVisible = visible; }
	bool IsCollisionDebugVisible() const { return m_CollisionDebugVisible; }
	void ToggleCollisionDebug() { m_CollisionDebugVisible = !m_CollisionDebugVisible; }

private:
	TerrainSystem m_Terrain;
	Camera        m_Camera;
	LightSystem   m_Lights;
	WaterSystem   m_Water;
	SkySystem     m_Sky;

	// Empty until a model is dropped into res/models. See docs/loading-blender-models.md.
	ModelSystem   m_Models;
	ActorSystem   m_Actors;
	PlayerActor*  m_Player = nullptr;

	Shader&  m_TerrainShader;
	Shader&  m_CubeLightShader;
	Shader&  m_SkyShader;
	Shader&  m_WaterShader;
	Shader&  m_UnderwaterShader;
	Shader&  m_ModelShader;
	Shader&  m_DebugLineShader;
	Texture& m_SkyTexture;
	Texture& m_WaterDuDv;

	CollisionDebugRenderer m_CollisionDebug;
	bool                   m_CollisionDebugVisible = false;

	// Offscreen buffer + fullscreen quad used for the underwater post-process.
	std::unique_ptr<Framebuffer> m_SceneBuffer;
	std::unique_ptr<PlaneRenderer> m_ScreenQuad;

	void RenderSceneAndWater(const RenderContext& renderContext);
	unsigned int m_ScreenWidth;
	unsigned int m_ScreenHeight;
	float		 m_WaterLevel = 0.0f;
	float	     m_Time = 0.0f;
};