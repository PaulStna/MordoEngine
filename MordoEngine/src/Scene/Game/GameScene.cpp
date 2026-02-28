#include "GameScene.h"
#include "../../Core/Managers/Manager.h"
#include "../../Core/Shader/Shader.h"
#include "../../Input/Input.h"

GameScene::GameScene(std::shared_ptr<Camera> camera, std::shared_ptr<TerrainSystem> terrainSystem)
	: m_Camera(camera),
	  m_TerrainSystem(terrainSystem),
	  m_CameraController(std::make_unique<GameCameraController>(m_Camera)),
	  m_SkySystem(std::make_unique<SkySystem>()),
	  m_LightSystem(std::make_unique<LightSystem>()),
	  m_TerrainShaderID("terrain"),
	  m_CubeLightShaderID("lightCube"),
	  m_SkyShaderID("skyBox"),
	  m_SkyTextureID("skyBox")
{
	glm::vec3 centerTerrainPosition = m_TerrainSystem->GetMiddleTerrainPosition();
	float yOffset = 0.2f;
	float scale = m_TerrainSystem->GetTerrainWorldScale();

	auto AddLight = [&](float offsetX, float offsetZ)
		{
			float x = centerTerrainPosition.x + offsetX * scale;
			float z = centerTerrainPosition.z + offsetZ * scale;
			float y = m_TerrainSystem->GetTerrainInterpolatedHeightAt(x, z, yOffset);
			m_LightSystem->AddPointLight(PointLight(glm::vec3(x, y, z)));
		};

	AddLight(0.0f, 0.0f);
	AddLight(50.0f, 50.0f);
	AddLight(-100.0f, -100.0f);
	AddLight(-100.0f, -100.0f);
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
	m_SkySystem->Update(deltaTime);
}

void GameScene::Render()
{
	glm::mat4 projection = m_Camera->GetProjectionMatrix();
	glm::mat4 view = m_Camera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 cameraPos = m_Camera->GetPosition();

	Shader& terrainShader = Manager<Shader>::Get(m_TerrainShaderID);
	Shader& cubeLightShader = Manager<Shader>::Get(m_CubeLightShaderID);
	m_LightSystem->Render(terrainShader, cubeLightShader, cameraPos, &projection, &view, &model);
	m_TerrainSystem->Render(terrainShader, cameraPos, &projection, &view, &model);

	Shader& skyShader = Manager<Shader>::Get(m_SkyShaderID);
	Texture& skyTexture = Manager<Texture>::Get(m_SkyTextureID);
	m_SkySystem->Render(skyShader, skyTexture, &projection, &view, &model);
}

GameScene::~GameScene()
{

}