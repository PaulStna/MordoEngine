#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/// The keyboard and mouse, sampled once per frame.
///
/// The distinction that matters is Down against Pressed. Down is the state —
/// true for as long as the key is held, which is what movement wants. Pressed
/// is the edge — true only on the frame it went down, which is what a toggle or
/// an interaction wants. Update is what makes the difference exist, by
/// remembering last frame.
///
/// This is the one place GLFW is read. Everything downstream is handed plain
/// data, so nothing below a scene compiles against a window.
class Input
{
public:
	/// @param window Must stay open for as long as anything polls this.
	static void Init(GLFWwindow* window);

	/// Samples the devices and rolls this frame's state into last frame's. Call
	/// once per frame, before anything reads it.
	static void Update();

	/// True for as long as the key is held.
	/// @param key GLFW key code.
	static bool KeyDown(int key);

	/// True only on the frame the key went down.
	static bool KeyPressed(int key);

	static float MouseX();
	static float MouseY();

	/// Movement since the last Update, in pixels. What mouse look reads.
	static float MouseDeltaX();
	static float MouseDeltaY();

	static bool LeftMouseDown();

	/// True only on the frame the button went down, so one click is one event.
	static bool LeftMousePressed();

	static bool RightMouseDown();
	static bool RightMousePressed();

	/// Hides the cursor and locks it to the window, which is what mouse look
	/// needs.
	static void DisableCursor();
	static void ShowCursor();
	static bool CursorVisible();

private:
	static GLFWwindow* window;

	// Sized past GLFW's highest key code so any code it reports can be indexed
	// without a bounds check on the hot path.
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

	static bool rightMouseDown;
	static bool rightMousePressed;
	static bool rightMouseDownLastFrame;

	static bool cursorVisible;
};
