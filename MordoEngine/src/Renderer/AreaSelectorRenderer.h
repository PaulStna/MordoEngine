#pragma once
#include "Renderer.h"
#include "../Camera/Camera.h"
#include "../Terrain/Terrain.h"
#include <memory>
#include <string>

class AreaSelectorRenderer 
{
private:
	GLuint m_Vao;
	GLuint m_PosVbo;
	GLuint m_HeightVbo;
	float m_Radius = 50.0f;
	float m_Segments = 256;
	float m_HeightOffSet = 10.0f;
	glm::vec3 m_WorldPosition;
	std::string m_ShaderID;
	std::shared_ptr<Camera> m_Camera;
	void CreateGLState();
	void PopulateBuffers();
	void InitVertices(std::vector<glm::vec2>& vertices);
	glm::vec3 RaycastToTerrain(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const terrain::Terrain& terrain);
public:
	AreaSelectorRenderer(std::shared_ptr<Camera> camera);
	void Render();
	void Update(terrain::Terrain& terrain);
	~AreaSelectorRenderer();
};
