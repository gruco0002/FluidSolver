#ifndef FLUIDSOLVER_FLUIDSOLVERTYPES_HPP
#define FLUIDSOLVER_FLUIDSOLVERTYPES_HPP

#include <string>
#include <functional>
#include <tuple>
#include "core/fluidSolver/IFluidSolver.hpp"

namespace FluidUi {
    class FluidSolverTypes {
    public:
        enum SolverSettingsType {
            SolverSettingsTypeIISPH,
            SolverSettingsTypeSESPH
        };

        struct FluidSolverType {

            std::string name_solver;
            std::string name_neighborhood_search;
            std::string name_kernel;

            std::function<FluidSolver::IFluidSolverBase *()> create_type;

            std::function<bool(const FluidSolver::IFluidSolverBase *)> is_type;

            SolverSettingsType settings_type;
            std::function<void *(FluidSolver::IFluidSolverBase *)> get_settings;

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


        const FluidSolverType *query_type(const FluidSolverTypeQuery &query) const;


    private:

        void add_types();

        void find_possible_names();


    };

}


#endif //FLUIDSOLVER_FLUIDSOLVERTYPES_HPP
