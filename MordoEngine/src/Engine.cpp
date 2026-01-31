#include "Engine.h"
#include <random>
#include "API/OpenGL/OpenGLBackend.h"
#include "Input/Input.h"
#include "Renderer/Cube/CubeRenderer.h"
#include "Core/Managers/Manager.h"
#include "Core/Texture/Texture.h"
#include "Core/Shader/Shader.h"

void Engine::Run()
{
	OpenGLBackend::Init(800, 800);
	Input::Init(OpenGLBackend::GetGLFWwindow());
	Manager<Texture>::Init();
	Manager<Shader>::Init();
	CubeRenderer cubeRenderer(32);

	float lastTime = 0.0f;

	while (!OpenGLBackend::WindowShouldClose())
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		OpenGLBackend::Update();

		Input::Update();
		if (Input::KeyPressed(GLFW_KEY_ESCAPE)) {
			OpenGLBackend::CloseWindow();
		}

		cubeRenderer.Update(deltaTime);
		cubeRenderer.Render();

		OpenGLBackend::SwapBuffers();
	}


	Manager<Texture>::Clear();
	Manager<Shader>::Clear();
	OpenGLBackend::Terminate();
}