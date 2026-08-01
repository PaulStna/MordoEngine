#pragma once

class TerrainSystem;
class Camera;
class ActorSystem;

// What the player asked for this frame, as plain data.
//
// Deliberately not the Input class itself: that header pulls in GLFW, and an
// animal deciding what to do has no business compiling against a window. The
// scene reads the device and fills this in, which also means a replay or a test
// can hand over the same struct with nobody noticing.
struct ActorInput
{
	// Edge, not held: true only on the frame the button went down. An actor that
	// reacts to this can assume one press is one event.
	bool interact = false;
};

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
	const ActorInput&    input;     // what was pressed this frame
};
