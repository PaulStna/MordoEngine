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
						 const glm::vec3& cameraPos,
						 const glm::mat4* projection,
						 const glm::mat4* view,
						 const glm::mat4* model)
{
	RenderLights(cubeLightShader, projection, view);
	ApplyUniforms(terrainShader, cameraPos);
}

void LightSystem::RenderLights(const Shader& lightCubeShader,
							   const glm::mat4* projection,
							   const glm::mat4* view)
{
	for (int i = 0; i < m_PointLights.size(); i++) {
		m_PointLights[i].Render(lightCubeShader, projection, view, nullptr);
	}
}

void LightSystem::ApplyUniforms(const Shader& shader, const glm::vec3& cameraPos)
{
	shader.Use();
	shader.SetVec3("viewPos", cameraPos);

	const DirLightData& dirLight = m_DirLight->GetData();
	shader.SetVec3("dirLight.direction", dirLight.direction);
	shader.SetVec3("dirLight.ambient", dirLight.ambient);
	shader.SetVec3("dirLight.diffuse", dirLight.diffuse);

	shader.SetInt("numPointLights", m_PointLights.size());
	for (int i = 0; i < m_PointLights.size() && i < MAX_POINT_IGHTS; i++) {
		const PointLightData& pointLight = m_PointLights[i].GetData();
		std::string path = "pointLights[" + std::to_string(i) + "].";
		shader.SetVec3(path + "position", pointLight.position);
		shader.SetVec3(path + "ambient", pointLight.ambient);
		shader.SetVec3(path + "diffuse", pointLight.diffuse);
		shader.SetVec3(path + "specular", pointLight.specular);
		shader.SetFloat(path + "constant", pointLight.constant);
		shader.SetFloat(path + "linear", pointLight.linear);
		shader.SetFloat(path + "quadratic", pointLight.quadratic);
	}
}

void LightSystem::AddPointLight(PointLight&& pointLight)
{
	m_PointLights.push_back(std::move(pointLight));
}

LightSystem::~LightSystem()
{
	m_PointLights.clear();
}