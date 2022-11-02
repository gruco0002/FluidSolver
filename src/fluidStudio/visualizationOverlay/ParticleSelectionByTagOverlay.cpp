#include "ParticleSelectionByTagOverlay.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <utility>

namespace FluidStudio {


    OverlayInstance::Display ParticleSelectionByTagOverlay::get_display() {
        return OverlayInstance::Display::ParticleTagTint;
    }

    uint32_t ParticleSelectionByTagOverlay::get_display_particle_tag() {
        return particle_tag;
    }

    glm::mat4 ParticleSelectionByTagOverlay::get_matrix() {
        return glm::translate(glm::mat4(1.0f), center_point);
        ;
    }

    void ParticleSelectionByTagOverlay::set_matrix(const glm::mat4& mat_4) {
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;

        glm::decompose(mat_4, scale, rotation, translation, skew, perspective);

        auto difference = translation - center_point;
        center_point = translation;

        move_all_particles_by_difference(difference);
    }

    OverlayInstance::AllowedTransforms ParticleSelectionByTagOverlay::get_allowed_transforms() {
        return AllowedTransforms::Translate;
    }

    ParticleSelectionByTagOverlay::ParticleSelectionByTagOverlay(std::shared_ptr<LibFluid::Simulator> simulator, uint32_t particle_tag) {
        this->simulator = std::move(simulator);
        this->particle_tag = particle_tag;

        calculate_center_point();
    }

    void ParticleSelectionByTagOverlay::calculate_center_point() {
        glm::vec3 center(0.0f);
        size_t counter = 0;

        auto& collection = simulator->data.collection;
        if (collection->is_type_present<LibFluid::MovementData3D>() && collection->is_type_present<LibFluid::ParticleInfo>()) {
            for (size_t i = 0; i < collection->size(); i++) {
                const auto& info = collection->get<LibFluid::ParticleInfo>(i);
                const auto& mv = collection->get<LibFluid::MovementData3D>(i);

                if (info.tag == particle_tag) {
                    center += mv.position;
                    counter += 1;
                }
            }
        }

        if (counter != 0) {
            center_point = center / (float)counter;
        }
    }

    void ParticleSelectionByTagOverlay::move_all_particles_by_difference(const glm::vec3& difference) {
        bool moved_particles = false;
        auto& collection = simulator->data.collection;
        if (collection->is_type_present<LibFluid::MovementData3D>() && collection->is_type_present<LibFluid::ParticleInfo>()) {
            for (size_t i = 0; i < collection->size(); i++) {
                const auto& info = collection->get<LibFluid::ParticleInfo>(i);
                auto& mv = collection->get<LibFluid::MovementData3D>(i);

                if (info.tag == particle_tag) {
                    mv.position += difference;
                    moved_particles = true;
                }
            }
        }

        if (moved_particles) {
            // notify the simulator that we altered the particle collection manually
            simulator->data.notify_that_data_changed();
        }
    }

    uint32_t ParticleSelectionByTagOverlay::get_particle_tag() const {
        return particle_tag;
    }

    const char* ParticleSelectionByTagOverlay::get_display_text() {
        update_display_text();

        if (display_text.empty()) {
            return nullptr;
        }
        return display_text.c_str();
    }

    void ParticleSelectionByTagOverlay::update_display_text() {
        auto descriptor = simulator->data.tag_descriptors->get_descriptor_by_tag(particle_tag);
        if (descriptor == nullptr) {
            display_text.clear();
        } else {
            if (display_text != descriptor->title) {
                display_text = descriptor->title;
            }
        }
    }
} // namespace FluidStudio