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

        // save particle data
        res["particles"] = context().particle_data_relative_filepath;

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
} // namespace LibFluid::Serialization