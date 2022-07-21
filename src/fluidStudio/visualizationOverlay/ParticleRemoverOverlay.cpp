#include "ParticleRemoverOverlay.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace FluidUi {

    OverlayInstance::Display ParticleRemoverOverlay::get_display() {
        return Display::Cube;
    }

    glm::mat4 ParticleRemoverOverlay::get_matrix() {
        auto scale = glm::scale(glm::mat4(1.0f), entity->parameters.volume.distance_from_center);
        auto translation = glm::translate(glm::mat4(1.0f), entity->parameters.volume.center);
        return translation * scale;
    }

    void ParticleRemoverOverlay::set_matrix(const glm::mat4& new_data) {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(new_data, scale, rotation, translation, skew, perspective);

        entity->parameters.volume.center = translation;

        scale.x = abs(scale.x);
        scale.y = abs(scale.y);
        scale.z = abs(scale.z);

        entity->parameters.volume.distance_from_center = scale;
    }

    OverlayInstance::AllowedTransforms ParticleRemoverOverlay::get_allowed_transforms() {
        return AllowedTransforms::TranslateAndScale;
    }
    ParticleRemoverOverlay::ParticleRemoverOverlay(std::shared_ptr<FluidSolver::ParticleRemover3D> entity) {
        this->entity = entity;
    }
} // namespace FluidUi