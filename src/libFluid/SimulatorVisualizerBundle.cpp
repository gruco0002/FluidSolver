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
            visualizer->initialize();
        }
    }

} // namespace FluidSolver
