#include "AnimalActor.h"
#include "../../Core/Model/Model.h"

AnimalActor::AnimalActor(Clips clips) : m_Clips(std::move(clips))
{
	SetName("animal");

	SetLayer(CollisionLayer::Creature);
	m_Interactable = true;
}

bool AnimalActor::Interact(Actor& source, ActorContext& context)
{
	switch (m_State)
	{
	case State::Idle:    return SetState(State::Walking);
	case State::Walking: return SetState(State::Running);
	default:             return SetState(State::Idle);
	}
}

const std::string& AnimalActor::ClipFor(State state) const
{
	switch (state)
	{
	case State::Walking: return m_Clips.walk;
	case State::Running: return m_Clips.run;
	default:             return m_Clips.idle;
	}
}

bool AnimalActor::SetState(State state)
{
	const std::string& clip = ClipFor(state);
	if (clip.empty() || !m_Body.PlayAnimation(clip))
		return false;

	m_State = state;
	return true;
}
