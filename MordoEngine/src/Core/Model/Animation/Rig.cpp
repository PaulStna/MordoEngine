#include "Rig.h"

namespace
{
	// Splits a node's local matrix into translation, rotation and scale. Done once
	// per node at load so a clip that only animates one of the three can fall back
	// to the authored values for the others.
	void Decompose(const glm::mat4& matrix,
		glm::vec3& position, glm::quat& rotation, glm::vec3& scale)
	{
		position = glm::vec3(matrix[3]);

		scale = { glm::length(glm::vec3(matrix[0])),
				  glm::length(glm::vec3(matrix[1])),
				  glm::length(glm::vec3(matrix[2])) };

		glm::mat3 rotationOnly = glm::mat3(matrix);
		for (int axis = 0; axis < 3; axis++)
		{
			if (scale[axis] > 1e-8f) rotationOnly[axis] /= scale[axis];
		}

		rotation = glm::normalize(glm::quat_cast(rotationOnly));
	}
}

Rig::Rig(const ModelData& data)
	: m_Nodes(data.nodes),
	m_Bones(data.bones),
	m_Animations(data.animations)
{
	m_Rest.resize(m_Nodes.size());
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		Decompose(m_Nodes[i].localTransform,
			m_Rest[i].position, m_Rest[i].rotation, m_Rest[i].scale);
	}
}

const std::string& Rig::GetAnimationName(size_t index) const
{
	static const std::string empty;
	return index < m_Animations.size() ? m_Animations[index].name : empty;
}

int Rig::FindAnimation(const std::string& name) const
{
	for (size_t i = 0; i < m_Animations.size(); i++)
	{
		if (m_Animations[i].name == name) return static_cast<int>(i);
	}
	return -1;
}
