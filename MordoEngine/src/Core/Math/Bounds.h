#pragma once
#include "Ray.h"
#include <glm/glm.hpp>

// An axis aligned box, the shape every collision question starts from.
//
// Two jobs, and they are not the same one:
//   - as a shape, it is what a BoxCollider tests against exactly, in the local
//     space of whatever owns it
//   - as a bound, it is the cheap stand-in for any other shape, which is what
//     makes "do these two things possibly touch" one comparison per axis
//
// An empty box is min > max on every axis, so Encapsulate on a fresh one lands
// on the first point instead of dragging the origin into the result.
struct Bounds
{
	glm::vec3 min{ 0.0f };
	glm::vec3 max{ 0.0f };

	Bounds() = default;
	Bounds(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

	static Bounds Empty();
	static Bounds FromCenterAndExtents(const glm::vec3& center, const glm::vec3& extents);

	// False for a box nothing was ever put into. Worth asking before trusting a
	// centre or a size, both of which are nonsense on an empty box.
	bool IsValid() const;

	glm::vec3 GetCenter()  const { return (min + max) * 0.5f; }
	glm::vec3 GetSize()    const { return max - min; }
	glm::vec3 GetExtents() const { return (max - min) * 0.5f; }

	// Grows the box to hold what it is given.
	void Encapsulate(const glm::vec3& point);
	void Encapsulate(const Bounds& other);

	// Pushes every face out by the same amount. Negative shrinks. Useful for the
	// margin a bind pose box needs once the thing inside it starts animating.
	void Expand(float amount);

	bool Contains(const glm::vec3& point) const;

	// Box against box, touching counts. This is the broad phase test.
	bool Overlaps(const Bounds& other) const;

	// The smallest axis aligned box holding this one after the transform. Under
	// rotation the result is looser than the shape it came from, which is the
	// accepted trade: it is a rejection test, and whatever survives it goes on
	// to an exact one.
	Bounds Transformed(const glm::mat4& matrix) const;

	// Slab test. On a hit, distance is where the ray enters, or 0 when it starts
	// inside. Returns false for a hit behind the origin.
	bool IntersectRay(const Ray& ray, float& distance) const;
};
