#pragma once
#include "../System/EditorSystem.h"
#include "../../../Renderer/Renderer.h"
#include "../../../Terrain/Terrain.h"

class EditorInputHandlerController
{
public:
	void Update(EditorSystem& editorSystem, terrain::Terrain& terrain, Renderer& renderer);
};
