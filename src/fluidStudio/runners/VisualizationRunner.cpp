#include "VisualizationRunner.hpp"

#include "visualizer/ISimulationVisualizer.hpp"

namespace FluidUi {
    void VisualizationRunner::execute_computation() {
        if (visualizer == nullptr)
            return;

        FluidSolver::CompatibilityReport report;
        visualizer->create_compatibility_report(report);
        if (report.has_issues()) {
            report.log_issues();
            return;
        }

        visualizer->render();
    }
} // namespace FluidUi