#include "PlaneRenderer.h"

PlaneRenderer::PlaneRenderer()
{
	CreateGLState();
    PopulateBuffers();
    glBindVertexArray(0);
}

void PlaneRenderer::CreateGLState()
{
    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void PlaneRenderer::PopulateBuffers()
{
    float vertices[] = {
        // X      Y      Z       U     V
         5.0f, -0.5f,  5.0f,   1.0f, 1.0f,
        -5.0f, -0.5f,  5.0f,   0.0f, 1.0f,
        -5.0f, -0.5f, -5.0f,   0.0f, 0.0f,
         5.0f, -0.5f,  5.0f,   1.0f, 1.0f,
        -5.0f, -0.5f, -5.0f,   0.0f, 0.0f,
         5.0f, -0.5f, -5.0f,   1.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void PlaneRenderer::Render()
{
    glBindVertexArray(m_Vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

PlaneRenderer::~PlaneRenderer()
{
    glDeleteBuffers(1, &m_Vbo);
    glDeleteVertexArrays(1, &m_Vao);
}