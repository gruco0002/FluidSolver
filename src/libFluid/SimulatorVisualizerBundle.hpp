#pragma once

#include "FluidInclude.hpp"

namespace FluidSolver {
    class SimulatorVisualizerBundle {
      public:
        std::shared_ptr<Simulation> simulation = nullptr;
        std::shared_ptr<ISimulationVisualizer> visualizer = nullptr;


    };
}