#include "GameCameraController.h"
#include "../../../Input/Input.h"

GameCameraController::GameCameraController(Camera& camera) : m_Camera(camera)
{
}

void GameCameraController::Update(float deltaTime, float velocity, const TerrainSystem& terrainSystem)
{
	glm::vec3 newCameraPosition = m_Camera.GetPosition();

	if (Input::KeyDown(GLFW_KEY_W)) newCameraPosition += m_Camera.GetForward() * velocity;
	if (Input::KeyDown(GLFW_KEY_A)) newCameraPosition -= m_Camera.GetRight() * velocity;
	if (Input::KeyDown(GLFW_KEY_S)) newCameraPosition -= m_Camera.GetForward() * velocity;
	if (Input::KeyDown(GLFW_KEY_D)) newCameraPosition += m_Camera.GetRight() * velocity;

	if (newCameraPosition != m_Camera.GetPosition()) {
		float yTerrainPosition = terrainSystem.GetTerrainInterpolatedHeightAt(
			newCameraPosition.x, newCameraPosition.z, m_HeightOffset);
		m_Camera.SetPosition(glm::vec3(newCameraPosition.x, yTerrainPosition, newCameraPosition.z));
	}

	m_Camera.ProcessMouseMovement(Input::MouseDeltaX(), Input::MouseDeltaY());
}

void GameCameraController::TouchTerrain(const TerrainSystem& terrainSystem)
{
	glm::vec3 p = m_Camera.GetPosition();
	float y = terrainSystem.GetTerrainInterpolatedHeightAt(p.x, p.z, m_HeightOffset);
	m_Camera.SetPosition(glm::vec3(p.x, y, p.z));
}