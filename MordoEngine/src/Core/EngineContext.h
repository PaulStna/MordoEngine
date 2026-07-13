#pragma once
#include "../World/World.h"

class EngineContext
{
public:
	EngineContext(unsigned int screenWidth, unsigned int screenHeight)
		: m_World(screenWidth, screenHeight) {}

	World& GetWorld() { return m_World; }

private:
	World m_World;
};