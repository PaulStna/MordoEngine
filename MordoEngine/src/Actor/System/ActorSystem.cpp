#include "ActorSystem.h"
#include "../ActorContext.h"
#include <algorithm>

void ActorSystem::Update(float deltaTime, ActorContext& context)
{
	// Indexed, and over the count taken before the loop: an actor may spawn
	// another one from its Update, which can reallocate the vector. Whatever
	// appears mid-frame starts updating on the next one.
	const std::size_t count = m_Actors.size();
	for (std::size_t i = 0; i < count; i++)
	{
		m_Actors[i]->Update(deltaTime, context);
	}

	// Separate pass so an actor that moves another one still gets the final
	// transform on screen, whatever order they updated in.
	for (const std::unique_ptr<Actor>& actor : m_Actors)
	{
		actor->SyncToModel();
	}

	const auto removed = std::remove_if(m_Actors.begin(), m_Actors.end(),
		[](const std::unique_ptr<Actor>& actor) { return actor->IsPendingDestroy(); });

	m_Actors.erase(removed, m_Actors.end());
}

void ActorSystem::Destroy(Actor& actor)
{
	actor.m_PendingDestroy = true;
}
