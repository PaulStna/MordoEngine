#pragma once
#include "Collider.h"

/// An axis aligned box in local space, which is a rotated box in the world once
/// its owner's transform is applied.
///
/// The default shape for anything with a model, since it comes straight off the
/// geometry: both phases are the same box, so the broad phase rejection and the
/// exact test agree by construction.
class BoxCollider : public Collider
{
public:
	/// @param bounds Local space, normally a model's own measured bounds.
	explicit BoxCollider(const Bounds& bounds) : m_Bounds(bounds) {}

	Bounds GetLocalBounds() const override { return m_Bounds; }
	bool   Raycast(const Ray& localRay, float& distance) const override;

	/// @param bounds Local space. Use this to Expand a bind pose box once the
	///               thing inside it starts animating out of that pose.
	void SetBounds(const Bounds& bounds) { m_Bounds = bounds; }

private:
	Bounds m_Bounds;
};
