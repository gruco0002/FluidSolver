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
    using namespace FluidSolver;
    types.push_back({
                            "SESPH",
                            "QuadraticNeighborhoodSearchDynamicAllocated",
                            "CubicSplineKernel",
                            []() {

                                return new SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>();
                            },
                            [](const IFluidSolverBase *b) {
                                return dynamic_cast<const SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated> *>(b) !=
                                       nullptr;
                            },
                            SolverSettingsTypeSESPH,
                            [](IFluidSolverBase *b) {
                                return &dynamic_cast<SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated> *>(b)->settings;
                            }
                    });

    types.push_back({
                            "SESPH",
                            "HashedNeighborhoodSearch",
                            "CubicSplineKernel",
                            []() {
                                return new SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>();
                            },
                            [](const IFluidSolverBase *b) {
                                return dynamic_cast<const SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch> *>(b) !=
                                       nullptr;
                            },
                            SolverSettingsTypeSESPH,
                            [](IFluidSolverBase *b) {
                                return &dynamic_cast<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch> *>(b)->settings;
                            }
                    });

    types.push_back({
                            "IISPH",
                            "QuadraticNeighborhoodSearchDynamicAllocated",
                            "CubicSplineKernel",
                            []() {
                                return new IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>();
                            },
                            [](const IFluidSolverBase *b) {
                                return dynamic_cast<const IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated> *>(b) !=
                                       nullptr;
                            },
                            SolverSettingsTypeIISPH,
                            [](IFluidSolverBase *b) {
                                return &dynamic_cast<IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated> *>(b)->settings;
                            }
                    });

    types.push_back({
                            "IISPH",
                            "HashedNeighborhoodSearch",
                            "CubicSplineKernel",
                            []() {
                                return new IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>();
                            },
                            [](const IFluidSolverBase *b) {
                                return dynamic_cast<const IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch> *>(b) !=
                                       nullptr;
                            },
                            SolverSettingsTypeIISPH,
                            [](IFluidSolverBase *b) {
                                return &dynamic_cast<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch> *>(b)->settings;
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