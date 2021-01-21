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

		if (dynamic_cast<FluidSolver::ParticleStatisticsSensor*>(sen)) {
			render_particle_statistics(dynamic_cast<FluidSolver::ParticleStatisticsSensor*>(sen));
		}

		ImGui::End();
	}



}

void FluidUi::StatisticsUi::initialize() {
	FLUID_ASSERT(window != nullptr)
}

void FluidUi::StatisticsUi::render_particle_statistics(FluidSolver::ParticleStatisticsSensor* sensor) {

	if (ImPlot::BeginPlot("Density")) {

		ImPlot::PlotLineG("Avg", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].average_density);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Min", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].min_density);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Max", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].max_density);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::EndPlot();
	}

	if (ImPlot::BeginPlot("Pressure")) {

		ImPlot::PlotLineG("Avg", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].average_pressure);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Min", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].min_pressure);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Max", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].max_pressure);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::EndPlot();
	}

	if (ImPlot::BeginPlot("Velocity")) {

		ImPlot::PlotLineG("Avg", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].average_velocity);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Min", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].min_velocity);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Max", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].max_pressure);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::EndPlot();
	}

	if (ImPlot::BeginPlot("Particles")) {

		ImPlot::PlotLineG("Normal", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].normal_particles);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Boundary", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].boundary_particles);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Inactive", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].inactive_particles);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::EndPlot();
	}

	if (ImPlot::BeginPlot("Energy")) {

		ImPlot::PlotLineG("Kin", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].kinetic_energy);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Pot", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].potential_energy);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::EndPlot();
	}


}
