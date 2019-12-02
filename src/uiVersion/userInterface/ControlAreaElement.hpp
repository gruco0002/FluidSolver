//
// Created by corbi on 21.11.2019.
//

#ifndef FLUIDSOLVER_CONTROLAREAELEMENT_HPP
#define FLUIDSOLVER_CONTROLAREAELEMENT_HPP

#include <CPPGUI.hpp>

namespace FluidUI {
    class ControlAreaElement : public cppgui::Element{

    private:
    class ControlAreaElementState: public cppgui::ElementState{
    public:
        bool Expanded = true;
        };

    ControlAreaElementState* GetState();

    protected:
        Element *Build(cppgui::StateContext stateContext) override;

        cppgui::ElementState *CreateState() override;

        bool StateMatchElement(cppgui::ElementState *state) override;

    };
}


#endif //FLUIDSOLVER_CONTROLAREAELEMENT_HPP
