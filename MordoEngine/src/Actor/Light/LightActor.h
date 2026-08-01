#pragma once
#include "../Actor.h"

class LightSystem;

/// A point light that lives in the world as an actor.
///
/// It covers both shapes at once, because they only differ in what you hang off
/// it: a bare light, useful while placing things by hand, with the debug cube
/// on; or a lamp, a torch, anything with a body, where giving it a model turns
/// the light from a floating cube into what the object emits.
///
/// The light itself still belongs to LightSystem, which is the one that knows
/// how to feed the shaders. This holds its index and keeps its position honest.
class LightActor : public Actor
{
private:
	// The system that owns the actual light. Must outlive this actor, which it
	// does: both belong to the world.
	LightSystem& m_Lights;
	std::size_t  m_LightIndex = 0;

	// Where the light sits inside the model, in model space. A lamp glows at
	// its head, not at its feet. Run through the transform, so it follows the
	// actor's scale and rotation on its own.
	glm::vec3 m_LocalOffset{ 0.0f };

	bool m_On = true;

public:
	/// @param lights      Registers a new light with this system and keeps its
	///                    index. Must outlive the actor.
	/// @param localOffset Where the light sits inside the model, model space, so
	///                    the actor's scale and rotation carry it. Zero puts the
	///                    light on the pivot.
	/// @param debugCube   Draws the white marker cube at the light. Wanted while
	///                    placing lights by hand, not wanted on a lamp that
	///                    already has a body.
	LightActor(LightSystem& lights, const glm::vec3& localOffset = glm::vec3(0.0f),
		bool debugCube = false);

	void Update(float deltaTime, ActorContext& context) override;

	/// Flips the switch.
	/// @return Always true: there is no state in which a lamp refuses to be a
	///         lamp, so the caller can always report that something happened.
	bool Interact(Actor& source, ActorContext& context) override;

	/// Off keeps everything, it only stops the light reaching the shaders. The
	/// body, if there is one, stays exactly where it was: an unlit lamp is still
	/// a lamp standing there.
	void SetOn(bool on);
	bool IsOn() const { return m_On; }

	/// Where the light actually ends up, world space, offset included.
	glm::vec3 GetLightPosition() const;

	/// This actor's slot in the LightSystem.
	std::size_t GetLightIndex() const { return m_LightIndex; }
};
