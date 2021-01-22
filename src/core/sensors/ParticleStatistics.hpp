#ifndef FLUIDSOLVER_PARTICLESTATISTICS_HPP
#define FLUIDSOLVER_PARTICLESTATISTICS_HPP

#include "core/fluidSolver/ParticleCollection.hpp"
#include "ISensor.hpp"
#include "SensorData.hpp"
#include <limits>

namespace FluidSolver {

	namespace Sensors {

		struct MMAData {
			float average = 0.0f;
			float maximum = std::numeric_limits<float>::min();
			float minimum = std::numeric_limits<float>::max();
		};

		class GlobalDensitySensor : public ISensor {
		public:

			SensorData<MMAData> data;

			virtual void initialize() override;
			virtual void calculate_and_store(const Timepoint& timepoint) override;
		};

		class GlobalPressureSensor : public ISensor {
		public:

			SensorData<MMAData> data;

			virtual void initialize() override;
			virtual void calculate_and_store(const Timepoint& timepoint) override;
		};

		class GlobalVelocitySensor : public ISensor {
		public:

			SensorData<MMAData> data;

			virtual void initialize() override;
			virtual void calculate_and_store(const Timepoint& timepoint) override;
		};

		class GlobalEnergySensor : public ISensor {
		public:


			struct EnergyData {
				float potential = 0.0f;
				float kinetic = 0.0f;
			};

			struct EnergySensorSettings {
				float relative_zero_height = 0.0f;
			} settings;

			SensorData<EnergyData> data;

			virtual void initialize() override;
			virtual void calculate_and_store(const Timepoint& timepoint) override;
		};

		class GlobalParticleCountSensor : public ISensor {
		public:

			struct ParticleCountData {
				size_t normal_particles = 0;
				size_t boundary_particles = 0;
				size_t inactive_particles = 0;
			};

			SensorData<ParticleCountData> data;


			virtual void initialize() override;
			virtual void calculate_and_store(const Timepoint& timepoint) override;
		};


	}

}
#endif //FLUIDSOLVER_PARTICLESTATISTICS_HPP
