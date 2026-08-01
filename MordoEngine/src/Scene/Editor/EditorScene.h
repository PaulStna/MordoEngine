#pragma once
#include "../Scene.h"
#include "../../Core/EngineContext.h"
#include "./Controllers/EditorCameraController.h"
#include "./Controllers/EditorInputHandlerController.h"
#include "./System/EditorSystem.h"
#include <memory>

/// Editing the terrain: a free-flying camera, a brush that raises and lowers
/// the ground, and a selector drawn on the surface to show where it will land.
///
/// It works on the same terrain the game scene walks on, because both borrow it
/// from the EngineContext. An edit made here is standing there when the mode is
/// switched back, with nothing saved or reloaded in between.
class EditorScene : public Scene
{
private:
	EngineContext& m_Context;
	std::unique_ptr<EditorCameraController> m_CameraController;
	std::unique_ptr<EditorSystem> m_EditorSystem;
	std::unique_ptr<EditorInputHandlerController> m_EditorInputController;

public:
	/// @param context Shared with the game scene, including the terrain being
	///                edited.
	EditorScene(EngineContext& context);

	void Render() override;
	void Update(float deltaTime) override;

	/// Nothing to reclaim: the editor camera flies from wherever it is, so
	/// entering this scene needs no fixing up.
	void OnEntry() override {}

	~EditorScene() override;
};
