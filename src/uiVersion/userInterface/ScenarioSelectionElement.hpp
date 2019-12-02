//
// Created by corbi on 02.12.2019.
//

#ifndef FLUIDSOLVER_SCENARIOSELECTIONELEMENT_HPP
#define FLUIDSOLVER_SCENARIOSELECTIONELEMENT_HPP

#include <CPPGUI.hpp>

namespace FluidUI {
    class ScenarioSelectionElement : public cppgui::Element {

    private:
        class ScenarioSelectionElementState : public cppgui::ElementState{
        public:
            bool IsExpanded = true;
        };

        ScenarioSelectionElementState* GetState();

    protected:
        Element *Build(cppgui::StateContext stateContext) override;

        Element* BuildScenarioInfo(cppgui::StateContext stateContext);

        cppgui::ElementState *CreateState() override;

        bool StateMatchElement(cppgui::ElementState *state) override;

    };

}

#endif //FLUIDSOLVER_SCENARIOSELECTIONELEMENT_HPP
