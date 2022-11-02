#include "ScenarioSerializer.hpp"

#include "serialization/serializers/EntitySerializer.hpp"
#include "serialization/serializers/SensorSerializer.hpp"

namespace LibFluid::Serialization {

    nlohmann::json ScenarioSerializer::serialize(std::shared_ptr<Simulator> simulator) {
        if (simulator == nullptr) {
            context().add_issue("Simulator is null!");
            return {};
        }

        nlohmann::json res;

        // save particle data path
        res["particles"] = context().particle_data_relative_filepath.string();

        // save general parameters
        res["gravity"] = simulator->parameters.gravity;
        res["particle-size"] = simulator->parameters.particle_size;
        res["rest-density"] = simulator->parameters.rest_density;

        // save entities
        EntitySerializer entity_serializer(context(), serializer_extensions());
        for (size_t i = 0; i < simulator->data.entities.size(); i++) {
            context().begin_section("entities[" + std::to_string(i) + "]");
            res["entities"].push_back(entity_serializer.serialize(simulator->data.entities[i]));
            context().end_section();
        }

        // save sensors
        SensorSerializer sensor_serializer(context(), serializer_extensions());
        for (size_t i = 0; i < simulator->data.sensors.size(); i++) {
            context().begin_section("sensors[" + std::to_string(i) + "]");
            res["sensors"].push_back(sensor_serializer.serialize(simulator->data.sensors[i]));
            context().end_section();
        }

        return res;
    }

    std::shared_ptr<Simulator> ScenarioSerializer::deserialize(const nlohmann::json& node) {
        auto result = std::make_shared<Simulator>();

        // get particle data path
        context().particle_data_relative_filepath = node["particles"].get<std::string>();

        // loading general parameters
        result->parameters.gravity = node["gravity"].get<float>();
        result->parameters.particle_size = node["particle-size"].get<float>();
        result->parameters.rest_density = node["rest-density"].get<float>();

        // loading entities
        EntitySerializer entity_serializer(context(), serializer_extensions());
        if (node.contains("entities")) {
            size_t i = 0;
            for (const auto& entity_node : node["entities"]) {
                context().begin_section("entities[" + std::to_string(i) + "]");
                result->data.entities.push_back(entity_serializer.deserialize(entity_node));
                context().end_section();
                i++;
            }
        }

        // loading sensors
        SensorSerializer sensor_serializer(context(), serializer_extensions());
        if (node.contains("sensors")) {
            size_t i = 0;
            for (const auto& sensor_node : node["sensors"]) {
                context().begin_section("sensors[" + std::to_string(i) + "]");
                result->data.sensors.push_back(sensor_serializer.deserialize(sensor_node));
                context().end_section();
                i++;
            }
        }

        return result;
    }
} // namespace LibFluid::Serialization