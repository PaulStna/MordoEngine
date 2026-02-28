#include "EditorInputHandlerController.h"
#include "../../../Input/Input.h"

void EditorInputHandlerController::Update(EditorSystem& editorSystem, TerrainSystem& terrainSystem)
{
	if (Input::LeftMousePressed()) {
		editorSystem.IncreaseTerrain(terrainSystem.GetTerrain());
	}

	if (Input::RightMousePressed()) {
		editorSystem.DecreaseTerrain(terrainSystem.GetTerrain());
	}

	if (Input::KeyPressed(GLFW_KEY_J)) {
		editorSystem.IncreaseSelector();
	}

	if (Input::KeyPressed(GLFW_KEY_K)) {
		editorSystem.DecreaseSelector();
	}

	if (Input::KeyPressed(GLFW_KEY_N)) {
		editorSystem.IncreaseBrushStrenght();
	}

	if (Input::KeyPressed(GLFW_KEY_M)) {
		editorSystem.DecreaseBrushStrenght();
	}

	terrainSystem.CheckForModifications();
}