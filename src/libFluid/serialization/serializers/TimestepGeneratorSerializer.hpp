#pragma once

#include "serialization/serializers/SerializerBase.hpp"
#include "time/TimestepGenerator.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization
{

    class TimestepGeneratorSerializer final : public SerializerBase
    {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<TimestepGenerator> timestep_generator);

        std::shared_ptr<TimestepGenerator> deserialize(const nlohmann::json &node);

      private:
        nlohmann::json serialize_constant_timestep_generator(std::shared_ptr<TimestepGenerator> timestep_generator);
        nlohmann::json serialize_dynamic_cfl_timestep_generator(std::shared_ptr<TimestepGenerator> timestep_generator);

        std::shared_ptr<TimestepGenerator> deserialize_constant_timestep_generator(const nlohmann::json &node);
        std::shared_ptr<TimestepGenerator> deserialize_dynamic_cfl_timestep_generator(const nlohmann::json &node);
    };
} // namespace LibFluid::Serialization
