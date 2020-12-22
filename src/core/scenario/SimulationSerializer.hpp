#pragma once

#include <core/Simulation.hpp>

namespace FluidSolver {

	class SimulationSerializer {

	public:
		static Simulation load_from_file(const std::string& filepath);

		static void save_to_file(const Simulation& simulation, const std::string& filepath, const std::string& data_filepath);


		static void load_particles(ParticleCollection& collection, const std::string& filepath);

		static void save_particles(ParticleCollection& collection, const std::string& filepath);

	};

}
