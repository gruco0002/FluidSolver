#include "TimestepGeneratorSerializer.hpp"

#include "serialization/helpers/DynamicPointerIs.hpp"
#include "time/ConstantTimestepGenerator.hpp"
#include "time/DynamicCflTimestepGenerator.hpp"

namespace LibFluid::Serialization {

    nlohmann::json TimestepGeneratorSerializer::serialize(std::shared_ptr<TimestepGenerator> timestep_generator) {
        if (dynamic_pointer_is<ConstantTimestepGenerator>(timestep_generator)) {
            return serialize_constant_timestep_generator(timestep_generator);
        } else if (dynamic_pointer_is<DynamicCflTimestepGenerator>(timestep_generator)) {
            return serialize_dynamic_cfl_timestep_generator(timestep_generator);
        }

        context().add_issue("Encountered unknown timestep generator type!");

        return {};
    }

    nlohmann::json TimestepGeneratorSerializer::serialize_constant_timestep_generator(std::shared_ptr<TimestepGenerator> timestep_generator) {
        auto t = std::dynamic_pointer_cast<ConstantTimestepGenerator>(timestep_generator);
        FLUID_ASSERT(t != nullptr, "Invalid timestep generator type!");

        nlohmann::json node;

        node["type"] = "constant";
        node["timestep"] = t->settings.timestep;

        return node;
    }

    nlohmann::json TimestepGeneratorSerializer::serialize_dynamic_cfl_timestep_generator(std::shared_ptr<TimestepGenerator> timestep_generator) {
        auto t = std::dynamic_pointer_cast<DynamicCflTimestepGenerator>(timestep_generator);
        FLUID_ASSERT(t != nullptr, "Invalid timestep generator type!");

        nlohmann::json node;

        node["type"] = "dynamic-cfl";
        node["max-timestep"] = t->settings.max_timestep;
        node["min-timestep"] = t->settings.min_timestep;
        node["cfl"] = t->settings.cfl_number;

        return node;
    }

    std::shared_ptr<TimestepGenerator> TimestepGeneratorSerializer::deserialize(const nlohmann::json& node) {
        auto type = node["type"].get<std::string>();

        if (type == "constant") {
            return deserialize_constant_timestep_generator(node);
        } else if (type == "dynamic-cfl") {
            return deserialize_dynamic_cfl_timestep_generator(node);
        }

        context().add_issue("Encountered unknown timestep generator type!");
        return nullptr;
    }

    std::shared_ptr<TimestepGenerator> TimestepGeneratorSerializer::deserialize_constant_timestep_generator(const nlohmann::json& node) {
        auto res = std::make_shared<ConstantTimestepGenerator>();
        res->settings.timestep = node["timestep"].get<float>();
        return res;
    }

    std::shared_ptr<TimestepGenerator> TimestepGeneratorSerializer::deserialize_dynamic_cfl_timestep_generator(const nlohmann::json& node) {
        auto res = std::make_shared<DynamicCflTimestepGenerator>();
        res->settings.max_timestep = node["max-timestep"].get<float>();
        res->settings.min_timestep = node["min-timestep"].get<float>();
        res->settings.cfl_number = node["cfl"].get<float>();
        return res;
    }
} // namespace LibFluid::Serialization