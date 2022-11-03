#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class NewSimulationModalWindow final : public UiElement {
      public:
        enum class NewSimulationKind {
            NewSimulationKind2D,
            NewSimulationKind3D
        };

        void update() override;

        void open_window(NewSimulationKind kind);

      private:
        bool should_open = false;
        NewSimulationKind kind;

        float rest_density = 1000.0f;
        float particle_size = 0.1f;

        void reset();
    };
} // namespace FluidStudio
