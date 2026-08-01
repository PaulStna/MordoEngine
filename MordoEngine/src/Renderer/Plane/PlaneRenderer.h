#pragma once
#include <glad/glad.h>

/// A unit quad spanning -1 to +1, with texture coordinates, drawn as two
/// triangles.
///
/// Deliberately geometry and nothing else: the same six vertices are a water
/// tile when a model matrix places them in the world, and the fullscreen quad
/// the underwater post-process samples the scene through when the vertex shader
/// passes them straight out. What it becomes is the shader's business, not
/// this class's.
///
/// Owns GL names, so it is neither copyable nor movable.
class PlaneRenderer
{
private:
	GLuint m_Vao;
	GLuint m_Vbo;
	GLuint m_Ebo;
	void CreateGLState();
	void PopulateBuffers();

public:
	PlaneRenderer();

	/// Draws the quad. The caller binds the shader and sets every uniform,
	/// including whatever matrix decides where the quad ends up.
	void Render();

	~PlaneRenderer();
};
