#include "ParticleStatistics.hpp"

#include "FluidInclude.hpp"
#include "Simulator.hpp"

#include <algorithm>

namespace LibFluid::Sensors {

    std::vector<SensorDataFieldDefinition> GlobalDensitySensor::get_definitions() {
        return {
                {"Maximum Density", SensorDataFieldDefinition::FieldType::Float, "", ""},
                {"Minimum Density", SensorDataFieldDefinition::FieldType::Float, "", ""},
                {"Average Density", SensorDataFieldDefinition::FieldType::Float, "", ""},
        };
    }

    MaxMinAvgSensorData GlobalDensitySensor::calculate_for_timepoint(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.collection != nullptr);
        auto collection = simulator_data.collection;
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

        // calculate data;
        MaxMinAvgSensorData d;

        size_t counter = 0;
        for (size_t i = 0; i < collection->size(); i++) {
            const ParticleData& pData = collection->get<ParticleData>(i);
            const ParticleInfo& iData = collection->get<ParticleInfo>(i);

            if (iData.type == ParticleType::ParticleTypeNormal) {
                d.average += pData.density;
                d.minimum = std::min(d.minimum, pData.density);
                d.maximum = std::max(d.maximum, pData.density);
                counter++;
            }
        }

        if (counter > 0) {
            d.average = d.average / counter;
        }

        return d;
    }

    void GlobalDensitySensor::add_data_fields_to_json_array(nlohmann::json& array, const MaxMinAvgSensorData& data) {
        array.push_back(data.maximum);
        array.push_back(data.minimum);
        array.push_back(data.average);
    }

    std::vector<SensorDataFieldDefinition> GlobalPressureSensor::get_definitions() {
        return {
                {"Maximum Pressure", SensorDataFieldDefinition::FieldType::Float, "", ""},
                {"Minimum Pressure", SensorDataFieldDefinition::FieldType::Float, "", ""},
                {"Average Pressure", SensorDataFieldDefinition::FieldType::Float, "", ""},
        };
    }
    MaxMinAvgSensorData GlobalPressureSensor::calculate_for_timepoint(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.collection != nullptr);
        auto collection = simulator_data.collection;
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

        // calculate data;
        MaxMinAvgSensorData d;

        size_t counter = 0;
        for (size_t i = 0; i < collection->size(); i++) {
            const ParticleData& pData = collection->get<ParticleData>(i);
            const ParticleInfo& iData = collection->get<ParticleInfo>(i);

            if (iData.type == ParticleType::ParticleTypeNormal) {
                d.average += pData.pressure;
                d.minimum = std::min(d.minimum, pData.pressure);
                d.maximum = std::max(d.maximum, pData.pressure);
                counter++;
            }
        }

        if (counter > 0) {
            d.average = d.average / counter;
        }

        return d;
    }
    void GlobalPressureSensor::add_data_fields_to_json_array(nlohmann::json& array, const MaxMinAvgSensorData& data) {
        array.push_back(data.maximum);
        array.push_back(data.minimum);
        array.push_back(data.average);
    }

    std::vector<SensorDataFieldDefinition> GlobalVelocitySensor::get_definitions() {
        return {
                {"Maximum Velocity", SensorDataFieldDefinition::FieldType::Float, "", ""},
                {"Minimum Velocity", SensorDataFieldDefinition::FieldType::Float, "", ""},
                {"Average Velocity", SensorDataFieldDefinition::FieldType::Float, "", ""},
        };
    }

    MaxMinAvgSensorData GlobalVelocitySensor::calculate_for_timepoint(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.collection != nullptr);
        auto collection = simulator_data.collection;
        FLUID_ASSERT(collection->is_type_present<MovementData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

        // calculate data;
        MaxMinAvgSensorData d;

        size_t counter = 0;
        for (size_t i = 0; i < collection->size(); i++) {
            const MovementData& mData = collection->get<MovementData>(i);
            const ParticleInfo& iData = collection->get<ParticleInfo>(i);

            if (iData.type == ParticleType::ParticleTypeNormal) {
                float velocity = glm::length(mData.velocity);
                d.average += velocity;
                d.minimum = std::min(d.minimum, velocity);
                d.maximum = std::max(d.maximum, velocity);
                counter++;
            }
        }

        if (counter > 0) {
            d.average = d.average / counter;
        }

        return d;
    }
    void GlobalVelocitySensor::add_data_fields_to_json_array(nlohmann::json& array, const MaxMinAvgSensorData& data) {
        array.push_back(data.maximum);
        array.push_back(data.minimum);
        array.push_back(data.average);
    }


    std::vector<SensorDataFieldDefinition> GlobalEnergySensor::get_definitions() {
        return {
                {"Kinetic Energy", SensorDataFieldDefinition::FieldType::Float, "", ""},
                {"Potential Energy", SensorDataFieldDefinition::FieldType::Float, "", ""},
        };
    }
    EnergySensorData GlobalEnergySensor::calculate_for_timepoint(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.collection != nullptr);
        auto collection = simulator_data.collection;
        FLUID_ASSERT(collection->is_type_present<MovementData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());

        // calculate data;
        EnergySensorData d;

        for (size_t i = 0; i < collection->size(); i++) {
            const MovementData& mData = collection->get<MovementData>(i);
            const ParticleInfo& iData = collection->get<ParticleInfo>(i);
            const ParticleData& pData = collection->get<ParticleData>(i);

            if (iData.type == ParticleType::ParticleTypeNormal) {
                float velocity = glm::length(mData.velocity);
                float relative_height = mData.position.y - settings.relative_zero_height;

                d.kinetic += 0.5f * pData.mass * velocity * velocity;
                d.potential += simulator_parameters.gravity * pData.mass * relative_height;
            }
        }

        return d;
    }
    void GlobalEnergySensor::add_data_fields_to_json_array(nlohmann::json& array, const EnergySensorData& data) {
        array.push_back(data.kinetic);
        array.push_back(data.potential);
    }

    std::vector<SensorDataFieldDefinition> GlobalParticleCountSensor::get_definitions() {
        return {
                {"Normal Particles", SensorDataFieldDefinition::FieldType::Int, "", ""},
                {"Boundary Particles", SensorDataFieldDefinition::FieldType::Int, "", ""},
                {"Inactive Particles", SensorDataFieldDefinition::FieldType::Int, "", ""},
        };
    }
    ParticleCountSensorData GlobalParticleCountSensor::calculate_for_timepoint(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.collection != nullptr);
        auto collection = simulator_data.collection;
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

        // calculate data;
        ParticleCountSensorData d;

        for (size_t i = 0; i < collection->size(); i++) {
            const ParticleInfo& iData = collection->get<ParticleInfo>(i);

            if (iData.type == ParticleType::ParticleTypeNormal) {
                d.normal_particles++;
            } else if (iData.type == ParticleType::ParticleTypeBoundary) {
                d.boundary_particles++;
            } else if (iData.type == ParticleType::ParticleTypeDead) {
                d.inactive_particles++;
            }
        }

        return d;
    }
    void GlobalParticleCountSensor::add_data_fields_to_json_array(nlohmann::json& array, const ParticleCountSensorData& data) {
        array.push_back(data.normal_particles);
        array.push_back(data.boundary_particles);
        array.push_back(data.inactive_particles);
    }
} // namespace LibFluid::Sensors