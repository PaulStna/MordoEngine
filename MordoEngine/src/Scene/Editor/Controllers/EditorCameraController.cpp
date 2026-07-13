#include "EditorCameraController.h"
#include "../../../Input/Input.h"

EditorCameraController::EditorCameraController(Camera& camera) : m_Camera(camera)
{
}

void EditorCameraController::Update(float deltaTime, float velocity)
{
    glm::vec3 newCameraPosition = m_Camera.GetPosition();

    if (Input::KeyDown(GLFW_KEY_W)) newCameraPosition += m_Camera.GetForward() * velocity;
    if (Input::KeyDown(GLFW_KEY_A)) newCameraPosition -= m_Camera.GetRight() * velocity;
    if (Input::KeyDown(GLFW_KEY_S)) newCameraPosition -= m_Camera.GetForward() * velocity;
    if (Input::KeyDown(GLFW_KEY_D)) newCameraPosition += m_Camera.GetRight() * velocity;

    if (newCameraPosition != m_Camera.GetPosition()) {
        m_Camera.SetPosition(newCameraPosition);
    }
    m_Camera.ProcessMouseMovement(Input::MouseDeltaX(), Input::MouseDeltaY());
}