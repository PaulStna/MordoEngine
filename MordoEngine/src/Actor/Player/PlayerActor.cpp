#include "PlayerActor.h"
#include "../ActorContext.h"
#include "../../Camera/Camera.h"
#include "../../Terrain/System/TerrainSystem.h"

PlayerActor::PlayerActor()
{
	SetName("player");
}

void PlayerActor::Update(float deltaTime, ActorContext& context)
{
	const Camera& camera = context.camera;

	m_EyePosition = camera.GetPosition();
	m_Forward = camera.GetForwardXZ();

	const float groundY = context.terrain.GetTerrainInterpolatedHeightAt(
		m_EyePosition.x, m_EyePosition.z);

	m_Transform.SetPosition({ m_EyePosition.x, groundY, m_EyePosition.z });
	m_Transform.SetYaw(glm::degrees(camera.GetYaw()));
}
