#pragma once
#include "Rig.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

// Plays one clip of a Rig: samples its keyframes at the current time, walks the
// node hierarchy and produces the matrices the vertex shader needs.
//
// One per body, never shared. Two foxes built on the same Rig get an Animator
// each, which is what lets one walk while the other runs without either of them
// touching the other's pose.
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
	// Non-owning. The Rig belongs to the Model, which is shared and outlives
	// every body built on it.
	const Rig* m_Rig = nullptr;

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
	glm::mat4 SampleChannel(const NodeChannel& channel, const Rig::RestPose& rest) const;

public:
	explicit Animator(const Rig& rig);

	bool   HasAnimations()     const { return m_Rig->HasAnimations(); }
	size_t GetAnimationCount() const { return m_Rig->GetAnimationCount(); }
	const std::string& GetAnimationName(size_t index) const;

	// Returns false if the rig has no clip by that name, leaving whatever is
	// playing alone.
	bool Play(const std::string& name, bool loop = true);
	void Play(size_t index, bool loop = true);

	// Jumps to a point inside the current clip, wrapping around its duration.
	// Play always restarts from zero, so without this two animals on the same
	// clip move in perfect lockstep and read as one animal drawn twice.
	void SetTime(float seconds);

	void Update(float deltaTime);

	const std::vector<glm::mat4>& GetBoneMatrices() const { return m_BoneMatrices; }
	const glm::mat4& GetNodeGlobal(int nodeIndex) const;

	// Where we are inside the current clip, in seconds. Handy for confirming that
	// time is actually advancing.
	float GetTimeSeconds() const;
	float GetDurationSeconds() const;
};
