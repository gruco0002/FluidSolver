//
// Created by corbi on 02.12.2019.
//

#ifndef FLUIDSOLVER_SIMULATIONSETTINGSELEMENT_HPP
#define FLUIDSOLVER_SIMULATIONSETTINGSELEMENT_HPP

#include <dependencies/cppgui/library/src/basic/Element.hpp>
#include <core/timestep/ITimestep.hpp>

namespace FluidUI {
    class SimulationSettingsElement : public cppgui::Element {

    private:
        class SimulationSettingsElementState : public cppgui::ElementState {
        public:

            enum TimestepType {
                TimestepTypeConstant,
                TimestepTypeDynamicCFL,
            };

            bool IsExpanded = true;

            TimestepType type = TimestepType::TimestepTypeConstant;

            std::string timestepText = "";
            std::string minTimestepText = "";
            std::string maxTimestepText = "";
            std::string cflNumberText = "";

            std::string restDensityText = "";

            void InitState(cppgui::StateContext stateContext) override;

            void OnTimestepChanged(FluidSolver::ITimestep* timestep);

        };

        SimulationSettingsElementState *GetState();

    protected:
        cppgui::ElementState *CreateState() override;

        bool StateMatchElement(cppgui::ElementState *state) override;

    protected:
        Element *Build(cppgui::StateContext stateContext) override;
    };
}

#endif //FLUIDSOLVER_SIMULATIONSETTINGSELEMENT_HPP
