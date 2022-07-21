#pragma once

#include "entities/ParticleRemover3D.hpp"
#include "visualizationOverlay/OverlayInstance.hpp"

namespace FluidStudio {

    class ParticleRemoverOverlay : public OverlayInstance {
      public:

        explicit ParticleRemoverOverlay(std::shared_ptr<LibFluid::ParticleRemover3D> entity);

        std::shared_ptr<LibFluid::ParticleRemover3D> entity = nullptr;

        Display get_display() override;
        glm::mat4 get_matrix() override;
        void set_matrix(const glm::mat4& mat_4) override;
        AllowedTransforms get_allowed_transforms() override;
    };


} // namespace FluidUi