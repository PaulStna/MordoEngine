#pragma once
#include "../System/EditorSystem.h"
#include "../../../Terrain/System/TerrainSystem.h"

/// Turns the editor's key and mouse state into brush commands.
///
/// Stateless on purpose: it holds nothing between frames, so what the editor
/// can do is entirely the set of calls made here, and rebinding a key changes
/// one line rather than a state machine.
class EditorInputHandlerController
{
public:
	/// Reads the devices and drives the brush for this frame: painting,
	/// resizing the selector, changing strength, and saving.
	void Update(EditorSystem& editorSystem, TerrainSystem& terrainSystem);
};
