#include "Bounds.h"
#include <algorithm>
#include <limits>

Bounds Bounds::Empty()
{
	// Inverted on purpose: the first Encapsulate then lands exactly on its point
	// instead of stretching a box that already contains the origin.
	const float large = std::numeric_limits<float>::max();
	return Bounds(glm::vec3(large), glm::vec3(-large));
}

Bounds Bounds::FromCenterAndExtents(const glm::vec3& center, const glm::vec3& extents)
{
	const glm::vec3 absolute = glm::abs(extents);
	return Bounds(center - absolute, center + absolute);
}

bool Bounds::IsValid() const
{
	return min.x <= max.x && min.y <= max.y && min.z <= max.z;
}

void Bounds::Encapsulate(const glm::vec3& point)
{
	min = glm::min(min, point);
	max = glm::max(max, point);
}

void Bounds::Encapsulate(const Bounds& other)
{
	if (!other.IsValid()) return;

	min = glm::min(min, other.min);
	max = glm::max(max, other.max);
}

void Bounds::Expand(float amount)
{
	min -= glm::vec3(amount);
	max += glm::vec3(amount);
}

bool Bounds::Contains(const glm::vec3& point) const
{
	return point.x >= min.x && point.x <= max.x
		&& point.y >= min.y && point.y <= max.y
		&& point.z >= min.z && point.z <= max.z;
}

bool Bounds::Overlaps(const Bounds& other) const
{
	// Separated on any one axis is enough to be apart, which is what makes this
	// worth doing before anything exact.
	return min.x <= other.max.x && max.x >= other.min.x
		&& min.y <= other.max.y && max.y >= other.min.y
		&& min.z <= other.max.z && max.z >= other.min.z;
}

Bounds Bounds::Transformed(const glm::mat4& matrix) const
{
	if (!IsValid()) return *this;

	// Every corner, because under rotation the extreme along an axis can be any
	// of the eight. Transforming only min and max would give a box that misses
	// most of the shape.
	Bounds result = Bounds::Empty();

	for (int corner = 0; corner < 8; corner++)
	{
		const glm::vec3 point{
			(corner & 1) ? max.x : min.x,
			(corner & 2) ? max.y : min.y,
			(corner & 4) ? max.z : min.z };

		result.Encapsulate(glm::vec3(matrix * glm::vec4(point, 1.0f)));
	}

	return result;
}

bool Bounds::IntersectRay(const Ray& ray, float& distance) const
{
	if (!IsValid()) return false;

	float entry = 0.0f;
	float exit = std::numeric_limits<float>::max();

	for (int axis = 0; axis < 3; axis++)
	{
		if (glm::abs(ray.direction[axis]) < 1e-8f)
		{
			// Parallel to this pair of planes. Either it started between them,
			// in which case this axis constrains nothing, or it never gets in.
			// Handled here rather than through a division by zero, which would
			// put a NaN into the comparisons below.
			if (ray.origin[axis] < min[axis] || ray.origin[axis] > max[axis])
			{
				return false;
			}
			continue;
		}

		const float inverse = 1.0f / ray.direction[axis];

		// Not named near/far: those are legacy macros out of windef.h, and this
		// would stop compiling the day anything pulls windows.h in first.
		float axisEntry = (min[axis] - ray.origin[axis]) * inverse;
		float axisExit = (max[axis] - ray.origin[axis]) * inverse;
		if (axisEntry > axisExit) std::swap(axisEntry, axisExit);

		entry = glm::max(entry, axisEntry);
		exit = glm::min(exit, axisExit);

		// Out the far side of one slab before entering another: it misses.
		if (exit < entry) return false;
	}

	distance = entry;
	return true;
}
