#pragma once

#include "FluidInclude.hpp"
#include "FluidStudioFwd.hpp"
#include "fluidSolver/Particle.hpp"

#include <glm/gtx/hash.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace FluidUi
{

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

        void load_colors_if_required();

        std::vector<glm::vec3> colors;
        std::vector<glm::vec3> vertices;

        std::unordered_map<glm::vec3, FluidSolver::ParticleType> mapped_colors;

        void import_data_into_simulation();
    };


} // namespace FluidUi