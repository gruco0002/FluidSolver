#pragma once

#include "helpers/CompatibilityReport.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"
#include "LibFluidForward.hpp"

#include <memory>

namespace LibFluid {
    class SimulatorVisualizerBundle : public Initializable,  public Reportable {
      public:
        std::shared_ptr<Simulator> simulator = nullptr;
        std::shared_ptr<ISimulationVisualizer> visualizer = nullptr;


        void create_compatibility_report(CompatibilityReport &report) override;

        void initialize() override;
    };
}