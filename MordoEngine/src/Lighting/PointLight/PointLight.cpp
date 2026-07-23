#include "PointLight.h"
#include <glm/gtc/matrix_transform.hpp>

PointLight::PointLight(const glm::vec3 position) : m_Renderer(std::make_unique<CubePointLightRenderer>())
{
	m_Data.position = position;
	m_Data.ambient = glm::vec3(0.05f);
	m_Data.diffuse = glm::vec3(1.0f);
	m_Data.specular = glm::vec3(.2f);
	m_Data.constant = 1.0f;
	m_Data.linear = 0.0014f;
	m_Data.quadratic = 0.000007f;
}

void PointLight::Update(float deltaTime)
{
	//
}

void PointLight::Render(const Shader& shader, const RenderContext& renderContext)
{
	shader.Use();
	shader.SetMat4("projection", renderContext.projection);
	shader.SetMat4("view", renderContext.view);

	glm::mat4 newModel = glm::mat4(1.0f);
	newModel = glm::translate(newModel, m_Data.position);
	newModel = glm::scale(newModel, glm::vec3(20.f));
	shader.SetMat4("model", newModel);

	m_Renderer->Render();
}

void PointLight::SetPosition(const glm::vec3& position)
{
	m_Data.position = position;
}