#include "SphereCollider.h"

Bounds SphereCollider::GetLocalBounds() const
{
	return Bounds::FromCenterAndExtents(m_Center, glm::vec3(m_Radius));
}

bool SphereCollider::Raycast(const Ray& localRay, float& distance) const
{
	// Solved as a quadratic in t along the ray. Not normalising the direction is
	// deliberate, so a scaled owner still hands back a world distance, which is
	// why the t squared term keeps its coefficient instead of being 1.
	const glm::vec3 toCenter = localRay.origin - m_Center;

	const float a = glm::dot(localRay.direction, localRay.direction);
	if (a < 1e-12f) return false;   // degenerate direction, nothing to march along

	const float b = 2.0f * glm::dot(toCenter, localRay.direction);
	const float c = glm::dot(toCenter, toCenter) - m_Radius * m_Radius;

	const float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f) return false;

	const float root = glm::sqrt(discriminant);

	// Not named near/far: those are legacy macros out of windef.h.
	const float nearRoot = (-b - root) / (2.0f * a);
	const float farRoot = (-b + root) / (2.0f * a);

	// The nearer root is behind the origin when the ray starts inside, so fall
	// through to the far one and report 0: it is touching it right now.
	if (nearRoot >= 0.0f)
	{
		distance = nearRoot;
		return true;
	}

	if (farRoot >= 0.0f)
	{
		distance = 0.0f;
		return true;
	}

	return false;   // both behind, the sphere is past the origin
}
