#pragma once
#include "../../../Camera/Camera.h"
#include "../../../Terrain/System/TerrainSystem.h"

/// Walks the camera over the terrain: reads the movement keys, then puts the
/// camera back on the ground wherever it ended up.
///
/// The camera is still what moves. This is the only thing that knows the camera
/// is meant to be standing on something, which is what keeps that rule out of
/// Camera itself and lets the editor fly the same camera freely.
class GameCameraController
{
private:
	// How far above the ground the camera rides, in world units. Eye height.
	float m_HeightOffset = .2f;

	// Borrowed from the world, which outlives every scene.
	Camera& m_Camera;

public:
	GameCameraController(Camera& camera);

	/// Moves the camera for this frame and re-seats it on the terrain.
	/// @param velocity World units per second.
	void Update(float deltaTime, float velocity, const TerrainSystem& terrainSystem);

	/// Drops the camera straight onto the ground under wherever it currently
	/// is, without moving it horizontally. What the game scene calls on entry,
	/// since the editor may have left the camera in the air or under the map.
	void TouchTerrain(const TerrainSystem& terrainSystem);
};
