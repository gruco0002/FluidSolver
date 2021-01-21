#ifndef FLUIDSOLVER_PARTICLESTATISTICS_HPP
#define FLUIDSOLVER_PARTICLESTATISTICS_HPP

#include "core/fluidSolver/ParticleCollection.hpp"
#include "ISensor.hpp"
#include "SensorData.hpp"

namespace FluidSolver {

	struct ParticleStatistics {
		float average_density;
		float max_density;
		float min_density;
		float average_pressure;
		float min_pressure;
		float max_pressure;

		size_t normal_particles;
		size_t boundary_particles;
		size_t inactive_particles;

		float potential_energy;
		float kinetic_energy;
		float average_velocity;
		float min_velocity;
		float max_velocity;

		static ParticleStatistics fill_data(ParticleCollection* collection);
	};

	class ParticleStatisticsSensor : public ISensor {
	public:

		SensorData<ParticleStatistics> data;

		void initialize() override;

		void calculate_and_store(const Timepoint& timepoint) override;
	};

}
#endif //FLUIDSOLVER_PARTICLESTATISTICS_HPP
