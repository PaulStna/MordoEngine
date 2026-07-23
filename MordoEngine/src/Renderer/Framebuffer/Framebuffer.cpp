#include "Framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer(int width, int height)
    : m_Width(width), m_Height(height)
{
	CreateGLState();
}

void Framebuffer::CreateGLState()
{
    glGenFramebuffers(1, &m_Fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

    glGenTextures(1, &m_TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_TextureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer, 0);

    glGenRenderbuffers(1, &m_Rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Framebuffer::BindBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glViewport(0, 0, m_Width, m_Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Framebuffer::UnbindBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    glDeleteRenderbuffers(1, &m_Rbo);
    glDeleteTextures(1, &m_TextureColorBuffer);
	glDeleteFramebuffers(1, &m_Fbo);
}
