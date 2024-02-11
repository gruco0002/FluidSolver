#include "SimulatorVisualizerBundle.hpp"

#include "Simulator.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

namespace LibFluid
{
    void SimulatorVisualizerBundle::create_compatibility_report(CompatibilityReport &report)
    {
        initialize();

        report.begin_scope(FLUID_NAMEOF(SimulatorVisualizerBundle));
        if (simulator == nullptr)
        {
            report.add_issue("Simulation is null!");
        }
        else
        {
            simulator->create_compatibility_report(report);
        }

        if (visualizer != nullptr)
        {
            visualizer->create_compatibility_report(report);
        }

        report.end_scope();
    }

    void SimulatorVisualizerBundle::initialize()
    {
        if (simulator != nullptr)
        {
            simulator->manual_initialize();
        }
        if (visualizer != nullptr)
        {
            if (simulator != nullptr)
            {
                if (visualizer->simulation_data.collection != simulator->data.collection)
                {
                    visualizer->simulation_data.collection = simulator->data.collection;
                    visualizer->simulation_data.notify_that_data_changed();
                }

                if (visualizer->simulation_data.neighborhood_interface != simulator->get_neighborhood_interface())
                {
                    visualizer->simulation_data.neighborhood_interface = simulator->get_neighborhood_interface();
                    visualizer->simulation_data.notify_that_data_changed();
                }

                if (visualizer->simulation_data.particle_size != simulator->parameters.particle_size)
                {
                    visualizer->simulation_data.particle_size = simulator->parameters.particle_size;
                    visualizer->simulation_data.notify_that_data_changed();
                }

                if (visualizer->simulation_data.rest_density != simulator->parameters.rest_density)
                {
                    visualizer->simulation_data.rest_density = simulator->parameters.rest_density;
                    visualizer->simulation_data.notify_that_data_changed();
                }
            }

            visualizer->initialize();
        }
    }

} // namespace LibFluid
