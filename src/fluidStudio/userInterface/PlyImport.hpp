#pragma once

#include "FluidInclude.hpp"
#include "FluidStudioFwd.hpp"
#include "fluidSolver/Particle.hpp"

#include <glm/gtx/hash.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace FluidStudio {

    class PlyImport {
      public:
        void render();

        void show();

        void set_window(FluidSolverWindow* window);

        explicit PlyImport(FluidSolverWindow* window);

      private:
        FluidSolverWindow* window;
        bool visible = false;

        std::string current_file;

        std::string loaded_file;

        int scale = 1;

        void load_colors_if_required();

        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> vertices;

        std::unordered_map<glm::vec3, LibFluid::ParticleType> mapped_colors;

        void import_data_into_simulation();
    };


} // namespace FluidUi