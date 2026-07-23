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
    m_UnderwaterShader(shaders.Get("underwater")),
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
    m_Water.AddWaterTile(
        glm::vec3(center.x, 0.0f, center.z),
        glm::vec3(700.0f, 1.0f, 700.0f),
        m_WaterLevel);
}

void World::Update(float deltaTime)
{
    m_Time += deltaTime;
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

void World::RenderOpaque(const RenderContext& renderContext)
{
    m_TerrainShader.Use();
    m_TerrainShader.SetVec4("plane", renderContext.clipPlane);

    m_Lights.Render(m_TerrainShader, m_CubeLightShader, renderContext);
    m_Terrain.Render(m_TerrainShader, renderContext);
    m_Sky.Render(m_SkyShader, m_SkyTexture, renderContext);
}

void World::Render(const RenderContext& renderContext)
{
    // 1) Capture the reflection/refraction textures. WaterSystem derives the
    // context for each pass and manages GL_CLIP_DISTANCE0 around them.
    m_Water.CaptureReflectionRefraction(
        renderContext,
        m_Camera,
        [this](const RenderContext& pass) { RenderOpaque(pass); });

    // 2) Render the whole scene into an offscreen buffer so it can be
    // post-processed (needed for the full-screen underwater effect).
    m_SceneBuffer->BindBuffer();
    glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Main opaque scene without clipping: shows terrain above and below water.
    RenderContext mainPass = renderContext;
    mainPass.clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    RenderOpaque(mainPass);

    // Water surface last, so it can blend over the terrain when submerged.
    m_Water.RenderSurface(m_WaterShader, m_WaterDuDv, mainPass);

    m_SceneBuffer->UnbindBuffer();

    // 3) Draw the scene to the screen through the underwater post-process.
    RenderPostProcess();
}

void World::RenderPostProcess()
{
    const bool underwater = m_Camera.GetPosition().y <= m_WaterLevel;

    // How deep below the surface the camera is, normalized to [0, 1].
    float submergence = 0.0f;
    if (underwater)
        submergence = glm::clamp((m_WaterLevel - m_Camera.GetPosition().y) / 300.0f, 0.0f, 1.0f);

    glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    m_UnderwaterShader.Use();
    m_UnderwaterShader.SetInt("sceneTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_SceneBuffer->GetTextureID());

    m_UnderwaterShader.SetInt("dudvMap", 1);
    glActiveTexture(GL_TEXTURE1);
    m_WaterDuDv.Use();

    m_UnderwaterShader.SetInt("underwater", underwater ? 1 : 0);
    m_UnderwaterShader.SetFloat("time", m_Time);
    m_UnderwaterShader.SetFloat("submergence", submergence);

    m_ScreenQuad->Render();

    glEnable(GL_DEPTH_TEST);
}