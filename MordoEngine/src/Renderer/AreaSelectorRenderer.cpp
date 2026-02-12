#include "AreaSelectorRenderer.h"
#include "../Core/Managers/Manager.h"
#include "../Core/Shader/Shader.h"
#include <iostream>


AreaSelectorRenderer::AreaSelectorRenderer(std::shared_ptr<Camera> camera) : m_Camera(camera)
{
	m_ShaderID = "terrainSelector";
	CreateGLState();
	PopulateBuffers();

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AreaSelectorRenderer::CreateGLState()
{
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_PosVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_PosVbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &m_HeightVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_HeightVbo);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
	glEnableVertexAttribArray(1);

}

void AreaSelectorRenderer::PopulateBuffers()
{
	std::vector<glm::vec2> verticesXZ;
	std::vector<float> heights(m_Segments, 0.0f);

	InitVertices(verticesXZ);

	glBindBuffer(GL_ARRAY_BUFFER, m_PosVbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		verticesXZ.size() * sizeof(verticesXZ[0]),
		&verticesXZ[0],
		GL_STATIC_DRAW
	);

	glBindBuffer(GL_ARRAY_BUFFER, m_HeightVbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		heights.size() * sizeof(heights[0]),
		&heights[0],
		GL_DYNAMIC_DRAW
	);
}

void AreaSelectorRenderer::InitVertices(std::vector<glm::vec2>& vertices)
{

	for (int i = 0; i < m_Segments; ++i)
	{
		float angle = (float)i / m_Segments * glm::two_pi<float>();

		vertices.push_back({
			glm::cos(angle) * m_Radius,
			glm::sin(angle) * m_Radius
			});
	}
}

void AreaSelectorRenderer::Update(terrain::Terrain& terrain)
{
	m_WorldPosition = RaycastToTerrain(
		m_Camera->GetPosition(),
		glm::normalize(m_Camera->GetForward()),
		terrain);
	float terrainSize = terrain.GetSize() * terrain.GetWorldScale();

	std::vector<float> heights(m_Segments);
	for (int i = 0; i < m_Segments; i++) {
		float angle = (float)i / m_Segments * glm::two_pi<float>();
		float posX = glm::cos(angle) * m_Radius + m_WorldPosition.x;
		float posZ = glm::sin(angle) * m_Radius + m_WorldPosition.z;

		if (posX < 0.0f || posX > terrainSize ||
			posZ < 0.0f || posZ > terrainSize) {
			
		}
		else {
			heights[i] = terrain.GetHeightInterpolated(posX, posZ) + m_HeightOffSet;
		}

	}

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_HeightVbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, heights.size() * sizeof(float), heights.data());
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::vec3 AreaSelectorRenderer::RaycastToTerrain(
	const glm::vec3& rayOrigin,
	const glm::vec3& rayDir,
	const terrain::Terrain& terrain)
{
	float terrainSize = terrain.GetSize() * terrain.GetWorldScale();

	float maxDistance = terrainSize;
	float stepSize = 0.5f;

	for (float t = 0; t < maxDistance; t += stepSize){
		glm::vec3 point = rayOrigin + rayDir * t;

		if (point.x < 0.0f || point.x > terrainSize ||
			point.z < 0.0f || point.z > terrainSize){
			break;
		}

		float terrainHeight = terrain.GetHeightInterpolated(point.x, point.z);

		if (point.y <= terrainHeight){
			return glm::vec3(point.x, 0.0f, point.z);
		}
	}

	return glm::vec3(rayOrigin.x, 0.0f, rayOrigin.z);
}

void AreaSelectorRenderer::Render()
{
	Shader& shader = Manager<Shader>::Get(m_ShaderID);
	shader.Use();

	glm::mat4 projection = m_Camera->GetProjectionMatrix();
	shader.SetMat4("projection", projection);

	glm::mat4 view = m_Camera->GetViewMatrix();
	shader.SetMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_WorldPosition);
	shader.SetMat4("model", model);

	glBindVertexArray(m_Vao);
	glLineWidth(10.0f);
	glDrawArrays(GL_LINE_LOOP, 0, m_Segments);
}

AreaSelectorRenderer::~AreaSelectorRenderer()
{
	glDeleteBuffers(1, &m_PosVbo);
	glDeleteBuffers(1, &m_HeightVbo);
	glDeleteVertexArrays(1, &m_Vao);
}