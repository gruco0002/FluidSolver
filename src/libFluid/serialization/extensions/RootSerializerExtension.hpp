#pragma once

#include "serialization/helpers/SerializationContext.hpp"

#include <nlohmann/json.hpp>
#include <optional>

namespace LibFluid::Serialization
{
    class RootSerializerExtension
    {
      public:
        virtual const char *get_node_name() const = 0;

        virtual std::optional<nlohmann::json> serialize(SerializationContext &context) = 0;
        virtual void deserialize(std::optional<nlohmann::json> node, SerializationContext &context) = 0;
    };
} // namespace LibFluid::Serialization
