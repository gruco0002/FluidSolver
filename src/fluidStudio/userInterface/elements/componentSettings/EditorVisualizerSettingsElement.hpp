#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class EditorVisualizerSettingsElement final : public UiElement {
      public:
        void update() override;
    };
} // namespace FluidStudio
