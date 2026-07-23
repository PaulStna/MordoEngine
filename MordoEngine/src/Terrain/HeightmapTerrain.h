#pragma once
#include "Terrain.h"
#include <string>

class HeightMapTerrain : public Terrain 
{
public:
	HeightMapTerrain(const std::string& filepath);
};
