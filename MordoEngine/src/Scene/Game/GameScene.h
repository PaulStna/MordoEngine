#pragma once
#include "../Scene.h"
#include "../../Camera/Camera.h"
#include "./Controllers/GameCameraController.h"
#include "../../Lighting/LightSystem.h"
#include "../../Terrain/System/TerrainSystem.h"
#include <memory>

class GameScene : public Scene
{
private:
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<TerrainSystem> m_TerrainSystem;
	std::unique_ptr<GameCameraController> m_CameraController;
	std::unique_ptr<Renderer> m_SkyBoxRenderer;
	std::unique_ptr<LightSystem> m_LightSystem;

public:
	GameScene(std::shared_ptr<Camera> camera, std::shared_ptr<TerrainSystem> terrainSystem);
	void Render() override;
	void Update(float deltaTime) override;
	void OnEntry() override;
	~GameScene() override;
};
