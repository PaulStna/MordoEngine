#pragma once
#include <glm/glm.hpp>

/// A half line: where it starts and where it points.
///
/// The direction is expected to be normalised, because every distance that
/// comes back out of an intersection is measured in units of it. Normalise once
/// at the source rather than inside each test.
struct Ray
{
	glm::vec3 origin{ 0.0f };
	glm::vec3 direction{ 0.0f, 0.0f, -1.0f };

	Ray() = default;
	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin), direction(direction) {}

	/// @param distance Measured in units of direction, so this is world units
	///                 only while direction is normalised.
	glm::vec3 At(float distance) const { return origin + direction * distance; }

	/// Brings the ray into another space, the origin as a point and the
	/// direction as a vector, which is the whole reason those two differ in the
	/// fourth component.
	///
	/// This is how a rotated or scaled shape is tested exactly: instead of
	/// dragging the shape out into the world and refitting it, the ray comes in.
	/// A scaling matrix leaves the direction unnormalised on purpose, so the
	/// same distance still names the same point in both spaces.
	///
	/// @param matrix Usually the inverse of the owner's model matrix, to go from
	///               world space into the owner's local space.
	Ray Transformed(const glm::mat4& matrix) const
	{
		return Ray(
			glm::vec3(matrix * glm::vec4(origin, 1.0f)),
			glm::vec3(matrix * glm::vec4(direction, 0.0f)));
	}
};
