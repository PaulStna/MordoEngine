#pragma once
#include "../../Core/Shader/Shader.h"
#include "../../Core/Texture/Texture.h"
#include "../../Core/Resources/ResourceLibrary.h"
#include "../../Renderer/RenderContext.h"
#include "../../Renderer/Terrain/TerrainMeshRenderer.h"
#include "../Mesh/Geomipmapping.h"
#include "../Terrain.h"
#include <memory>

/// Owns the terrain height data, the meshing technique that turns it into
/// geometry, and the GL renderer that draws it.
///
/// Drives the three together and owns the material uniforms — the splatting
/// thresholds and the texture scale — which are per pass and belong to nobody
/// below.
class TerrainSystem
{
private:
	// How many times the ground textures repeat across the terrain.
	float m_TextureScale = 50.0f;

	// Normalized heights where one splatting texture gives way to the next, so
	// grass turns to dirt and dirt to rock. Blended, not stepped.
	float m_HeightThreshold1 = 0.3f;
	float m_HeightThreshold2 = 0.7f;

	// Borrowed from the ResourceLibrary, low to high.
	Texture& m_Texture1;
	Texture& m_Texture2;
	Texture& m_Texture3;

	// Held by pointer so the three stay independently replaceable: a different
	// generator or a different meshing technique swaps one of them out.
	std::unique_ptr<Terrain>             m_Terrain;
	std::unique_ptr<Geomipmapping>       m_Mesh;
	std::unique_ptr<TerrainMeshRenderer> m_MeshRenderer;

public:
	/// @param textures Library the ground textures are borrowed from. Must
	///                 outlive the system.
	TerrainSystem(ResourceLibrary<Texture>& textures);

	void Update(float deltaTime);

	/// Selects the levels of detail for this pass and draws every patch.
	void Render(const Shader& shader, const RenderContext& renderContext);

	/// Centre of the terrain at ground height, which is where things are spawned
	/// from when nothing better is known.
	glm::vec3 GetMiddleTerrainPosition() const;

	/// World units between two neighbouring grid posts.
	int GetTerrainWorldScale() const;

	/// How far a full-range height sample reaches in world units.
	float GetTerrainHeightScale() const;

	/// Ground height under a world-space point, interpolated between grid posts
	/// so something walking over it moves smoothly.
	/// @param yOffSet Added to the result, for a body whose pivot is not at its
	///                feet.
	float GetTerrainInterpolatedHeightAt(float x, float z, float yOffSet = 0) const;

	Terrain& GetTerrain() const;

	/// Re-uploads whatever the editor changed since the last call, then clears
	/// the record. Doing nothing when nothing was edited is the normal case.
	void CheckForModifications();

	/// Writes the heightmap back to disk in the raw form it was loaded from.
	void SaveTerrain();

	~TerrainSystem();
};
