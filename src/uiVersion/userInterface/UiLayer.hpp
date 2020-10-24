#ifndef FLUIDSOLVER_UILAYER_HPP
#define FLUIDSOLVER_UILAYER_HPP

#include "ScenariosWindow.hpp"

namespace FluidUi {
    class FluidSolverWindow;

    class UiLayer {


    public:
        FluidSolverWindow* window = nullptr;
        void render();
        void initialize();


    private:
        ScenariosWindow scenarios;


    };

}


#endif //FLUIDSOLVER_UILAYER_HPP
