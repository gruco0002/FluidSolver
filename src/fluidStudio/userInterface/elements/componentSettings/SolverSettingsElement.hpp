#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class SolverSettingsElement final : public UiElement {
      public:
        void update() override;

      private:
        void update_solver_parameters();
    };
} // namespace FluidStudio
