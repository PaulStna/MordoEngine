#include "EditorInputHandlerController.h"
#include "../../../Input/Input.h"

void EditorInputHandlerController::Update(EditorSystem& editorSystem, terrain::Terrain& terrain, Renderer& renderer)
{
	if (Input::LeftMousePressed()) {
		editorSystem.IncreaseTerrain(terrain);
	}

	if (Input::RightMousePressed()) {
		editorSystem.DecreaseTerrain(terrain);
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

	if (terrain.HasModifications()) {
		renderer.UpdateBuffers(terrain);
		terrain.ClearModifications();
	}
}