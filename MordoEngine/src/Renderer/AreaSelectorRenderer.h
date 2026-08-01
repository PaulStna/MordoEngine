#pragma once
#include "Renderer.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

/// The editor's brush outline: a ring of points on the XZ plane, lifted onto
/// the terrain so it reads as painted on the surface rather than floating over
/// it.
///
/// Position and height live in two separate vertex buffers on purpose. The ring
/// is the same circle every frame and is uploaded once as static geometry;
/// only the per-vertex height changes as the brush moves, so following the
/// ground costs one glBufferSubData over a float per segment instead of
/// rebuilding the circle.
///
/// Owns GL names, so it is neither copyable nor movable.
class AreaSelectorRenderer : public Renderer
{
private:
	GLuint m_Vao;
	GLuint m_PosVbo;      // the ring on the XZ plane, uploaded once
	GLuint m_HeightVbo;   // one Y per segment, refreshed as the brush moves
	int m_Segments;
	glm::vec3 m_WorldPosition;
	void CreateGLState();
	void PopulateBuffers(float radius);
	void InitVertices(std::vector<glm::vec2>& vertices, float radius);

public:
	/// @param radius   Brush radius in world units.
	/// @param segments Points around the ring. Higher is smoother and costs one
	///                 vertex each; this also fixes the size of the height
	///                 buffer, so SetHeights must supply exactly this many.
	AreaSelectorRenderer(Shader& shader, float radius, int segments);

	void Render(const RenderContext& renderContext) override;

	/// Lifts the ring onto the ground.
	/// @param heights  One world-space Y per segment, in ring order. Must hold
	///                 as many entries as the renderer was built with.
	/// @param position Where the brush centre is, world space.
	void SetHeights(const std::vector<float>& heights, const glm::vec3& position);

	/// Rebuilds the ring at a new radius, which re-uploads the position buffer.
	/// @param radius World units.
	void SetRadio(float radius);

	~AreaSelectorRenderer();
};
