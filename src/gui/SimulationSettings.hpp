//
// Created by corbi on 09.07.2019.
//

#ifndef FLUIDSOLVER_SIMULATIONSETTINGS_HPP
#define FLUIDSOLVER_SIMULATIONSETTINGS_HPP

#include <dependencies/cppgui/src/Panel.hpp>
#include <ParticleRenderer.hpp>
#include <FluidSolverWindow.hpp>

#include <cppgui/src/Stack.hpp>

namespace FluidSolver {
    namespace Gui {
    class SimulationSettings : public cppgui::Panel {

    public:
        SimulationSettings(ParticleRenderer* particleRenderer, FluidSolverWindow* window);

        ParticleRenderer* particleRenderer;

        FluidSolverWindow* window;

    private:
        void setup();

        void setupColorExpanderStack(cppgui::Stack* colorExpanderStack);

        void setupParameterExpanderStack(cppgui::Stack* parameterExpanderStack);

        void setupScenarioExpanderStack(cppgui::Stack*  scenarioExpanderStack);

    };
    }
}

#endif //FLUIDSOLVER_SIMULATIONSETTINGS_HPP
