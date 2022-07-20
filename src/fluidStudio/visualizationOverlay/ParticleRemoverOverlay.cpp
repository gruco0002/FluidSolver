#include "ParticleRemoverOverlay.hpp"

namespace FluidUi {

    OverlayInstance::Display ParticleRemoverOverlay::get_display() {
        return Display::Cube;
    }

    glm::mat4 ParticleRemoverOverlay::get_matrix() {
        return glm::mat4();
    }
    void ParticleRemoverOverlay::set_matrix(const glm::mat4& mat_4) {

    }

    OverlayInstance::AllowedTransforms ParticleRemoverOverlay::get_allowed_transforms() {
        return AllowedTransforms::TranslateAndScale;
    }
} // namespace FluidUi