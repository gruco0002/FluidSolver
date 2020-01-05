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


        bool Visualizer_Init = false;
        size_t Visualizer_Width = 0;
        size_t Visualizer_Height = 0;
        int Visualizer_Selection = 0;

        void VisualizerSettings();


        float Statistics_ScaleMin = 0.0f;
        float Statistics_ScaleMax = 2.0f;
        bool Statistics_GraphSelection[100]{false};
        std::vector<const float *> Statistics_yData;

        void Statistics();

    };
}


#endif //FLUIDSOLVER_MAINUI_HPP