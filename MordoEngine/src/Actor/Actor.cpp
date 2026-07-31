#include "Actor.h"
#include "../Core/Model/Model.h"
#include "../Terrain/System/TerrainSystem.h"

void Actor::PlaceOnTerrain(const TerrainSystem& terrain, const glm::vec2& xz,
	float heightOffset)
{
	m_Transform.SetPosition({ xz.x,
		terrain.GetTerrainInterpolatedHeightAt(xz.x, xz.y, heightOffset),
		xz.y });
}
