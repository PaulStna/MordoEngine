#pragma once
#include <glad/glad.h>

/// The white marker cube drawn at a point light's position.
///
/// A placement aid, not lighting: it shows where a light is while one is being
/// positioned by hand. A light attached to something with a body turns it off,
/// since the body is already visible at that spot.
///
/// Owns GL names, so it is neither copyable nor movable.
class CubePointLightRenderer
{
private:
	GLuint m_Vao;
	GLuint m_Vbo;
	void CreateGLState();
	void PopulateBuffers();

public:
	CubePointLightRenderer();

	/// Draws the cube at the origin of whatever model matrix is currently set,
	/// so the caller places it.
	void Render();

	~CubePointLightRenderer();
};
