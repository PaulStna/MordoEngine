#include "World.h"
#include "../Core/Shader/Shader.h"
#include "../Core/Texture/Texture.h"
#include <glad/glad.h>

World::World(unsigned int screenWidth, unsigned int screenHeight,
    ResourceLibrary<Shader>& shaders, ResourceLibrary<Texture>& textures)
    : m_Terrain(shaders, textures),
    m_Camera(m_Terrain.GetMiddleTerrainPosition(), screenWidth, screenHeight),
    m_Lights(),
    m_Water(),
    m_Sky(),
    m_TerrainShader(shaders.Get("terrain")),
    m_CubeLightShader(shaders.Get("lightCube")),
    m_SkyShader(shaders.Get("skyBox")),
    m_WaterShader(shaders.Get("water")),
    m_SkyTexture(textures.Get("skyBox")),
    m_WaterDuDv(textures.Get("dudvMap"))
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
    float waterLevel = m_Terrain.GetTerrainHeightScale() * 0.3f;
    m_Water.AddWaterTile(
        glm::vec3(center.x, 0.0f, center.z),
        glm::vec3(700.0f, 1.0f, 700.0f),
        waterLevel);
}

void World::Update(float deltaTime)
{
    m_Terrain.Update(deltaTime);
    m_Lights.Update(deltaTime);
    m_Sky.Update(deltaTime);
    m_Water.Update(deltaTime);
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
    m_TerrainShader.Use();
    m_TerrainShader.SetVec4("plane", ctx.clipPlane);

    m_Lights.Render(m_TerrainShader, m_CubeLightShader, ctx.cameraPos,
        &ctx.projection, &ctx.view, &ctx.model);
    m_Terrain.Render(m_TerrainShader, ctx.cameraPos,
        &ctx.projection, &ctx.view, &ctx.model);
    m_Sky.Render(m_SkyShader, m_SkyTexture,
        &ctx.projection, &ctx.view, &ctx.model);
}

void World::Render(const RenderContext& ctx)
{
    glm::mat4 projection = ctx.projection;

    glEnable(GL_CLIP_DISTANCE0);
    m_Water.Render(
        m_WaterShader, m_WaterDuDv, m_Camera, &projection, nullptr,
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