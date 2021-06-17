#ifndef FLUIDSOLVER_SCENARIOSWINDOW_HPP
#define FLUIDSOLVER_SCENARIOSWINDOW_HPP

#include <string>
#include <vector>

namespace FluidUi
{
    class FluidSolverWindow;

    class ScenariosWindow {
      public:
        FluidSolverWindow* window = nullptr;

        void render();

        void initialize();

        ~ScenariosWindow();

      private:
        std::vector<char*> scenarios;

        std::string path = "../scenarios/";

        int current_item = 0;

        bool initialized = false;

        void update_path();

        void load();
    };
} // namespace FluidUi

#endif // FLUIDSOLVER_SCENARIOSWINDOW_HPP
