#pragma once
#include "../../../Terrain/Terrain.h"
#include "../../../Camera/Camera.h"
#include <glm/vec3.hpp>
#include "../../../Renderer/Renderer.h"
#include <memory>

class EditorSystem {

private:
	std::unique_ptr<Renderer> m_Renderer;
	int m_Segments = 256;
	float m_Radius = 15.0f;
	int m_HeightOffSet = 12.0f;
	glm::vec3 m_LastWorldPosition;
	glm::vec3 RaycastToTerrain(
		const glm::vec3& rayOrigin,
		const glm::vec3& rayDir,
		const terrain::Terrain& terrain);
public:
	EditorSystem();
	void Update(const terrain::Terrain& terrain, const Camera& camera);
	void Render(const Camera& camera);
};
