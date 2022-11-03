#pragma once

#include "ComponentRunner.hpp"
#include "visualizer/GLRenderer.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <memory>

namespace FluidStudio {
    class EditorVisualizationRunner : public ComponentRunner {
      protected:
        void execute_computation() override;

      public:
        std::shared_ptr<LibFluid::GLRenderer> visualizer = nullptr;

        EditorVisualizationRunner();

      private:
        void render_visualization(std::shared_ptr<LibFluid::ISimulationVisualizer> simulation_visualizer);
    };
} // namespace FluidStudio
