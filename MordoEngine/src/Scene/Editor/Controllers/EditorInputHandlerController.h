#pragma once
#include "../System/EditorSystem.h"
#include "../../../Terrain/System/TerrainSystem.h"

class EditorInputHandlerController
{
public:
	void Update(EditorSystem& editorSystem, TerrainSystem& terrainSystem);
};
