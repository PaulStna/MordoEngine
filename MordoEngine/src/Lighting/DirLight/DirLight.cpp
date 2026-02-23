#include "DirLight.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

DirLight::DirLight(float daySpeed) : m_DaySpeed(daySpeed)
{
	m_Data.direction = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Data.diffuse = glm::vec3(0.7f);
	m_Data.ambient = glm::vec3(0.3f);
}

void DirLight::Update(float deltaTime)
{
	m_TimeOfDay += m_DaySpeed * deltaTime;

	if (m_TimeOfDay > glm::two_pi<float>()) {
		m_TimeOfDay -= glm::two_pi<float>();
	}

	float x = 0.0f;
	float y = sin(m_TimeOfDay);
	float z = cos(m_TimeOfDay);

	m_Data.direction = -glm::normalize(glm::vec3(x, y, z));
}