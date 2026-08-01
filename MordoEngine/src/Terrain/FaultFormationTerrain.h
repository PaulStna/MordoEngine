#pragma once
#include "Terrain.h"
#include <glm/vec2.hpp>

/// Generates terrain by fault formation: repeatedly draw a random line across
/// the grid and raise everything on one side of it.
///
/// Ridges come out sharp, so the result is run through a FIR filter afterwards
/// to erode them into something that reads as landscape rather than as stacked
/// steps.
class FaultFormationTerrain : public Terrain
{
public:
	/// @param terrainSize Side length in vertices; the grid holds this squared.
	/// @param worldScale  World units between two grid posts.
	/// @param iterations  How many faults to cut. More means finer detail and a
	///                    longer build.
	/// @param minHeight   Low end of the range the result is rescaled into.
	/// @param maxHeight   High end of the same range.
	/// @param filter      Erosion strength, 0 to 1. 0 leaves the faults sharp;
	///                    towards 1 smooths them into rolling ground.
	FaultFormationTerrain(std::size_t terrainSize, int worldScale,  int iterations,
		uint16_t minHeight, uint16_t maxHeight, float filter);

private:
	float m_Filter;

	void CreateFaultFormation(int iterations);
	void GetRandomPoints(glm::vec2& p1, glm::vec2& p2);
	void SmoothTerrainWithFIRFilter();
	float ApplyFIRFilterToPoint(int x, int z, float PrevVal);
};
