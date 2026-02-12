#pragma once
#include "Scene.h"
#include "../Camera/Camera.h"
#include "../Terrain/Terrain.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Geomipmapping.h"
#include "../Lighting/Sun.h"
#include <memory>

class GameScene : public Scene
{
private:
	

public:
	GameScene();
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<terrain::Terrain> m_Terrain;
	std::shared_ptr<Geomipmapping> m_Render;
	std::unique_ptr<Sun> m_Sun;
	void Render() override;
	void Update(float deltaTime) override;
	~GameScene() override;
};
