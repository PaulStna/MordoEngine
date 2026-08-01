#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/// The window and the GL context: creating them, swapping buffers, and closing
/// down.
///
/// Along with the renderers this is the only code allowed to touch the graphics
/// API directly. Everything else asks the engine for a frame and never learns
/// which windowing library is underneath.
class OpenGLBackend
{
public:
	// Current framebuffer size in pixels. Updated by the resize callback, so
	// anything that caches a projection or an offscreen buffer has to re-read
	// these rather than keep its own copy.
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	/// Polls window events and refreshes the input state for this frame.
	static void Update();

	static void Terminate();

	/// Creates the window and loads the GL function pointers. Nothing that
	/// touches GL may run before this.
	/// @param width  Initial window width in pixels.
	/// @param height Initial window height in pixels.
	static void Init(int width, int height);

	/// The raw window, for the one thing that needs it: handing it to Input.
	static GLFWwindow* GetGLFWwindow();

	static bool WindowShouldClose();

	/// Asks the loop to end. The frame in progress still finishes.
	static void CloseWindow();

	static void SwapBuffers();

private:
	OpenGLBackend();
	static GLFWwindow* window;
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};
