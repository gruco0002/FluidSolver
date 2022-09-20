#pragma once

#include "userInterface/UiData.hpp"

namespace FluidStudio {
    struct UiElement {
      public:
        UiData ui_data;

        virtual void initialize(const UiData& ui_data);

        virtual void update() = 0;

        virtual ~UiElement();

    };
}
