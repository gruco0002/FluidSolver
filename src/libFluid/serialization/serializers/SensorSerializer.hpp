#pragma once

#include "sensors/Sensor.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization
{

    class SensorSerializer final : public SerializerBase
    {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<Sensor> sensor);

        std::shared_ptr<Sensor> deserialize(const nlohmann::json &node);

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

        void deserialize_sensor_shared_data(const nlohmann::json &node, std::shared_ptr<Sensor> sensor);

        std::shared_ptr<Sensor> deserialize_global_density_sensor(const nlohmann::json &node);
        std::shared_ptr<Sensor> deserialize_global_pressure_sensor(const nlohmann::json &node);
        std::shared_ptr<Sensor> deserialize_global_velocity_sensor(const nlohmann::json &node);
        std::shared_ptr<Sensor> deserialize_global_energy_sensor(const nlohmann::json &node);
        std::shared_ptr<Sensor> deserialize_global_particle_count_sensor(const nlohmann::json &node);
        std::shared_ptr<Sensor> deserialize_sensor_plane(const nlohmann::json &node);
        std::shared_ptr<Sensor> deserialize_compressed_neighborhood_storage_sensor(const nlohmann::json &node);
        std::shared_ptr<Sensor> deserialize_iisph_sensor(const nlohmann::json &node);
    };

} // namespace LibFluid::Serialization
