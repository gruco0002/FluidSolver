#include "SimulationSerializer.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <core/entities/ParticleSpawner.hpp>
#include <core/entities/ParticleRemover.hpp>

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

	Simulation SimulationSerializer::load_from_file(const std::string& filepath)
	{
		return Simulation();
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