#include "LightSystem.h"
#include "./PointLight/PointLightData.h"
#include "./DirLight/DirLightData.h"

LightSystem::LightSystem() : m_DirLight(std::make_unique<DirLight>(0.9f))
{

}

void LightSystem::Update(float deltaTime)
{
	m_DirLight->Update(deltaTime);
}

void LightSystem::Render(const Shader& terrainShader,
	 					 const Shader& cubeLightShader,
						 const RenderContext& renderContext)
{
	RenderLights(cubeLightShader, renderContext);
	ApplyUniforms(terrainShader, renderContext);
}

void LightSystem::RenderLights(const Shader& lightCubeShader, const RenderContext& renderContext)
{
	for (int i = 0; i < m_PointLights.size(); i++) {
		m_PointLights[i].Render(lightCubeShader, renderContext);
	}
}

void LightSystem::ApplyUniforms(const Shader& shader, const RenderContext& renderContext)
{
	shader.Use();
	shader.SetVec3("viewPos", renderContext.cameraPos);

	const DirLightData& dirLight = m_DirLight->GetData();
	shader.SetVec3("dirLight.direction", dirLight.direction);
	shader.SetVec3("dirLight.ambient", dirLight.ambient);
	shader.SetVec3("dirLight.diffuse", dirLight.diffuse);

	// Packed rather than indexed by position in the vector: a light that is off
	// is skipped and the ones after it move down, so the shader always reads a
	// run of live lights starting at zero.
	//
	// The count is what was actually written, which also keeps numPointLights
	// from ever claiming more than the array holds.
	int slot = 0;
	for (std::size_t i = 0; i < m_PointLights.size() && slot < MAX_POINT_IGHTS; i++) {
		if (!m_PointLights[i].IsEnabled()) continue;

		const PointLightData& pointLight = m_PointLights[i].GetData();
		std::string path = "pointLights[" + std::to_string(slot) + "].";
		shader.SetVec3(path + "position", pointLight.position);
		shader.SetVec3(path + "ambient", pointLight.ambient);
		shader.SetVec3(path + "diffuse", pointLight.diffuse);
		shader.SetVec3(path + "specular", pointLight.specular);
		shader.SetFloat(path + "constant", pointLight.constant);
		shader.SetFloat(path + "linear", pointLight.linear);
		shader.SetFloat(path + "quadratic", pointLight.quadratic);
		slot++;
	}

	shader.SetInt("numPointLights", slot);
}

std::size_t LightSystem::AddPointLight(PointLight&& pointLight)
{
	m_PointLights.push_back(std::move(pointLight));
	return m_PointLights.size() - 1;
}

void LightSystem::SetPointLightPosition(std::size_t index, const glm::vec3& position)
{
	if (index < m_PointLights.size())
	{
		m_PointLights[index].SetPosition(position);
	}
}

void LightSystem::SetPointLightEnabled(std::size_t index, bool enabled)
{
	if (index < m_PointLights.size())
	{
		m_PointLights[index].SetEnabled(enabled);
	}
}

bool LightSystem::IsPointLightEnabled(std::size_t index) const
{
	return index < m_PointLights.size() && m_PointLights[index].IsEnabled();
}

LightSystem::~LightSystem()
{
	m_PointLights.clear();
}