#include "IisphSensor.hpp"

#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"
#include "fluidSolver/solver/IISPHFluidSolver3D.hpp"

namespace LibFluid::Sensors
{

    void IISPHSensor::create_compatibility_report(CompatibilityReport &report)
    {
        report.begin_scope("IISPH Sensor");
        if (simulator_data.fluid_solver == nullptr)
        {
            report.add_issue("Solver is null!");
        }

        if (!is_type_compatible())
        {
            report.add_issue("Solver type is incompatible!");
        }

        report.end_scope();
    }
    bool IISPHSensor::is_type_compatible() const
    {
        return try_fetch_data_from_iisph_solver(simulator_data.fluid_solver, nullptr, nullptr);
    }
    size_t IISPHSensor::get_last_iteration_count() const
    {
        size_t res = 0;
        auto success = try_fetch_data_from_iisph_solver(simulator_data.fluid_solver, &res, nullptr);
        FLUID_ASSERT(success, "Fluid solver type was incompatible!");
        return res;
    }
    float IISPHSensor::get_last_average_predicted_density_error() const
    {
        float res = 0.0f;
        auto success = try_fetch_data_from_iisph_solver(simulator_data.fluid_solver, nullptr, &res);
        FLUID_ASSERT(success, "Fluid solver type was incompatible!");
        return res;
    }

    template <typename T>
    bool try_fetch_data_from_iisph_solver_helper(const std::shared_ptr<IFluidSolverBase> &solver,
                                                 size_t *last_iteration_count,
                                                 float *last_average_predicted_density_error)
    {
        auto tmp = std::dynamic_pointer_cast<T>(solver);
        if (tmp == nullptr)
        {
            return false;
        }

        if (last_iteration_count != nullptr)
        {
            // return last iteration count
            *last_iteration_count = tmp->stat_last_iteration_count;
        }

        if (last_average_predicted_density_error != nullptr)
        {
            // return last average predicted density error
            *last_average_predicted_density_error = tmp->stat_last_average_predicted_density_error;
        }

        return true;
    }

    bool IISPHSensor::try_fetch_data_from_iisph_solver(const std::shared_ptr<IFluidSolverBase> &solver,
                                                       size_t *last_iteration_count,
                                                       float *last_average_predicted_density_error)
    {
        {
            if (try_fetch_data_from_iisph_solver_helper<
                    IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(
                    solver, last_iteration_count, last_average_predicted_density_error))
            {
                return true;
            }
            if (try_fetch_data_from_iisph_solver_helper<
                    IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(
                    solver, last_iteration_count, last_average_predicted_density_error))
            {
                return true;
            }
            if (try_fetch_data_from_iisph_solver_helper<
                    IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(
                    solver, last_iteration_count, last_average_predicted_density_error))
            {
                return true;
            }
        }

        {
            // TODO: add IISPH 2d
        }
        return false;
    }
    std::vector<SensorDataFieldDefinition> IISPHSensor::get_definitions()
    {
        return {{"Last Iteration Count", SensorDataFieldDefinition::FieldType::Int, "", ""},
                {"Last Average Predicted Density Error", SensorDataFieldDefinition::FieldType::Float, "", ""}};
    }
    IISPHSensorInfo IISPHSensor::calculate_for_timepoint(const Timepoint &timepoint)
    {
        return {get_last_iteration_count(), get_last_average_predicted_density_error()};
    }
    void IISPHSensor::add_data_fields_to_json_array(nlohmann::json &array, const IISPHSensorInfo &data)
    {
        array.push_back((int)data.stat_last_iteration_count);
        array.push_back((float)data.stat_last_average_predicted_density_error);
    }
} // namespace LibFluid::Sensors
