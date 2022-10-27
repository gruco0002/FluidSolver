#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class TagDescriptorSettingsElement final : public UiElement {
      public:
        void update() override;
    };
} // namespace FluidStudio
