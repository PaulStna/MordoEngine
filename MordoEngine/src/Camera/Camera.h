#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		unsigned int windowWidth = 800,
		unsigned int windowHeight = 600);
	void Update();
	void SetEulerRotation(glm::vec3 rotation);
	void SetMinPitch(float value);
	void SetMaxPitch(float value);
	void SetWindowSize(unsigned int width, unsigned int height);
	void AddPitch(float value);
	void AddYaw(float value);
	void AddHeight(float value);
	void SetPosition(glm::vec3 position);
	void ProcessMouseMovement(float xoffset, float yoffset);
	void LookAt(glm::vec3 target);
	const glm::mat4& GetViewMatrix() const;
	const glm::mat4& GetInverseViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	const glm::vec3& GetPosition() const;
	const glm::vec3& GetEulerRotation() const;
	const glm::quat& GetQuaternionRotation() const;
	const glm::vec3& GetForward() const;
	const glm::vec3& GetUp() const;
	const glm::vec3& GetRight() const;
	const glm::vec3 GetForwardXZ() const;
	float GetPitch() const;
	float GetYaw() const;

private:
	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_rotation = glm::vec3(0.0f);
	glm::quat m_rotationQ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_inverseViewMatrix = glm::mat4(1.0f);
	glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	float m_minPitch = -1.5f;
	float m_maxPitch = 1.5f;
	unsigned int WINDOW_WIDTH;
	unsigned int WINDOW_HEIGHT;
	float m_zoom = 45.0f;
};
