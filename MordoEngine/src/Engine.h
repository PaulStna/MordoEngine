#pragma once

/// The entry point of the engine: opens the window, builds the EngineContext
/// and the scenes, and runs the frame loop until the window closes.
///
/// Everything else is reached from what Run builds, so this is the one place
/// that knows the order the engine comes up in.
class Engine
{
public:
	/// Runs until the window is closed, then tears down. Returns once.
	static void Run();
};
