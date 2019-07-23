//
// Created by corbi on 09.07.2019.
//

#ifndef FLUIDSOLVER_SIMULATIONSETTINGS_HPP
#define FLUIDSOLVER_SIMULATIONSETTINGS_HPP

#include <dependencies/cppgui/src/Panel.hpp>
#include <ParticleRenderer.hpp>
#include <FluidSolverWindow.hpp>

#include <cppgui/src/Stack.hpp>
#include <cppgui/src/StyledLabel.hpp>
#include <core/interface/IParticleCollection.hpp>

namespace FluidSolver {
    namespace Gui {
        class SimulationSettings : public cppgui::Panel {

        public:
            SimulationSettings(ParticleRenderer *particleRenderer, FluidSolverWindow *window,
                               IParticleCollection *particleCollection);

            ParticleRenderer *particleRenderer;

            FluidSolverWindow *window;

            IParticleCollection *particleCollection;

            uint32_t selectedParticle = 0;

            void UpdateData();

        private:
            void setup();

            void setupColorExpanderStack(cppgui::Stack *colorExpanderStack);

            void setupParameterExpanderStack(cppgui::Stack *parameterExpanderStack);

            void setupScenarioExpanderStack(cppgui::Stack *scenarioExpanderStack);

            void setupInformationExpanderStack(cppgui::Stack *informationExpanderStack);

            void setupStatisticsExapanderStack(cppgui::Stack *statisticsExpanderStack);

            void setupRecordingExpanderStack(cppgui::Stack *recordingExpanderStack);

            cppgui::Stack *generateInfoStack(std::string text, cppgui::StyledLabel *other);

            // single particle information labels
            cppgui::StyledLabel *id;
            cppgui::StyledLabel *position;
            cppgui::StyledLabel *velocity;
            cppgui::StyledLabel *density;
            cppgui::StyledLabel *pressure;
            cppgui::StyledLabel *mass;
            void UpdateInfoLabels();

            // statistics labels
            cppgui::StyledLabel *avgDensity;
            cppgui::StyledLabel *energy;
            cppgui::StyledLabel *maxVelocity;
            cppgui::StyledLabel *deadParticles;
            cppgui::StyledLabel *aliveParticles;
            cppgui::StyledLabel *boundaryParticles;
            cppgui::StyledLabel *cflNumber;
            void UpdateStatLabels();

            bool shouldUpdate = false;

        };
    }
}

#endif //FLUIDSOLVER_SIMULATIONSETTINGS_HPP
