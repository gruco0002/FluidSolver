#include "EditorVisualizationRunner.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"

namespace FluidStudio {

    void EditorVisualizationRunner::execute_computation() {
        render_visualization(std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(visualizer));
        render_visualization(std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(visualizer));
    }
    EditorVisualizationRunner::EditorVisualizationRunner() {
        run_asynchronously = false;
    }
    void EditorVisualizationRunner::render_visualization(std::shared_ptr<LibFluid::ISimulationVisualizer> simulation_visualizer) {
        if (simulation_visualizer == nullptr)
            return;

        LibFluid::CompatibilityReport report;
        simulation_visualizer->create_compatibility_report(report);
        if (report.has_issues()) {
            report.log_issues();
            return;
        }

        simulation_visualizer->render();
    }
} // namespace FluidStudio