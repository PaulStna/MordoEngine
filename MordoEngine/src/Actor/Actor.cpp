#include "Actor.h"
#include "../Core/Model/Model.h"
#include "../Physics/BoxCollider.h"
#include "../Terrain/System/TerrainSystem.h"
#include <glm/gtc/matrix_inverse.hpp>

void Actor::PlaceOnTerrain(const TerrainSystem& terrain, const glm::vec2& xz,
	float heightOffset)
{
	m_Transform.SetPosition({ xz.x,
		terrain.GetTerrainInterpolatedHeightAt(xz.x, xz.y, heightOffset),
		xz.y });
}

void Actor::SetModel(Model* model)
{
	m_Body.SetModel(model);

	if (model && model->GetLocalBounds().IsValid())
	{
		m_Collider = std::make_unique<BoxCollider>(model->GetLocalBounds());
	}
	else
	{
		// Nothing to fit to. Left without a collider rather than with a
		// guessed one, so an actor that cannot be hit says so honestly.
		m_Collider.reset();
	}
}

void Actor::SetCollider(std::unique_ptr<Collider> collider)
{
	m_Collider = std::move(collider);
}

Bounds Actor::GetWorldBounds() const
{
	if (!m_Collider) return Bounds::Empty();

	return m_Collider->GetLocalBounds().Transformed(m_Transform.GetMatrix());
}

bool Actor::Raycast(const Ray& worldRay, float& distance) const
{
	if (!m_Collider) return false;

	// The ray goes to the shape rather than the shape coming out to the ray.
	// One inverse, and rotation and scale are handled exactly instead of by
	// refitting a box that would only get looser the more the actor is turned.
	const Ray localRay = worldRay.Transformed(
		glm::inverse(m_Transform.GetMatrix()));

	return m_Collider->Raycast(localRay, distance);
}
