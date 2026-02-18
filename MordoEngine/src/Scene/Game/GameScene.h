#pragma once
#include "../Scene.h"
#include "../../Camera/Camera.h"
#include "../../Terrain/Terrain.h"
#include "../../Renderer/Renderer.h"
#include "../../Lighting/Sun.h"
#include "./Controllers/GameCameraController.h"
#include <memory>

class GameScene : public Scene
{
private:
	std::shared_ptr<terrain::Terrain> m_Terrain;
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<Renderer> m_Renderer;
	std::unique_ptr<GameCameraController> m_CameraController;
	std::unique_ptr<Sun> m_Sun;

public:
	GameScene(std::shared_ptr<terrain::Terrain> terrain, std::shared_ptr<Camera> camera, std::shared_ptr<Renderer> renderer);
	void Render() override;
	void Update(float deltaTime) override;
	void OnEntry() override;
	~GameScene() override;
};
