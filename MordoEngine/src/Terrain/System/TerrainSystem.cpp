#include "TerrainSystem.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Texture/Texture.h"
#include "../HeightmapTerrain.h"
#include "../FaultFormationTerrain.h"
#include "../MidpointDisplacement.h"
#include <iostream>

TerrainSystem::TerrainSystem() : m_Texture1ID("grass"), m_Texture2ID("dirt"), m_Texture3ID("rock"),
m_Terrain(std::make_unique<HeightMapTerrain>("res/maps/heightmap.raw"))
//m_Terrain(std::make_unique<FaultFormationTerrain>(1057, 3.0f, 50, 0, terrain::RAW_HEIGHT_MAX, 0.15f))
{
	m_Terrain->SetWorldScale(4.0f);
	m_Terrain->SetHeightScale(1000.0f);
	m_TerrainRenderer = std::make_unique<Geomipmapping>(Manager<Shader>::Get("terrain"), GetTerrain(), 33.0f);
}

void TerrainSystem::Update(float deltaTime)
{
	//
}

void TerrainSystem::Render(const Shader& shader,
						   const glm::vec3& cameraPos,
						   const glm::mat4* projection,
						   const glm::mat4* view,
						   const glm::mat4* model)
{
	shader.Use();
	if (projection) {
		shader.SetMat4("projection", *projection);
	}
	if (view) {
		shader.SetMat4("view", *view);
	}
	if (model) {
		shader.SetMat4("model", *model);
	}

	shader.SetInt("texture1", 0);
	glActiveTexture(GL_TEXTURE0);
	Manager<Texture>::Get(m_Texture1ID).Use();

	shader.SetInt("texture2", 1);
	glActiveTexture(GL_TEXTURE1);
	Manager<Texture>::Get(m_Texture2ID).Use();

	shader.SetInt("texture3", 2);
	glActiveTexture(GL_TEXTURE2);
	Manager<Texture>::Get(m_Texture3ID).Use();

	shader.SetFloat("textureScale", m_TextureScale);
	shader.SetFloat("heightThreshold1", m_HeightThreshold1);
	shader.SetFloat("heightThreshold2", m_HeightThreshold2);

	m_TerrainRenderer->Render(cameraPos, view, projection, model);
}

glm::vec3 TerrainSystem::GetMiddleTerrainPosition() const
{
	float x = m_Terrain->GetSize() * m_Terrain->GetWorldScale() / 2;
	float z = m_Terrain->GetSize() * m_Terrain->GetWorldScale() / 2;
	return glm::vec3(
		x,
		m_Terrain->GetHeightInterpolated(x, z),
		z
	);
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
	return m_Terrain->GetHeightInterpolated(x, z) +
		yOffSet * m_Terrain->GetHeightScale();
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