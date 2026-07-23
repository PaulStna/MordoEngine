#include "TerrainSystem.h"
#include "../HeightmapTerrain.h"
#include "../FaultFormationTerrain.h"
#include "../MidpointDisplacement.h"
#include <iostream>

TerrainSystem::TerrainSystem(ResourceLibrary<Shader>& shaders, ResourceLibrary<Texture>& textures)
	: m_Texture1(textures.Get("grass")),
	m_Texture2(textures.Get("dirt")),
	m_Texture3(textures.Get("rock")),
	m_Terrain(std::make_unique<HeightMapTerrain>("res/maps/heightmap.raw"))
{
	m_Terrain->SetWorldScale(4.0f);
	m_Terrain->SetHeightScale(1000.0f);
	m_TerrainRenderer = std::make_unique<Geomipmapping>(shaders.Get("terrain"), GetTerrain(), 33.0f);
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

	m_TerrainRenderer->Render(renderContext);
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
	if (m_Terrain->HasModifications()) {
		m_TerrainRenderer->UpdateBuffers(GetTerrain());
		m_Terrain->ClearModifications();
	}
}

void TerrainSystem::SaveTerrain()
{
	m_Terrain->SaveHeightMap("res/maps/heightmap.raw");
}

TerrainSystem::~TerrainSystem()
{
	m_Terrain->UnloadHeightMap();
}