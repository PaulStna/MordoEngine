#pragma once
#include "Scene.h"
#include "../Core/EngineContext.h"
#include <unordered_map>
#include <memory>
#include <string>

/// Owns the scenes and decides which one is running.
///
/// Every scene is built up front and kept, rather than created on the way in
/// and destroyed on the way out, so switching modes is a pointer change and
/// nothing is rebuilt. What the scenes share — the world, the terrain, the
/// resources — belongs to the EngineContext below them and is never rebuilt
/// either. That is what lets the editor hand the game a terrain it just
/// changed.
class SceneManager
{
private:
	EngineContext& m_Context;
	std::unordered_map<std::string, std::unique_ptr<Scene>> m_Scenes;

	// Non-owning: points into m_Scenes. nullptr until a scene is made active.
	Scene* m_ActiveScene;

	std::string m_ActiveSceneName;

public:
	/// @param context Shared by every scene added here. Must outlive the
	///                manager.
	SceneManager(EngineContext& context);

	/// @param name Key used to activate it later.
	void AddScene(const std::string& name, std::unique_ptr<Scene> scene);

	void RemoveScene(const std::string& name);

	/// @return Null until something has been made active.
	Scene* GetActiveScene() const;

	/// Switches scenes and calls OnEntry on the new one, which is where a scene
	/// reclaims the state it shares with the others.
	void SetActiveScene(const std::string& name);

	/// @return Null when nothing is registered under that name.
	Scene* GetScene(const std::string& name);

	/// Forwards to the active scene.
	void Update(float deltaTime);
	void Render();

	~SceneManager();
};
