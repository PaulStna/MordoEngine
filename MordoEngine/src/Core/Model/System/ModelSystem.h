#pragma once
#include "../Body.h"
#include "../../Shader/Shader.h"
#include "../../Transform/Transform.h"
#include "../../../Renderer/RenderContext.h"

// Draws bodies. Owns nothing: the models live in the ResourceLibrary, the bodies
// live on their actors, and this only knows how to put the two together.
class ModelSystem
{
public:
	// The uniforms every body in the pass shares, set once before a run of
	// Render calls rather than again for each one.
	void BeginPass(const Shader& shader, const RenderContext& renderContext);

	// One body, at the transform its actor decided. Call BeginPass first.
	// An empty body draws nothing.
	void Render(const Body& body, const Transform& transform, const Shader& shader);
};
