#include "LightActor.h"
#include "../ActorContext.h"
#include "../../Lighting/LightSystem.h"

LightActor::LightActor(LightSystem& lights, const glm::vec3& localOffset, bool debugCube)
	: m_Lights(lights), m_LocalOffset(localOffset)
{
	SetName("light");

	// Something you can walk up to and switch. The layer is what a query filters
	// on; the flag is what stops Interact being offered on things that would
	// only refuse.
	SetLayer(CollisionLayer::Interactable);
	m_Interactable = true;

	// The position is meaningless until the actor is placed;
	// The first Update puts the light where it belongs.
	m_LightIndex = m_Lights.AddPointLight(PointLight(glm::vec3(0.0f), debugCube));
}

void LightActor::SetOn(bool on)
{
	m_On = on;
	m_Lights.SetPointLightEnabled(m_LightIndex, m_On);
}

bool LightActor::Interact(Actor& source, ActorContext& context)
{
	SetOn(!m_On);
	return true;
}

glm::vec3 LightActor::GetLightPosition() const
{
	return glm::vec3(m_Transform.GetMatrix() * glm::vec4(m_LocalOffset, 1.0f));
}

void LightActor::Update(float deltaTime, ActorContext& context)
{
	m_Lights.SetPointLightPosition(m_LightIndex, GetLightPosition());
}
