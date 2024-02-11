#pragma once

#include "SensorBase.hpp"

#include <limits>

namespace LibFluid::Sensors
{

    struct MaxMinAvgSensorData
    {
        float average = 0.0f;
        float maximum = std::numeric_limits<float>::min();
        float minimum = std::numeric_limits<float>::max();
    };

    class GlobalDensitySensor : public SensorBase<MaxMinAvgSensorData>
    {
      public:
        std::vector<SensorDataFieldDefinition> get_definitions() override;
        MaxMinAvgSensorData calculate_for_timepoint(const Timepoint &timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json &array, const MaxMinAvgSensorData &data) override;
    };

    class GlobalPressureSensor : public SensorBase<MaxMinAvgSensorData>
    {
      public:
        std::vector<SensorDataFieldDefinition> get_definitions() override;
        MaxMinAvgSensorData calculate_for_timepoint(const Timepoint &timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json &array, const MaxMinAvgSensorData &data) override;
    };

    class GlobalVelocitySensor : public SensorBase<MaxMinAvgSensorData>
    {
      public:
        std::vector<SensorDataFieldDefinition> get_definitions() override;
        MaxMinAvgSensorData calculate_for_timepoint(const Timepoint &timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json &array, const MaxMinAvgSensorData &data) override;
    };

    struct EnergySensorData
    {
        float potential = 0.0f;
        float kinetic = 0.0f;
    };

    class GlobalEnergySensor : public SensorBase<EnergySensorData>
    {
      public:
        struct EnergySensorSettings
        {
            float relative_zero_height = 0.0f;
        } settings;

        std::vector<SensorDataFieldDefinition> get_definitions() override;
        EnergySensorData calculate_for_timepoint(const Timepoint &timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json &array, const EnergySensorData &data) override;
    };

    struct ParticleCountSensorData
    {
        size_t normal_particles = 0;
        size_t boundary_particles = 0;
        size_t inactive_particles = 0;
    };

    class GlobalParticleCountSensor : public SensorBase<ParticleCountSensorData>
    {
      public:
        std::vector<SensorDataFieldDefinition> get_definitions() override;
        ParticleCountSensorData calculate_for_timepoint(const Timepoint &timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json &array, const ParticleCountSensorData &data) override;
    };

} // namespace LibFluid::Sensors
