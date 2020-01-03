//
// Created by corbi on 03.01.2020.
//

#ifndef FLUIDSOLVER_MAINUI_HPP
#define FLUIDSOLVER_MAINUI_HPP


class FluidSolverWindow;

namespace FluidUI {
    class MainUi {
    public:
        FluidSolverWindow* window = nullptr;

        explicit MainUi(FluidSolverWindow *window);

        void Run();

    private:

        void SimulationControl();
    };
}


#endif //FLUIDSOLVER_MAINUI_HPP
