#pragma once

#include "OverlayInstance.hpp"
#include "Simulator.hpp"

#include <memory>


namespace FluidStudio {

    class ParticleSelectionByTagOverlay final : public OverlayInstance {
      public:
        ParticleSelectionByTagOverlay(std::shared_ptr<LibFluid::Simulator> simulator, uint32_t particle_tag);

        Display get_display() override;
        uint32_t get_display_particle_tag() override;
        glm::mat4 get_matrix() override;
        void set_matrix(const glm::mat4& mat_4) override;
        AllowedTransforms get_allowed_transforms() override;
        const char* get_display_text() override;
        uint32_t get_particle_tag() const;

      private:
        std::shared_ptr<LibFluid::Simulator> simulator;
        uint32_t particle_tag;
        std::string display_text;

        glm::vec3 center_point = glm::vec3(0.0f);


      private:
        void calculate_center_point();
        void move_all_particles_by_difference(const glm::vec3& difference);

        void update_display_text();
    };

} // namespace FluidStudio