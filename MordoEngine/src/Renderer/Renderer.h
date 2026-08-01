#pragma once
#include "../Core/Shader/Shader.h"
#include "RenderContext.h"

/// Base for anything that owns GPU buffers and issues draw calls.
///
/// This is the only layer allowed to touch OpenGL. A subsystem's data half
/// produces plain CPU geometry, its system half decides what to draw and sets
/// the per-pass uniforms, and a Renderer is what turns that into GL calls.
///
/// Deriving types own GL names, so they delete copy and move; to keep one in a
/// container, hold it through a unique_ptr.
class Renderer
{
protected:
	// The program this renderer draws with. A reference because it is borrowed
	// from the ResourceLibrary and must outlive the renderer.
	Shader& p_Shader;

public:
	/// @param shader Must outlive the renderer. Owned by the ResourceLibrary.
	Renderer(Shader& shader) : p_Shader(shader) {}

	/// Issues this renderer's draw calls for one pass.
	virtual void Render(const RenderContext& renderContext) = 0;

	virtual ~Renderer() = default;
};
