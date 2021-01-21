#ifndef FLUIDSOLVER_PARTICLESTATISTICS_HPP
#define FLUIDSOLVER_PARTICLESTATISTICS_HPP

#include "core/fluidSolver/ParticleCollection.hpp"
#include "ISensor.hpp"
#include "SensorData.hpp"
#include <limits>

namespace FluidSolver {

	struct ParticleStatistics {
		float average_density = 0.0f;
		float max_density = std::numeric_limits<float>::min();
		float min_density = std::numeric_limits<float>::max();
		float average_pressure = 0.0f;
		float min_pressure = std::numeric_limits<float>::max();
		float max_pressure = std::numeric_limits<float>::min();

		size_t normal_particles = 0;
		size_t boundary_particles = 0;
		size_t inactive_particles = 0;

		float potential_energy = 0;
		float kinetic_energy = 0;

		float average_velocity = 0;
		float min_velocity = std::numeric_limits<float>::max();
		float max_velocity = std::numeric_limits<float>::min();

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
