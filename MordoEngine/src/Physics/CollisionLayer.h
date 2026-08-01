#pragma once
#include <cstdint>

// What kind of thing a collider is, as a bit, so a query can ask for some of
// them and ignore the rest.
//
// A mask is the difference between "what am I looking at" and "what blocks me",
// which are the same geometry asked two different questions. Without it every
// query walks everything and filters by hand at the call site, and that is the
// part that becomes unworkable once there is more than one kind of query.
enum class CollisionLayer : std::uint32_t
{
	None         = 0,

	Default      = 1u << 0,   // scenery, props, whatever has no better label
	Interactable = 1u << 1,   // answers to Interact
	Creature     = 1u << 2,   // animals, the player, anything alive
	Solid        = 1u << 3,   // meant to block movement, once something does

	All          = 0xFFFFFFFFu
};

inline CollisionLayer operator|(CollisionLayer a, CollisionLayer b)
{
	return static_cast<CollisionLayer>(
		static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

inline CollisionLayer operator&(CollisionLayer a, CollisionLayer b)
{
	return static_cast<CollisionLayer>(
		static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));
}

// Whether a layer is in a mask. The one thing every query needs.
inline bool LayerMatches(CollisionLayer layer, CollisionLayer mask)
{
	return (static_cast<std::uint32_t>(layer) & static_cast<std::uint32_t>(mask)) != 0;
}
