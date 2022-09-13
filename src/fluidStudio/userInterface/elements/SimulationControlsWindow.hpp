#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class SimulationControlsWindow final : public UiElement {
      public:
        void update() override;
    };
} // namespace FluidStudio
