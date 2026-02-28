#include "GameScene.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Shader/Shader.h"
#include "../../Input/Input.h"
#include "../../Renderer/SkyBoxRenderer.h"

GameScene::GameScene(std::shared_ptr<Camera> camera, std::shared_ptr<TerrainSystem> terrainSystem)
	: Scene("terrain"), m_Camera(camera), m_TerrainSystem(terrainSystem), m_LightSystem(std::make_unique<LightSystem>())
{
	m_CameraController = std::make_unique<GameCameraController>(m_Camera);
	m_SkyBoxRenderer = std::make_unique<SkyBoxRenderer>(
		Manager<Shader>::Get("skyBox")
	);
}

void GameScene::OnEntry()
{
	m_CameraController->TouchTerrain(*m_TerrainSystem);
}

void GameScene::Update(float deltaTime)
{
	float velocity = 100.0f * m_TerrainSystem->GetTerrainWorldScale() * deltaTime;
	m_CameraController->Update(deltaTime, velocity, *m_TerrainSystem);
    m_LightSystem->Update(deltaTime);
}

void GameScene::Render()
{
    glm::mat4 projection = m_Camera->GetProjectionMatrix();
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 cameraPos = m_Camera->GetPosition();

    Shader& terrainShader = Manager<Shader>::Get("terrain");
    m_LightSystem->Render(terrainShader, cameraPos, &projection, &view, &model);
    m_TerrainSystem->Render(terrainShader, cameraPos, &projection, &view, &model);
    m_SkyBoxRenderer->Render(&view, &projection, nullptr);
}

GameScene::~GameScene()
{

}