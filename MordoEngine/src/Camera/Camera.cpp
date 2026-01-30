#include "Camera.h"
#include <algorithm>

Camera::Camera(glm::vec3 position, unsigned int windowWidth, unsigned int windowHeight)
    : m_position(position), WINDOW_WIDTH(windowWidth), WINDOW_HEIGHT(windowHeight)
{
    LookAt(glm::vec3{0.0f});
}

void Camera::Update() {
    // Wrap yaw to 0 to 2*PI range
    m_rotation.y = glm::mod(m_rotation.y, glm::two_pi<float>());
    m_rotation.z = 0.0f; // No roll

    // Build the view matrix using quaternion for smooth rotation
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position);
    glm::mat4 rotationMatrix = glm::mat4_cast(glm::normalize(glm::quat(m_rotation)));

    // Combine translation and rotation
    glm::mat4 cameraMatrix = translationMatrix * rotationMatrix;

    // View matrix is inverse of camera matrix
    m_viewMatrix = glm::inverse(cameraMatrix);

    // Store inverse view matrix for extracting camera vectors
    m_inverseViewMatrix = cameraMatrix;

    // Extract camera direction vectors from inverse view matrix
    m_right = glm::vec3(m_inverseViewMatrix[0]);
    m_up = glm::vec3(m_inverseViewMatrix[1]);
    m_forward = -glm::vec3(m_inverseViewMatrix[2]);

    // Update quaternion
    m_rotationQ = glm::quat(m_rotation);
}

void Camera::SetPosition(glm::vec3 position) {
    m_position = position;
    Update();
}

void Camera::SetEulerRotation(glm::vec3 rotation) {
    m_rotation = rotation;
    m_rotationQ = glm::quat(m_rotation);
    Update();
}

void Camera::AddPitch(float value) {
    m_rotation.x += value;
    m_rotation.x = std::clamp(m_rotation.x, m_minPitch, m_maxPitch);
    m_rotationQ = glm::quat(m_rotation);
    Update();
}

void Camera::AddYaw(float value) {
    m_rotation.y += value;
    Update();
}

void Camera::AddHeight(float value) {
    m_position.y += value;
    Update();
}

void Camera::SetMinPitch(float value) {
    m_minPitch = value;
    Update();
}

void Camera::SetMaxPitch(float value) {
    m_maxPitch = value;
    Update();
}

void Camera::SetWindowSize(unsigned int width, unsigned int height) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    float sensitivity = 0.0025f;
    AddYaw(-xoffset * sensitivity);    
    AddPitch(yoffset * sensitivity);    
}

void Camera::LookAt(glm::vec3 target) {
    glm::vec3 direction = glm::normalize(target - m_position);
    float yaw = atan2(-direction.x, -direction.z);
    float pitch = asin(-direction.y);
    pitch = std::clamp(pitch, m_minPitch, m_maxPitch);
    m_rotation = glm::vec3(pitch, yaw, 0.0f);
    Update();
}

const glm::mat4& Camera::GetViewMatrix() const {
    return m_viewMatrix;
}

const glm::mat4& Camera::GetInverseViewMatrix() const {
    return m_inverseViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(m_zoom),
        static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT),
        0.1f, 200.0f);
}

const glm::vec3& Camera::GetPosition() const {
    return m_position;
}

const glm::vec3& Camera::GetEulerRotation() const {
    return m_rotation;
}

const glm::quat& Camera::GetQuaternionRotation() const {
    return m_rotationQ;
}

const glm::vec3& Camera::GetForward() const {
    return m_forward;
}

const glm::vec3& Camera::GetUp() const {
    return m_up;
}

const glm::vec3& Camera::GetRight() const {
    return m_right;
}

const glm::vec3 Camera::GetForwardXZ() const {
    return glm::normalize(glm::vec3(m_forward.x, 0.0f, m_forward.z));
}

float Camera::GetPitch() const { 
    return m_rotation.x; 
}
float Camera::GetYaw() const {
    return m_rotation.y; 
}