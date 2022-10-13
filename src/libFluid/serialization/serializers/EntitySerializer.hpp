#pragma once

#include "entities/SimulationEntity.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {

    class EntitySerializer final : public SerializerBase {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<SimulationEntity> entity);

      private:
        nlohmann::json serialize_particle_remover(std::shared_ptr<SimulationEntity> entity);
        nlohmann::json serialize_particle_spawner(std::shared_ptr<SimulationEntity> entity);
        nlohmann::json serialize_particle_remover_3d(std::shared_ptr<SimulationEntity> entity);
        nlohmann::json serialize_particle_boundary_preprocessor(std::shared_ptr<SimulationEntity> entity);
    };

} // namespace LibFluid::Serialization
