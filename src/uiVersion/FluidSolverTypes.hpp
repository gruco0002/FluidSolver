#ifndef FLUIDSOLVER_FLUIDSOLVERTYPES_HPP
#define FLUIDSOLVER_FLUIDSOLVERTYPES_HPP

#include "core/fluidSolver/IFluidSolver.hpp"

#include <functional>
#include <memory>
#include <string>
#include <tuple>

namespace FluidUi
{
    class FluidSolverTypes {
      public:
        enum SolverSettingsType
        {
            SolverSettingsTypeIISPH,
            SolverSettingsTypeSESPH,
            SolverSettingsTypeSESPH3D
        };

        struct FluidSolverType
        {

            std::string name_solver;
            std::string name_neighborhood_search;
            std::string name_kernel;

            std::function<std::shared_ptr<FluidSolver::IFluidSolverBase>()> create_type;

            std::function<bool(const std::shared_ptr<FluidSolver::IFluidSolverBase>&)> is_type;

            SolverSettingsType settings_type;
            std::function<void*(std::shared_ptr<FluidSolver::IFluidSolverBase>)> get_settings;
        };

        struct FluidSolverTypeQuery
        {
            std::string name_solver;
            std::string name_neighborhood_search;
            std::string name_kernel;
        };


        std::vector<FluidSolverType> types;
        std::vector<std::string> names_solver;
        std::vector<std::string> names_neighborhood_search;
        std::vector<std::string> names_kernel;

        FluidSolverTypes();

        enum class QueryMainFocus
        {
            None,
            Solver,
            NeighborhoodSearch,
            Kernel
        };


        const FluidSolverType* query_type(const FluidSolverTypeQuery& query,
                                          QueryMainFocus focus = QueryMainFocus::None) const;

        const FluidSolverType* query_type(const std::shared_ptr<FluidSolver::IFluidSolverBase>& query) const;


      private:
        void add_types();

        void find_possible_names();
    };

} // namespace FluidUi


#endif // FLUIDSOLVER_FLUIDSOLVERTYPES_HPP
