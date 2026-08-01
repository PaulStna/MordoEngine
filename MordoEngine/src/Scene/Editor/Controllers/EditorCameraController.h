#pragma once
#include "../../../Camera/Camera.h"

/// Flies the camera freely for the editor: no gravity, no ground, no terrain.
///
/// The counterpart to the game controller, and the reason that rule lives in a
/// controller rather than in Camera: the same camera is walked in one scene and
/// flown in the other.
class EditorCameraController
{
private:
	// Borrowed from the world, which outlives every scene.
	Camera& m_Camera;

public:
	EditorCameraController(Camera& camera);

	/// @param velocity World units per second.
	void Update(float deltaTime, float velocity);
};
