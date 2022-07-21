#include "Framebuffer.hpp"

#include <stdexcept>

uint32_t Engine::Graphics::Framebuffer::currentlyBound = 0;

Engine::Graphics::Framebuffer::Framebuffer(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
    GenerateFramebuffer();
}

Engine::Graphics::Framebuffer::~Framebuffer()
{
    DeleteFramebuffer();
}

void Engine::Graphics::Framebuffer::Bind(bool resizeViewport)
{

    if (currentlyBound == ID)
        return;

    if (resizeViewport)
    {
        glViewport(0, 0, width, height);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, ID);
    currentlyBound = ID;
}

void Engine::Graphics::Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    currentlyBound = 0;
}

void Engine::Graphics::Framebuffer::AddAttachment(GLenum attachment, Engine::Graphics::Texture2D* texture)
{
    Bind();

    // check if existing, if yes throw error
    if (attachments.find(attachment) != attachments.end())
        throw std::invalid_argument("Framebuffer Exception: Attachment already exists!");

    // check size
    if (this->width != texture->getWidth() || this->height != texture->getHeight())
        throw std::invalid_argument("Framebuffer Exception: Texture does not have the same size as the framebuffer!");

    // add to attachments
    attachments[attachment] = texture;
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->GetID(), 0);

    if (attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT15)
        drawBuffers.push_back(attachment);

    SetDrawBuffers();
    CheckFramebufferStatus();
}

void Engine::Graphics::Framebuffer::SetDrawBuffers()
{
    Bind();
    glDrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void Engine::Graphics::Framebuffer::CheckFramebufferStatus()
{
    Bind();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::invalid_argument("Framebuffer Exception: Framebuffer is not complete!");
}

void Engine::Graphics::Framebuffer::GenerateFramebuffer()
{
    glGenFramebuffers(1, &ID);
}

void Engine::Graphics::Framebuffer::DeleteFramebuffer()
{
    Unbind();
    for (auto tex : createdInternal)
    {
        delete tex;
    }
    glDeleteFramebuffers(1, &ID);
}
