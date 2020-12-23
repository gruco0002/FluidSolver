#pragma once

#include <core/Simulation.hpp>

namespace FluidSolver {

	class SimulationSerializer {

	public:

		struct Settings {
			bool save_particle_data = true;
			std::string particle_data_relative_filepath = "particles.data";
		};

		static Simulation load_from_file(const std::string& filepath);

		static void save_to_file(const Simulation& simulation, const std::string& filepath, const Settings& settings);


		static void load_particles(ParticleCollection& collection, const std::string& filepath);

		static void save_particles(ParticleCollection& collection, const std::string& filepath);

	};

}
