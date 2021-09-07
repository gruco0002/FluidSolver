#include "StatisticsUi.hpp"

#include "FluidInclude.hpp"
#include "FluidSolverWindow.hpp"
#include "ImguiHelper.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

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

                if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalDensitySensor>(sen))
                {
                    render_density_sensor_graph(
                        std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalDensitySensor>(sen));
                }
                else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalPressureSensor>(sen))
                {
                    render_pressure_sensor_graph(
                        std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalPressureSensor>(sen));
                }
                else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalVelocitySensor>(sen))
                {
                    render_velocity_sensor_graph(
                        std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalVelocitySensor>(sen));
                }
                else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalEnergySensor>(sen))
                {
                    render_energy_sensor_graph(
                        std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalEnergySensor>(sen));
                }
                else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalParticleCountSensor>(sen))
                {
                    render_particle_count_sensor(
                        std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalParticleCountSensor>(sen));
                }
                else if (std::dynamic_pointer_cast<FluidSolver::Sensors::SensorPlane>(sen))
                {
                    render_sensor_plane_sensor(std::dynamic_pointer_cast<FluidSolver::Sensors::SensorPlane>(sen));
                }
                else if (std::dynamic_pointer_cast<FluidSolver::Sensors::CompressedNeighborStorageSensor>(sen))
                {
                    render_compressed_neighborhood_storage_sensor(
                        std::dynamic_pointer_cast<FluidSolver::Sensors::CompressedNeighborStorageSensor>(sen));
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

void FluidUi::StatisticsUi::render_density_sensor_graph(
    std::shared_ptr<FluidSolver::Sensors::GlobalDensitySensor> sensor)
{
    if (ImPlot::BeginPlot("Density"))
    {
        plot_mmadata(sensor->data);
        ImPlot::EndPlot();
    }
}

void FluidUi::StatisticsUi::render_pressure_sensor_graph(
    std::shared_ptr<FluidSolver::Sensors::GlobalPressureSensor> sensor)
{
    if (ImPlot::BeginPlot("Pressure"))
    {
        plot_mmadata(sensor->data);
        ImPlot::EndPlot();
    }
}

void FluidUi::StatisticsUi::render_velocity_sensor_graph(
    std::shared_ptr<FluidSolver::Sensors::GlobalVelocitySensor> sensor)
{
    if (ImPlot::BeginPlot("Velocity"))
    {
        plot_mmadata(sensor->data);
        ImPlot::EndPlot();
    }
}

void FluidUi::StatisticsUi::render_energy_sensor_graph(std::shared_ptr<FluidSolver::Sensors::GlobalEnergySensor> sensor)
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

void FluidUi::StatisticsUi::render_particle_count_sensor(
    std::shared_ptr<FluidSolver::Sensors::GlobalParticleCountSensor> sensor)
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

void FluidUi::StatisticsUi::render_sensor_plane_sensor(std::shared_ptr<FluidSolver::Sensors::SensorPlane> sensor)
{


    auto& image = sensor->get_last_image();

    FluidSolver::ISimulationVisualizer::Size size;
    size.width = image.width();
    size.height = image.height();

    if (size.width == 0 || size.height == 0)
    {
        ImGui::Text("Currently no visualization data available!");
    }

    {
        // create or recreate the gpu image
        if (rendered_image == nullptr || rendered_image->getWidth() != size.width ||
            rendered_image->getHeight() != size.height)
        {
            delete rendered_image;
            rendered_image = nullptr;

            // create new image
            auto color_settings = new Engine::Graphics::Texture2DSettings();
            color_settings->GenerateMipmaps = false;
            color_settings->TextureMagnifyingFiltering = GL_NEAREST;
            color_settings->TextureMinifyingFiltering = GL_NEAREST;
            rendered_image = new Engine::Graphics::Texture2D(size.width, size.height, color_settings, GL_RGBA,
                                                             Engine::ComponentType::ComponentTypeUnsignedByte);
        }

        // update the gpu image
        rendered_image->SetData(image.data(), image.size());
    }

    if (rendered_image != nullptr)
    {
        auto max_width = ImGui::GetWindowWidth() - 20;
        auto max_height = ImGui::GetWindowHeight() - 80;
        float ratio = (float)rendered_image->getWidth() / (float)rendered_image->getHeight();

        float width = 1;
        float height = 1;

        if (ratio * max_height <= max_width)
        {
            height = max_height;
            width = ratio * max_height;
        }
        else
        {
            width = max_width;
            height = max_width / ratio;
        }

        ImGui::Image((ImTextureID)rendered_image->GetID(), {width, height}, {0, 1}, {1, 0});
    }
}

void FluidUi::StatisticsUi::render_compressed_neighborhood_storage_sensor(
    std::shared_ptr<FluidSolver::Sensors::CompressedNeighborStorageSensor> sensor)
{
    if (ImPlot::BeginPlot("Neighborhood Storage"))
    {

        ImPlot::PlotLineG(
            "Neighbors Avg",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::CompressedNeighborStorageSensor::Info>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].neighbor_count_average);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Neighbors Max",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::CompressedNeighborStorageSensor::Info>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].neighbor_count_maximum);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Neighbors Min",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::CompressedNeighborStorageSensor::Info>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].neighbor_count_minimum);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Delta Bytes Avg",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::CompressedNeighborStorageSensor::Info>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].used_delta_bytes_average);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Delta Bytes Max",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::CompressedNeighborStorageSensor::Info>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].used_delta_bytes_maximum);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::PlotLineG(
            "Delta Bytes Min",
            [](void* data, int x) {
                auto s = (FluidSolver::SensorData<FluidSolver::Sensors::CompressedNeighborStorageSensor::Info>*)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].used_delta_bytes_minimum);
            },
            (void*)&sensor->data, sensor->data.size());

        ImPlot::EndPlot();
    }
}
