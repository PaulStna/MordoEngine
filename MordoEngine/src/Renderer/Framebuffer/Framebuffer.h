#pragma once
#include <glad/glad.h>

/// An offscreen target: a colour texture plus a depth and stencil renderbuffer,
/// for a pass whose result is read back as a texture rather than shown.
///
/// This is what the water reflection and refraction passes draw into, and what
/// the underwater post-process samples.
///
/// Binding also sets the viewport to this buffer's size and clears it, so
/// preparing a target is one call and no caller ever manages a viewport itself.
/// That matters because these are rarely the same size as the window, and a
/// missed viewport shows up as a pass rendered into a corner.
class Framebuffer
{
private:
	GLuint m_Fbo;
	GLuint m_TextureColorBuffer;
	GLuint m_Rbo;
	int m_Width;
	int m_Height;
	void CreateGLState();

public:
	/// @param width  Target width in pixels. Independent of the window, so a
	///               pass can be rendered at a lower resolution than the screen.
	/// @param height Target height in pixels.
	Framebuffer(int width = 800, int height = 800);

	/// Makes this the draw target, sets the viewport to its size and clears
	/// colour and depth.
	void BindBuffer();

	/// Returns drawing to the default framebuffer. Does not restore the previous
	/// viewport, so whoever draws to the screen next sets its own.
	void UnbindBuffer();

	/// The colour attachment, to be sampled by a later pass. Valid for the life
	/// of the Framebuffer.
	GLuint GetTextureID() const { return m_TextureColorBuffer; }

	~Framebuffer();
};
