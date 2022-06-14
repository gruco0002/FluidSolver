#pragma once

namespace FluidUi {
    class FluidSolverWindow;

    class TimelineUi {
      public:
        FluidSolverWindow* window = nullptr;

        void render();

      private:
        int current_index = 0;
    };
} // namespace FluidUi