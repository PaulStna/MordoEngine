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

ActorHit ActorSystem::Raycast(const Ray& ray, float maxDistance,
	CollisionLayer mask, const Actor* ignore)
{
	ActorHit nearest;
	nearest.distance = maxDistance;

	for (const std::unique_ptr<Actor>& actor : m_Actors)
	{
		if (actor.get() == ignore) continue;
		if (!LayerMatches(actor->GetLayer(), mask)) continue;

		float distance = 0.0f;
		if (!actor->Raycast(ray, distance)) continue;

		// Strictly nearer, so the first of two at the same distance wins rather
		// than the last, and so a miss past maxDistance is dropped here.
		if (distance >= nearest.distance) continue;

		nearest.actor = actor.get();
		nearest.distance = distance;
	}

	// Distance is only meaningful alongside an actor; on a miss it is still the
	// maxDistance it started at, which is why the bool conversion asks about the
	// pointer and not about this.
	return nearest;
}

void ActorSystem::OverlapBounds(const Bounds& bounds, std::vector<Actor*>& out,
	CollisionLayer mask, const Actor* ignore)
{
	out.clear();

	for (const std::unique_ptr<Actor>& actor : m_Actors)
	{
		if (actor.get() == ignore) continue;
		if (!LayerMatches(actor->GetLayer(), mask)) continue;

		// Invalid for an actor with no collider, which is how those stay out of
		// every query without being special cased.
		const Bounds world = actor->GetWorldBounds();
		if (world.IsValid() && world.Overlaps(bounds))
		{
			out.push_back(actor.get());
		}
	}
}

void ActorSystem::Destroy(Actor& actor)
{
	actor.m_PendingDestroy = true;
}
