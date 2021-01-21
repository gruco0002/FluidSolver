#ifndef FLUIDSOLVER_ISENSOR_HPP
#define FLUIDSOLVER_ISENSOR_HPP

#include "core/timestep/ITimestep.hpp"

namespace FluidSolver {

	struct SimulationParameters;

	class ISensor {

	public:

		struct SensorParameters {
			std::string name;
			bool save_to_file = false;
			SimulationParameters* simulation_parameters = nullptr;
		} parameters;


		virtual void initialize() = 0;

		virtual void calculate_and_store(const Timepoint& timepoint) = 0;



	};
}

#endif //FLUIDSOLVER_ISENSOR_HPP
