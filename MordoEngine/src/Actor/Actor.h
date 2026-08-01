#pragma once
#include "../Core/Transform/Transform.h"
#include "../Core/Model/Body.h"
#include "../Core/Math/Bounds.h"
#include "../Core/Math/Ray.h"
#include "../Physics/Collider.h"
#include "../Physics/CollisionLayer.h"
#include <memory>
#include <string>

class Model;
class TerrainSystem;
struct ActorContext;

/// Anything in the world that has a place in it and may do something with time:
/// the player, an animal, a chest, a lamp.
///
/// An actor never draws itself. It owns where it is and what it looks like, and
/// ActorSystem walks the list once per pass handing both to ModelSystem. This
/// split is not tidiness: a render path runs once per pass, three times a frame
/// as things stand, so anything done there is done three times. Gameplay
/// belongs in Update, which runs once.
///
/// Used as-is for a prop that only stands somewhere. Derive from it when the
/// behaviour is genuinely its own, and prefer a component for what repeats
/// between several kinds of actor.
class Actor
{
public:
	Actor() = default;
	virtual ~Actor() = default;

	// An actor owns a body and a collider, and two actors sharing either would
	// fight over it.
	Actor(const Actor&) = delete;
	Actor& operator=(const Actor&) = delete;

	/// Once per frame, from ActorSystem::Update.
	/// @param deltaTime Seconds since the last frame.
	/// @param context   Everything the actor may reach for while it acts.
	///                  The default does nothing, which is the right amount of
	///                  work for a prop.
	virtual void Update(float deltaTime, ActorContext& context) {}

	/// Somebody used this.
	///
	/// The verb is deliberately vague, because what "use" means belongs to the
	/// type being used and to nobody else. A lamp toggles, a chest opens, an
	/// animal reacts. Whoever pressed the button never learns which.
	///
	/// @param source Who did it, so an actor can tell the player apart from
	///               another actor and answer differently. Nothing is assumed
	///               about it.
	/// @return Whether anything actually happened, which is what the caller
	///         needs in order to decide about feedback: a sound, a prompt, a
	///         refusal. The default refuses everything.
	virtual bool Interact(Actor& source, ActorContext& context) { return false; }

	/// Whether this is worth offering at all.
	///
	/// Kept apart from Interact so a crosshair can light up without anything
	/// being used, and so a query can skip whatever would refuse.
	bool IsInteractable() const { return m_Interactable; }

	/// Where the actor is, and the only copy of it: the body carries the pose,
	/// not the placement, so there is nothing to keep in step with this.
	Transform& GetTransform() { return m_Transform; }
	const Transform& GetTransform() const { return m_Transform; }

	/// Gives the actor a body built on that model.
	///
	/// Also fits a box collider to the model's own bounds, since that is the
	/// right answer nearly always. Call SetCollider afterwards to override it,
	/// and note that order: another SetModel would fit a box over the top again.
	///
	/// @param model Shared and outliving the actor; the body it builds is the
	///              actor's own, which is what lets two actors share one model
	///              and still differ. Null clears both body and collider.
	void SetModel(Model* model);

	/// The actor's own animation state. Empty for an actor with no visible body,
	/// such as a trigger volume or a spawn point.
	Body& GetBody() { return m_Body; }
	const Body& GetBody() const { return m_Body; }

	/// The shape used to hit this actor, in local space.
	/// @param collider Null means nothing can hit it: no raycast finds it and no
	///                 overlap reports it.
	void SetCollider(std::unique_ptr<Collider> collider);

	const Collider* GetCollider() const { return m_Collider.get(); }
	bool HasCollider() const { return m_Collider != nullptr; }

	/// The collider's box brought out into the world, refitted to stay axis
	/// aligned. This is the cheap test: whatever survives it goes to Raycast.
	/// @return Invalid bounds when there is no collider.
	Bounds GetWorldBounds() const;

	/// Exact hit against this actor.
	///
	/// Takes a world space ray and brings it into local space itself, so callers
	/// never deal with the transform.
	///
	/// @param distance Written only on a hit, in world units.
	/// @return false when the ray misses, and always when there is no collider.
	bool Raycast(const Ray& worldRay, float& distance) const;

	CollisionLayer GetLayer() const { return m_Layer; }
	void SetLayer(CollisionLayer layer) { m_Layer = layer; }

	const std::string& GetName() const { return m_Name; }
	void SetName(std::string name) { m_Name = std::move(name); }

	/// Drops the actor onto the ground.
	/// @param xz           World space X and Z. Y is taken from the terrain.
	/// @param heightOffset Added above the ground, for a model whose pivot is
	///                     not at its base.
	void PlaceOnTerrain(const TerrainSystem& terrain, const glm::vec2& xz,
		float heightOffset = 0.0f);

	bool IsPendingDestroy() const { return m_PendingDestroy; }

protected:
	Transform                 m_Transform;
	Body                      m_Body;

	// Owned, and polymorphic: null is the normal state for anything that cannot
	// be hit.
	std::unique_ptr<Collider> m_Collider;

	std::string               m_Name;

	// Set by whichever type actually answers Interact, so the two cannot drift
	// apart the way an overridable predicate would.
	bool m_Interactable = false;

	CollisionLayer m_Layer = CollisionLayer::Default;

private:
	friend class ActorSystem;
	bool m_PendingDestroy = false;
};
