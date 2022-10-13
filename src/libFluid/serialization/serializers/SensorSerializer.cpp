#include "SensorSerializer.hpp"

#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/IisphSensor.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"
#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"


namespace LibFluid::Serialization {

    nlohmann::json SensorSerializer::serialize(std::shared_ptr<Sensor> sensor) {
        if (dynamic_pointer_is<Sensors::GlobalDensitySensor>(sensor)) {
            return serialize_global_density_sensor(sensor);
        }
        if (dynamic_pointer_is<Sensors::GlobalPressureSensor>(sensor)) {
            return serialize_global_pressure_sensor(sensor);
        }
        if (dynamic_pointer_is<Sensors::GlobalVelocitySensor>(sensor)) {
            return serialize_global_velocity_sensor(sensor);
        }
        if (dynamic_pointer_is<Sensors::GlobalEnergySensor>(sensor)) {
            return serialize_global_energy_sensor(sensor);
        }
        if (dynamic_pointer_is<Sensors::GlobalParticleCountSensor>(sensor)) {
            return serialize_global_particle_count_sensor(sensor);
        }
        if (dynamic_pointer_is<Sensors::SensorPlane>(sensor)) {
            return serialize_sensor_plane(sensor);
        }
        if (dynamic_pointer_is<Sensors::CompressedNeighborStorageSensor>(sensor)) {
            return serialize_compressed_neighborhood_storage_sensor(sensor);
        }
        if (dynamic_pointer_is<Sensors::IISPHSensor>(sensor)) {
            return serialize_iisph_sensor(sensor);
        }

        context().add_issue("Unhandled sensor type encountered!");
        return {};
    }
    nlohmann::json SensorSerializer::serialize_sensor_shared_data(std::shared_ptr<Sensor> sensor) {
        nlohmann::json result;

        result["name"] = sensor->parameters.name;
        result["save-to-file"] = sensor->parameters.save_to_file;
        result["keep-data-in-memory"] = sensor->parameters.keep_data_in_memory;
        result["filename"] = sensor->parameters.filename;

        return result;
    }
    nlohmann::json SensorSerializer::serialize_global_density_sensor(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::GlobalDensitySensor>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "global-density-sensor";

        return node;
    }
    nlohmann::json SensorSerializer::serialize_global_pressure_sensor(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::GlobalPressureSensor>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "global-pressure-sensor";

        return node;
    }
    nlohmann::json SensorSerializer::serialize_global_velocity_sensor(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::GlobalVelocitySensor>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "global-velocity-sensor";

        return node;
    }
    nlohmann::json SensorSerializer::serialize_global_energy_sensor(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::GlobalEnergySensor>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "global-energy-sensor";
        node["relative-zero-height"] = sen->settings.relative_zero_height;

        return node;
    }
    nlohmann::json SensorSerializer::serialize_global_particle_count_sensor(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::GlobalParticleCountSensor>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "global-particle-count-sensor";

        return node;
    }
    nlohmann::json SensorSerializer::serialize_sensor_plane(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::SensorPlane>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "sensor-plane";

        // sensor plane custom settings
        node["size"]["width"] = sen->settings.width;
        node["size"]["height"] = sen->settings.height;

        node["location"]["origin"] = sen->settings.origin;
        node["location"]["span-x"] = sen->settings.span_x;
        node["location"]["span-y"] = sen->settings.span_y;

        node["samples"]["x"] = sen->settings.number_of_samples_x;
        node["samples"]["y"] = sen->settings.number_of_samples_y;
        node["samples"]["sub-sample-grid-size"] = sen->settings.sub_sample_grid_size;

        node["image"]["min-value"] = sen->settings.min_image_value;
        node["image"]["max-value"] = sen->settings.max_image_value;


        switch (sen->settings.sensor_type) {
            case Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeDensity:
                node["data"] = "density";
                break;
            case Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypePressure:
                node["data"] = "pressure";
                break;
            case Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity:
                node["data"] = "velocity";
                break;
            default:
                context().add_issue("Unknown sensor plane type encountered!");
                break;
        }

        node["calculate-every-nth-step"] = sen->settings.calculate_plane_every_nth_step;

        return node;
    }
    nlohmann::json SensorSerializer::serialize_compressed_neighborhood_storage_sensor(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::CompressedNeighborStorageSensor>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "compressed-neighborhood-storage-sensor";

        return node;
    }
    nlohmann::json SensorSerializer::serialize_iisph_sensor(std::shared_ptr<Sensor> sensor) {
        auto sen = std::dynamic_pointer_cast<Sensors::IISPHSensor>(sensor);
        FLUID_ASSERT(sen != nullptr, "Sensor has wrong type!");

        nlohmann::json node = serialize_sensor_shared_data(sensor);

        node["type"] = "iisph-sensor";

        return node;
    }

} // namespace LibFluid::Serialization
