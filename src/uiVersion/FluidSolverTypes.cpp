#include "FluidSolverTypes.hpp"

#include "core/fluidSolver/SESPHFluidSolver.hpp"
#include "core/fluidSolver/IISPHFluidSolver.hpp"

#include "core/fluidSolver/kernel/CubicSplineKernel.hpp"

#include "core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp"
#include "core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp"

#include <set>

FluidUi::FluidSolverTypes::FluidSolverTypes() {
    add_types();
    find_possible_names();
}

void FluidUi::FluidSolverTypes::add_types() {
    types.push_back({
                            "SESPH",
                            "QuadraticNeighborhoodSearchDynamicAllocated",
                            "CubicSplineKernel",
                            []() {
                                using namespace FluidSolver;
                                return new SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>();
                            }
                    });

    types.push_back({
                            "SESPH",
                            "HashedNeighborhoodSearch",
                            "CubicSplineKernel",
                            []() {
                                using namespace FluidSolver;
                                return new SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>();
                            }
                    });

    types.push_back({
                            "IISPH",
                            "QuadraticNeighborhoodSearchDynamicAllocated",
                            "CubicSplineKernel",
                            []() {
                                using namespace FluidSolver;
                                return new IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>();
                            }
                    });

    types.push_back({
                            "IISPH",
                            "HashedNeighborhoodSearch",
                            "CubicSplineKernel",
                            []() {
                                using namespace FluidSolver;
                                return new IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>();
                            }
                    });
}

const FluidUi::FluidSolverTypes::FluidSolverType *
FluidUi::FluidSolverTypes::query_type(const FluidUi::FluidSolverTypes::FluidSolverTypeQuery &query) const {
    for (auto &t: types) {
        if (t.name_solver == query.name_solver) {
            if (t.name_neighborhood_search == query.name_neighborhood_search) {
                if (t.name_kernel == query.name_kernel) {
                    return &t;
                }
            }
        }
    }
    return nullptr;
}

void FluidUi::FluidSolverTypes::find_possible_names() {
    std::set<std::string> tmp_solver;
    std::set<std::string> tmp_ns;
    std::set<std::string> tmp_kernel;

    for (auto &t: types) {
        tmp_solver.insert(t.name_solver);
        tmp_ns.insert(t.name_neighborhood_search);
        tmp_kernel.insert(t.name_kernel);
    }

    names_solver.clear();
    names_solver.reserve(tmp_solver.size());
    names_solver.insert(names_solver.end(), tmp_solver.begin(), tmp_solver.end());

    names_neighborhood_search.clear();
    names_neighborhood_search.reserve(tmp_ns.size());
    names_neighborhood_search.insert(names_neighborhood_search.end(), tmp_ns.begin(), tmp_ns.end());


    names_kernel.clear();
    names_kernel.reserve(tmp_kernel.size());
    names_kernel.insert(names_kernel.end(), tmp_kernel.begin(), tmp_kernel.end());

}
