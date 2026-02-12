#include "Input.h"

GLFWwindow* Input::window = nullptr;

bool Input::keyDown[372]{};
bool Input::keyPressed[372]{};
bool Input::keyDownLastFrame[372]{};

double Input::mouseX = 0.0;
double Input::mouseY = 0.0;
double Input::mouseDeltaX = 0.0;
double Input::mouseDeltaY = 0.0;

bool Input::leftMouseDown = false;
bool Input::leftMousePressed = false;
bool Input::leftMouseDownLastFrame = false;

bool Input::cursorVisible = false;

void Input::Init(GLFWwindow* win) {
	window = win;

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	mouseX = x;
	mouseY = y;

}

void Input::Update() {
	// Keyboard
	for (int i = 32; i < 349; i++) {
		keyDown[i] = glfwGetKey(window, i) == GLFW_PRESS;
		keyPressed[i] = keyDown[i] && !keyDownLastFrame[i];
		keyDownLastFrame[i] = keyDown[i];
	}

	// Mouse position
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	mouseDeltaX = x - mouseX;
	mouseDeltaY = mouseY - y;

	mouseX = x;
	mouseY = y;

	// Left mouse
	leftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	leftMousePressed = leftMouseDown && !leftMouseDownLastFrame;
	leftMouseDownLastFrame = leftMouseDown;

	cursorVisible = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

bool Input::KeyDown(int key) {
	return keyDown[key];
}

bool Input::KeyPressed(int key) {
	return keyPressed[key];
}

float Input::MouseX() {
	return static_cast<float>(mouseX);
}

float Input::MouseY() {
	return static_cast<float>(mouseY);
}

float Input::MouseDeltaX() {
	return static_cast<float>(mouseDeltaX);
}

float Input::MouseDeltaY() {
	return static_cast<float>(mouseDeltaY);
}

bool Input::LeftMouseDown() {
	return leftMouseDown;
}

bool Input::LeftMousePressed() {
	return leftMousePressed;
}

void Input::DisableCursor() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::ShowCursor() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool Input::CursorVisible() {
	return cursorVisible;
}
