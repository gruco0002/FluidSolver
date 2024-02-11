#pragma once

#include <glad/glad.h>

#include "Texture2D.hpp"

#include <map>
#include <vector>

namespace Engine
{
    namespace Graphics
    {

        class Framebuffer
        {
          public:
            Framebuffer(uint32_t width, uint32_t height);

            ~Framebuffer();

            void Bind(bool resizeViewport = false);

            void Unbind();

            void AddAttachment(GLenum attachment, Texture2D *texture);

          private:
            static uint32_t currentlyBound;

            uint32_t ID;

            uint32_t width;
            uint32_t height;

            std::vector<GLenum> drawBuffers;

            std::map<GLenum, Texture2D *> attachments;

            std::vector<Texture2D *> createdInternal;

            void SetDrawBuffers();

            void CheckFramebufferStatus();

            void GenerateFramebuffer();

            void DeleteFramebuffer();
        };

    } // namespace Graphics
} // namespace Engine
