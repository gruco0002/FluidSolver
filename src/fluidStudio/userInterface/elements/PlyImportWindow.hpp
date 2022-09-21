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
        float particle_size = 0.1f;
        float rest_density = 1000.0f;

        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> vertices;

        std::unordered_map<glm::vec3, LibFluid::ParticleType> mapped_colors;

      private:
        void load_colors_if_required();

        void import_data_into_simulation();

        void reset();
    };


} // namespace FluidStudio