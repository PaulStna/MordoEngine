#pragma once
#include "Terrain.h"

/// Generates terrain by midpoint displacement, the diamond-square algorithm:
/// set the corners, then repeatedly find the middle of each square and each
/// diamond and push it up or down by a shrinking random amount.
///
/// Every pass halves the step and reduces the displacement, so detail arrives
/// at finer and finer scales. That shrinking is what roughness controls.
class MidpointDisplacement : public Terrain
{
public:
	/// @param terrainSize Side length in vertices; must be a power of two plus
	///                    one for the halving to land on exact posts.
	/// @param worldScale  World units between two grid posts.
	/// @param roughness   How slowly the displacement decays between passes.
	///                    Low gives smooth hills, high gives jagged ground.
	/// @param minHeight   Low end of the range the result is rescaled into.
	/// @param maxHeight   High end of the same range.
	MidpointDisplacement(std::size_t terrainSize, int worldScale, float roughness,
		std::size_t minHeight, std::size_t maxHeight);

private:
	float m_Roughness;
	void CreateMidpointDisplacement();
	void DiamondStep(int RectSize, float CurHeight);
	void SquareStep(int RectSize, float CurHeight);
};
