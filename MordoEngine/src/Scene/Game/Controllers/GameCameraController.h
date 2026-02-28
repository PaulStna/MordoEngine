#pragma once
#include "../../../Camera/Camera.h"
#include "../../../Terrain/System/TerrainSystem.h"
#include <memory>

class GameCameraController
{
private:
	float m_HeightOffset = .2f;
	std::weak_ptr<Camera> m_Camera;

public:
	GameCameraController(std::weak_ptr<Camera> camera);
	void Update(float deltaTime, float velocity, const TerrainSystem& terrainSystem);
	void TouchTerrain(const TerrainSystem& terrainSystem);
};