#pragma once
#include "../ModelData.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

/// The skeleton of a model and the clips authored on it, exactly as they came
/// out of the file.
///
/// Shared and never modified: every copy of a model reads the same Rig, because
/// none of this changes while the game runs. What does change — where you are
/// inside a clip, the matrices that come out of it — lives in Animator, one per
/// body.
///
/// Keeping the two apart is what makes a second copy of a character cost a few
/// kilobytes of pose instead of a second copy of every keyframe in the file.
class Rig
{
public:
	/// The node's own transform split into parts, so a clip that animates only
	/// rotation can leave position and scale at their authored values.
	struct RestPose
	{
		glm::vec3 position{ 0.0f };
		glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 scale{ 1.0f };
	};

	/// @param data Loaded model data. Only the animation half is kept, and it is
	///             copied, so the ModelData may be discarded afterwards.
	explicit Rig(const ModelData& data);

	const std::vector<NodeData>&      GetNodes()      const { return m_Nodes; }
	const std::vector<RestPose>&      GetRestPoses()  const { return m_Rest; }
	const std::vector<BoneData>&      GetBones()      const { return m_Bones; }
	const std::vector<AnimationData>& GetAnimations() const { return m_Animations; }

	bool   HasAnimations()     const { return !m_Animations.empty(); }
	size_t GetAnimationCount() const { return m_Animations.size(); }

	/// @param index 0 to GetAnimationCount() - 1.
	/// @return The clip's name, or an empty string when the index is out of
	///         range. Never throws, so a probe for what a file contains is safe.
	const std::string& GetAnimationName(size_t index) const;

	/// Looks a clip up by name.
	///
	/// Here rather than in Animator so the question can be asked of a model
	/// without having a player around.
	///
	/// @return Index of the clip, or -1 when the rig has none by that name.
	int FindAnimation(const std::string& name) const;

private:
	std::vector<NodeData>      m_Nodes;
	std::vector<RestPose>      m_Rest;
	std::vector<BoneData>      m_Bones;
	std::vector<AnimationData> m_Animations;
};
