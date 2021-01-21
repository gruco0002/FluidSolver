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

		ImPlot::PlotLineG("Average Density", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].average_density);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Min Density", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].min_density);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::PlotLineG("Max Density", [](void* data, int x) {
			auto s = (FluidSolver::SensorData< FluidSolver::ParticleStatistics>*)data;
			return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].max_density);
			}, (void*)&sensor->data, sensor->data.size());

		ImPlot::EndPlot();
	}

	// TODO: implement

	/*
		ImPlot::PlotLineG("Average Pressure", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.average_pressure);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Min Pressure", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.min_pressure);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Max Pressure", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.max_pressure);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Average Velocity", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.average_velocity);
			}, (void*)data.data(), data.size());


		ImPlot::PlotLineG("Max Velocity", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.max_velocity);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Min Velocity", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.min_velocity);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Pot. Energy", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.potential_energy);
			}, (void*)data.data(), data.size());
		ImPlot::PlotLineG("Kin. Energy", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.kinetic_energy);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Normal Particles", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.normal_particles);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Inactive Particles", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.inactive_particles);
			}, (void*)data.data(), data.size());

		ImPlot::PlotLineG("Boundary Particles", [](void* data, int x) {
			pair* tmp = (pair*)data;
			return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.boundary_particles);
			}, (void*)data.data(), data.size());


	*/

}
