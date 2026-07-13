#include "EditorScene.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Shader/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

EditorScene::EditorScene(EngineContext& ctx)
    : m_Ctx(ctx)
{
    World& world = m_Ctx.GetWorld();
    m_CameraController = std::make_unique<EditorCameraController>(world.GetCamera());
    m_EditorSystem = std::make_unique<EditorSystem>(Manager<Shader>::Get("terrainSelector"));
    m_EditorInputController = std::make_unique<EditorInputHandlerController>();
}

void EditorScene::Update(float deltaTime)
{
    World& world = m_Ctx.GetWorld();
    float velocity = 100.0f * world.GetTerrain().GetTerrainWorldScale() * deltaTime;
    m_CameraController->Update(deltaTime, velocity);
    m_EditorSystem->Update(world.GetTerrain().GetTerrain(), world.GetCamera());
    m_EditorInputController->Update(*m_EditorSystem, world.GetTerrain());
}

void EditorScene::Render()
{
    World& world = m_Ctx.GetWorld();
    Camera& camera = world.GetCamera();

    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    Shader& terrainShader = Manager<Shader>::Get("terrain");
    world.GetTerrain().Render(terrainShader, camera.GetPosition(), &projection, &view, &model);

    model = glm::translate(model, m_EditorSystem->GetWorldPosition());
    m_EditorSystem->Render(&view, &projection, &model);
}

EditorScene::~EditorScene()
{
    //
}