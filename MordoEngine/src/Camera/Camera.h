#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

/// Where the scene is viewed from, and the matrices that follow from it.
///
/// It holds an orientation and derives everything else: the basis vectors, the
/// view matrix and its inverse. Update is what recomputes them, so a setter
/// alone is not enough to see the change.
///
/// **Angles here are radians.** Transform takes degrees, so anything copying a
/// camera angle onto an object has to convert.
///
/// The camera knows nothing about the world it sits in. Whether it walks the
/// ground or flies freely belongs to a controller, which is what lets two
/// scenes drive the same camera differently.
class Camera
{
public:
	/// @param position     World space.
	/// @param windowWidth  Viewport width in pixels; drives the projection
	///                     aspect ratio. Update it with SetWindowSize on resize
	///                     or the image stretches.
	/// @param windowHeight Viewport height in pixels.
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		unsigned int windowWidth = 800,
		unsigned int windowHeight = 600);

	/// Recomputes the basis vectors and the view matrices from the current
	/// orientation. Call after changing anything below.
	void Update();

	/// @param rotation Pitch, yaw and roll in radians.
	void SetEulerRotation(glm::vec3 rotation);

	/// @param value Radians. Clamps how far down the camera can look, which is
	///              what stops it turning upside down.
	void SetMinPitch(float value);

	/// @param value Radians, the upward limit.
	void SetMaxPitch(float value);

	void SetWindowSize(unsigned int width, unsigned int height);

	void SetLookAt(glm::vec3 lookAt);

	/// @param value Radians, added to the current pitch and clamped to the
	///              min/max above.
	void AddPitch(float value);

	/// @param value Radians, added to the current yaw. Not clamped: yaw wraps.
	void AddYaw(float value);

	/// @param value World units, added to the Y position only.
	void AddHeight(float value);

	void SetPosition(glm::vec3 position);

	/// Applies a mouse movement as a look.
	/// @param xoffset Pixels since the last frame.
	/// @param yoffset Pixels since the last frame.
	void ProcessMouseMovement(float xoffset, float yoffset);

	/// Points the camera at a world-space target.
	void LookAt(glm::vec3 target);

	/// Valid as of the last Update.
	const glm::mat4& GetViewMatrix() const;

	/// Camera-to-world, the inverse of the view matrix, kept in step by Update.
	/// Nothing calls this today; it is here for whatever needs to go from view
	/// space back out to the world.
	const glm::mat4& GetInverseViewMatrix() const;

	glm::mat4 GetProjectionMatrix() const;

	const glm::vec3& GetPosition() const;

	/// Pitch, yaw and roll in radians.
	const glm::vec3& GetEulerRotation() const;

	const glm::quat& GetQuaternionRotation() const;

	/// Full 3D facing, normalized. This is what a ray aimed down the view uses.
	const glm::vec3& GetForward() const;

	const glm::vec3& GetUp() const;
	const glm::vec3& GetRight() const;

	/// Facing flattened onto the ground plane and renormalized, for movement
	/// that should not drift upwards when looking up.
	const glm::vec3 GetForwardXZ() const;

	/// Radians.
	float GetPitch() const;

	/// Radians. Transform::SetYaw takes degrees, so convert before passing this
	/// to one.
	float GetYaw() const;

private:
	glm::vec3 m_position = glm::vec3(0.0f);

	// Radians, as pitch, yaw, roll.
	glm::vec3 m_rotation = glm::vec3(0.0f);

	glm::quat m_rotationQ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	// Both derived in Update, never set directly.
	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_inverseViewMatrix = glm::mat4(1.0f);
	glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);

	// Radians. Just under a right angle, so looking straight up or down never
	// degenerates the basis.
	float m_minPitch = -1.5f;
	float m_maxPitch = 1.5f;

	unsigned int WINDOW_WIDTH;
	unsigned int WINDOW_HEIGHT;

	// Vertical field of view in degrees, which is what glm::perspective wants
	// after conversion.
	float m_zoom = 45.0f;
};
