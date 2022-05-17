#pragma once

#include "CompatibilityReport.hpp"
#include "FluidInclude.hpp"

namespace FluidSolver {
    class SimulatorVisualizerBundle {
      public:
        std::shared_ptr<Simulator> simulation = nullptr;
        std::shared_ptr<ISimulationVisualizer> visualizer = nullptr;


        void create_compatibility_report(CompatibilityReport &report);
    };
}