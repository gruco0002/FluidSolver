#include "SimulationSerializer.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <core/entities/ParticleSpawner.hpp>
#include <core/entities/ParticleRemover.hpp>
#include <core/Log.hpp>
#include <filesystem>

namespace YAML {
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.SetStyle(YAML::EmitterStyle::Flow);
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) {
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};
}


namespace FluidSolver {

	YAML::Node save_particle_spawner(const ParticleSpawner* spawner) {
		YAML::Node res;
		res["type"] = "particle-spawner";

		res["position"] = spawner->parameters.position;
		res["direction"] = spawner->parameters.direction;
		res["width"] = spawner->parameters.width;
		res["initial-velocity"] = spawner->parameters.initial_velocity;
		res["rest-density"] = spawner->parameters.rest_density;
		res["mass"] = spawner->parameters.mass;

		return res;
	}

	ParticleSpawner* load_particle_spawner(const YAML::Node& node) {
		auto spawner = new ParticleSpawner();
		spawner->parameters.position = node["position"].as<glm::vec2>();
		spawner->parameters.direction = node["direction"].as<glm::vec2>();
		spawner->parameters.width = node["width"].as<float>();
		spawner->parameters.initial_velocity = node["initial-velocity"].as<float>();
		spawner->parameters.rest_density = node["rest-density"].as<float>();
		spawner->parameters.mass = node["mass"].as<float>();
		return spawner;
	}

	YAML::Node save_particle_remover(const ParticleRemover* remover) {
		YAML::Node res;
		res["type"] = "particle-remover";

		res["area"]["left"] = remover->parameters.area.left;
		res["area"]["right"] = remover->parameters.area.right;
		res["area"]["top"] = remover->parameters.area.top;
		res["area"]["bottom"] = remover->parameters.area.bottom;
		res["remove-if-outside"] = remover->parameters.remove_if_outside;

		return res;
	}


	ParticleRemover* load_particle_remover(const YAML::Node& node) {
		auto remover = new ParticleRemover();

		remover->parameters.area.left = node["area"]["left"].as<float>();
		remover->parameters.area.right = node["area"]["right"].as<float>();
		remover->parameters.area.top = node["area"]["top"].as<float>();
		remover->parameters.area.bottom = node["area"]["bottom"].as<float>();
		remover->parameters.remove_if_outside = node["remove-if-outside"].as<bool>();

		return remover;
	}



	YAML::Node save_scenario(const Simulation& simulation, const std::string& data_filepath) {
		YAML::Node res;

		// TODO: save particles to special file
		res["particles"] = data_filepath;
		SimulationSerializer::save_particles(*simulation.parameters.collection, data_filepath);

		res["gravity"] = simulation.parameters.gravity;
		res["particle-size"] = simulation.parameters.particle_size;
		res["rest-density"] = simulation.parameters.rest_density;

		for (auto ent : simulation.parameters.entities) {
			auto spawner = dynamic_cast<ParticleSpawner*>(ent);
			if (spawner) {
				res["entities"].push_back(save_particle_spawner(spawner));
			}

			auto rem = dynamic_cast<ParticleRemover*>(ent);
			if (rem) {
				res["entities"].push_back(save_particle_remover(rem));
			}
		}

		return res;
	}

	void load_scenario(const YAML::Node& node, Simulation& simulation, const std::string& data_directory) {
		auto particles = node["particles"].as<std::string>();
		auto particles_file = std::filesystem::path(data_directory) / std::filesystem::path(particles);
		if (!std::filesystem::exists(particles_file)) {
			Log::error("[LOADING] Particles file was not found under '" + particles_file.string() + "'!");
			return;
		}
		SimulationSerializer::load_particles(*simulation.parameters.collection, particles_file.string());

		simulation.parameters.gravity = node["gravity"].as<float>();
		simulation.parameters.particle_size = node["particle-size"].as<float>();
		simulation.parameters.rest_density = node["rest-density"].as<float>();

		if (node["entities"]) {
			for (auto& ent_node : node["entities"]) {
				if (ent_node["type"].as<std::string>() == "particle-spawner") {
					simulation.parameters.entities.push_back(load_particle_spawner(ent_node));
				}
				else if (ent_node["type"].as<std::string>() == "particle-remover") {
					simulation.parameters.entities.push_back(load_particle_remover(ent_node));
				}
				else {
					Log::warning("[LOADING] Unkown entity type: '" + ent_node["type"].as<std::string>() + "'!");
				}
			}
		}


	}

	Simulation SimulationSerializer::load_from_file(const std::string& filepath)
	{
		// setup basic parameters
		Simulation res;
		res.parameters.invalidate = true;

		res.parameters.collection = new ParticleCollection();
		res.parameters.sensor_storage = new SensorDataStorage();

		// load stuff
		YAML::Node config = YAML::LoadFile(filepath);
		if (!config["version"] || config["version"].as<int>() != 1) {
			Log::error("[LOADING] Invalid or missing version. Version has to be 1.");
			return res;
		}


		load_scenario(config["scenario"], res, std::filesystem::path(filepath).parent_path().string());


		return res;
	}


	void SimulationSerializer::save_to_file(const Simulation& simulation, const std::string& filepath, const std::string& data_filepath)
	{
		YAML::Node config;

		config["version"] = "1";
		config["scenario"] = save_scenario(simulation, data_filepath);


		std::ofstream filestream(filepath);
		filestream << config;
	}

	void SimulationSerializer::load_particles(ParticleCollection& collection, const std::string& filepath)
	{
		if (!collection.is_type_present<MovementData>())collection.add_type<MovementData>();
		if (!collection.is_type_present<ParticleData>())collection.add_type<ParticleData>();
		if (!collection.is_type_present<ParticleInfo>())collection.add_type<ParticleInfo>();

		collection.clear();

		std::ifstream data(filepath, std::ios::in | std::ios::binary);

		size_t size;
		data.read(reinterpret_cast<char*>(&size), sizeof(size));

		collection.resize(size);

		for (size_t i = 0; i < size; i++) {
			auto& m = collection.get<MovementData>(i);
			auto& d = collection.get<ParticleData>(i);
			auto& p = collection.get<ParticleInfo>(i);

			data.read(reinterpret_cast<char*>(&m.position), sizeof(m.position));
			data.read(reinterpret_cast<char*>(&m.velocity), sizeof(m.velocity));
			data.read(reinterpret_cast<char*>(&m.acceleration), sizeof(m.acceleration));

			data.read(reinterpret_cast<char*>(&d.mass), sizeof(d.mass));
			data.read(reinterpret_cast<char*>(&d.density), sizeof(d.density));
			data.read(reinterpret_cast<char*>(&d.pressure), sizeof(d.pressure));

			data.read(reinterpret_cast<char*>(&p.type), sizeof(p.tag));

		}

	}

	void SimulationSerializer::save_particles(ParticleCollection& collection, const std::string& filepath)
	{
		FLUID_ASSERT(collection.is_type_present<MovementData>());
		FLUID_ASSERT(collection.is_type_present<ParticleInfo>());
		FLUID_ASSERT(collection.is_type_present<ParticleData>());

		std::ofstream data(filepath, std::ios::out | std::ios::binary);

		// size of the collection	
		size_t size = collection.size();
		data.write(reinterpret_cast<char*>(&size), sizeof(size));

		for (size_t i = 0; i < collection.size(); i++) {

			auto& m = collection.get<MovementData>(i);
			auto& d = collection.get<ParticleData>(i);
			auto& p = collection.get<ParticleInfo>(i);

			data.write(reinterpret_cast<char*>(&m.position), sizeof(m.position));
			data.write(reinterpret_cast<char*>(&m.velocity), sizeof(m.velocity));
			data.write(reinterpret_cast<char*>(&m.acceleration), sizeof(m.acceleration));

			data.write(reinterpret_cast<char*>(&d.mass), sizeof(d.mass));
			data.write(reinterpret_cast<char*>(&d.density), sizeof(d.density));
			data.write(reinterpret_cast<char*>(&d.pressure), sizeof(d.pressure));

			data.write(reinterpret_cast<char*>(&p.type), sizeof(p.tag));

		}

	}


}