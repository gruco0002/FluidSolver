#pragma once

#include "visualizationOverlay/OverlayInstance.hpp"

namespace FluidUi {

    class ParticleRemoverOverlay : public OverlayInstance {
      public:
        Display get_display() override;
        glm::mat4 get_matrix() override;
        void set_matrix(const glm::mat4& mat_4) override;
        AllowedTransforms get_allowed_transforms() override;
    };


}