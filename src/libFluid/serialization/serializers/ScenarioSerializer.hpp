#pragma once

#include "Simulator.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization
{

    class ScenarioSerializer final : public SerializerBase
    {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<Simulator> simulator);

        std::shared_ptr<Simulator> deserialize(const nlohmann::json &node);
    };
} // namespace LibFluid::Serialization
