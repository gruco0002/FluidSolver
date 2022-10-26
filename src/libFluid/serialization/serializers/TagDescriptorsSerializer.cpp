#include "TagDescriptorsSerializer.hpp"
namespace LibFluid::Serialization {


    nlohmann::json TagDescriptorsSerializer::serialize(std::shared_ptr<TagDescriptors> descriptors) {
        nlohmann::json result = nlohmann::json::array();

        for (const auto& descriptor : descriptors->descriptors) {
            result.push_back(serialize_descriptor(descriptor));
        }

        return result;
    }
    
    std::shared_ptr<TagDescriptors> TagDescriptorsSerializer::deserialize(const nlohmann::json& node) {
        auto result = std::make_shared<TagDescriptors>();

        for (const auto& value : node) {
            result->descriptors.push_back(deserialize_descriptor(value));
        }

        return result;
    }

    nlohmann::json TagDescriptorsSerializer::serialize_descriptor(const TagDescriptors::Descriptor& descriptor) {
        nlohmann::json result;

        result["title"] = descriptor.title;
        result["description"] = descriptor.description;
        result["particle-tag"] = descriptor.particle_tag;

        return result;
    }

    TagDescriptors::Descriptor TagDescriptorsSerializer::deserialize_descriptor(const nlohmann::json& node) {
        TagDescriptors::Descriptor descriptor;

        descriptor.title = node["title"].get<std::string>();
        descriptor.description = node["description"].get<std::string>();
        descriptor.particle_tag = node["particle-tag"].get<uint32_t>();

        return descriptor;
    }
} // namespace LibFluid::Serialization