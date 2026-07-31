#pragma once
#include "../Core/Transform/Transform.h"
#include "../Core/Model/Body.h"
#include <string>

class Model;
class TerrainSystem;
struct ActorContext;

// Anything in the world that has a place in it and may do something with time:
// the player, an animal, a chest, a lamp, etc...
//
// An actor never draws itself. It owns where it is and what it looks like, and
// ActorSystem walks the list once per pass handing both to ModelSystem. This
// split is not tidiness: World::RenderOpaque runs three times per frame
// (reflection, refraction, screen), so anything that happens there happens three
// times. Gameplay belongs in Update, which runs once.
//
// Used as-is for a prop that only stands somewhere. Derive from it when the
// behaviour is genuinely its own, and prefer a component for what repeats
// between several kinds of actor.
class Actor
{
public:
	Actor() = default;
	virtual ~Actor() = default;

	// An actor owns a body, and two actors sharing one would fight over its
	// animation.
	Actor(const Actor&) = delete;
	Actor& operator=(const Actor&) = delete;

	// Once per frame, from ActorSystem::Update. The default does nothing, which
	// is the right amount of work for a prop.
	virtual void Update(float deltaTime, ActorContext& context) {}

	// Where the actor is, and the only copy of it: the body carries the pose,
	// not the placement, so there is nothing to keep in step with this.
	Transform& GetTransform() { return m_Transform; }
	const Transform& GetTransform() const { return m_Transform; }

	// Gives the actor a body built on that model, or clears it with null. The
	// model is shared and outlives the actor; the body it builds is the actor's
	// own, which is what lets two actors share one model and still differ.
	void SetModel(Model* model) { m_Body.SetModel(model); }

	// The actor's own animation state. Empty for an actor with no visible body
	// (a trigger volume, a spawn point).
	Body& GetBody() { return m_Body; }
	const Body& GetBody() const { return m_Body; }

	const std::string& GetName() const { return m_Name; }
	void SetName(std::string name) { m_Name = std::move(name); }

	void PlaceOnTerrain(const TerrainSystem& terrain, const glm::vec2& xz,
		float heightOffset = 0.0f);

	bool IsPendingDestroy() const { return m_PendingDestroy; }

protected:
	Transform   m_Transform;
	Body        m_Body;
	std::string m_Name;

private:
	friend class ActorSystem;
	bool m_PendingDestroy = false;
};
