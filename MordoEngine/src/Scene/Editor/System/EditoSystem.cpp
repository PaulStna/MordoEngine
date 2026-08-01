#include "EditorSystem.h"
#include "../../../Renderer/AreaSelectorRenderer.h"
#include <iostream>
#include <glm/glm.hpp>

EditorSystem::EditorSystem(Shader& areaSelectorShader)
{
	m_Renderer = std::make_unique<AreaSelectorRenderer>(areaSelectorShader, m_Radius, m_Segments);
	m_LastWorldPosition = glm::vec3(0.0f);
}

void EditorSystem::Update(const Terrain& terrain, const Camera& camera)
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
			heights[i] = terrain.GetHeightInterpolated(posX, posZ) + m_HeightOffset;
		}
	}
	m_Renderer->SetHeights(heights, m_LastWorldPosition);
}

glm::vec3 EditorSystem::RaycastToTerrain(
	const glm::vec3& rayOrigin,
	const glm::vec3& rayDirection,
	const Terrain& terrain)
{
	float terrainSize = terrain.GetSize() * terrain.GetWorldScale();

	float maxDistance = terrainSize;
	float stepSize = 0.5f;

	for (float t = 0; t < maxDistance; t += stepSize) {
		glm::vec3 point = rayOrigin + rayDirection * t;

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

void EditorSystem::Render(const RenderContext& renderContext)
{
	m_Renderer->Render(renderContext);
}

glm::vec3 EditorSystem::GetWorldPosition() const
{
	return m_LastWorldPosition;
}

void EditorSystem::IncreaseSelector()
{
	ModifySelector(m_RadiusStep);
}

void EditorSystem::DecreaseSelector()
{
	ModifySelector(-m_RadiusStep);
}

void EditorSystem::ModifySelector(float radius)
{
	m_Radius += radius;
	if (m_Radius > 35.0f) {
		m_Radius = 35.f;
	}

	if (m_Radius < 5.0f) {
		m_Radius = 5.f;
	}

	m_Renderer->SetRadio(m_Radius);
}

void EditorSystem::IncreaseTerrain(Terrain& terrain)
{
	ModifyTerrain(terrain, m_BrushStrength);
}

void EditorSystem::DecreaseTerrain(Terrain& terrain)
{
	ModifyTerrain(terrain, -m_BrushStrength);
}

void EditorSystem::ModifyTerrain(Terrain& terrain, float heightFactor)
{
	float worldScale = terrain.GetWorldScale();
	int centerX = (int)(m_LastWorldPosition.x / worldScale);
	int centerZ = (int)(m_LastWorldPosition.z / worldScale);
	int size = terrain.GetSize();

	for (int z = centerZ - m_Radius; z <= centerZ + m_Radius; ++z)
	{
		for (int x = centerX - m_Radius; x <= centerX + m_Radius; ++x)
		{
			if (x < 0 || z < 0 || x >= size || z >= size)
				continue;

			float dx = (float)(x - centerX);
			float dz = (float)(z - centerZ);
			float dist = sqrtf(dx * dx + dz * dz);

			if (dist > m_Radius)
				continue;

			float t = dist / m_Radius;
			float falloff = 1.0f - t;

			float currentHeight = terrain.GetHeightAt(x, z);
			float newHeight = currentHeight + falloff * heightFactor * terrain.GetHeightScale();
			terrain.SetHeightAt(newHeight, x, z);
			terrain.MarkVertexAsModified(x, z);
		}
	}
}

void EditorSystem::IncreaseBrushStrength()
{
	ModifyBrushStrength(m_BrushStrengthStep);
}
void EditorSystem::DecreaseBrushStrength()
{
	ModifyBrushStrength(-m_BrushStrengthStep);
}

void EditorSystem::ModifyBrushStrength(float strengthFactor)
{
	m_BrushStrength += strengthFactor;
	if (m_BrushStrength > 50)
	{
		m_BrushStrength = 50;
	}

	if (m_BrushStrength < 0)
	{
		m_BrushStrength = 0.5f;
	}
}