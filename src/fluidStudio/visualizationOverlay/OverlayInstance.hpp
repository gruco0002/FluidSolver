#pragma once

#include <glm/glm.hpp>

namespace FluidStudio {

    class OverlayInstance {
      public:
        enum class Display {
            None,
            Cube,
            ParticleTagTint
        };


        enum class AllowedTransforms {
            Translate,
            Scale,
            TranslateAndScale,
            TranslateAndRotate
        };

      public:
        inline virtual Display get_display() {
            return Display::None;
        };

        inline virtual uint32_t get_display_particle_tag() {
            return 0;
        }

        inline virtual const char* get_display_text() {
            return nullptr;
        }

        virtual glm::mat4 get_matrix() = 0;
        virtual void set_matrix(const glm::mat4&) = 0;
        virtual AllowedTransforms get_allowed_transforms() = 0;

        virtual ~OverlayInstance() = default;
    };


} // namespace FluidStudio