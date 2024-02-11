#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio
{

    class TimelineWindow final : public UiElement
    {
      public:
        void update() override;

      private:
        int current_index = 0;
    };
} // namespace FluidStudio
