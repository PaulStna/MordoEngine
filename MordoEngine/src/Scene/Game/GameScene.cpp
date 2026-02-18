#include "GameScene.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Shader/Shader.h"
#include "../../Input/Input.h"

GameScene::GameScene(std::shared_ptr<terrain::Terrain> terrain, std::shared_ptr<Camera> camera, std::shared_ptr<Renderer> renderer) 
	: Scene("terrain"), m_Terrain(terrain) , m_Camera(camera) , m_Renderer(renderer) , m_Sun(std::make_unique<Sun>(0.9f))
{
	m_CameraController = std::make_unique<GameCameraController>(m_Camera);
}

void GameScene::OnEntry()
{
	m_CameraController->TouchTerrain(*m_Terrain);
}

void GameScene::Update(float deltaTime)
{
	m_Sun->Update(deltaTime);
	float velocity = 100.0f * m_Terrain->GetWorldScale() * deltaTime;
	m_CameraController->Update(deltaTime, velocity, *m_Terrain);
}

void GameScene::Render()
{
	glm::mat4 projection = m_Camera->GetProjectionMatrix();
	glm::mat4 view = m_Camera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 lightDir = m_Sun->GetReverseLightDirection();
	m_Renderer->Render(
		m_Camera->GetPosition(),
		&view,
		&projection,
		&model,
		&lightDir
	);
}

GameScene::~GameScene()
{
	
}