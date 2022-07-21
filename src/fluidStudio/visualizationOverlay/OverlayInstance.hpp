#pragma once

#include <glm/glm.hpp>

namespace FluidStudio {

    class OverlayInstance {
      public:
        enum class Display {
            None,
            Cube
        };


        enum class AllowedTransforms {
            Translate,
            Scale,
            TranslateAndScale
        };

      public:
        inline virtual Display get_display() {
            return Display::None;
        };
        virtual glm::mat4 get_matrix() = 0;
        virtual void set_matrix(const glm::mat4&) = 0;
        virtual AllowedTransforms get_allowed_transforms() = 0;

        virtual ~OverlayInstance() = default;
    };


} // namespace FluidUi