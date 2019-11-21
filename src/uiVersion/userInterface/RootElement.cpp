//
// Created by corbi on 21.11.2019.
//

#include "RootElement.hpp"

RootElement::RootElement(FluidSolverWindow *window) : window(window) {

}

cppgui::Element *RootElement::Build(cppgui::StateContext stateContext) {
    return nullptr;
}

cppgui::ElementState *RootElement::CreateState() {
    auto state = new RootState();
    state->fluidSolverWindow = window;
    return state;
}

bool RootElement::StateMatchElement(cppgui::ElementState *state) {
    return dynamic_cast<RootState *>(state) != nullptr;
}
