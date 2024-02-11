#include "SensorGraphWindows.hpp"

#include "FluidSolverWindow.hpp"
#include "ImguiHelper.hpp"
#include "sensors/IisphSensor.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorDataStore.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <implot.h>

namespace FluidStudio
{
    void SensorGraphWindows::open_sensor_window(size_t index)
    {
        if (open_sensor_windows.size() <= index)
            open_sensor_windows.resize(index + 1, false);
        open_sensor_windows[index] = true;
    }

    void plot_mmadata(const LibFluid::SensorDataStore<LibFluid::Sensors::MaxMinAvgSensorData> &data)
    {
        ImPlot::PlotLineG(
            "Avg",
            [](int x, void *data) {
                auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::MaxMinAvgSensorData> *)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].average);
            },
            (void *)&data, data.size());

        ImPlot::PlotLineG(
            "Min",
            [](int x, void *data) {
                auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::MaxMinAvgSensorData> *)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].minimum);
            },
            (void *)&data, data.size());

        ImPlot::PlotLineG(
            "Max",
            [](int x, void *data) {
                auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::MaxMinAvgSensorData> *)data;
                return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].maximum);
            },
            (void *)&data, data.size());
    }

    bool SensorGraphWindows::is_sensor_window_open(size_t index)
    {
        if (open_sensor_windows.size() <= index)
            return false;
        return open_sensor_windows[index];
    }

    bool *SensorGraphWindows::sensor_window_open_ptr(size_t index)
    {
        if (open_sensor_windows.size() <= index)
        {
            open_sensor_windows.resize(index + 1, false);
        }
        return (bool *)&(open_sensor_windows[index]);
    }

    void SensorGraphWindows::render_density_sensor_graph(std::shared_ptr<LibFluid::Sensors::GlobalDensitySensor> sensor)
    {
        if (ImPlot::BeginPlot("Density"))
        {
            plot_mmadata(sensor->get_sensor_data_store());
            ImPlot::EndPlot();
        }
    }

    void SensorGraphWindows::render_pressure_sensor_graph(
        std::shared_ptr<LibFluid::Sensors::GlobalPressureSensor> sensor)
    {
        if (ImPlot::BeginPlot("Pressure"))
        {
            plot_mmadata(sensor->get_sensor_data_store());
            ImPlot::EndPlot();
        }
    }

    void SensorGraphWindows::render_velocity_sensor_graph(
        std::shared_ptr<LibFluid::Sensors::GlobalVelocitySensor> sensor)
    {
        if (ImPlot::BeginPlot("Velocity"))
        {
            plot_mmadata(sensor->get_sensor_data_store());
            ImPlot::EndPlot();
        }
    }

    void SensorGraphWindows::render_energy_sensor_graph(std::shared_ptr<LibFluid::Sensors::GlobalEnergySensor> sensor)
    {
        if (ImPlot::BeginPlot("Energy"))
        {
            ImPlot::PlotLineG(
                "Kin",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::EnergySensorData> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].kinetic);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Pot",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::EnergySensorData> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].potential);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::EndPlot();
        }
    }

    void SensorGraphWindows::render_particle_count_sensor(
        std::shared_ptr<LibFluid::Sensors::GlobalParticleCountSensor> sensor)
    {
        if (ImPlot::BeginPlot("Energy"))
        {
            ImPlot::PlotLineG(
                "Normal",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::ParticleCountSensorData> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].normal_particles);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Boundary",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::ParticleCountSensorData> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].boundary_particles);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Inactive",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::ParticleCountSensorData> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].inactive_particles);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::EndPlot();
        }
    }

    void SensorGraphWindows::render_sensor_plane_sensor(std::shared_ptr<LibFluid::Sensors::SensorPlane> sensor)
    {
        auto &image = sensor->get_last_image();

        LibFluid::ISimulationVisualizer::Size size;
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

            ImGui::Image((ImTextureID)rendered_image->GetID(), {width, height});
        }
    }

    void SensorGraphWindows::render_compressed_neighborhood_storage_sensor(
        std::shared_ptr<LibFluid::Sensors::CompressedNeighborStorageSensor> sensor)
    {
        if (ImPlot::BeginPlot("Neighborhood Storage"))
        {
            ImPlot::PlotLineG(
                "Neighbors Avg",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::CompressedNeighborStorageSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].neighbor_count_average);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Neighbors Max",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::CompressedNeighborStorageSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].neighbor_count_maximum);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Neighbors Min",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::CompressedNeighborStorageSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].neighbor_count_minimum);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Delta Bytes Avg",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::CompressedNeighborStorageSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].used_delta_bytes_average);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Delta Bytes Max",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::CompressedNeighborStorageSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].used_delta_bytes_maximum);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Delta Bytes Min",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::CompressedNeighborStorageSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].used_delta_bytes_minimum);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::EndPlot();
        }
    }

    void SensorGraphWindows::render_iisph_sensor(std::shared_ptr<LibFluid::Sensors::IISPHSensor> sensor)
    {
        if (ImPlot::BeginPlot("Neighborhood Storage"))
        {
            ImPlot::PlotLineG(
                "Iteration Count",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::IISPHSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time, s->data()[x].stat_last_iteration_count);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::PlotLineG(
                "Avg Pred Density Error",
                [](int x, void *data) {
                    auto s = (LibFluid::SensorDataStore<LibFluid::Sensors::IISPHSensorInfo> *)data;
                    return ImPlotPoint(s->times()[x].simulation_time,
                                       s->data()[x].stat_last_average_predicted_density_error);
                },
                (void *)&sensor->get_sensor_data_store(), sensor->get_sensor_data_store().size());

            ImPlot::EndPlot();
        }
    }

    void SensorGraphWindows::update()
    {
        if (!ui_data.window().is_safe_to_access_simulation_data())
        {
            return;
        }

        for (size_t i = 0; i < ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.size(); i++)
        {
            if (is_sensor_window_open(i))
            {
                auto sen = ui_data.window().simulator_visualizer_bundle.simulator->data.sensors[i];

                if (ImGui::Begin(sen->parameters.name.c_str(), sensor_window_open_ptr(i)))
                {
                    if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalDensitySensor>(sen))
                    {
                        render_density_sensor_graph(
                            std::dynamic_pointer_cast<LibFluid::Sensors::GlobalDensitySensor>(sen));
                    }
                    else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalPressureSensor>(sen))
                    {
                        render_pressure_sensor_graph(
                            std::dynamic_pointer_cast<LibFluid::Sensors::GlobalPressureSensor>(sen));
                    }
                    else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalVelocitySensor>(sen))
                    {
                        render_velocity_sensor_graph(
                            std::dynamic_pointer_cast<LibFluid::Sensors::GlobalVelocitySensor>(sen));
                    }
                    else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalEnergySensor>(sen))
                    {
                        render_energy_sensor_graph(
                            std::dynamic_pointer_cast<LibFluid::Sensors::GlobalEnergySensor>(sen));
                    }
                    else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalParticleCountSensor>(sen))
                    {
                        render_particle_count_sensor(
                            std::dynamic_pointer_cast<LibFluid::Sensors::GlobalParticleCountSensor>(sen));
                    }
                    else if (std::dynamic_pointer_cast<LibFluid::Sensors::SensorPlane>(sen))
                    {
                        render_sensor_plane_sensor(std::dynamic_pointer_cast<LibFluid::Sensors::SensorPlane>(sen));
                    }
                    else if (std::dynamic_pointer_cast<LibFluid::Sensors::CompressedNeighborStorageSensor>(sen))
                    {
                        render_compressed_neighborhood_storage_sensor(
                            std::dynamic_pointer_cast<LibFluid::Sensors::CompressedNeighborStorageSensor>(sen));
                    }
                    else if (std::dynamic_pointer_cast<LibFluid::Sensors::IISPHSensor>(sen))
                    {
                        render_iisph_sensor(std::dynamic_pointer_cast<LibFluid::Sensors::IISPHSensor>(sen));
                    }
                }

                ImGui::End();
            }
        }
    }
} // namespace FluidStudio
