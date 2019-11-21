//
// Created by corbi on 21.11.2019.
//

#ifndef FLUIDSOLVER_ROOTELEMENT_HPP
#define FLUIDSOLVER_ROOTELEMENT_HPP

#include <cppgui/library/src/CPPGUI.hpp>
#include <uiVersion/FluidSolverWindow.hpp>

class RootElement : public cppgui::Element {



public:
    class RootState : public cppgui::ElementState {
    public:
        FluidSolverWindow *fluidSolverWindow = nullptr;
    };

protected:
    Element *Build(cppgui::StateContext stateContext) override;

    cppgui::ElementState *CreateState() override;

    bool StateMatchElement(cppgui::ElementState *state) override;

public:


    RootElement(FluidSolverWindow* window);

private:
    FluidSolverWindow* window = nullptr;

};


#endif //FLUIDSOLVER_ROOTELEMENT_HPP
