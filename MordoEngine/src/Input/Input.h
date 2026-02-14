#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Input {
public:
	static void Init(GLFWwindow* window);
	static void Update();

	static bool KeyDown(int key);
	static bool KeyPressed(int key);

	static float MouseX();
	static float MouseY();
	static float MouseDeltaX();
	static float MouseDeltaY();

	static bool LeftMouseDown();
	static bool LeftMousePressed();

	static void DisableCursor();
	static void ShowCursor();
	static bool CursorVisible();

private:
	static GLFWwindow* window;

	static bool keyDown[372];
	static bool keyPressed[372];
	static bool keyDownLastFrame[372];

	static double mouseX;
	static double mouseY;
	static double mouseDeltaX;
	static double mouseDeltaY;

	static bool leftMouseDown;
	static bool leftMousePressed;
	static bool leftMouseDownLastFrame;

	static bool cursorVisible;
};