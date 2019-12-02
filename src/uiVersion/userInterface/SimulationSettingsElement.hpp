//
// Created by corbi on 02.12.2019.
//

#ifndef FLUIDSOLVER_SIMULATIONSETTINGSELEMENT_HPP
#define FLUIDSOLVER_SIMULATIONSETTINGSELEMENT_HPP

#include <dependencies/cppgui/library/src/basic/Element.hpp>

namespace FluidUI {
    class SimulationSettingsElement : public cppgui::Element {

    private:
        class SimulationSettingsElementState : public cppgui::ElementState{
        public:
            bool IsExpanded = true;
            std::string timestepText = "";
            std::string restDensityText = "";

            void InitState(cppgui::StateContext stateContext) override;

        };

        SimulationSettingsElementState* GetState();

    protected:
        cppgui::ElementState *CreateState() override;

        bool StateMatchElement(cppgui::ElementState *state) override;

    protected:
        Element *Build(cppgui::StateContext stateContext) override;
    };
}

#endif //FLUIDSOLVER_SIMULATIONSETTINGSELEMENT_HPP
