#include "SimulatorVisualizerBundle.hpp"

#include "Simulator.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

namespace FluidSolver {
    void SimulatorVisualizerBundle::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(SimulatorVisualizerBundle));
        if (simulation == nullptr) {
            report.add_issue("Simulation is null!");
        } else {
            simulation->create_compatibility_report(report);
        }

        if (visualizer != nullptr) {
            visualizer->create_compatibility_report(report);
        }

        report.end_scope();
    }

    void SimulatorVisualizerBundle::initialize() {
        if (simulation != nullptr) {
            simulation->manual_initialize();
        }
        if (visualizer != nullptr) {
            if (simulation != nullptr) {
                if (visualizer->simulation_data.collection != simulation->data.collection) {
                    visualizer->simulation_data.collection = simulation->data.collection;
                    visualizer->simulation_data.notify_that_data_changed();
                }

                if (visualizer->simulation_data.neighborhood_interface != simulation->get_neighborhood_interface()) {
                    visualizer->simulation_data.neighborhood_interface = simulation->get_neighborhood_interface();
                    visualizer->simulation_data.notify_that_data_changed();
                }

                if (visualizer->simulation_data.particle_size != simulation->parameters.particle_size) {
                    visualizer->simulation_data.particle_size = simulation->parameters.particle_size;
                    visualizer->simulation_data.notify_that_data_changed();
                }

                if (visualizer->simulation_data.rest_density != simulation->parameters.rest_density) {
                    visualizer->simulation_data.rest_density = simulation->parameters.rest_density;
                    visualizer->simulation_data.notify_that_data_changed();
                }
            }

            visualizer->initialize();
        }
    }

} // namespace FluidSolver
