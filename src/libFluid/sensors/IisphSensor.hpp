#pragma once

#include "FluidInclude.hpp"
#include "sensors/ISensor.hpp"
#include "sensors/SensorData.hpp"

namespace LibFluid::Sensors {

    class IISPHSensor : public ISensor {
      public:
        struct Info {
            size_t stat_last_iteration_count = 0;
            float stat_last_average_predicted_density_error = 0.0f;
        };

        SensorData<Info> data;

        virtual void calculate_and_store(const Timepoint& timepoint) override;
        virtual void save_data_to_file(SensorWriter& writer) override;

        virtual void create_compatibility_report(CompatibilityReport& report) override;

      private:
        bool is_type_compatible() const;
        size_t get_last_iteration_count() const;
        float get_last_average_predicted_density_error() const;

      private:
        static bool try_fetch_data_from_iisph_solver(const std::shared_ptr<IFluidSolverBase>& solver, size_t* last_iteration_count, float* last_average_predicted_density_error);
    };

} // namespace FluidSolver::Sensors
