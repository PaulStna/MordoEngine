#pragma once
#include "Terrain.h"

class MidpointDisplacement : public terrain::Terrain {
public:
	MidpointDisplacement(std::size_t terrainSize, float roughness, std::size_t minHeight, std::size_t maxHeight);
private:
	float m_Roughness;
	void CreateMidpointDisplacement();
	void DiamondStep(int RectSize, float CurHeight);
	void SquareStep(int RectSize, float CurHeight);
};