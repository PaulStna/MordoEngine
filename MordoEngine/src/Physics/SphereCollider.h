#pragma once
#include "Collider.h"

// A sphere in local space. Here as the second shape rather than because anything
// needs it yet: it is what proves the abstraction carries its weight, since it
// shares the broad phase with the box and nothing else.
//
// Worth using for something whose silhouette is roughly round, or for an actor
// with no model at all, where there is no geometry to fit a box to.
//
// Under non-uniform scale it stays a sphere in local space and comes out an
// ellipsoid in the world, which is the correct result and one the box cannot
// give.
class SphereCollider : public Collider
{
public:
	SphereCollider(const glm::vec3& center, float radius)
		: m_Center(center), m_Radius(radius) {}

	explicit SphereCollider(float radius) : m_Radius(radius) {}

	Bounds GetLocalBounds() const override;
	bool   Raycast(const Ray& localRay, float& distance) const override;

	const glm::vec3& GetCenter() const { return m_Center; }
	float            GetRadius() const { return m_Radius; }

private:
	glm::vec3 m_Center{ 0.0f };
	float     m_Radius = 1.0f;
};
