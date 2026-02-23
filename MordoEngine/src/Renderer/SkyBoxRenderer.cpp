#include "SkyBoxRenderer.h"
#include "../Core/Managers/Manager.h"
#include "../Core/Texture/Texture.h"

SkyBoxRenderer::SkyBoxRenderer(Shader& shader) : Renderer(shader)
{
	CreateGLState();
	PopulateBuffers();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SkyBoxRenderer::CreateGLState()
{
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
}

void SkyBoxRenderer::PopulateBuffers()
{
	std::vector<float> vertices;
	InitVertices(vertices);


	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(vertices[0]),
		&vertices[0],
		GL_STATIC_DRAW
	);
}

void SkyBoxRenderer::InitVertices(std::vector<float>& vertices)
{
	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	vertices.insert(vertices.end(), std::begin(skyboxVertices), std::end(skyboxVertices));
}

void SkyBoxRenderer::Render(const glm::mat4* view,
							const glm::mat4* projection,
							const glm::mat4* model)
{
	glm::mat4 skyboxView = glm::mat4(glm::mat3(*view));
	p_Shader.Use();
	Renderer::Render(&skyboxView, projection, nullptr);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glBindVertexArray(m_Vao);
	glActiveTexture(GL_TEXTURE0);
	Texture& texture = Manager<Texture>::Get("skyBox");
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture.ID);
	p_Shader.SetInt("skybox", 0);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

SkyBoxRenderer::~SkyBoxRenderer()
{
	glDeleteBuffers(1, &m_Vbo);
	glDeleteVertexArrays(1, &m_Vao);
}