#include "StatisticsUi.hpp"
#include "core/FluidInclude.hpp"
#include "uiVersion/ImguiHelper.hpp"
#include <implot.h>
#include "uiVersion/FluidSolverWindow.hpp"
#include "core/sensors/ParticleStatistics.hpp"


void FluidUi::StatisticsUi::render() {
    if (window->simulation.parameters.sensor_storage == nullptr)
        return;

    ImGui::Begin("Sensor Data");

    render_particle_statistics(window->simulation.parameters.sensor_storage);

    ImGui::End();
}

void FluidUi::StatisticsUi::initialize() {
    FLUID_ASSERT(window != nullptr)
}

void FluidUi::StatisticsUi::render_particle_statistics(FluidSolver::SensorDataStorage *storage) {

    using pair = const std::pair<FluidSolver::SensorDataStorage::Time, FluidSolver::ParticleStatistics>;

    auto &sensors = storage->used_sensor_ids<FluidSolver::ParticleStatistics>();

    for (auto &id : sensors) {
        auto &name = storage->get_name<FluidSolver::ParticleStatistics>(id);
        auto &data = storage->get<FluidSolver::ParticleStatistics>(id);

        if (ImPlot::BeginPlot(name.c_str())) {
            ImPlot::PlotLine("Average Density", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.average_density);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Min Density", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.min_density);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Max Density", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.max_density);
            }, (void *) data.data(), data.size());


            ImPlot::PlotLine("Average Pressure", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.average_pressure);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Min Pressure", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.min_pressure);
            }, (void *) data.data(), data.size());
            ImPlot::PlotLine("Max Pressure", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.max_pressure);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Average Velocity", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.average_velocity);
            }, (void *) data.data(), data.size());


            ImPlot::PlotLine("Max Velocity", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.max_velocity);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Min Velocity", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.min_velocity);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Pot. Energy", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.potential_energy);
            }, (void *) data.data(), data.size());
            ImPlot::PlotLine("Kin. Energy", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.kinetic_energy);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Normal Particles", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.normal_particles);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Inactive Particles", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.inactive_particles);
            }, (void *) data.data(), data.size());

            ImPlot::PlotLine("Boundary Particles", [](void *data, int x) {
                pair *tmp = (pair *) data;
                return ImPlotPoint(tmp[x].first.simulation_time, tmp[x].second.boundary_particles);
            }, (void *) data.data(), data.size());


            ImPlot::EndPlot();
        }

    }
}
