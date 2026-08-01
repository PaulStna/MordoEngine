#pragma once
#include "Rig.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

/// Plays one clip of a Rig: samples its keyframes at the current time, walks the
/// node hierarchy and produces the matrices the vertex shader needs.
///
/// One per body, never shared. Two characters built on the same Rig get an
/// Animator each, which is what lets one walk while the other runs without
/// either of them touching the other's pose.
///
/// It covers both kinds of animation a file can carry, because both come down
/// to the same node transforms: a skinned mesh reads GetBoneMatrices(), and a
/// rigid mesh whose node moves reads GetNodeGlobal().
///
/// Morph target animation is the one kind this does not do; that needs
/// per-vertex blending rather than node transforms, so it would be a separate
/// path.
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
	/// @param rig Must outlive the Animator. It belongs to the Model, which is
	///            shared, so this holds a reference to it rather than a copy.
	explicit Animator(const Rig& rig);

	bool   HasAnimations()     const { return m_Rig->HasAnimations(); }
	size_t GetAnimationCount() const { return m_Rig->GetAnimationCount(); }

	/// @return Empty string when the index is out of range.
	const std::string& GetAnimationName(size_t index) const;

	/// Starts a clip by name, from the beginning.
	/// @param name Clip name as it appears in the source file.
	/// @param loop When false the clip holds on its last frame instead of
	///             restarting.
	/// @return false if the rig has no clip by that name, leaving whatever is
	///         playing alone.
	bool Play(const std::string& name, bool loop = true);

	/// @param index Clip index; out of range leaves the current clip playing.
	void Play(size_t index, bool loop = true);

	/// Jumps to a point inside the current clip, wrapping around its duration.
	///
	/// Play always restarts from zero, so without this two creatures on the same
	/// clip move in perfect lockstep and read as one creature drawn twice.
	///
	/// @param seconds Offset into the clip. Wraps, so a value past the end is
	///                fine.
	void SetTime(float seconds);

	/// Advances the clock and recomputes the pose.
	/// @param deltaTime Seconds since the last frame. A clip that is not looping
	///                  stops at its last frame rather than wrapping.
	void Update(float deltaTime);

	/// One final matrix per bone, in the order the shader expects. Empty for a
	/// rig with no skin.
	const std::vector<glm::mat4>& GetBoneMatrices() const { return m_BoneMatrices; }

	/// The live transform of one node, for geometry that rides a node instead of
	/// being skinned.
	/// @param nodeIndex Index into the rig's nodes.
	/// @return Identity when the index is out of range, so a piece with no node
	///         draws where it was authored instead of collapsing to the origin.
	const glm::mat4& GetNodeGlobal(int nodeIndex) const;

	/// Where we are inside the current clip, in seconds. 0 when nothing is
	/// playing.
	float GetTimeSeconds() const;

	/// Length of the current clip in seconds. 0 when nothing is playing.
	float GetDurationSeconds() const;
};
