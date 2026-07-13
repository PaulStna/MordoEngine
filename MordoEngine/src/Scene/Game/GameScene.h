#pragma once
#include "../Scene.h"
#include "../../Core/EngineContext.h"
#include "./Controllers/GameCameraController.h"
#include <memory>

class GameScene : public Scene
{
private:
    EngineContext& m_Ctx;
    std::unique_ptr<GameCameraController> m_CameraController;

public:
    GameScene(EngineContext& ctx);
    void Render() override;
    void Update(float deltaTime) override;
    void OnEntry() override;
    ~GameScene() override;
};