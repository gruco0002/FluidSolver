#include "StatisticsUi.hpp"

#include "core/FluidInclude.hpp"
#include "core/sensors/ParticleStatistics.hpp"
#include "uiVersion/FluidSolverWindow.hpp"
#include "uiVersion/ImguiHelper.hpp"

#include <implot.h>


void FluidUi::StatisticsUi::render()
{


    if (!window->is_safe_to_access_simulation_data())
    {
        return;
    }

    for (size_t i = 0; i < window->simulation.parameters.sensors.size(); i++)
    {

        if (is_sensor_window_open(i))
        {

            auto sen = window->simulation.parameters.sensors[i];

            if (ImGui::Begin(sen->parameters.name.c_str(), sensor_window_open_ptr(i)))
            {

                if (dynamic_cast<FluidSolver::Sensors::GlobalDensitySensor*>(sen))
                {
                    render_density_sensor_graph(dynamic_cast<FluidSolver::Sensors::GlobalDensitySensor*>(sen));
                }
                else if (dynamic_cast<FluidSolver::Sensors::GlobalPressureSensor*>(sen))
                {
                    render_pressure_sensor_graph(dynamic_cast<FluidSolver::Sensors::GlobalPressureSensor*>(sen));
                }
                else if (dynamic_cast<FluidSolver::Sensors::GlobalVelocitySensor*>(sen))
                {
                    render_velocity_sensor_graph(dynamic_cast<FluidSolver::Sensors::GlobalVelocitySensor*>(sen));
                }
                else if (dynamic_cast<FluidSolver::Sensors::GlobalEnergySensor*>(sen))
                {
                    render_energy_sensor_graph(dynamic_cast<FluidSolver::Sensors::GlobalEnergySensor*>(sen));
                }
                else if (dynamic_cast<FluidSolver::Sensors::GlobalParticleCountSensor*>(sen))
                {
                    render_particle_count_sensor(dynamic_cast<FluidSolver::Sensors::GlobalParticleCountSensor*>(sen));
                }
            }

            ImGui::End();
        }
    }
}

void FluidUi::StatisticsUi::initialize()
{
    FLUID_ASSERT(window != nullptr)
}

void FluidUi::StatisticsUi::open_sensor_window(size_t index)
{
    if (open_sensor_windows.size() <= index)
        open_sensor_windows.resize(index + 1, false);
    open_sensor_windows[index] = true;
}

void plot_mmadata(const FluidSolver::SensorData<FluidSolver::Sensors::MMAData>& data)
{

    ImPlot::PlotLineG(
        "Avg",
        [](void* data, int x) {
            auto s = (FluidSolver::SensorData<FluidSolver::Sensors::MMAData>*)data;
            return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].average);
        },
        (void*)&data, data.size());

    ImPlot::PlotLineG(
        "Min",
        [](void* data, int x) {
            auto s = (FluidSolver::SensorData<FluidSolver::Sensors::MMAData>*)data;
            return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].minimum);
        },
        (void*)&data, data.size());

    ImPlot::PlotLineG(
        "Max",
        [](void* data, int x) {
            auto s = (FluidSolver::SensorData<FluidSolver::Sensors::MMAData>*)data;
            return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].maximum);
        },
        (void*)&data, data.size());
}

bool FluidUi::StatisticsUi::is_sensor_window_open(size_t index)
{
    if (open_sensor_windows.size() <= index)
        return false;
    return open_sensor_windows[index];
}

bool* FluidUi::StatisticsUi::sensor_window_open_ptr(size_t index)
{
    if (open_sensor_windows.size() <= index)
    {
        open_sensor_windows.resize(index + 1, false);
    }
    return (bool*)&(open_sensor_windows[index]);
}

void FluidUi::StatisticsUi::render_density_sensor_graph(FluidSolver::Sensors::GlobalDensitySensor* sensor)
{
    if (ImPlot::BeginPlot("Density"))
    {
        plot_mmadata(sensor->data);
        ImPlot::EndPlot();
    }
}

void FluidUi::StatisticsUi::render_pressure_sensor_graph(FluidSolver::Sensors::GlobalPressureSensor* sensor)
{
    if (ImPlot::BeginPlot("Pressure"))
    {
        plot_mmadata(sensor->data);
        ImPlot::EndPlot();
    }
}

void FluidUi::StatisticsUi::render_velocity_sensor_graph(FluidSolver::Sensors::GlobalVelocitySensor* sensor)
{
    if (ImPlot::BeginPlot("Velocity"))
    {
        plot_mmadata(sensor->data);
        ImPlot::EndPlot();
    }
}

void FluidUi::StatisticsUi::render_energy_sensor_graph(FluidSolver::Sensors::GlobalEnergySensor* sensor)
{
    if (ImPlot::BeginPlot("Energy"))
    {

        ImPlot::PlotLineG(
            "Kin",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::GlobalEnergySensor::EnergyData>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].kinetic);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Pot",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::GlobalEnergySensor::EnergyData>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].potential);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::EndPlot();
    }
}

void FluidUi::StatisticsUi::render_particle_count_sensor(FluidSolver::Sensors::GlobalParticleCountSensor* sensor)
{
    if (ImPlot::BeginPlot("Energy"))
    {

        ImPlot::PlotLineG(
            "Normal",
            [](void* data, int x) {
                auto s =
                    (FluidSolver::SensorData<FluidSolver::Sensors::GlobalParticleCountSensor::ParticleCountData>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].normal_particles);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Boundary",
            [](void* data, int x) {
                auto s =
                    (FluidSolver::SensorData<FluidSolver::Sensors::GlobalParticleCountSensor::ParticleCountData>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].boundary_particles);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Inactive",
            [](void* data, int x) {
                auto s =
                    (FluidSolver::SensorData<FluidSolver::Sensors::GlobalParticleCountSensor::ParticleCountData>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].inactive_particles);
            },
            (void*)&sensor->data, sensor->data.size());


        ImPlot::EndPlot();
    }
}
