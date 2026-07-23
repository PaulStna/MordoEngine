#pragma once
#include "Terrain.h"
#include <glm/vec2.hpp>

class FaultFormationTerrain : public Terrain
{
public:
	FaultFormationTerrain(std::size_t terrainSize, int worldScale,  int iterations,
		uint16_t minHeight, uint16_t maxHeight, float filter);

private:
	float m_Filter;

	void CreateFaultFormation(int iterations);
	void GetRandomPoints(glm::vec2& p1, glm::vec2& p2);
	void SmoothTerrainWithFIRFilter();
	float ApplyFIRFilterToPoint(int x, int z, float PrevVal);
};