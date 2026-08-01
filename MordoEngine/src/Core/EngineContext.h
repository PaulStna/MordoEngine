#pragma once
#include "Resources/ResourceLibrary.h"
#include "Resources/ResourceLoader.h"
#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "Model/Model.h"
#include "../World/World.h"
#include <memory>

/// Everything that lives for the whole program: the resource libraries and the
/// world built on them.
///
/// The constructor is the engine's startup order made explicit — shaders,
/// textures and models are loaded before the world that borrows them, so
/// nothing downstream has to check whether a resource is ready yet. The
/// libraries are declared before the world so they also outlive it on the way
/// down.
///
/// Requires a current GL context: the loaders it calls upload to the GPU.
class EngineContext
{
public:
	/// @param screenWidth  Framebuffer width in pixels, handed to the world for
	///                     its projection and offscreen buffers.
	/// @param screenHeight Framebuffer height in pixels, same.
	/// @throws std::runtime_error if a shader or texture cannot be loaded.
	///         Missing models are skipped rather than fatal.
	EngineContext(unsigned int screenWidth, unsigned int screenHeight)
	{
		LoadShaders(m_Shaders);
		LoadTextures(m_Textures);
		LoadModels(m_Models, m_Textures);
		m_World = std::make_unique<World>(screenWidth, screenHeight, m_Shaders, m_Textures, m_Models);
	}

	World& GetWorld() { return *m_World; }
	ResourceLibrary<Shader>& Shaders() { return m_Shaders; }
	ResourceLibrary<Texture>& Textures() { return m_Textures; }
	ResourceLibrary<Model>& Models() { return m_Models; }

private:
	// Declared before the world, so they are destroyed after it: the world holds
	// references into all three.
	ResourceLibrary<Shader>  m_Shaders;
	ResourceLibrary<Texture> m_Textures;
	ResourceLibrary<Model>   m_Models;

	// Held by pointer because it cannot be built until the libraries above are
	// populated, which happens in the constructor body.
	std::unique_ptr<World>   m_World;
};
