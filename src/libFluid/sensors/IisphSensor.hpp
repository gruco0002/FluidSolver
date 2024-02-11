#pragma once

#include "sensors/SensorBase.hpp"

namespace LibFluid::Sensors
{

    struct IISPHSensorInfo
    {
        size_t stat_last_iteration_count = 0;
        float stat_last_average_predicted_density_error = 0.0f;
    };

    class IISPHSensor : public SensorBase<IISPHSensorInfo>
    {
      public:
        std::vector<SensorDataFieldDefinition> get_definitions() override;
        IISPHSensorInfo calculate_for_timepoint(const Timepoint &timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json &array, const IISPHSensorInfo &data) override;
        virtual void create_compatibility_report(CompatibilityReport &report) override;

      private:
        bool is_type_compatible() const;
        size_t get_last_iteration_count() const;
        float get_last_average_predicted_density_error() const;

      private:
        static bool try_fetch_data_from_iisph_solver(const std::shared_ptr<IFluidSolverBase> &solver,
                                                     size_t *last_iteration_count,
                                                     float *last_average_predicted_density_error);
    };

} // namespace LibFluid::Sensors
