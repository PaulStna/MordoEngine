#pragma once
#include "WaterTileData.h"

class WaterTile
{
private:
	WaterTileData m_Data;

public:
	WaterTile(const glm::vec3 position,const glm::vec3 scale, const float yPos);
	void Update(float deltaTime);
	const WaterTileData& GetData() const { return m_Data; }
};
