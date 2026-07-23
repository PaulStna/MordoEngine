#include "TerrainSystem.h"
#include "../HeightmapTerrain.h"
#include "../FaultFormationTerrain.h"
#include "../MidpointDisplacement.h"
#include <iostream>

TerrainSystem::TerrainSystem(ResourceLibrary<Texture>& textures)
	: m_Texture1(textures.Get("grass")),
	m_Texture2(textures.Get("dirt")),
	m_Texture3(textures.Get("rock")),
	m_Terrain(std::make_unique<HeightMapTerrain>("res/maps/heightmap.raw"))
{
	m_Terrain->SetWorldScale(4.0f);
	m_Terrain->SetHeightScale(1000.0f);

	m_Mesh = std::make_unique<Geomipmapping>(GetTerrain(), 33);
	m_MeshRenderer = std::make_unique<TerrainMeshRenderer>();
	m_MeshRenderer->Upload(m_Mesh->GetVertices(), m_Mesh->GetIndices());
}

void TerrainSystem::Update(float deltaTime)
{
	//
}

void TerrainSystem::Render(const Shader& shader, const RenderContext& renderContext)
{
	shader.Use();
	shader.SetMat4("projection", renderContext.projection);
	shader.SetMat4("view", renderContext.view);
	shader.SetMat4("model", renderContext.model);

	shader.SetInt("texture1", 0);
	glActiveTexture(GL_TEXTURE0);
	m_Texture1.Use();

	shader.SetInt("texture2", 1);
	glActiveTexture(GL_TEXTURE1);
	m_Texture2.Use();

	shader.SetInt("texture3", 2);
	glActiveTexture(GL_TEXTURE2);
	m_Texture3.Use();

	shader.SetFloat("textureScale", m_TextureScale);
	shader.SetFloat("heightThreshold1", m_HeightThreshold1);
	shader.SetFloat("heightThreshold2", m_HeightThreshold2);

	m_Mesh->SelectLods(renderContext.cameraPos);
	m_MeshRenderer->Render(m_Mesh->GetDrawCalls());
}

glm::vec3 TerrainSystem::GetMiddleTerrainPosition() const
{
	float x = m_Terrain->GetSize() * m_Terrain->GetWorldScale() / 2;
	float z = m_Terrain->GetSize() * m_Terrain->GetWorldScale() / 2;
	return glm::vec3(x, m_Terrain->GetHeightInterpolated(x, z), z);
}

int TerrainSystem::GetTerrainWorldScale() const
{
	return m_Terrain->GetWorldScale();
}

float TerrainSystem::GetTerrainHeightScale() const
{
	return m_Terrain->GetHeightScale();
}

float TerrainSystem::GetTerrainInterpolatedHeightAt(float x, float z, float yOffSet) const
{
	return m_Terrain->GetHeightInterpolated(x, z) + yOffSet * m_Terrain->GetHeightScale();
}

terrain::Terrain& TerrainSystem::GetTerrain() const
{
	return *m_Terrain;
}

void TerrainSystem::CheckForModifications()
{
	if (!m_Terrain->HasModifications()) {
		return;
	}

	const std::vector<int>& changed = m_Mesh->RebuildModifiedVertices(*m_Terrain);
	m_MeshRenderer->UpdateVertices(m_Mesh->GetVertices(), changed);
	m_Terrain->ClearModifications();
}

void TerrainSystem::SaveTerrain()
{
	m_Terrain->SaveHeightMap("res/maps/heightmap.raw");
}

TerrainSystem::~TerrainSystem()
{
	m_Terrain->UnloadHeightMap();
}
