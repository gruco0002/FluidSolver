#ifndef FLUIDSOLVER_STATISTICSUI_HPP
#define FLUIDSOLVER_STATISTICSUI_HPP



namespace FluidUi {
    class FluidSolverWindow;

    class StatisticsUi {

    public:

        FluidSolverWindow* window = nullptr;

        void render();
        void initialize();



    };

}


#endif //FLUIDSOLVER_STATISTICSUI_HPP
