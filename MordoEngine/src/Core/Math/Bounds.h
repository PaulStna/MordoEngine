#pragma once
#include "Ray.h"
#include <glm/glm.hpp>

/// An axis aligned box, the shape every collision question starts from.
///
/// It carries two jobs that are easy to confuse. As a shape, it is what a
/// BoxCollider tests against exactly, in the local space of whatever owns it.
/// As a bound, it is the cheap stand-in for any other shape, which is what
/// makes "do these two things possibly touch" one comparison per axis.
///
/// An empty box is min above max on every axis, so Encapsulate on a fresh one
/// lands on the first point instead of dragging the origin into the result.
struct Bounds
{
	glm::vec3 min{ 0.0f };
	glm::vec3 max{ 0.0f };

	Bounds() = default;
	Bounds(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

	/// A box holding nothing, inverted so the first Encapsulate lands exactly on
	/// its point. Not the same as a zero-sized box, which already contains the
	/// origin and would stretch to it.
	static Bounds Empty();

	/// @param extents Half the size along each axis, not the full size. The sign
	///                is ignored, so a negative extent still gives a real box.
	static Bounds FromCenterAndExtents(const glm::vec3& center, const glm::vec3& extents);

	/// False for a box nothing was ever put into. Worth asking before trusting a
	/// centre or a size, both of which are nonsense on an empty box.
	bool IsValid() const;

	glm::vec3 GetCenter()  const { return (min + max) * 0.5f; }
	glm::vec3 GetSize()    const { return max - min; }
	glm::vec3 GetExtents() const { return (max - min) * 0.5f; }

	/// Grows the box until it also holds the point.
	void Encapsulate(const glm::vec3& point);

	/// Grows the box until it also holds the other box. An empty other is
	/// ignored rather than poisoning this one with its inverted bounds.
	void Encapsulate(const Bounds& other);

	/// Pushes every face out by the same amount.
	/// @param amount World units to move each face outwards; negative shrinks.
	///               This is the margin a bind pose box needs once the thing
	///               inside it starts animating out of that pose.
	void Expand(float amount);

	/// Points exactly on a face count as inside.
	bool Contains(const glm::vec3& point) const;

	/// Box against box, touching counts. This is the broad phase test: being
	/// separated on any single axis is enough to be apart.
	bool Overlaps(const Bounds& other) const;

	/// The smallest axis aligned box holding this one after the transform, built
	/// from all eight corners because under rotation the extreme along an axis
	/// can be any of them.
	///
	/// The result is looser than the shape it came from, which is the accepted
	/// trade: it is a rejection test, and whatever survives it goes on to an
	/// exact one. An empty box transforms to itself.
	Bounds Transformed(const glm::mat4& matrix) const;

	/// Slab test against the box.
	/// @param ray      Direction is expected normalised; the distance comes back
	///                 in units of it.
	/// @param distance Written only on a hit: where the ray enters, or 0 when it
	///                 starts inside. Left untouched otherwise.
	/// @return false when the ray misses, when the box is empty, and when the
	///         box lies entirely behind the origin.
	bool IntersectRay(const Ray& ray, float& distance) const;
};
