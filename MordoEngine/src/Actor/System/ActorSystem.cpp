#include "ActorSystem.h"
#include "../ActorContext.h"
#include "../../Core/Model/Model.h"
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

	// Animation after every actor has had its say, so a clip picked this frame
	// is the one that advances, whatever order the actors updated in.
	for (const std::unique_ptr<Actor>& actor : m_Actors)
	{
		actor->GetBody().Update(deltaTime);
	}

	const auto removed = std::remove_if(m_Actors.begin(), m_Actors.end(),
		[](const std::unique_ptr<Actor>& actor) { return actor->IsPendingDestroy(); });

	m_Actors.erase(removed, m_Actors.end());
}

void ActorSystem::Render(ModelSystem& modelSystem,
	const Shader& shader, const RenderContext& renderContext)
{
	modelSystem.BeginPass(shader, renderContext);

	for (const std::unique_ptr<Actor>& actor : m_Actors)
	{
		modelSystem.Render(actor->GetBody(), actor->GetTransform(), shader);
	}
}

void ActorSystem::Destroy(Actor& actor)
{
	actor.m_PendingDestroy = true;
}
