#pragma once

class TerrainSystem;
class Camera;
class ActorSystem;

// What an actor is allowed to reach for while it updates, handed to it instead
// of letting it know about World. Same idea as RenderContext, one layer up:
// everything an actor needs to act, and nothing it needs to draw.
//
// Built fresh every frame in World::Update, so it holds references and has no
// default constructor on purpose.
struct ActorContext
{
	const TerrainSystem& terrain;   // ground height under an actor
	const Camera&        camera;    // where the player is looking
	ActorSystem&         actors;    // spawning, queries, damage
};
