#pragma once

/// One mode the engine can be in: the game, the terrain editor, whatever comes
/// next.
///
/// A scene decides what runs and what is drawn, and owns the controllers that
/// read input for it. It does not own the world or the resources — those live
/// in the EngineContext and outlive every scene, which is what lets switching
/// modes keep the terrain that was being edited.
class Scene
{
protected:
	Scene() = default;

public:
	/// Once per frame while this scene is the active one.
	virtual void Update(float deltaTime) = 0;

	virtual void Render() = 0;

	/// Called when this scene becomes the active one. The place to claim state
	/// shared with other scenes: cursor mode, GL settings, where the camera is
	/// pointing.
	virtual void OnEntry() = 0;

	virtual ~Scene() = default;
};
