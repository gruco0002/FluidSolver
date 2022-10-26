#pragma once

#include "group/TagDescriptors.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {
    class TagDescriptorsSerializer final : public SerializerBase {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<TagDescriptors> descriptors);

        std::shared_ptr<TagDescriptors> deserialize(const nlohmann::json& node);

      private:
        nlohmann::json serialize_descriptor(const TagDescriptors::Descriptor& descriptor);
        TagDescriptors::Descriptor deserialize_descriptor(const nlohmann::json& node);
    };
} // namespace LibFluid::Serialization
