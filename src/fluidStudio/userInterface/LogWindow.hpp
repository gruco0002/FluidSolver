#pragma once

#include <string>
#include <vector>

namespace FluidStudio {

    class FluidSolverWindow;

    class LogWindow {

      public:
        FluidSolverWindow* window = nullptr;

        void render();

        void initialize();

        bool scroll_to_bottom = true;

      private:
        std::vector<std::string> messages;
    };

} // namespace FluidUi
