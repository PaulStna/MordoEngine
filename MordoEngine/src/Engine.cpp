#include "Engine.h"
#include "API/OpenGL/OpenGLBackend.h"
#include "Input/Input.h"
#include "Scene/Scene.h"
#include "Scene/GameScene.h"
#include "Core/Managers/Manager.h"
#include "Core/Texture/Texture.h"
#include "Core/Shader/Shader.h"
#include <memory>

void Engine::Run()
{
	OpenGLBackend::Init(800, 800);
	Input::Init(OpenGLBackend::GetGLFWwindow());
	Manager<Texture>::Init();
	Manager<Shader>::Init();
	std::unique_ptr<Scene> gameScene = std::make_unique<GameScene>();

	float lastTime = 0.0f;

	while (!OpenGLBackend::WindowShouldClose())
	{
		static bool drawModeInLines = false;
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		OpenGLBackend::Update();

		Input::Update();
		if (Input::KeyPressed(GLFW_KEY_ESCAPE)) {
			OpenGLBackend::CloseWindow();
		}

		if (Input::KeyPressed(GLFW_KEY_SPACE)) {
			drawModeInLines = !drawModeInLines;
			if (drawModeInLines) { 
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		gameScene->Update(deltaTime);
		gameScene->Render();

		OpenGLBackend::SwapBuffers();
	}


	Manager<Texture>::Clear();
	Manager<Shader>::Clear();
	OpenGLBackend::Terminate();
}