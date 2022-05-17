#pragma once

#include "ComponentRunner.hpp"
#include "FluidInclude.hpp"

namespace FluidUi {
    class SimulationRunner : public ComponentRunner {
      public:

        SimulationRunner();

        std::shared_ptr<FluidSolver::Simulator> simulation = nullptr;

      protected:
        void execute_computation() override;
    };
} // namespace FluidUi
