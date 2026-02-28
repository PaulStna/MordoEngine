#pragma once
#include "../../Core/Shader/Shader.h"
#include "../../Renderer/Geomipmapping.h"
#include "../Terrain.h"
#include <memory>

class TerrainSystem
{
private:
	float m_TextureScale = 50.0f;
	float m_HeightThreshold1 = 0.3f;
	float m_HeightThreshold2 = 0.7f;
	std::string m_Texture1ID;
	std::string m_Texture2ID;
	std::string m_Texture3ID;
	std::unique_ptr<terrain::Terrain> m_Terrain;
	std::unique_ptr<Geomipmapping> m_TerrainRenderer;

public:
	TerrainSystem();
	void Update(float deltaTime);
	void Render(const Shader& shader,
				const glm::vec3& cameraPos,
				const glm::mat4* projection,
				const glm::mat4* view,
				const glm::mat4* model);
	glm::vec3 GetMiddleTerrainPosition() const;
	int GetTerrainWorldScale() const;
	float GetTerrainHeightScale() const;
	float GetTerrainInterpolatedHeightAt(float x, float z, float yOffset = 0) const;
	terrain::Terrain& GetTerrain() const;
	void CheckForModifications();
	~TerrainSystem();
};
