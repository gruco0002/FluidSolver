#ifndef FLUIDSOLVER_ITIMESTEP_HPP
#define FLUIDSOLVER_ITIMESTEP_HPP

#include <chrono>
#include <core/fluidSolver/ParticleCollection.hpp>

namespace FluidSolver {

	struct Timepoint {
		std::chrono::time_point<std::chrono::system_clock> system_time = {};
		float simulation_time = 0;
		float current_time_step = 0;
	};

	class ITimestep {

	protected:

		float current_timestep = 0.0f;

	public:
		struct TimestepParameters {
			ParticleCollection* particle_collection = nullptr;
			float particle_size = 1.0f;
		} parameters;

		virtual void calculate_current_timestep() = 0;

		float get_current_timestep() const;

		virtual ~ITimestep() = default;
	};
}


#endif //FLUIDSOLVER_ITIMESTEP_HPP
