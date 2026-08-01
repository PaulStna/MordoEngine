#pragma once
#include "../Body.h"
#include "../../Shader/Shader.h"
#include "../../Transform/Transform.h"
#include "../../../Renderer/RenderContext.h"

/// Draws bodies.
///
/// Owns nothing: the models live in the ResourceLibrary, the bodies live on
/// their actors, and this only knows how to put the two together. That is why
/// it is stateless and why one instance can serve every pass.
class ModelSystem
{
public:
	/// Sets the uniforms every body in the pass shares, once, before a run of
	/// Render calls rather than again for each one.
	void BeginPass(const Shader& shader, const RenderContext& renderContext);

	/// Draws one body at the transform its actor decided.
	///
	/// @param body      An empty body draws nothing, so an actor without a model
	///                  needs no special case at the call site.
	/// @param transform Where the actor put itself, world space.
	/// @param shader    Must be the same one BeginPass was given, and BeginPass
	///                  must have run for this pass already.
	void Render(const Body& body, const Transform& transform, const Shader& shader);
};
