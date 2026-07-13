#pragma once
#include "../Camera/Camera.h"
#include "../Terrain/System/TerrainSystem.h"
#include "../Water/System/WaterSystem.h"
#include "../Lighting/LightSystem.h"
#include "../Sky/System/SkySystem.h"
#include "../Renderer/RenderContext.h"

class World
{
public:
	World(unsigned int screenWidth, unsigned int screenHeight);
	void Update(float deltaTime);
	void RenderOpaque(const RenderContext& ctx);
	void Render(const RenderContext& ctx);
	RenderContext MakeRenderContext() const;
	Camera& GetCamera() { return m_Camera; }
	TerrainSystem& GetTerrain() { return m_TerrainSystem; }

private:
	TerrainSystem m_TerrainSystem;   
	Camera        m_Camera;
	LightSystem   m_LightSystem;
	WaterSystem   m_WaterSystem;
	SkySystem     m_SkySystem;
};