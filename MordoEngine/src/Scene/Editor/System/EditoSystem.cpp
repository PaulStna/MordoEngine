#include "EditorSystem.h"
#include <glm/glm.hpp>
#include "../../../Core/Managers/Manager.h"
#include "../../../Core/Shader/Shader.h"
#include "../../../Renderer/AreaSelectorRenderer.h"

EditorSystem::EditorSystem()
{
	m_Renderer = std::make_unique<AreaSelectorRenderer>(m_Radius, m_Segments);
	m_LastWorldPosition = glm::vec3(0.0f);
}

void EditorSystem::Update(const terrain::Terrain& terrain, const Camera& camera)
{
	glm::vec3 m_WorldPosition = RaycastToTerrain(
		camera.GetPosition(),
		glm::normalize(camera.GetForward()),
		terrain);

	if (m_WorldPosition == m_LastWorldPosition) {
		return;
	}

	m_LastWorldPosition = m_WorldPosition;
	float terrainSize = terrain.GetSize() * terrain.GetWorldScale();

	std::vector<float> heights(m_Segments);
	for (int i = 0; i < m_Segments; i++) {
		float angle = (float)i / m_Segments * glm::two_pi<float>();
		float posX = glm::cos(angle) * m_Radius + m_LastWorldPosition.x;
		float posZ = glm::sin(angle) * m_Radius + m_LastWorldPosition.z;

		if (posX < 0.0f || posX > terrainSize ||
			posZ < 0.0f || posZ > terrainSize) {
			heights[i] = m_LastWorldPosition.y;
		}
		else {
			heights[i] = terrain.GetHeightInterpolated(posX, posZ) + m_HeightOffSet;
		}
	}
	m_Renderer->SetHeights(heights, m_LastWorldPosition);
}

glm::vec3 EditorSystem::RaycastToTerrain(
	const glm::vec3& rayOrigin,
	const glm::vec3& rayDir,
	const terrain::Terrain& terrain)
{
	float terrainSize = terrain.GetSize() * terrain.GetWorldScale();

	float maxDistance = terrainSize;
	float stepSize = 0.5f;

	for (float t = 0; t < maxDistance; t += stepSize) {
		glm::vec3 point = rayOrigin + rayDir * t;

		if (point.x < 0.0f || point.x > terrainSize ||
			point.z < 0.0f || point.z > terrainSize) {
			break;
		}

		float terrainHeight = terrain.GetHeightInterpolated(point.x, point.z);

		if (point.y <= terrainHeight) {
			return glm::vec3(point.x, 0.0f, point.z);
		}
	}

	return glm::vec3(rayOrigin.x, 0.0f, rayOrigin.z);
}

void EditorSystem::Render(const Camera& camera)
{
	Shader& shader = Manager<Shader>::Get("terrainSelector");
	shader.Use();

	glm::mat4 projection = camera.GetProjectionMatrix();
	shader.SetMat4("projection", projection);

	glm::mat4 view = camera.GetViewMatrix();
	shader.SetMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_LastWorldPosition);
	shader.SetMat4("model", model);

	m_Renderer->Render(shader, glm::vec3(0.0f));
}