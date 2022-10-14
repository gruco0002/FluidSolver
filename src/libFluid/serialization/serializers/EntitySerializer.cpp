#include "EntitySerializer.hpp"

#include "entities/BoundaryPreprocessor.hpp"
#include "entities/ParticleRemover.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "entities/ParticleSpawner.hpp"
#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"


namespace LibFluid::Serialization {

    nlohmann::json EntitySerializer::serialize(std::shared_ptr<SimulationEntity> entity) {
        if (dynamic_pointer_is<ParticleSpawner>(entity)) {
            return serialize_particle_spawner(entity);
        }
        if (dynamic_pointer_is<ParticleRemover>(entity)) {
            return serialize_particle_remover(entity);
        }
        if (dynamic_pointer_is<ParticleRemover3D>(entity)) {
            return serialize_particle_remover_3d(entity);
        }
        if (dynamic_pointer_is<BoundaryPreprocessor>(entity)) {
            return serialize_particle_boundary_preprocessor(entity);
        }

        context().add_issue("Unhandled entity type encountered!");
        return {};
    }

    nlohmann::json EntitySerializer::serialize_particle_remover(std::shared_ptr<SimulationEntity> entity) {
        auto remover = std::dynamic_pointer_cast<ParticleRemover>(entity);
        FLUID_ASSERT(remover != nullptr, "Invalid type passed into function!");

        nlohmann::json res;
        res["type"] = "particle-remover";

        res["area"]["left"] = remover->parameters.area.left;
        res["area"]["right"] = remover->parameters.area.right;
        res["area"]["top"] = remover->parameters.area.top;
        res["area"]["bottom"] = remover->parameters.area.bottom;
        res["remove-if-outside"] = remover->parameters.remove_if_outside;

        return res;
    }

    nlohmann::json EntitySerializer::serialize_particle_spawner(std::shared_ptr<SimulationEntity> entity) {
        auto spawner = std::dynamic_pointer_cast<ParticleSpawner>(entity);
        FLUID_ASSERT(spawner != nullptr, "Invalid type passed into function!");

        nlohmann::json res;
        res["type"] = "particle-spawner";

        res["position"] = spawner->parameters.position;
        res["direction"] = spawner->parameters.direction;
        res["width"] = spawner->parameters.width;
        res["initial-velocity"] = spawner->parameters.initial_velocity;
        res["rest-density"] = spawner->parameters.rest_density;
        res["mass"] = spawner->parameters.mass;

        return res;
    }

    nlohmann::json EntitySerializer::serialize_particle_remover_3d(std::shared_ptr<SimulationEntity> entity) {
        auto remover = std::dynamic_pointer_cast<ParticleRemover3D>(entity);
        FLUID_ASSERT(remover != nullptr, "Invalid type passed into function!");

        nlohmann::json res;
        res["type"] = "particle-remover-3d";

        res["volume"]["center"] = remover->parameters.volume.center;
        res["volume"]["distance-from-center"] = remover->parameters.volume.distance_from_center;
        res["remove-if-outside"] = remover->parameters.remove_if_outside;

        return res;
    }

    nlohmann::json EntitySerializer::serialize_particle_boundary_preprocessor(std::shared_ptr<SimulationEntity> entity) {
        auto preprocessor = std::dynamic_pointer_cast<BoundaryPreprocessor>(entity);
        FLUID_ASSERT(preprocessor != nullptr, "Invalid type passed into function!");

        nlohmann::json res;
        res["type"] = "boundary-preprocessor";

        return res;
    }

    std::shared_ptr<SimulationEntity> EntitySerializer::deserialize(const nlohmann::json& node) {
        auto type = node["type"].get<std::string>();

        if (type == "particle-remover") {
            return deserialize_particle_remover(node);
        } else if (type == "particle-spawner") {
            return deserialize_particle_spawner(node);
        } else if (type == "particle-remover-3d") {
            return deserialize_particle_remover_3d(node);
        } else if (type == "boundary-preprocessor") {
            return deserialize_particle_boundary_preprocessor(node);
        }

        context().add_issue("Encountered invalid entity type!");
        return nullptr;
    }

    std::shared_ptr<SimulationEntity> EntitySerializer::deserialize_particle_remover(const nlohmann::json& node) {
        auto remover = std::make_shared<ParticleRemover>();

        remover->parameters.area.left = node["area"]["left"].get<float>();
        remover->parameters.area.right = node["area"]["right"].get<float>();
        remover->parameters.area.top = node["area"]["top"].get<float>();
        remover->parameters.area.bottom = node["area"]["bottom"].get<float>();
        remover->parameters.remove_if_outside = node["remove-if-outside"].get<bool>();

        return remover;
    }

    std::shared_ptr<SimulationEntity> EntitySerializer::deserialize_particle_spawner(const nlohmann::json& node) {
        auto spawner = std::make_shared<ParticleSpawner>();

        spawner->parameters.position = node["position"].get<glm::vec2>();
        spawner->parameters.direction = node["direction"].get<glm::vec2>();
        spawner->parameters.width = node["width"].get<float>();
        spawner->parameters.initial_velocity = node["initial-velocity"].get<float>();
        spawner->parameters.rest_density = node["rest-density"].get<float>();
        spawner->parameters.mass = node["mass"].get<float>();

        return spawner;
    }

    std::shared_ptr<SimulationEntity> EntitySerializer::deserialize_particle_remover_3d(const nlohmann::json& node) {
        auto remover = std::make_shared<ParticleRemover3D>();

        remover->parameters.volume.center = node["volume"]["center"].get<glm::vec3>();
        remover->parameters.volume.distance_from_center = node["volume"]["distance-from-center"].get<glm::vec3>();
        remover->parameters.remove_if_outside = node["remove-if-outside"].get<bool>();

        return remover;
    }

    std::shared_ptr<SimulationEntity> EntitySerializer::deserialize_particle_boundary_preprocessor(const nlohmann::json& node) {
        auto res = std::make_shared<BoundaryPreprocessor>();

        return res;
    }
} // namespace LibFluid::Serialization