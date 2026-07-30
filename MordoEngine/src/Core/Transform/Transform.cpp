#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::mat4& Transform::GetMatrix() const
{
	if (m_Dirty)
	{
		const glm::vec3 radians = glm::radians(m_Rotation);

		m_Matrix = glm::translate(glm::mat4(1.0f), m_Position);
		m_Matrix = glm::rotate(m_Matrix, radians.y, glm::vec3(0.0f, 1.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, radians.x, glm::vec3(1.0f, 0.0f, 0.0f));
		m_Matrix = glm::rotate(m_Matrix, radians.z, glm::vec3(0.0f, 0.0f, 1.0f));
		m_Matrix = glm::scale(m_Matrix, m_Scale);

		m_Dirty = false;
	}

	return m_Matrix;
}
