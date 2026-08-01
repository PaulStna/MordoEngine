#include "GameScene.h"
#include "../../Input/Input.h"

GameScene::GameScene(EngineContext& context)
    : m_Context(context),
    m_CameraController(std::make_unique<GameCameraController>(context.GetWorld().GetCamera()))
{
}

void GameScene::OnEntry()
{
    m_CameraController->TouchTerrain(m_Context.GetWorld().GetTerrain());
}

void GameScene::Update(float deltaTime)
{
    World& world = m_Context.GetWorld();
    float velocity = 100.0f * world.GetTerrain().GetTerrainWorldScale() * deltaTime;
    m_CameraController->Update(deltaTime, velocity, world.GetTerrain());

    // The device is read here and handed over as plain data, so nothing under
    // World ends up compiling against GLFW.
    ActorInput input;
    input.interact = Input::LeftMousePressed();

    // F3 shows the collision volumes and the aim ray. A view of the world, not
    // a change to it, so it sits here next to the other input and nothing
    // downstream knows about it.
    if (Input::KeyPressed(GLFW_KEY_F3)) world.ToggleCollisionDebug();

    world.Update(deltaTime, input);
}

void GameScene::Render()
{
    World& world = m_Context.GetWorld();
    world.Render();
}

GameScene::~GameScene()
{
}