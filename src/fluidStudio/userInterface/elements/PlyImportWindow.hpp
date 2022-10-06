#pragma once

#include "FluidInclude.hpp"
#include "FluidStudioFwd.hpp"
#include "fluidSolver/Particle.hpp"

#include "userInterface/UiElement.hpp"

#include <glm/gtx/hash.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace FluidStudio {

    class PlyImportWindow final : public UiElement {
      public:
        void update() override;

        void open_window();

      private:
        bool visible = false;

        std::string current_file;

        std::string loaded_file;

        int particle_multiplier = 1;

        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> vertices;

        struct MapInformation {
            LibFluid::ParticleType particle_type = LibFluid::ParticleTypeNormal;
            LibFluid::pTag_t particle_tag = 0;
        };

        std::unordered_map<glm::vec3, MapInformation> mapped_colors;

      private:
        void load_colors_if_required();

        void import_data_into_simulation();

        void reset();

        float get_particle_mass() const;

        bool can_import() const;
    };


} // namespace FluidStudio