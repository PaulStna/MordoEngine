#pragma once
#include "../Actor.h"
#include <memory>
#include <type_traits>
#include "../../Core/Shader/Shader.h"
#include "../../Renderer/RenderContext.h"
#include "../../Core/Model/System/ModelSystem.h"
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

	// Updates every actor, advances their animations and drops whatever asked to
	// be destroyed.
	void Update(float deltaTime, ActorContext& context);

	// Draws every actor that has a body. This is the only list: an actor that is
	// gone is not drawn, because nothing else holds a copy of what it looked
	// like.
	void Render(ModelSystem& modelSystem,
		const Shader& shader, const RenderContext& renderContext);

	// Marks the actor for removal at the end of the current Update. Erasing it
	// on the spot would pull the vector out from under the loop walking it, and
	// an actor is allowed to call this on itself. The body goes with it; the
	// model it borrowed stays in the ResourceLibrary for whoever else wants it.
	void Destroy(Actor& actor);

	bool IsEmpty() const { return m_Actors.empty(); }
	std::size_t Count() const { return m_Actors.size(); }
};
