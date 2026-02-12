#pragma once
#include "../Scene.h"
#include "../../Terrain/Terrain.h"
#include "../../Renderer/Geomipmapping.h"
#include "../../Camera/Camera.h"
#include "../../Renderer/AreaSelectorRenderer.h"
#include <memory>

class EditorScene : public Scene
{
private:
	std::shared_ptr<Camera> m_Camera;
	std::shared_ptr<Geomipmapping> m_Renderer;
	std::unique_ptr<AreaSelectorRenderer> m_TerrainSelector;
	std::shared_ptr<terrain::Terrain> m_Terrain;
public:
	EditorScene(std::shared_ptr<terrain::Terrain> terrain, std::shared_ptr<Geomipmapping> renderer , std::shared_ptr<Camera> camera);
	void Render() override;
	void Update(float deltaTime) override;
	~EditorScene() override;
};
