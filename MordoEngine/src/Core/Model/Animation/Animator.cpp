#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace
{
	const glm::mat4 IDENTITY{ 1.0f };

	// Splits a node's local matrix into translation, rotation and scale. Done once
	// per node at construction so a clip that only animates one of the three can
	// fall back to the authored values for the others.
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

	// Keyframe lists are sorted by time, so this walks until it finds the pair
	// bracketing the requested time.
	glm::vec3 SampleVec3(const std::vector<KeyFrame<glm::vec3>>& keys,
		float time, const glm::vec3& fallback)
	{
		if (keys.empty())    return fallback;
		if (keys.size() == 1) return keys[0].value;

		for (size_t i = 0; i + 1 < keys.size(); i++)
		{
			if (time < keys[i + 1].time)
			{
				const float span = keys[i + 1].time - keys[i].time;
				const float t = span > 0.0f ? (time - keys[i].time) / span : 0.0f;
				return glm::mix(keys[i].value, keys[i + 1].value,
					glm::clamp(t, 0.0f, 1.0f));
			}
		}

		return keys.back().value;
	}

	glm::quat SampleQuat(const std::vector<KeyFrame<glm::quat>>& keys,
		float time, const glm::quat& fallback)
	{
		if (keys.empty())    return fallback;
		if (keys.size() == 1) return glm::normalize(keys[0].value);

		for (size_t i = 0; i + 1 < keys.size(); i++)
		{
			if (time < keys[i + 1].time)
			{
				const float span = keys[i + 1].time - keys[i].time;
				const float t = span > 0.0f ? (time - keys[i].time) / span : 0.0f;
				return glm::normalize(glm::slerp(keys[i].value, keys[i + 1].value,
					glm::clamp(t, 0.0f, 1.0f)));
			}
		}

		return glm::normalize(keys.back().value);
	}
}

Animator::Animator(const ModelData& data)
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

	m_Globals.assign(m_Nodes.size(), IDENTITY);
	m_BoneMatrices.assign(m_Bones.size(), IDENTITY);
	m_ChannelForNode.assign(m_Nodes.size(), -1);

	// Start on the first clip so a model that has animation is animating without
	// anyone having to ask. Call Play to pick a different one.
	if (!m_Animations.empty())
	{
		Play(size_t{ 0 }, true);
	}
	else
	{
		// No clips: resolve the rest pose once so the bone matrices are still
		// valid and a skinned mesh renders in its bind pose instead of collapsing.
		ComputePose();
	}
}

void Animator::BindChannels()
{
	m_ChannelForNode.assign(m_Nodes.size(), -1);

	if (m_Current >= m_Animations.size()) return;

	const std::vector<NodeChannel>& channels = m_Animations[m_Current].channels;
	for (size_t c = 0; c < channels.size(); c++)
	{
		const int node = channels[c].nodeIndex;
		if (node >= 0 && node < static_cast<int>(m_ChannelForNode.size()))
		{
			m_ChannelForNode[node] = static_cast<int>(c);
		}
	}
}

glm::mat4 Animator::SampleChannel(const NodeChannel& channel, const RestPose& rest) const
{
	const glm::vec3 position = SampleVec3(channel.positions, m_TimeTicks, rest.position);
	const glm::quat rotation = SampleQuat(channel.rotations, m_TimeTicks, rest.rotation);
	const glm::vec3 scale = SampleVec3(channel.scales, m_TimeTicks, rest.scale);

	return glm::translate(glm::mat4(1.0f), position)
		* glm::mat4_cast(rotation)
		* glm::scale(glm::mat4(1.0f), scale);
}

void Animator::ComputePose()
{
	// One forward pass is enough because the loader flattened the tree depth
	// first, so a parent is always at a lower index than its children.
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		glm::mat4 local = m_Nodes[i].localTransform;

		const int channel = m_ChannelForNode[i];
		if (m_Playing && channel >= 0)
		{
			local = SampleChannel(m_Animations[m_Current].channels[channel], m_Rest[i]);
		}

		const int parent = m_Nodes[i].parent;
		m_Globals[i] = parent < 0 ? local : m_Globals[parent] * local;
	}

	for (size_t b = 0; b < m_Bones.size(); b++)
	{
		const int node = m_Bones[b].nodeIndex;
		m_BoneMatrices[b] = (node >= 0 ? m_Globals[node] : IDENTITY)
			* m_Bones[b].inverseBind;
	}
}

void Animator::Play(size_t index, bool loop)
{
	if (index >= m_Animations.size()) return;

	m_Current = index;
	m_Loop = loop;
	m_TimeTicks = 0.0f;
	m_Playing = true;

	BindChannels();
	ComputePose();
}

bool Animator::Play(const std::string& name, bool loop)
{
	for (size_t i = 0; i < m_Animations.size(); i++)
	{
		if (m_Animations[i].name == name)
		{
			Play(i, loop);
			return true;
		}
	}
	return false;
}

void Animator::Update(float deltaTime)
{
	if (!m_Playing || m_Current >= m_Animations.size()) return;

	const AnimationData& animation = m_Animations[m_Current];
	m_TimeTicks += animation.ticksPerSecond * deltaTime;

	if (animation.durationTicks > 0.0f)
	{
		if (m_Loop)
		{
			m_TimeTicks = std::fmod(m_TimeTicks, animation.durationTicks);
			if (m_TimeTicks < 0.0f) m_TimeTicks += animation.durationTicks;
		}
		else if (m_TimeTicks >= animation.durationTicks)
		{
			m_TimeTicks = animation.durationTicks;
			m_Playing = false;   // hold the last pose
		}
	}

	ComputePose();
}

const glm::mat4& Animator::GetNodeGlobal(int nodeIndex) const
{
	if (nodeIndex < 0 || nodeIndex >= static_cast<int>(m_Globals.size()))
	{
		return IDENTITY;
	}
	return m_Globals[nodeIndex];
}

const std::string& Animator::GetAnimationName(size_t index) const
{
	static const std::string empty;
	return index < m_Animations.size() ? m_Animations[index].name : empty;
}

float Animator::GetTimeSeconds() const
{
	if (m_Current >= m_Animations.size()) return 0.0f;

	const float ticksPerSecond = m_Animations[m_Current].ticksPerSecond;
	return ticksPerSecond > 0.0f ? m_TimeTicks / ticksPerSecond : 0.0f;
}

float Animator::GetDurationSeconds() const
{
	if (m_Current >= m_Animations.size()) return 0.0f;

	const AnimationData& animation = m_Animations[m_Current];
	return animation.ticksPerSecond > 0.0f
		? animation.durationTicks / animation.ticksPerSecond
		: 0.0f;
}
