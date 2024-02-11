#include "SolverSerializer.hpp"

#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp"
#include "fluidSolver/solver/IISPHFluidSolver.hpp"
#include "fluidSolver/solver/IISPHFluidSolver3D.hpp"
#include "fluidSolver/solver/SESPHFluidSolver.hpp"
#include "fluidSolver/solver/SESPHFluidSolver3D.hpp"
#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"

namespace LibFluid::Serialization
{

    nlohmann::json SolverSerializer::serialize(std::shared_ptr<IFluidSolverBase> solver)
    {
        nlohmann::json node;

        // check for different solver types
        if (auto casted = std::dynamic_pointer_cast<
                SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(solver))
        {
            node["type"] = "sesph";
            node["neighborhood-search"]["type"] = "quadratic-dynamic-allocated";
            node["kernel"]["type"] = "cubic-spline-kernel";

            serialize_sesph_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(solver))
        {
            node["type"] = "sesph";
            node["neighborhood-search"]["type"] = "hashed";
            node["kernel"]["type"] = "cubic-spline-kernel";

            serialize_sesph_settings(node, casted->settings);
        }
        else if (auto casted = std::dynamic_pointer_cast<
                     IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(solver))
        {
            node["type"] = "iisph";
            node["neighborhood-search"]["type"] = "quadratic-dynamic-allocated";
            node["kernel"]["type"] = "cubic-spline-kernel";

            serialize_iisph_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(solver))
        {
            node["type"] = "iisph";
            node["neighborhood-search"]["type"] = "hashed";
            node["kernel"]["type"] = "cubic-spline-kernel";

            serialize_iisph_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(
                         solver))
        {
            node["type"] = "sesph-3d";
            node["neighborhood-search"]["type"] = "quadratic-dynamic-allocated-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            serialize_sesph_3d_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(
                         solver))
        {
            node["type"] = "sesph-3d";
            node["neighborhood-search"]["type"] = "hashed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            serialize_sesph_3d_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(
                         solver))
        {
            node["type"] = "sesph-3d";
            node["neighborhood-search"]["type"] = "compressed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            serialize_sesph_3d_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(
                         solver))
        {
            node["type"] = "iisph-3d";
            node["neighborhood-search"]["type"] = "quadratic-dynamic-allocated-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            serialize_iisph_3d_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(
                         solver))
        {
            node["type"] = "iisph-3d";
            node["neighborhood-search"]["type"] = "hashed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            serialize_iisph_3d_settings(node, casted->settings);
        }
        else if (auto casted =
                     std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(
                         solver))
        {
            node["type"] = "iisph-3d";
            node["neighborhood-search"]["type"] = "compressed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            serialize_iisph_3d_settings(node, casted->settings);
        }
        else
        {
            context().add_issue("Encountered unhandled solver, neighborhood search, kernel combination!");
        }

        return node;
    }

    void SolverSerializer::serialize_sesph_settings(nlohmann::json &node, const SESPHSettings &settings)
    {
        node["stiffness"] = settings.StiffnessK;
        node["viscosity"] = settings.Viscosity;
    }

    void SolverSerializer::serialize_sesph_3d_settings(nlohmann::json &node, const SESPHSettings3D &settings)
    {
        node["stiffness"] = settings.StiffnessK;
        node["viscosity"] = settings.Viscosity;

        node["single-layer-boundary-enabled"] = settings.single_layer_boundary;
        if (settings.single_layer_boundary)
        {
            node["single-layer-settings"]["gamma-1"] = settings.single_layer_boundary_gamma_1;
            node["single-layer-settings"]["gamma-2"] = settings.single_layer_boundary_gamma_2;
        }
    }

    void SolverSerializer::serialize_iisph_settings(nlohmann::json &node, const IISPHSettings &settings)
    {
        node["gamma"] = settings.Gamma;
        node["omega"] = settings.Omega;
        node["max-density-error"] = settings.MaxDensityErrorAllowed;
        node["max-iterations"] = settings.MaxNumberOfIterations;
        node["min-iterations"] = settings.MinNumberOfIterations;
        node["viscosity"] = settings.Viscosity;
    }

    void SolverSerializer::serialize_iisph_3d_settings(nlohmann::json &node, const IISPHSettings3D &settings)
    {
        node["gamma"] = settings.gamma;
        node["omega"] = settings.omega;
        node["max-density-error"] = settings.max_density_error_allowed;
        node["max-iterations"] = settings.max_number_of_iterations;
        node["min-iterations"] = settings.min_number_of_iterations;
        node["viscosity"] = settings.viscosity;

        node["single-layer-boundary-enabled"] = settings.single_layer_boundary;
        if (settings.single_layer_boundary)
        {
            node["single-layer-settings"]["gamma-1"] = settings.single_layer_boundary_gamma_1;
            // TODO: eventually gamma should be in here
        }
    }

    std::shared_ptr<IFluidSolverBase> SolverSerializer::deserialize(const nlohmann::json &node)
    {
        auto solver_type = node["type"].get<std::string>();
        auto neighborhood_search_type = node["neighborhood-search"]["type"].get<std::string>();
        auto kernel_type = node["kernel"]["type"].get<std::string>();

        if (kernel_type == "cubic-spline-kernel")
        {
            using Kn = CubicSplineKernel;

            if (neighborhood_search_type == "quadratic-dynamic-allocated")
            {
                using Ns = QuadraticNeighborhoodSearchDynamicAllocated;

                if (solver_type == "sesph")
                {
                    auto res = std::make_shared<SESPHFluidSolver<Kn, Ns>>();
                    deserialize_sesph_settings(res->settings, node);
                    return res;
                }
                else if (solver_type == "iisph")
                {
                    auto res = std::make_shared<IISPHFluidSolver<Kn, Ns>>();
                    deserialize_iisph_settings(res->settings, node);
                    return res;
                }
            }
            else if (neighborhood_search_type == "hashed")
            {
                using Ns = HashedNeighborhoodSearch;

                if (solver_type == "sesph")
                {
                    auto res = std::make_shared<SESPHFluidSolver<Kn, Ns>>();
                    deserialize_sesph_settings(res->settings, node);
                    return res;
                }
                else if (solver_type == "iisph")
                {
                    auto res = std::make_shared<IISPHFluidSolver<Kn, Ns>>();
                    deserialize_iisph_settings(res->settings, node);
                    return res;
                }
            }
        }
        else if (kernel_type == "cubic-spline-kernel-3d")
        {
            using Kn = CubicSplineKernel3D;

            if (neighborhood_search_type == "quadratic-dynamic-allocated-3d")
            {
                using Ns = QuadraticNeighborhoodSearch3D;

                if (solver_type == "sesph-3d")
                {
                    auto res = std::make_shared<SESPHFluidSolver3D<Kn, Ns>>();
                    deserialize_sesph_3d_settings(res->settings, node);
                    return res;
                }
                else if (solver_type == "iisph-3d")
                {
                    auto res = std::make_shared<IISPHFluidSolver3D<Kn, Ns>>();
                    deserialize_iisph_3d_settings(res->settings, node);
                    return res;
                }
            }
            else if (neighborhood_search_type == "hashed-3d")
            {
                using Ns = HashedNeighborhoodSearch3D;

                if (solver_type == "sesph-3d")
                {
                    auto res = std::make_shared<SESPHFluidSolver3D<Kn, Ns>>();
                    deserialize_sesph_3d_settings(res->settings, node);
                    return res;
                }
                else if (solver_type == "iisph-3d")
                {
                    auto res = std::make_shared<IISPHFluidSolver3D<Kn, Ns>>();
                    deserialize_iisph_3d_settings(res->settings, node);
                    return res;
                }
            }
            else if (neighborhood_search_type == "compressed-3d")
            {
                using Ns = CompressedNeighborhoodSearch;

                if (solver_type == "sesph-3d")
                {
                    auto res = std::make_shared<SESPHFluidSolver3D<Kn, Ns>>();
                    deserialize_sesph_3d_settings(res->settings, node);
                    return res;
                }
                else if (solver_type == "iisph-3d")
                {
                    auto res = std::make_shared<IISPHFluidSolver3D<Kn, Ns>>();
                    deserialize_iisph_3d_settings(res->settings, node);
                    return res;
                }
            }
        }

        // we did not match to any type combination
        context().add_issue("Encountered unknown solver, neighborhood search, kernel combination!");
        return nullptr;
    }

    void SolverSerializer::deserialize_sesph_settings(SESPHSettings &settings, const nlohmann::json &node)
    {
        settings.StiffnessK = node["stiffness"].get<float>();
        settings.Viscosity = node["viscosity"].get<float>();
    }

    void SolverSerializer::deserialize_sesph_3d_settings(SESPHSettings3D &settings, const nlohmann::json &node)
    {
        settings.StiffnessK = node["stiffness"].get<float>();
        settings.Viscosity = node["viscosity"].get<float>();

        settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
        if (settings.single_layer_boundary)
        {
            settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
            settings.single_layer_boundary_gamma_2 = node["single-layer-settings"]["gamma-2"].get<float>();
        }
    }

    void SolverSerializer::deserialize_iisph_settings(IISPHSettings &settings, const nlohmann::json &node)
    {
        settings.Gamma = node["gamma"].get<float>();
        settings.Omega = node["omega"].get<float>();
        settings.MaxDensityErrorAllowed = node["max-density-error"].get<float>();
        settings.MaxNumberOfIterations = node["max-iterations"].get<size_t>();
        settings.MinNumberOfIterations = node["min-iterations"].get<size_t>();
        settings.Viscosity = node["viscosity"].get<float>();
    }

    void SolverSerializer::deserialize_iisph_3d_settings(IISPHSettings3D &settings, const nlohmann::json &node)
    {
        settings.gamma = node["gamma"].get<float>();
        settings.omega = node["omega"].get<float>();
        settings.max_density_error_allowed = node["max-density-error"].get<float>();
        settings.max_number_of_iterations = node["max-iterations"].get<size_t>();
        settings.min_number_of_iterations = node["min-iterations"].get<size_t>();
        settings.viscosity = node["viscosity"].get<float>();

        settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
        if (settings.single_layer_boundary)
        {
            settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
            // TODO: eventually gamma should be in here
        }
    }

} // namespace LibFluid::Serialization
