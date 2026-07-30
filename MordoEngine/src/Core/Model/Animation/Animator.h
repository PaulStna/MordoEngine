#pragma once
#include "../ModelData.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

// Plays one animation clip of a model: samples its keyframes at the current
// time, walks the node hierarchy and produces the matrices the vertex shader
// needs.
//
// Covers both kinds of animation a file can carry, because both come down to the
// same node transforms:
//   - a skinned mesh reads GetBoneMatrices()
//   - a rigid mesh whose node moves reads GetNodeGlobal()
//
// Morph target animation is the one kind this does not do; that needs per-vertex
// blending rather than node transforms, so it would be a separate path.
class Animator
{
private:
	// The node's own transform split into parts, so a clip that animates only
	// rotation can leave position and scale at their authored values.
	struct RestPose
	{
		glm::vec3 position{ 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f };
	};

	std::vector<NodeData>      m_Nodes;
	std::vector<RestPose>      m_Rest;
	std::vector<BoneData>      m_Bones;
	std::vector<AnimationData> m_Animations;

	std::vector<glm::mat4> m_Globals;        // one per node
	std::vector<glm::mat4> m_BoneMatrices;   // one per bone, this is what the shader gets

	// Channel index per node for the clip being played, -1 where the clip does
	// not touch that node. Rebuilt on Play so sampling is a lookup and not a
	// search through every channel.
	std::vector<int> m_ChannelForNode;

	size_t m_Current = 0;
	float  m_TimeTicks = 0.0f;
	bool   m_Loop = true;
	bool   m_Playing = false;

	void      BindChannels();
	void      ComputePose();
	glm::mat4 SampleChannel(const NodeChannel& channel, const RestPose& rest) const;

public:
	explicit Animator(const ModelData& data);

	bool   HasAnimations() const { return !m_Animations.empty(); }
	size_t GetAnimationCount() const { return m_Animations.size(); }
	const std::string& GetAnimationName(size_t index) const;

	// Returns false if the file has no clip by that name, leaving whatever is
	// playing alone.
	bool Play(const std::string& name, bool loop = true);
	void Play(size_t index, bool loop = true);

	void Update(float deltaTime);

	const std::vector<glm::mat4>& GetBoneMatrices() const { return m_BoneMatrices; }
	const glm::mat4& GetNodeGlobal(int nodeIndex) const;

	// Where we are inside the current clip, in seconds. Handy for confirming that
	// time is actually advancing.
	float GetTimeSeconds() const;
	float GetDurationSeconds() const;
};
