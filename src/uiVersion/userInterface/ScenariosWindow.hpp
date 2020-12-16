#ifndef FLUIDSOLVER_SCENARIOSWINDOW_HPP
#define FLUIDSOLVER_SCENARIOSWINDOW_HPP

#include <vector>
#include <string>
#include "PATH.hpp"

namespace FluidUi {
    class FluidSolverWindow;

    class ScenariosWindow {
    public:
        FluidSolverWindow *window = nullptr;

        void render();

        void initialize();

        ~ScenariosWindow();

    private:
        std::vector<char *> scenarios;

        std::string path = FLUID_ROOT_DIR + std::string("scenarios/");

        int current_item = 0;

        bool initialized = false;

        void update_path();

        void load();


    };
}

#endif //FLUIDSOLVER_SCENARIOSWINDOW_HPP
