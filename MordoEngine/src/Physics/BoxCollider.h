#pragma once
#include "Collider.h"

// An axis aligned box in local space, which is a rotated box in the world once
// its owner's transform is applied. The default shape for anything with a model,
// since it comes straight off the geometry.
class BoxCollider : public Collider
{
public:
	explicit BoxCollider(const Bounds& bounds) : m_Bounds(bounds) {}

	Bounds GetLocalBounds() const override { return m_Bounds; }
	bool   Raycast(const Ray& localRay, float& distance) const override;

	void SetBounds(const Bounds& bounds) { m_Bounds = bounds; }

private:
	Bounds m_Bounds;
};
