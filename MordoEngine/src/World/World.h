#pragma once
#include "../Camera/Camera.h"
#include "../Terrain/System/TerrainSystem.h"
#include "../Water/System/WaterSystem.h"
#include "../Lighting/LightSystem.h"
#include "../Sky/System/SkySystem.h"
#include "../Core/Model/System/ModelSystem.h"
#include "../Renderer/RenderContext.h"
#include "../Renderer/Framebuffer/Framebuffer.h"
#include "../Renderer/Plane/PlaneRenderer.h"
#include "../Core/Resources/ResourceLibrary.h"
#include <memory>

class Shader;
class Texture;

class World
{
public:
    World(unsigned int screenWidth, unsigned int screenHeight,
        ResourceLibrary<Shader>& shaders, ResourceLibrary<Texture>& textures);

    void Update(float deltaTime);
    void RenderOpaque(const RenderContext& renderContext);
    void Render();
    RenderContext MakeRenderContext() const;

    Camera& GetCamera() { return m_Camera; }
    TerrainSystem& GetTerrain() { return m_Terrain; }

private:
    TerrainSystem m_Terrain;   
    Camera        m_Camera;
    LightSystem   m_Lights;
    WaterSystem   m_Water;
    SkySystem     m_Sky;

    // Empty until a model is dropped into res/models. See docs/loading-blender-models.md.
    ModelSystem   m_Models;

    Shader& m_TerrainShader;
    Shader& m_CubeLightShader;
    Shader& m_SkyShader;
    Shader& m_WaterShader;
    Shader& m_UnderwaterShader;
    Shader& m_ModelShader;
    Texture& m_SkyTexture;
    Texture& m_WaterDuDv;

    // Offscreen buffer + fullscreen quad used for the underwater post-process.
    std::unique_ptr<Framebuffer> m_SceneBuffer;
    std::unique_ptr<PlaneRenderer> m_ScreenQuad;

    void RenderSceneAndWater(const RenderContext& renderContext);
    unsigned int m_ScreenWidth;
    unsigned int m_ScreenHeight;
    float m_WaterLevel = 0.0f;
    float m_Time = 0.0f;
};