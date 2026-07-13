#pragma once
#include "../../../Camera/Camera.h"
#include "../../../Terrain/System/TerrainSystem.h"

class GameCameraController
{
private:
	float m_HeightOffset = .2f;
	Camera& m_Camera;

public:
	GameCameraController(Camera& camera);
	void Update(float deltaTime, float velocity, const TerrainSystem& terrainSystem);
	void TouchTerrain(const TerrainSystem& terrainSystem);
};