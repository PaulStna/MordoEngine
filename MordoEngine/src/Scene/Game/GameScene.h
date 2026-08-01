#pragma once
#include "../Scene.h"
#include "../../Core/EngineContext.h"
#include "./Controllers/GameCameraController.h"
#include <memory>

/// Playing the game: the camera walks the terrain, the world updates, and a
/// click interacts with whatever is under the crosshair.
///
/// It reads the input devices and turns them into an ActorInput, which is the
/// only form the actor layer sees. That is the boundary that keeps GLFW out of
/// everything below.
class GameScene : public Scene
{
private:
	EngineContext& m_Context;
	std::unique_ptr<GameCameraController> m_CameraController;

public:
	/// @param context Shared with every other scene; the world it holds is not
	///                rebuilt when scenes switch.
	GameScene(EngineContext& context);

	void Render() override;
	void Update(float deltaTime) override;

	/// Snaps the camera back onto the ground.
	///
	/// Needed because the editor flies freely and may have left it anywhere,
	/// including under the terrain, and the two scenes share one camera.
	void OnEntry() override;

	~GameScene() override;
};
