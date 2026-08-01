#pragma once
#include "../Core/Math/Bounds.h"
#include "../Core/Math/Ray.h"

/// The shape of a thing, for the purpose of being hit or being in the way.
///
/// Everything here is in the owner's local space, never the world's. The owner
/// has the transform; asking the shape to know about it would mean rebuilding
/// the shape every time something moves, and would make a rotated shape wrong
/// or loose depending on how it was stored. So the transform goes the other
/// way: the caller brings the query in.
///
/// The two methods are the two halves of how collision is done everywhere, and
/// keeping them apart is what lets this scale to shapes that do not exist yet.
/// The broad phase is a box whatever the real shape is, cheap and uniform
/// enough to be indexed, sorted or put in a tree later without any shape
/// knowing about it; the narrow phase is exact and the shape's own business.
/// Adding a capsule is writing one of these, and nothing that queries needs to
/// know.
///
/// When physics arrives it slots in beside these rather than through them: a
/// shape against shape dispatch for contacts, and a broad phase structure to
/// replace the linear scan ActorSystem does today. Neither changes this
/// contract.
class Collider
{
public:
	virtual ~Collider() = default;

	/// The box that holds this shape, in local space.
	/// @return Must be valid, since it is what every rejection test runs on.
	virtual Bounds GetLocalBounds() const = 0;

	/// Exact intersection against the shape itself.
	///
	/// @param localRay Already transformed into the owner's local space by the
	///                 caller, and deliberately left unnormalised by that
	///                 transform.
	/// @param distance Written only on a hit.
	/// @return false on a miss, leaving distance untouched.
	///
	/// @note The distance that comes back is still in world units, which is
	///       worth knowing because it looks like it should not be. Transforming
	///       the direction as a vector and leaving it unnormalised means
	///       M * (O' + t*D') == O + t*D, so the same t names the same point in
	///       both spaces. Normalising the local direction is what would break
	///       that, so no implementation may do it.
	virtual bool Raycast(const Ray& localRay, float& distance) const = 0;
};
