#include "FluidSolverTypes.hpp"

#include "core/fluidSolver/IISPHFluidSolver.hpp"
#include "core/fluidSolver/SESPHFluidSolver.hpp"
#include "core/fluidSolver/SESPHFluidSolver3D.hpp"
#include "core/fluidSolver/kernel/CubicSplineKernel.hpp"
#include "core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp"
#include "core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp"
#include "core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp"

#include <set>

FluidUi::FluidSolverTypes::FluidSolverTypes()
{
    add_types();
    find_possible_names();
}

void FluidUi::FluidSolverTypes::add_types()
{
    using namespace FluidSolver;
    types.push_back(
        {"SESPH", "QuadraticNeighborhoodSearchDynamicAllocated", "CubicSplineKernel",
         []() {
             return std::make_shared<
                 SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>();
         },
         [](const std::shared_ptr<IFluidSolverBase>& b) {
             return std::dynamic_pointer_cast<
                        const SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(b) !=
                    nullptr;
         },
         SolverSettingsTypeSESPH,
         [](std::shared_ptr<IFluidSolverBase> b) {
             return &std::dynamic_pointer_cast<
                         SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(b)
                         ->settings;
         }});

    types.push_back(
        {"SESPH", "HashedNeighborhoodSearch", "CubicSplineKernel",
         []() { return std::make_shared<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(); },
         [](const std::shared_ptr<IFluidSolverBase>& b) {
             return std::dynamic_pointer_cast<const SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(b) !=
                    nullptr;
         },
         SolverSettingsTypeSESPH,
         [](std::shared_ptr<IFluidSolverBase> b) {
             return &std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(b)
                         ->settings;
         }});

    types.push_back(
        {"IISPH", "QuadraticNeighborhoodSearchDynamicAllocated", "CubicSplineKernel",
         []() {
             return std::make_shared<
                 IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>();
         },
         [](const std::shared_ptr<IFluidSolverBase>& b) {
             return std::dynamic_pointer_cast<
                        const IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(b) !=
                    nullptr;
         },
         SolverSettingsTypeIISPH,
         [](std::shared_ptr<IFluidSolverBase> b) {
             return &std::dynamic_pointer_cast<
                         IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(b)
                         ->settings;
         }});

    types.push_back(
        {"IISPH", "HashedNeighborhoodSearch", "CubicSplineKernel",
         []() { return std::make_shared<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(); },
         [](const std::shared_ptr<IFluidSolverBase>& b) {
             return std::dynamic_pointer_cast<const IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(b) !=
                    nullptr;
         },
         SolverSettingsTypeIISPH,
         [](std::shared_ptr<IFluidSolverBase> b) {
             return &std::dynamic_pointer_cast<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(b)
                         ->settings;
         }});

    types.push_back(
        {"SESPH-3D", "QuadraticNeighborhoodSearch3D", "CubicSplineKernel3D",
         []() { return std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(); },
         [](const std::shared_ptr<IFluidSolverBase>& b) {
             return std::dynamic_pointer_cast<
                        const SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(b) != nullptr;
         },
         SolverSettingsTypeSESPH3D,
         [](std::shared_ptr<IFluidSolverBase> b) {
             return &std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(
                         b)
                         ->settings;
         }});

    types.push_back(
        {"SESPH-3D", "HashedNeighborhoodSearch3D", "CubicSplineKernel3D",
         []() { return std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(); },
         [](const std::shared_ptr<IFluidSolverBase>& b) {
             return std::dynamic_pointer_cast<
                        const SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(b) != nullptr;
         },
         SolverSettingsTypeSESPH3D,
         [](std::shared_ptr<IFluidSolverBase> b) {
             return &std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(b)
                         ->settings;
         }});
}

const FluidUi::FluidSolverTypes::FluidSolverType* FluidUi::FluidSolverTypes::query_type(
    const FluidUi::FluidSolverTypes::FluidSolverTypeQuery& query, QueryMainFocus focus) const
{

    const FluidUi::FluidSolverTypes::FluidSolverType* best_so_far = nullptr;
    int matched_factors = 0;

    for (auto& t : types)
    {
        if (focus != QueryMainFocus::None)
        {
            int current_matched_factors = 0;
            if (focus == QueryMainFocus::Solver)
            {
                if (t.name_solver == query.name_solver)
                {
                    current_matched_factors = 1;
                    if (t.name_neighborhood_search == query.name_neighborhood_search)
                    {
                        current_matched_factors++;
                    }
                    if (t.name_kernel == query.name_kernel)
                    {
                        current_matched_factors++;
                    }
                }
            }
            else if (focus == QueryMainFocus::NeighborhoodSearch)
            {
                if (t.name_neighborhood_search == query.name_neighborhood_search)
                {
                    current_matched_factors = 1;
                    if (t.name_solver == query.name_solver)
                    {
                        current_matched_factors++;
                    }
                    if (t.name_kernel == query.name_kernel)
                    {
                        current_matched_factors++;
                    }
                }
            }
            else if (focus == QueryMainFocus::Kernel)
            {
                if (t.name_kernel == query.name_kernel)
                {
                    current_matched_factors = 1;
                    if (t.name_neighborhood_search == query.name_neighborhood_search)
                    {
                        current_matched_factors++;
                    }
                    if (t.name_solver == query.name_solver)
                    {
                        current_matched_factors++;
                    }
                }
            }

            if (current_matched_factors > matched_factors)
            {
                matched_factors = current_matched_factors;
                best_so_far = &t;
            }
        }

        if (t.name_solver == query.name_solver)
        {
            if (t.name_neighborhood_search == query.name_neighborhood_search)
            {
                if (t.name_kernel == query.name_kernel)
                {
                    return &t;
                }
            }
        }
    }
    return best_so_far;
}

const FluidUi::FluidSolverTypes::FluidSolverType* FluidUi::FluidSolverTypes::query_type(
    const std::shared_ptr<FluidSolver::IFluidSolverBase>& query) const
{
    for (auto& t : types)
    {
        if (t.is_type(query))
        {
            return &t;
        }
    }
    return nullptr;
}

void FluidUi::FluidSolverTypes::find_possible_names()
{
    std::set<std::string> tmp_solver;
    std::set<std::string> tmp_ns;
    std::set<std::string> tmp_kernel;

    for (auto& t : types)
    {
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
