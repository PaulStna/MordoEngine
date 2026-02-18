#pragma once
#include "../../../Camera/Camera.h"
#include "../../../Terrain/Terrain.h"
#include <memory>

class GameCameraController
{
private:
	float m_HeightOffset = .2f;
	std::weak_ptr<Camera> m_Camera;

public:
	GameCameraController(std::weak_ptr<Camera> camera);
	void Update(float deltaTime, float velocity, const terrain::Terrain& terrain);
	void TouchTerrain(const terrain::Terrain& terrain);
};