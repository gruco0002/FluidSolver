#pragma once

#include "CompatibilityReport.hpp"
#include "FluidInclude.hpp"
#include "Reportable.hpp"
#include "Initializable.hpp"

namespace FluidSolver {
    class SimulatorVisualizerBundle : public Initializable,  public Reportable {
      public:
        std::shared_ptr<Simulator> simulation = nullptr;
        std::shared_ptr<ISimulationVisualizer> visualizer = nullptr;


        void create_compatibility_report(CompatibilityReport &report) override;

        void initialize() override;
    };
}