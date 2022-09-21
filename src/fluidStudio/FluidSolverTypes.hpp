#pragma once

#include "fluidSolver/IFluidSolver.hpp"

#include <functional>
#include <memory>
#include <string>
#include <tuple>

namespace FluidStudio {
    class FluidSolverTypes {
      public:
        enum SolverSettingsType {
            SolverSettingsTypeIISPH,
            SolverSettingsTypeSESPH,
            SolverSettingsTypeSESPH3D,
            SolverSettingsTypeIISPH3D
        };

        struct FluidSolverType {
            std::string name_solver;
            std::string name_neighborhood_search;
            std::string name_kernel;

            std::function<std::shared_ptr<LibFluid::IFluidSolverBase>()> create_type;

            std::function<bool(const std::shared_ptr<LibFluid::IFluidSolverBase>&)> is_type;

            SolverSettingsType settings_type;
            std::function<void*(std::shared_ptr<LibFluid::IFluidSolverBase>)> get_settings;
        };

        struct FluidSolverTypeQuery {
            std::string name_solver;
            std::string name_neighborhood_search;
            std::string name_kernel;
        };


        std::vector<FluidSolverType> types;
        std::vector<std::string> names_solver;
        std::vector<std::string> names_neighborhood_search;
        std::vector<std::string> names_kernel;

        FluidSolverTypes();

        enum class QueryMainFocus {
            None,
            Solver,
            NeighborhoodSearch,
            Kernel
        };


        const FluidSolverType* query_type(const FluidSolverTypeQuery& query,
                QueryMainFocus focus = QueryMainFocus::None) const;

        const FluidSolverType* query_type(const std::shared_ptr<LibFluid::IFluidSolverBase>& query) const;


      private:
        void add_types();

        void find_possible_names();
    };

} // namespace FluidStudio
