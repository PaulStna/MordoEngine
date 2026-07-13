#include "Engine.h"
#include "API/OpenGL/OpenGLBackend.h"
#include "Input/Input.h"
#include "Core/Managers/Manager.h"
#include "Core/Texture/Texture.h"
#include "Core/Shader/Shader.h"
#include "Core/EngineContext.h"
#include "Scene/SceneManager.h"

void Engine::Run()
{
	OpenGLBackend::Init(800, 800);
	Input::Init(OpenGLBackend::GetGLFWwindow());
	Manager<Texture>::Init();
	Manager<Shader>::Init();

	{
		EngineContext context(OpenGLBackend::SCR_WIDTH, OpenGLBackend::SCR_HEIGHT);
		SceneManager  sceneManager(context);

		float lastTime = 0.0f;
		while (!OpenGLBackend::WindowShouldClose())
		{
			static bool drawModeInLines = false;
			float currentTime = glfwGetTime();
			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			OpenGLBackend::Update();

			Input::Update();
			if (Input::KeyPressed(GLFW_KEY_ESCAPE)) OpenGLBackend::CloseWindow();
			if (Input::KeyPressed(GLFW_KEY_SPACE))  drawModeInLines = !drawModeInLines;

			glPolygonMode(GL_FRONT_AND_BACK, drawModeInLines ? GL_LINE : GL_FILL);

			sceneManager.Update(deltaTime);
			sceneManager.Render();
			OpenGLBackend::SwapBuffers();
		}
	}

	Manager<Texture>::Clear();
	Manager<Shader>::Clear();
	OpenGLBackend::Terminate();
}