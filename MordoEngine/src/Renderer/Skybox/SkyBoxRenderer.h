#pragma once
#include <glad/glad.h>

/// The cube the skybox cubemap is sampled through.
///
/// Position only, with no texture coordinates: a cubemap is looked up by
/// direction, and for a cube centred on the camera the vertex position already
/// is that direction. Drawing it is one bind and one call, with the shader
/// doing the work of keeping it centred and pushing it to the far plane.
///
/// Owns GL names, so it is neither copyable nor movable.
class SkyBoxRenderer
{
private:
	GLuint m_Vao;
	GLuint m_Vbo;
	void CreateGLState();
	void PopulateBuffers();

public:
	SkyBoxRenderer();

	/// Draws the cube. The caller binds the shader and the cubemap first, and
	/// is responsible for the depth state the sky needs.
	void Render();

	~SkyBoxRenderer();
};
