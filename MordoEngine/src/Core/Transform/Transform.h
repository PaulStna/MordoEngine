#pragma once
#include <glm/glm.hpp>

/// Where something sits in the world: position, rotation and scale, plus the
/// matrix they combine into.
///
/// Rotation is stored in degrees and applied yaw (Y), then pitch (X), then roll
/// (Z), with scale applied last. The matrix is cached and rebuilt only after a
/// setter marks it dirty, because a render path runs once per pass and every
/// pass asks each object for its matrix again.
class Transform
{
private:
	glm::vec3 m_Position{ 0.0f };
	glm::vec3 m_Rotation{ 0.0f };   // degrees
	glm::vec3 m_Scale{ 1.0f };

	// Rebuilt on the next GetMatrix after any setter runs. Mutable because
	// GetMatrix is const to its callers: refreshing the cache is not a change
	// anyone outside can observe.
	mutable glm::mat4 m_Matrix{ 1.0f };
	mutable bool      m_Dirty = true;

public:
	Transform() = default;
	explicit Transform(const glm::vec3& position) : m_Position(position) {}

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetRotation() const { return m_Rotation; }
	const glm::vec3& GetScale()    const { return m_Scale; }

	void SetPosition(const glm::vec3& position) { m_Position = position; m_Dirty = true; }

	/// @param degrees Yaw, pitch and roll in degrees, not radians.
	void SetRotation(const glm::vec3& degrees) { m_Rotation = degrees;  m_Dirty = true; }
	void SetScale(const glm::vec3& scale) { m_Scale = scale;       m_Dirty = true; }

	/// @param uniform Applied to all three axes at once.
	void SetScale(float uniform) { SetScale(glm::vec3(uniform)); }

	/// Moves only the Y axis, leaving X and Z where they are.
	void SetHeight(float y) { m_Position.y = y; m_Dirty = true; }

	/// @param degrees Rotation about Y in degrees. Camera yaw is in radians, so
	///                convert before passing it here.
	void SetYaw(float degrees) { m_Rotation.y = degrees; m_Dirty = true; }

	void Translate(const glm::vec3& delta) { m_Position += delta; m_Dirty = true; }
	void Rotate(const glm::vec3& deltaDegrees) { m_Rotation += deltaDegrees; m_Dirty = true; }

	/// The model matrix, rebuilt only if something changed since the last call.
	/// Safe to call several times a frame, which the render passes do.
	const glm::mat4& GetMatrix() const;
};
