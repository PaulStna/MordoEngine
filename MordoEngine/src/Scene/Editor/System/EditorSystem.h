#pragma once
#include "../../../Terrain/Terrain.h"
#include "../../../Camera/Camera.h"
#include "../../../Renderer/AreaSelectorRenderer.h"
#include "../../../Renderer/RenderContext.h"
#include "../../../Core/Shader/Shader.h"
#include <glm/vec3.hpp>
#include <memory>

/// The terrain brush: works out what the camera is pointing at, draws the
/// selector there, and raises or lowers the ground inside it.
///
/// Edits go through Terrain::MarkVertexAsModified rather than straight into the
/// mesh, so a stroke re-uploads the few hundred vertices it touched instead of
/// the whole quarter-million-vertex buffer.
class EditorSystem
{
private:
	std::unique_ptr<AreaSelectorRenderer> m_Renderer;

	// Points around the selector ring. Fixed at construction, since it sizes
	// the renderer's height buffer.
	int m_Segments = 256;

	// Brush radius in world units.
	float m_Radius = 25.0f;

	// How far above the ground the ring is drawn, so it does not z-fight with
	// the surface it is lying on.
	int m_HeightOffset = 12;

	// World units a full-strength stroke moves the ground per step.
	float m_BrushStrength = 10.0f;

	// How much one press changes the radius and the strength.
	float m_RadiusStep = 2.0f;
	float m_BrushStrengthStep = 0.5f;

	// Where the brush landed last Update, so Render and the edit calls agree on
	// one position for the frame.
	glm::vec3 m_LastWorldPosition;

	/// Walks the ray forward until it goes under the terrain, which is enough
	/// for a brush and needs no acceleration structure.
	/// @return Where it crossed the surface, world space.
	glm::vec3 RaycastToTerrain(
		const glm::vec3& rayOrigin,
		const glm::vec3& rayDirection,
		const Terrain& terrain);

	void ModifySelector(float radius);

	/// @param heightFactor Signed multiplier on the brush strength, so the same
	///                     path raises and lowers.
	void ModifyTerrain(Terrain& terrain, float heightFactor);

	void ModifyBrushStrength(float strengthFactor);

public:
	/// @param areaSelectorShader Draws the selector ring. Borrowed; must outlive
	///                           the system.
	EditorSystem(Shader& areaSelectorShader);

	/// Casts from the camera to find where the brush is pointing and lifts the
	/// selector onto the ground there.
	void Update(const Terrain& terrain, const Camera& camera);

	void Render(const RenderContext& renderContext);

	/// Where the brush is pointing, world space, as of the last Update.
	glm::vec3 GetWorldPosition() const;

	void IncreaseSelector();
	void DecreaseSelector();

	/// Raises the ground inside the selector, falling off towards its edge.
	void IncreaseTerrain(Terrain& terrain);

	/// Lowers it the same way.
	void DecreaseTerrain(Terrain& terrain);

	void IncreaseBrushStrength();
	void DecreaseBrushStrength();
};
