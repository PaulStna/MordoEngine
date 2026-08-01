#include "PlayerActor.h"
#include "../ActorContext.h"
#include "../System/ActorSystem.h"
#include "../../Camera/Camera.h"
#include "../../Terrain/System/TerrainSystem.h"

PlayerActor::PlayerActor()
{
	SetName("player");

	// The player is a creature, not scenery, so a query looking for things to
	// hit can leave it out by mask rather than by name.
	SetLayer(CollisionLayer::Creature);
}

void PlayerActor::Update(float deltaTime, ActorContext& context)
{
	const Camera& camera = context.camera;

	m_EyePosition = camera.GetPosition();
	m_Forward = camera.GetForwardXZ();
	m_AimDirection = glm::normalize(camera.GetForward());

	const float groundY = context.terrain.GetTerrainInterpolatedHeightAt(
		m_EyePosition.x, m_EyePosition.z);

	m_Transform.SetPosition({ m_EyePosition.x, groundY, m_EyePosition.z });
	m_Transform.SetYaw(glm::degrees(camera.GetYaw()));

	// The cursor is hidden in the game scene and the camera turns with the
	// mouse, so the crosshair is the middle of the screen and the aim ray is
	// just the view. No unprojecting a cursor position into the world.
	const Ray aim(m_EyePosition, m_AimDirection);

	// Every frame, not only when the button is down, so whatever draws a prompt
	// knows what is under the crosshair before anything is pressed. Ignoring
	// ourselves matters the day the player gets a collider of its own.
	const ActorHit hit = context.actors.Raycast(aim, m_Reach,
		CollisionLayer::All, this);

	m_LookedAt = hit.actor;
	m_LookedAtDistance = hit.distance;

	if (context.input.interact && m_LookedAt && m_LookedAt->IsInteractable())
	{
		m_LookedAt->Interact(*this, context);
	}
}
