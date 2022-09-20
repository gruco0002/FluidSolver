#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class VisualizerSettingsElement final : public UiElement {
      public:
        void update() override;
    };
} // namespace FluidStudio
