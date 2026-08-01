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

/// Everything that exists in the scene, and the order it is updated and drawn
/// in.
///
/// It owns the systems and drives them; the systems own their own data. Nothing
/// below is given a World to talk back to — they receive a RenderContext or an
/// ActorContext instead, which is what keeps the dependency pointing one way.
///
/// The render path is the part worth knowing. Render calls RenderOpaque once
/// per pass that needs the opaque scene: the water reflection, the water
/// refraction, and the screen. Anything put in RenderOpaque therefore happens
/// three times a frame and shows up in reflections, which is why gameplay and
/// animation live in Update and the debug overlay is drawn after all of it.
class World
{
public:
	/// @param screenWidth  Framebuffer width in pixels, for the projection and
	///                     the offscreen buffers.
	/// @param screenHeight Framebuffer height in pixels.
	/// @param shaders      Borrowed for the life of the world; must outlive it.
	/// @param textures     Same.
	/// @param models       Same. A model that failed to load is simply absent,
	///                     and the scene comes up without whatever wanted it.
	World(unsigned int screenWidth, unsigned int screenHeight,
		ResourceLibrary<Shader>& shaders,
		ResourceLibrary<Texture>& textures,
		ResourceLibrary<Model>& models
	);

	/// Runs the world forward one frame: every system, then every actor.
	/// @param input Defaults to nothing pressed, so a scene that reads no device
	///              at all still drives the world.
	void Update(float deltaTime, const ActorInput& input = ActorInput{});

	/// Draws the terrain, the actors and the sky for one pass.
	///
	/// Called once per pass, so never put anything here that must happen once a
	/// frame.
	void RenderOpaque(const RenderContext& renderContext);

	/// Draws the whole frame: the water's capture passes, the scene, the water
	/// surface, the underwater post-process when submerged, and the debug
	/// overlay last.
	void Render();

	/// The context for the screen pass, which the water passes derive theirs
	/// from.
	RenderContext MakeRenderContext() const;

	Camera& GetCamera() { return m_Camera; }
	TerrainSystem& GetTerrain() { return m_Terrain; }

	PlayerActor& GetPlayer() { return *m_Player; }

	/// Off by default. Draws every collider and the player's aim ray over the
	/// finished frame; costs nothing while it is off, since the renderer is only
	/// asked for anything inside the guard.
	void SetCollisionDebugVisible(bool visible) { m_CollisionDebugVisible = visible; }
	bool IsCollisionDebugVisible() const { return m_CollisionDebugVisible; }
	void ToggleCollisionDebug() { m_CollisionDebugVisible = !m_CollisionDebugVisible; }

private:
	TerrainSystem m_Terrain;
	Camera        m_Camera;
	LightSystem   m_Lights;
	WaterSystem   m_Water;
	SkySystem     m_Sky;

	// Stateless: it only knows how to put a body and a model together.
	ModelSystem   m_Models;

	ActorSystem   m_Actors;

	// Non-owning: the actor system owns it. Set during construction and stable
	// afterwards, since actors never move once spawned.
	PlayerActor*  m_Player = nullptr;

	// All borrowed from the resource libraries, which outlive the world.
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

	// Offscreen buffer and fullscreen quad used for the underwater
	// post-process. Held by pointer because both own GL names.
	std::unique_ptr<Framebuffer> m_SceneBuffer;
	std::unique_ptr<PlaneRenderer> m_ScreenQuad;

	void RenderSceneAndWater(const RenderContext& renderContext);

	unsigned int m_ScreenWidth;
	unsigned int m_ScreenHeight;
	float		 m_WaterLevel = 0.0f;

	// Running time in seconds, for anything that animates on a clock.
	float	     m_Time = 0.0f;
};
