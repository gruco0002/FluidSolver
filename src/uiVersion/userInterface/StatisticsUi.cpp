#include "StatisticsUi.hpp"
#include "core/FluidInclude.hpp"
#include "uiVersion/ImguiHelper.hpp"
#include <implot.h>
#include "uiVersion/FluidSolverWindow.hpp"
#include "core/sensors/ParticleStatistics.hpp"


void FluidUi::StatisticsUi::render() {


	if (!window->is_safe_to_access_simulation_data()) {
		return;
	}

	for (auto sen : window->simulation.parameters.sensors) {
		ImGui::Begin(sen->parameters.name.c_str());

		if (dynamic_cast<FluidSolver::Sensors::GlobalDensitySensor*>(sen)) {
			render_density_sensor_graph(dynamic_cast<FluidSolver::Sensors::GlobalDensitySensor*>(sen));
		}
		else if (dynamic_cast<FluidSolver::Sensors::GlobalPressureSensor*>(sen)) {
			render_pressure_sensor_graph(dynamic_cast<FluidSolver::Sensors::GlobalPressureSensor*>(sen));
		}
		else if (dynamic_cast<FluidSolver::Sensors::GlobalVelocitySensor*>(sen)) {
			render_velocity_sensor_graph(dynamic_cast<FluidSolver::Sensors::GlobalVelocitySensor*>(sen));
		}

		ImGui::End();
	}



}

void FluidUi::StatisticsUi::initialize() {
	FLUID_ASSERT(window != nullptr)
}

void plot_mmadata(const FluidSolver::SensorData< FluidSolver::Sensors::MMAData>& data) {

	ImPlot::PlotLineG("Avg", [](void* data, int x) {
		auto s = (FluidSolver::SensorData< FluidSolver::Sensors::MMAData>*)data;
		return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].average);
		}, (void*)&data, data.size());

	ImPlot::PlotLineG("Min", [](void* data, int x) {
		auto s = (FluidSolver::SensorData< FluidSolver::Sensors::MMAData>*)data;
		return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].minimum);
		}, (void*)&data, data.size());

	ImPlot::PlotLineG("Max", [](void* data, int x) {
		auto s = (FluidSolver::SensorData< FluidSolver::Sensors::MMAData>*)data;
		return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].maximum);
		}, (void*)&data, data.size());

}

void FluidUi::StatisticsUi::render_density_sensor_graph(FluidSolver::Sensors::GlobalDensitySensor* sensor)
{
	if (ImPlot::BeginPlot("Density")) {
		plot_mmadata(sensor->data);
		ImPlot::EndPlot();
	}
}

void FluidUi::StatisticsUi::render_pressure_sensor_graph(FluidSolver::Sensors::GlobalPressureSensor* sensor)
{
	if (ImPlot::BeginPlot("Pressure")) {
		plot_mmadata(sensor->data);
		ImPlot::EndPlot();
	}
}

void FluidUi::StatisticsUi::render_velocity_sensor_graph(FluidSolver::Sensors::GlobalVelocitySensor* sensor)
{
	if (ImPlot::BeginPlot("Velocity")) {
		plot_mmadata(sensor->data);
		ImPlot::EndPlot();
	}
}


