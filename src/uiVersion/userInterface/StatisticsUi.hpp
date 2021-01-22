#ifndef FLUIDSOLVER_STATISTICSUI_HPP
#define FLUIDSOLVER_STATISTICSUI_HPP

#include <core/sensors/ParticleStatistics.hpp>

namespace FluidUi {
	class FluidSolverWindow;

	class StatisticsUi {

	public:

		FluidSolverWindow* window = nullptr;

		void render();
		void initialize();

	private:
		void render_density_sensor_graph(FluidSolver::Sensors::GlobalDensitySensor* sensor);
		void render_pressure_sensor_graph(FluidSolver::Sensors::GlobalPressureSensor* sensor);
		void render_velocity_sensor_graph(FluidSolver::Sensors::GlobalVelocitySensor* sensor);

	};

}


#endif //FLUIDSOLVER_STATISTICSUI_HPP
