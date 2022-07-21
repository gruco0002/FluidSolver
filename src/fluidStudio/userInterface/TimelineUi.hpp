#pragma once

namespace FluidStudio {
    class FluidSolverWindow;

    class TimelineUi {
      public:
        FluidSolverWindow* window = nullptr;

        void render();

      private:
        int current_index = 0;
    };
} // namespace FluidUi