//
// Created by corbi on 21.11.2019.
//

#include "SimulationControlElement.hpp"
#include "RootElement.hpp"

cppgui::Element *FluidUI::SimulationControlElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;

    auto root = stateContext.FindAncestor<RootElement::RootState>();
    bool paused = root->fluidSolverWindow->Pause;

    return new Row(std::vector<Element *>({

                                                  new Button(new Icon(new GuiIconSource(paused ? "play_arrow" : "pause"), paused ? Colors::DarkGreen() : Colors::DarkRed()), [=]() {
                                                      if (root == nullptr)
                                                          return;
                                                      root->fluidSolverWindow->Pause = !paused;
                                                      OnStateChanged();
                                                  }),
                                                  new Button(new Icon(new GuiIconSource("replay")), [=]() {
                                                      if (root == nullptr)
                                                          return;
                                                      root->fluidSolverWindow->resetData();
                                                      OnStateChanged();
                                                  }),

                                          }), MainAxisAlignmentSpaceAround);
}
