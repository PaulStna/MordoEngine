#include "BoxCollider.h"

bool BoxCollider::Raycast(const Ray& localRay, float& distance) const
{
	// Broad and narrow phase happen to be the same test for this shape. That is
	// only true here, which is why the two stay separate methods.
	return m_Bounds.IntersectRay(localRay, distance);
}
