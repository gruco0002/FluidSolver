#ifndef FLUIDSOLVER_STATISTICSUI_HPP
#define FLUIDSOLVER_STATISTICSUI_HPP

#include "core/sensors/SensorDataStorage.hpp"

namespace FluidUi {
    class FluidSolverWindow;

    class StatisticsUi {

    public:

        FluidSolverWindow* window = nullptr;

        void render();
        void initialize();

    private:
        void render_particle_statistics(FluidSolver::SensorDataStorage* storage);

    };

}


#endif //FLUIDSOLVER_STATISTICSUI_HPP
