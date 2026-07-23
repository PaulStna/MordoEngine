#include "FaultFormationTerrain.h"
#include <iostream>
#include <random>
#include <stdexcept>
#include <algorithm>

FaultFormationTerrain::FaultFormationTerrain(std::size_t terrainSize, int worldScale, int iterations,
	uint16_t minHeight = 0, uint16_t maxHeight = Terrain::RAW_HEIGHT_MAX, float filter = 0.2f)
{
	Initialize(terrainSize, worldScale, minHeight, maxHeight);
	m_Filter = filter;
	CreateFaultFormation(iterations);
	RescaleData(p_MinHeight, p_MaxHeight);
}

void FaultFormationTerrain::CreateFaultFormation(int iterations)
{
	float deltaHeight = p_MaxHeight - p_MinHeight;

	for (int i = 0; i < iterations; i++) {
		float iterationRatio = static_cast<float>(i) / static_cast<float>(iterations);
		float increment = (1.0f - iterationRatio) * deltaHeight / static_cast<float>(iterations);

		glm::vec2 p1, p2;
		GetRandomPoints(p1, p2);

		float dirX = p2.x - p1.x;
		float dirZ = p2.y - p1.y;

		for (std::size_t x = 0; x < p_Width; x++) {
			for (std::size_t z = 0; z < p_Depth; z++) {
				float dx = static_cast<float>(x) - p1.x;
				float dz = static_cast<float>(z) - p1.y;

				float cross = dx * dirZ - dirX * dz;

				if (cross > 0.0f) {
					float currentHeight = GetHeightAt(x, z);
					SetHeightAt(currentHeight + increment, x, z);
				}
			}
		}
		SmoothTerrainWithFIRFilter();
	}
}

void FaultFormationTerrain::GetRandomPoints(glm::vec2& p1, glm::vec2& p2)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, static_cast<int>(p_Width) - 1);

	p1.x = static_cast<float>(dis(gen));
	p1.y = static_cast<float>(dis(gen));

	do {
		p2.x = static_cast<float>(dis(gen));
		p2.y = static_cast<float>(dis(gen));
	} while (p1 == p2);
}

void FaultFormationTerrain::SmoothTerrainWithFIRFilter()
{
	// Smooth from left to right
	for (int z = 0; z < GetSize(); z++) {
		float previousHeight = GetHeightAt(0, z);
		for (int x = 1; x < GetSize(); x++) {
			previousHeight = ApplyFIRFilterToPoint(x, z, previousHeight);
		}
	}

	// Smooth from right to left
	for (int z = 0; z < GetSize(); z++) {
		float previousHeight = GetHeightAt(GetSize() - 1, z);
		for (int x = GetSize() - 2; x >= 0; x--) {
			previousHeight = ApplyFIRFilterToPoint(x, z, previousHeight);
		}
	}

	// Smooth from bottom to top
	for (int x = 0; x < GetSize(); x++) {
		float previousHeight = GetHeightAt(x, 0);
		for (int z = 1; z < GetSize(); z++) {
			previousHeight = ApplyFIRFilterToPoint(x, z, previousHeight);
		}
	}

	// Smooth from top to bottom
	for (int x = 0; x < GetSize(); x++) {
		float previousHeight = GetHeightAt(x, GetSize() - 1);
		for (int z = GetSize() - 2; z >= 0; z--) {
			previousHeight = ApplyFIRFilterToPoint(x, z, previousHeight);
		}
	}
}

float FaultFormationTerrain::ApplyFIRFilterToPoint(int x, int z, float previousHeight)
{
	float currentHeight = GetHeightAt(x, z);
	float smoothedHeight = m_Filter * previousHeight + (1.0f - m_Filter) * currentHeight;
	SetHeightAt(smoothedHeight, x, z);
	return smoothedHeight;
}