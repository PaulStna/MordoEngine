#pragma once
#include "../../Core/Shader/Shader.h"
#include "../../Core/Texture/Texture.h"
#include "../../Renderer/Skybox/SkyBoxRenderer.h"
#include "../../Renderer/RenderContext.h"
#include <memory>

/// Draws the sky.
///
/// The thinnest of the systems, because a cubemap sky has no state worth
/// keeping: it exists so the sky is drawn through the same
/// data / renderer / system split as everything else rather than being a
/// special case in World.
class SkySystem
{
private:
	// Held by pointer because SkyBoxRenderer owns GL names and is neither
	// copyable nor movable.
	std::unique_ptr<SkyBoxRenderer> m_Renderer;

public:
	SkySystem();

	void Update(float deltaTime);

	/// Draws the sky behind everything already in the depth buffer.
	/// @param texture The cubemap, bound as a cubemap rather than a 2D texture.
	void Render(const Shader& shader,
		        const Texture& texture,
				const RenderContext& renderContext);
};
