#pragma once
#include "../../Core/Shader/Shader.h"
#include "../../Core/Texture/Texture.h"
#include "../../Core/Resources/ResourceLibrary.h"
#include "../../Renderer/RenderContext.h"
#include "../../Renderer/Terrain/TerrainMeshRenderer.h"
#include "../Mesh/Geomipmapping.h"
#include "../Terrain.h"
#include <memory>

// Owns the terrain height data, the meshing technique that turns it into
// geometry, and the GL renderer that draws it. Drives the three together and
// applies the terrain material uniforms.
class TerrainSystem
{
private:
	float m_TextureScale = 50.0f;
	float m_HeightThreshold1 = 0.3f;
	float m_HeightThreshold2 = 0.7f;
	Texture& m_Texture1;
	Texture& m_Texture2;
	Texture& m_Texture3;
	std::unique_ptr<terrain::Terrain>    m_Terrain;
	std::unique_ptr<Geomipmapping>       m_Mesh;
	std::unique_ptr<TerrainMeshRenderer> m_MeshRenderer;

public:
	TerrainSystem(ResourceLibrary<Texture>& textures);
	void Update(float deltaTime);
	void Render(const Shader& shader, const RenderContext& renderContext);
	glm::vec3 GetMiddleTerrainPosition() const;
	int GetTerrainWorldScale() const;
	float GetTerrainHeightScale() const;
	float GetTerrainInterpolatedHeightAt(float x, float z, float yOffSet = 0) const;
	terrain::Terrain& GetTerrain() const;
	void CheckForModifications();
	void SaveTerrain();
	~TerrainSystem();
};
