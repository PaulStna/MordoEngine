#pragma once
#include "Animation/Animator.h"
#include <memory>
#include <string>

class Model;

// What an actor looks like: the shared Model it borrows its shape from, plus the
// animation state that is nobody else's.
//
// This is the half that cannot be shared. Two foxes point at one Model - one set
// of GPU buffers, one copy of the textures and of every keyframe - and carry a
// Body each, so one can walk while the other runs and moving one leaves the
// other where it was.
//
// Empty by default, which is the right state for an actor with no visible body:
// a trigger volume, a spawn point, the player.
class Body
{
public:
	Body() = default;

	// Owns an animator, so it moves with its actor rather than being copied
	// alongside it.
	Body(const Body&) = delete;
	Body& operator=(const Body&) = delete;
	Body(Body&&) = default;
	Body& operator=(Body&&) = default;

	// The model is non-owning: models live in the ResourceLibrary and outlive
	// every body built on them. Builds the animator when that model has a rig,
	// so a body is ready to play the moment it has a model. Pass null to clear.
	void SetModel(Model* model);

	Model* GetModel() const { return m_Model; }
	bool   IsEmpty()  const { return m_Model == nullptr; }

	// Advances the animation, if there is one. Cheap no-op otherwise.
	void Update(float deltaTime);

	// Returns false if the model has no clip by that name, or no rig at all,
	// leaving whatever is playing alone.
	bool PlayAnimation(const std::string& name, bool loop = true);

	// Where this body starts inside its clip. Same model, same clip, different
	// offset is what keeps two of the same animal from moving as one.
	void SetAnimationTime(float seconds);

	bool IsAnimated() const { return m_Animator != nullptr; }

	// Null for a body with no animation. ModelSystem hands it straight to the
	// model at draw time.
	const Animator* GetAnimator() const { return m_Animator.get(); }

private:
	Model* m_Model = nullptr;

	// Null until a model with a rig is set. One per body, never shared.
	std::unique_ptr<Animator> m_Animator;
};
