#pragma once
#include "./Controllers/EditorCameraController.h"
#include "./System/EditorSystem.h"
#include "../Scene.h"
#include "../../Terrain/Terrain.h"
#include "../../Camera/Camera.h"
#include "../../Renderer/Renderer.h"
#include "./Controllers/EditorInputHandlerController.h"
#include <memory>

class EditorScene : public Scene
{
private:
	std::shared_ptr<terrain::Terrain> m_Terrain;
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<Renderer> m_Renderer;
	std::unique_ptr<EditorCameraController> m_CameraController;
	std::unique_ptr<EditorSystem> m_EditorSystem;
	std::unique_ptr<EditorInputHandlerController> m_EditorInputController;

public:
	EditorScene(std::shared_ptr<terrain::Terrain> terrain, std::shared_ptr<Camera> camera, std::shared_ptr<Renderer> renderer);
	void Render() override;
	void Update(float deltaTime) override;
	void OnEntry() override {
		//
	}
	~EditorScene() override;
};
