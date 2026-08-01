#pragma once
#include "Terrain.h"
#include <string>

/// A terrain whose heights come from a file rather than from a generator.
///
/// This is the one the game uses; the procedural generators exist beside it so
/// the same Terrain interface can be filled either way, and so the editor has
/// something to start from when there is no file yet.
class HeightMapTerrain : public Terrain
{
public:
	/// @param filepath Raw uint16 heightmap. The file carries no dimensions, so
	///                 it must be square and is sized from its own length.
	HeightMapTerrain(const std::string& filepath);
};
