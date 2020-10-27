#ifndef FLUIDSOLVER_UILAYER_HPP
#define FLUIDSOLVER_UILAYER_HPP

#include "ScenariosWindow.hpp"
#include "StatisticsUi.hpp"

namespace FluidUi {
    class FluidSolverWindow;

    class UiLayer {


    public:
        FluidSolverWindow* window = nullptr;
        void render();
        void initialize();


    private:
        ScenariosWindow scenarios;
        StatisticsUi statisticsUi;

        void render_solver_parameters();

    };

}


#endif //FLUIDSOLVER_UILAYER_HPP
