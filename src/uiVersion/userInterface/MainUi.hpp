//
// Created by corbi on 03.01.2020.
//

#ifndef FLUIDSOLVER_MAINUI_HPP
#define FLUIDSOLVER_MAINUI_HPP


#include <string>
#include <vector>

class FluidSolverWindow;

namespace FluidUI {
    class MainUi {
    public:
        FluidSolverWindow *window = nullptr;

        explicit MainUi(FluidSolverWindow *window);

        void Run();

    private:

        void SimulationControl();


        std::vector<std::string> Scenario_Names;
        int Scenario_selectedScenario = 0;
        bool Scenario_init = false;
        void Scenario();


        float Simulation_RestDensity = 0.0f;
        bool Simulation_Init = false;
        int Simulation_SelectedTimestep = 0;
        void Simulation();


        bool FluidSolver_Init = false;
        int FluidSolver_SelectedSolver = 0;
        void FluidSolver();


        void VisualizerSettings();

    };
}


#endif //FLUIDSOLVER_MAINUI_HPP
