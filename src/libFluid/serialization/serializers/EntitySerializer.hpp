#pragma once

#include "entities/SimulationEntity.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization
{

    class EntitySerializer final : public SerializerBase
    {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<SimulationEntity> entity);

        std::shared_ptr<SimulationEntity> deserialize(const nlohmann::json &node);

      private:
        nlohmann::json serialize_particle_remover(std::shared_ptr<SimulationEntity> entity);
        nlohmann::json serialize_particle_spawner(std::shared_ptr<SimulationEntity> entity);
        nlohmann::json serialize_particle_remover_3d(std::shared_ptr<SimulationEntity> entity);
        nlohmann::json serialize_particle_boundary_preprocessor(std::shared_ptr<SimulationEntity> entity);
        nlohmann::json serialize_velocity_alteration_by_tag(std::shared_ptr<SimulationEntity> entity);

        std::shared_ptr<SimulationEntity> deserialize_particle_remover(const nlohmann::json &node);
        std::shared_ptr<SimulationEntity> deserialize_particle_spawner(const nlohmann::json &node);
        std::shared_ptr<SimulationEntity> deserialize_particle_remover_3d(const nlohmann::json &node);
        std::shared_ptr<SimulationEntity> deserialize_particle_boundary_preprocessor(const nlohmann::json &node);
        std::shared_ptr<SimulationEntity> deserialize_velocity_alteration_by_tag(const nlohmann::json &node);
    };

} // namespace LibFluid::Serialization
