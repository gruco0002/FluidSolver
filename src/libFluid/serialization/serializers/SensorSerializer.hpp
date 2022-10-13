#pragma once

#include "sensors/Sensor.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {

    class SensorSerializer final : public SerializerBase {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<Sensor> sensor);

      private:
        nlohmann::json serialize_sensor_shared_data(std::shared_ptr<Sensor> sensor);

        nlohmann::json serialize_global_density_sensor(std::shared_ptr<Sensor> sensor);
        nlohmann::json serialize_global_pressure_sensor(std::shared_ptr<Sensor> sensor);
        nlohmann::json serialize_global_velocity_sensor(std::shared_ptr<Sensor> sensor);
        nlohmann::json serialize_global_energy_sensor(std::shared_ptr<Sensor> sensor);
        nlohmann::json serialize_global_particle_count_sensor(std::shared_ptr<Sensor> sensor);
        nlohmann::json serialize_sensor_plane(std::shared_ptr<Sensor> sensor);
        nlohmann::json serialize_compressed_neighborhood_storage_sensor(std::shared_ptr<Sensor> sensor);
        nlohmann::json serialize_iisph_sensor(std::shared_ptr<Sensor> sensor);
    };

} // namespace LibFluid::Serialization
