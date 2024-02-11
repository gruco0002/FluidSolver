#pragma once
#include "ComponentRunner.hpp"
#include "LibFluidForward.hpp"

namespace FluidStudio
{
    class VisualizationRunner : public ComponentRunner
    {
      protected:
        void execute_computation() override;

      public:
        std::shared_ptr<LibFluid::ISimulationVisualizer> visualizer = nullptr;
    };
} // namespace FluidStudio
