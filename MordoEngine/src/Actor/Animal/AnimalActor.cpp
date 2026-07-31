#include "AnimalActor.h"
#include "../../Core/Model/Model.h"

AnimalActor::AnimalActor(Clips clips) : m_Clips(std::move(clips))
{
	SetName("animal");
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
	if (!m_Model || clip.empty() || !m_Model->PlayAnimation(clip))
		return false;

	m_State = state;
	return true;
}
