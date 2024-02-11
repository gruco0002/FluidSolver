#pragma once

#include "sensors/SensorBase.hpp"

#include <limits>

namespace LibFluid::Sensors
{

    struct CompressedNeighborStorageSensorInfo
    {
        float used_delta_bytes_average = 0.0f;
        float used_delta_bytes_minimum = std::numeric_limits<float>::max();
        float used_delta_bytes_maximum = std::numeric_limits<float>::lowest();

        float neighbor_count_average = 0.0f;
        float neighbor_count_minimum = std::numeric_limits<float>::max();
        float neighbor_count_maximum = std::numeric_limits<float>::lowest();
    };

    class CompressedNeighborStorageSensor : public SensorBase<CompressedNeighborStorageSensorInfo>
    {
      public:
        std::vector<SensorDataFieldDefinition> get_definitions() override;
        CompressedNeighborStorageSensorInfo calculate_for_timepoint(const Timepoint &timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json &array,
                                           const CompressedNeighborStorageSensorInfo &data) override;

        virtual void create_compatibility_report(CompatibilityReport &report) override;
    };

} // namespace LibFluid::Sensors
