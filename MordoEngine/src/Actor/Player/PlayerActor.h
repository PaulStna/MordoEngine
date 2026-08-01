#pragma once
#include "../Actor.h"

/// The player's presence in the world.
///
/// The camera is still the thing being driven: movement and mouse look move it
/// through the camera controller, untouched. This actor reads where it ended up
/// and turns it into something the rest of the world can ask about — a position
/// on the ground and a facing — so a chest or an animal never has to know that
/// a camera exists.
///
/// It is also the one that aims. Every frame it casts down the view and
/// remembers what it found, so the crosshair has something to report before
/// anything is pressed, and so using something is just calling Interact on what
/// was already there.
///
/// The dependency runs camera to player for as long as input moves the camera.
/// The day the player has to collide, be pushed, or be moved by anything else,
/// it flips: this actor becomes the one that moves and the camera follows it.
/// Only Update changes when that happens.
class PlayerActor : public Actor
{
private:
	glm::vec3 m_Forward{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_EyePosition{ 0.0f };

	// The full 3D look direction, which is what a ray needs. m_Forward is the
	// flattened one, and aiming at something above or below would miss with it.
	glm::vec3 m_AimDirection{ 0.0f, 0.0f, -1.0f };

	// Non-owning, and only valid for the frame it was written in: the actor it
	// names may be destroyed before the next one. nullptr = looking at nothing.
	Actor* m_LookedAt = nullptr;
	float  m_LookedAtDistance = 0.0f;

	// How far the player can use something from. Generous, because the world
	// scale here is large: the terrain alone is 513 posts at 4 units each.
	float m_Reach = 400.0f;

public:
	PlayerActor();

	void Update(float deltaTime, ActorContext& context) override;

	/// Facing on the ground plane, normalised and flattened. This is what "in
	/// front of the player" means for interaction and attacks.
	const glm::vec3& GetForward() const { return m_Forward; }

	/// Where the player looks from, world space.
	const glm::vec3& GetEyePosition() const { return m_EyePosition; }

	/// The full 3D look direction, normalised. Together with the eye position
	/// this is the ray everything aimed at the world starts from.
	const glm::vec3& GetAimDirection() const { return m_AimDirection; }

	/// What the crosshair is on, or null.
	///
	/// Refreshed every frame, so it is only good for the frame it was read in:
	/// an actor destroyed since would leave this dangling if it were kept.
	Actor* GetLookedAtActor() const { return m_LookedAt; }

	/// World units to whatever GetLookedAtActor returned. Meaningless when that
	/// is null.
	float GetLookedAtDistance() const { return m_LookedAtDistance; }

	/// How far the player can reach, in world units.
	float GetReach() const { return m_Reach; }
	void  SetReach(float reach) { m_Reach = reach; }
};
