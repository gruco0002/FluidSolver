#include "IisphSensor.hpp"

#include "fluidSolver/IISPHFluidSolver3D.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"

namespace LibFluid::Sensors {

    void IISPHSensor::calculate_and_store(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.fluid_solver != nullptr);
        FLUID_ASSERT(is_type_compatible());

        data.push_back(timepoint, {get_last_iteration_count(), get_last_average_predicted_density_error()});
    }

    void IISPHSensor::save_data_to_file(SensorWriter& writer) {
        if (writer.begin_header()) {
            writer.push_back_header<Timepoint>("Timepoint");
            writer.push_back_header<size_t>("Last Iteration Count");
            writer.push_back_header<float>("Last Avg Pred Density Error");
            writer.end_header();
        }


        // data writing
        for (size_t i = saved_data_until; i < data.size(); i++) {
            auto& tmp = data.data()[i];
            writer << data.times()[i] << tmp.stat_last_iteration_count << tmp.stat_last_average_predicted_density_error << SensorWriter::Control::Next;
        }


        if (parameters.keep_data_in_memory_after_saving) {
            saved_data_until = data.size();
        } else {
            data.clear();
            saved_data_until = 0;
        }
    }

    void IISPHSensor::create_compatibility_report(CompatibilityReport& report) {
        report.begin_scope("IISPH Sensor");
        if (simulator_data.fluid_solver == nullptr) {
            report.add_issue("Solver is null!");
        }

        if (!is_type_compatible()) {
            report.add_issue("Solver type is incompatible!");
        }

        report.end_scope();
    }
    bool IISPHSensor::is_type_compatible() const {
        return try_fetch_data_from_iisph_solver(simulator_data.fluid_solver, nullptr, nullptr);
    }
    size_t IISPHSensor::get_last_iteration_count() const {
        size_t res = 0;
        auto success = try_fetch_data_from_iisph_solver(simulator_data.fluid_solver, &res, nullptr);
        FLUID_ASSERT(success, "Fluid solver type was incompatible!");
        return res;
    }
    float IISPHSensor::get_last_average_predicted_density_error() const {
        float res = 0.0f;
        auto success = try_fetch_data_from_iisph_solver(simulator_data.fluid_solver, nullptr, &res);
        FLUID_ASSERT(success, "Fluid solver type was incompatible!");
        return res;
    }


    template<typename T>
    bool try_fetch_data_from_iisph_solver_helper(const std::shared_ptr<IFluidSolverBase>& solver, size_t* last_iteration_count, float* last_average_predicted_density_error) {
        auto tmp = std::dynamic_pointer_cast<T>(solver);
        if (tmp == nullptr) {
            return false;
        }

        if (last_iteration_count != nullptr) {
            // return last iteration count
            *last_iteration_count = tmp->stat_last_iteration_count;
        }

        if (last_average_predicted_density_error != nullptr) {
            // return last average predicted density error
            *last_average_predicted_density_error = tmp->stat_last_average_predicted_density_error;
        }

        return true;
    }


    bool IISPHSensor::try_fetch_data_from_iisph_solver(const std::shared_ptr<IFluidSolverBase>& solver, size_t* last_iteration_count, float* last_average_predicted_density_error) {
        {
            if (try_fetch_data_from_iisph_solver_helper<IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(solver, last_iteration_count, last_average_predicted_density_error)) {
                return true;
            }
            if (try_fetch_data_from_iisph_solver_helper<IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(solver, last_iteration_count, last_average_predicted_density_error)) {
                return true;
            }
            if (try_fetch_data_from_iisph_solver_helper<IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(solver, last_iteration_count, last_average_predicted_density_error)) {
                return true;
            }
        }

        {
            // TODO: add IISPH 2d
        }
        return false;
    }
} // namespace FluidSolver::Sensors