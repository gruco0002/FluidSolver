//
// Created by corbi on 17.04.2019.
//

#ifndef CPPGUI_DEMO_FRAMEBUFFER_HPP
#define CPPGUI_DEMO_FRAMEBUFFER_HPP

#include <glad/glad.h>
#include "Texture2D.hpp"
#include <map>
#include <vector>

namespace Engine{
    namespace  Graphics{

class Framebuffer {


public:

    Framebuffer(uint32_t width, uint32_t height);

    ~Framebuffer();

    void Bind(bool resizeViewport = false);

    void Unbind();

    void AddAttachment(GLenum attachment, Texture2D* texture);


private:

    static uint32_t  currentlyBound;

    uint32_t ID;

    uint32_t  width;
    uint32_t  height;

    std::vector<GLenum> drawBuffers;

    std::map<GLenum, Texture2D*> attachments;

    std::vector<Texture2D*> createdInternal;

    void SetDrawBuffers();

    void CheckFramebufferStatus();

    void GenerateFramebuffer();

    void DeleteFramebuffer();



};

    }
}

#endif //CPPGUI_DEMO_FRAMEBUFFER_HPP
