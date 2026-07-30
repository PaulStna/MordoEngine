#pragma once
#include <glm/glm.hpp>

// Rotation is in degrees.
// Applied in the following order: yaw (Y), then pitch (X), then roll (Z).
//
// The matrix is cached and only rebuilt when one of these values changes.
// I think it's worth it because the water reflection and refraction passes
// redraw the entire scene, so GetMatrix() is called several times per frame
// for each object.
//
// I'm not sure if this can be optimized further yet.
class Transform
{
private:
	glm::vec3 m_Position{ 0.0f };
	glm::vec3 m_Rotation{ 0.0f };   // degrees
	glm::vec3 m_Scale{ 1.0f };

	mutable glm::mat4 m_Matrix{ 1.0f };
	mutable bool      m_Dirty = true;

public:
	Transform() = default;
	explicit Transform(const glm::vec3& position) : m_Position(position) {}

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetRotation() const { return m_Rotation; }
	const glm::vec3& GetScale()    const { return m_Scale; }

	void SetPosition(const glm::vec3& position) { m_Position = position; m_Dirty = true; }
	void SetRotation(const glm::vec3& degrees) { m_Rotation = degrees;  m_Dirty = true; }
	void SetScale(const glm::vec3& scale) { m_Scale = scale;       m_Dirty = true; }

	void SetScale(float uniform) { SetScale(glm::vec3(uniform)); }
	void SetHeight(float y) { m_Position.y = y; m_Dirty = true; }
	void SetYaw(float degrees) { m_Rotation.y = degrees; m_Dirty = true; }

	void Translate(const glm::vec3& delta) { m_Position += delta; m_Dirty = true; }
	void Rotate(const glm::vec3& deltaDegrees) { m_Rotation += deltaDegrees; m_Dirty = true; }

	const glm::mat4& GetMatrix() const;
};
