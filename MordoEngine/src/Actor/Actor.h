#pragma once
#include "../Core/Transform/Transform.h"
#include <string>

class Model;
class TerrainSystem;
struct ActorContext;

// Anything in the world that has a place in it and may do something with time:
// the player, an animal, a chest, a lamp, etc...
//
// An actor never draws itself. It owns where it is, and ModelSystem keeps
// drawing every model in its single pass exactly as before. This split is not
// tidiness: World::RenderOpaque runs three times per frame (reflection,
// refraction, screen), so anything that happens there happens three times.
// Gameplay belongs in Update, which runs once.
//
// Used as-is for a prop that only stands somewhere. Derive from it when the
// behaviour is genuinely its own, and prefer a component for what repeats
// between several kinds of actor.
class Actor
{
public:
	Actor() = default;
	virtual ~Actor() = default;

	// An actor points at a model it does not own, so copying one would leave two
	// actors fighting over the same transform.
	Actor(const Actor&) = delete;
	Actor& operator=(const Actor&) = delete;

	// Once per frame, from ActorSystem::Update. The default does nothing, which
	// is the right amount of work for a prop.
	virtual void Update(float deltaTime, ActorContext& context) {}

	// Where the actor is. This is the authoritative transform: the one on the
	// model is a copy that SyncToModel refreshes.
	Transform& GetTransform() { return m_Transform; }
	const Transform& GetTransform() const { return m_Transform; }

	// The model this actor drives, or null for an actor with no visible body
	// (a trigger volume, a spawn point). 
	// The model is owned by ModelSystem and outlives the actor.
	void SetModel(Model* model) { m_Model = model; }
	Model* GetModel() const { return m_Model; }

	const std::string& GetName() const { return m_Name; }
	void SetName(std::string name) { m_Name = std::move(name); }

	void PlaceOnTerrain(const TerrainSystem& terrain, const glm::vec2& xz,
		float heightOffset = 0.0f);

	// Pushes the transform into the model. ActorSystem calls this for every
	// actor after they have all updated, so an actor that moves another one
	// still lands the right transform on screen.
	void SyncToModel();

	bool IsPendingDestroy() const { return m_PendingDestroy; }

protected:
	Transform   m_Transform;
	Model*      m_Model = nullptr;
	std::string m_Name;

private:
	friend class ActorSystem;
	bool m_PendingDestroy = false;
};
