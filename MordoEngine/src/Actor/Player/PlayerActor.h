#pragma once
#include "../Actor.h"

// The player's presence in the world.
//
// The camera is still the thing being driven: WASD and the mouse move it
// through GameCameraController, untouched. This actor reads where it ended up
// and turns it into something the rest of the world can ask about, a position
// on the ground and a facing, so a chest or an animal never has to know that a
// camera exists.
//
// The dependency runs camera -> player for as long as input moves the camera.
// The day the player has to collide, be pushed, or be moved by anything else,
// it flips: this actor becomes the one that moves and the camera follows it.
// Only Update changes when that happens.
class PlayerActor : public Actor
{
private:
	glm::vec3 m_Forward{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_EyePosition{ 0.0f };

public:
	PlayerActor();

	void Update(float deltaTime, ActorContext& context) override;

	// Facing on the ground plane, normalised. This is what "in front of the
	// player" means for interaction and attacks.
	const glm::vec3& GetForward() const { return m_Forward; }

	// Where the player looks from
	const glm::vec3& GetEyePosition() const { return m_EyePosition; }
};
