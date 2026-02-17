#pragma once
#include "../../../Terrain/Terrain.h"
#include "../../../Camera/Camera.h"
#include "../../../Renderer/Renderer.h"
#include "../../../Core/Shader/Shader.h"
#include <glm/vec3.hpp>
#include <memory>

class EditorSystem 
{
private:
	std::unique_ptr<Renderer> m_Renderer;
	int m_Segments = 256;
	float m_Radius = 25.0f;
	int m_HeightOffSet = 12.0f;
	float m_BrushStrenght = 10.0f;
	float m_RadiusStep = 2.0f;
	float m_BrushStrenghtStep = 0.5f;
	glm::vec3 m_LastWorldPosition;
	glm::vec3 RaycastToTerrain(
		const glm::vec3& rayOrigin,
		const glm::vec3& rayDir,
		const terrain::Terrain& terrain);
	void ModyfySelector(float radius);
	void ModifyTerrain(terrain::Terrain& terrain, float heightFactor);
	void ModifyBrushStrenght(float strenghtFactor);

public:
	EditorSystem(Shader& areaSelectorShader);
	void Update(const terrain::Terrain& terrain, const Camera& camera);
	void Render(glm::mat4* view,
				glm::mat4* projection,
				glm::mat4* model,
				glm::vec3* lightDir);
	glm::vec3 GetWorldPosition() const;
	void IncreaseSelector();
	void DecreaseSelector();

	void IncreaseTerrain(terrain::Terrain& terrain);
	void DecreaseTerrain(terrain::Terrain& terrain);

	void IncreaseBrushStrenght();
	void DecreaseBrushStrenght();
};
