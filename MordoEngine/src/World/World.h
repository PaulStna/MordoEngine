#pragma once
#include "../Camera/Camera.h"
#include "../Terrain/System/TerrainSystem.h"
#include "../Water/System/WaterSystem.h"
#include "../Lighting/LightSystem.h"
#include "../Sky/System/SkySystem.h"
#include "../Renderer/RenderContext.h"
#include "../Core/Resources/ResourceLibrary.h"

class Shader;
class Texture;

class World
{
public:
    World(unsigned int screenWidth, unsigned int screenHeight,
        ResourceLibrary<Shader>& shaders, ResourceLibrary<Texture>& textures);

    void Update(float deltaTime);
    void RenderOpaque(const RenderContext& ctx);
    void Render(const RenderContext& ctx);
    RenderContext MakeRenderContext() const;

    Camera& GetCamera() { return m_Camera; }
    TerrainSystem& GetTerrain() { return m_Terrain; }

private:
    TerrainSystem m_Terrain;   
    Camera        m_Camera;
    LightSystem   m_Lights;
    WaterSystem   m_Water;
    SkySystem     m_Sky;

    Shader& m_TerrainShader;
    Shader& m_CubeLightShader;
    Shader& m_SkyShader;
    Shader& m_WaterShader;
    Texture& m_SkyTexture;
    Texture& m_WaterDuDv;
};