#include "GameCameraController.h"
#include "../../../Input/Input.h"

GameCameraController::GameCameraController(std::weak_ptr<Camera> camera) : m_Camera(camera)
{

}

void GameCameraController::Update(float deltaTime, float velocity, const terrain::Terrain& terrain)
{
	auto camera = m_Camera.lock();
	if (!camera) {
		return;
	}
	glm::vec3 newCameraPosition = camera->GetPosition();

	if (Input::KeyDown(GLFW_KEY_W)) {
		newCameraPosition += camera->GetForward() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_A)) {
		newCameraPosition -= camera->GetRight() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_S)) {
		newCameraPosition -= camera->GetForward() * velocity;
	}
	if (Input::KeyDown(GLFW_KEY_D)) {
		newCameraPosition += camera->GetRight() * velocity;
	}

	if (newCameraPosition != camera->GetPosition()) {
		float yTerrainPosition = terrain.GetHeightInterpolated(newCameraPosition.x, newCameraPosition.z)
							   + m_HeightOffset * terrain.GetHeightScale();
		glm::vec3 finalPosition = glm::vec3(newCameraPosition.x, yTerrainPosition, newCameraPosition.z);
		camera->SetPosition(finalPosition);
	}

	camera->ProcessMouseMovement(Input::MouseDeltaX(), Input::MouseDeltaY());
}

void GameCameraController::TouchTerrain(const terrain::Terrain& terrain)
{
	auto camera = m_Camera.lock();
	if (!camera) {
		return;
	}
	glm::vec3 cameraPosition = camera->GetPosition();
	float yTerrainPosition = terrain.GetHeightInterpolated(cameraPosition.x, cameraPosition.z)
		+ m_HeightOffset * terrain.GetHeightScale();
	glm::vec3 finalPosition = glm::vec3(cameraPosition.x, yTerrainPosition, cameraPosition.z);
	camera->SetPosition(finalPosition);
}