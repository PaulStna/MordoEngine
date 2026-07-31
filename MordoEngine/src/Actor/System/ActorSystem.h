#pragma once
#include "../Actor.h"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

struct ActorContext;

// Owns every actor in the scene and drives them once per frame.
//
// Actors are held by pointer for two reasons: Update is virtual, so they cannot
// live in the vector by value, and the addresses have to survive a spawn,
// because actors point at each other and at their models.
class ActorSystem
{
private:
	std::vector<std::unique_ptr<Actor>> m_Actors;

public:
	// Creates an actor of type T and hands it back. The reference is good until
	// that actor is destroyed; spawning more never moves it.
	template<typename T = Actor, typename... Args>
	T& Spawn(Args&&... args)
	{
		static_assert(std::is_base_of_v<Actor, T>, "T must derive from Actor");

		std::unique_ptr<T> actor = std::make_unique<T>(std::forward<Args>(args)...);
		T& reference = *actor;
		m_Actors.push_back(std::move(actor));
		return reference;
	}

	// Updates every actor, then syncs their transforms into their models and
	// drops whatever asked to be destroyed.
	void Update(float deltaTime, ActorContext& context);

	// Marks the actor for removal at the end of the current Update. Erasing it
	// on the spot would pull the vector out from under the loop walking it, and
	// an actor is allowed to call this on itself.
	//
	// NOTE: this drops the actor, not its model. ModelSystem has no way to
	// remove one yet, so a destroyed actor leaves its body standing where it
	// was. Fine while nothing is destroyed; needs fixing before anything dies.
	void Destroy(Actor& actor);

	bool IsEmpty() const { return m_Actors.empty(); }
	std::size_t Count() const { return m_Actors.size(); }
};
