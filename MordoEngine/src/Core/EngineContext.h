#pragma once
#include "Resources/ResourceLibrary.h"
#include "Resources/ResourceLoader.h"
#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "../World/World.h"
#include <memory>

class EngineContext
{
public:
	EngineContext(unsigned int screenWidth, unsigned int screenHeight)
	{
		LoadShaders(m_Shaders);
		LoadTextures(m_Textures);
		m_World = std::make_unique<World>(screenWidth, screenHeight, m_Shaders, m_Textures);
	}

	World& GetWorld() { return *m_World; }
	ResourceLibrary<Shader>& Shaders() { return m_Shaders; }
	ResourceLibrary<Texture>& Textures() { return m_Textures; }

private:
	ResourceLibrary<Shader>  m_Shaders;
	ResourceLibrary<Texture> m_Textures;
	std::unique_ptr<World>   m_World;
};