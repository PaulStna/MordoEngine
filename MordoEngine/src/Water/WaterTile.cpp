#include "WaterTile.h"

WaterTile::WaterTile(const glm::vec3 position, const glm::vec3 scale, const float yPos)
{
	m_Data.position = position;
	m_Data.scale = scale;
	m_Data.yPos = yPos;
}

void WaterTile::Update(float deltaTime)
{
	m_Data.moveFactor += 0.03f * deltaTime;
	if (m_Data.moveFactor >= 1.0f)
		m_Data.moveFactor -= 1.0f;  
}