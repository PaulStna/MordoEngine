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
	float m_Radius = 50.0f;
	int m_HeightOffSet = 12.0f;
	glm::vec3 m_LastWorldPosition;
	glm::vec3 RaycastToTerrain(
		const glm::vec3& rayOrigin,
		const glm::vec3& rayDir,
		const terrain::Terrain& terrain);
public:
	EditorSystem(Shader& areaSelectorShader);
	void Update(const terrain::Terrain& terrain, const Camera& camera);
	void Render(glm::mat4* view,
				glm::mat4* projection,
				glm::mat4* model,
				glm::vec3* lightDir);
	glm::vec3 GetWorldPosition() const;
};
