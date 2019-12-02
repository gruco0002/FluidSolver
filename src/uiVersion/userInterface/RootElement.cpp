//
// Created by corbi on 21.11.2019.
//

#include "RootElement.hpp"
#include "ControlAreaElement.hpp"

FluidUI::RootElement::RootElement(FluidSolverWindow *window) : window(window) {

}

cppgui::Element *FluidUI::RootElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;
    return new Theme(new Row(std::vector<Element *>({new ControlAreaElement()}),
                             MainAxisAlignmentEnd,
                             CrossAxisAlignmentStart));
}

cppgui::ElementState *FluidUI::RootElement::CreateState() {
    auto state = new RootState();
    state->fluidSolverWindow = window;
    return state;
}

bool FluidUI::RootElement::StateMatchElement(cppgui::ElementState *state) {
    return dynamic_cast<RootState *>(state) != nullptr;
}

void FluidUI::RootElement::RootState::InitState(cppgui::StateContext stateContext) {
    ElementState::InitState(stateContext);
    this->TickerNotificationTime = 0.05f;
}

void FluidUI::RootElement::RootState::OnTick() {
    OnStateChanged();
}
