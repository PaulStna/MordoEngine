#pragma once
#include "WaterTileData.h"

/// One square patch of water: where it sits, how big it is, and how far along
/// its ripple animation is.
///
/// Holds no GL state. PlaneRenderer draws it, and WaterSystem owns the
/// framebuffers and decides when.
class WaterTile
{
private:
	WaterTileData m_Data;

public:
	/// @param position Centre of the tile, world space. The Y component is not
	///                 what the surface sits at; yPos is.
	/// @param scale    Size in world units, as a scale applied to the unit quad.
	/// @param yPos     World-space height of the surface itself.
	WaterTile(const glm::vec3 position,const glm::vec3 scale, const float yPos);

	/// Advances the ripple animation.
	/// @param deltaTime Seconds since the last frame.
	void Update(float deltaTime);

	const WaterTileData& GetData() const { return m_Data; }
};
