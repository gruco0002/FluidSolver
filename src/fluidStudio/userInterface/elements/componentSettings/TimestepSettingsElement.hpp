#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio
{
    class TimestepSettingsElement final : public UiElement
    {
      public:
        void update() override;
    };
} // namespace FluidStudio
