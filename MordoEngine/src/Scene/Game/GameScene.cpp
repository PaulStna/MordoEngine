#include "GameScene.h"

GameScene::GameScene(EngineContext& ctx)
    : m_Ctx(ctx),
    m_CameraController(std::make_unique<GameCameraController>(ctx.GetWorld().GetCamera()))
{
}

void GameScene::OnEntry()
{
    m_CameraController->TouchTerrain(m_Ctx.GetWorld().GetTerrain());
}

void GameScene::Update(float deltaTime)
{
    World& world = m_Ctx.GetWorld();
    float velocity = 100.0f * world.GetTerrain().GetTerrainWorldScale() * deltaTime;
    m_CameraController->Update(deltaTime, velocity, world.GetTerrain());
    world.Update(deltaTime);
}

void GameScene::Render()
{
    World& world = m_Ctx.GetWorld();
    RenderContext ctx = world.MakeRenderContext();
    world.Render(ctx);
}

GameScene::~GameScene()
{
}