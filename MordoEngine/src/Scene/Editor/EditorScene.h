#pragma once
#include "../Scene.h"
#include "../../Core/EngineContext.h"
#include "./Controllers/EditorCameraController.h"
#include "./Controllers/EditorInputHandlerController.h"
#include "./System/EditorSystem.h"
#include <memory>

class EditorScene : public Scene
{
private:
	EngineContext& m_Ctx;
	std::unique_ptr<EditorCameraController> m_CameraController;
	std::unique_ptr<EditorSystem> m_EditorSystem;
	std::unique_ptr<EditorInputHandlerController> m_EditorInputController;

public:
	EditorScene(EngineContext& ctx);
	void Render() override;
	void Update(float deltaTime) override;
	void OnEntry() override {}
	~EditorScene() override;
};