#pragma once
#include "../Actor.h"

class LightSystem;

// A point light that lives in the world as an actor.
//
// Covers both shapes at once, because they only differ in what you hang off it:
//   - a bare light, useful while placing things by hand, with the debug cube on
//   - a lamp, a torch, anything with a body: give it a model and the light
//     stops being a floating cube and becomes what the object emits
//
// The light itself still belongs to LightSystem, which is the one that knows
// how to feed the shaders. This holds its index and keeps its position honest.
class LightActor : public Actor
{
private:
	LightSystem& m_Lights;
	std::size_t  m_LightIndex = 0;

	// Where the light sits inside the model, in model space. A lamp glows at
	// its head, not at its feet. Run through the transform, so it follows the
	// actor's scale and rotation on its own.
	glm::vec3 m_LocalOffset{ 0.0f };

	bool m_On = true;

public:
	LightActor(LightSystem& lights, const glm::vec3& localOffset = glm::vec3(0.0f),
		bool debugCube = false);

	void Update(float deltaTime, ActorContext& context) override;

	// Flips the switch. Always succeeds, so it always reports true: there is no
	// state in which a lamp refuses to be a lamp.
	bool Interact(Actor& source, ActorContext& context) override;

	// Off keeps everything, it only stops the light reaching the shaders. The
	// body, if there is one, stays exactly where it was: an unlit lamp is still
	// a lamp standing there.
	void SetOn(bool on);
	bool IsOn() const { return m_On; }

	// Where the light actually ends up, offset included.
	glm::vec3 GetLightPosition() const;

	std::size_t GetLightIndex() const { return m_LightIndex; }
};
