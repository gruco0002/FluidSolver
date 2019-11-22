//
// Created by corbi on 21.11.2019.
//

#include "SimulationControlElement.hpp"
#include "RootElement.hpp"

cppgui::Element *FluidUI::SimulationControlElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;

    auto root = stateContext.FindAncestor<RootElement::RootState>();

    return new Row(std::vector<Element *>({

                                                  new Button(new Icon(new GuiIconSource("play_arrow")), [=]() {
                                                      if (root == nullptr)
                                                          return;
                                                      root->fluidSolverWindow->Pause = false;
                                                  }),
                                                  new Button(new Icon(new GuiIconSource("pause")), [=]() {
                                                      if (root == nullptr)
                                                          return;
                                                      root->fluidSolverWindow->Pause = true;
                                                  }),
                                                  new Button(new Icon(new GuiIconSource("replay")), [=]() {
                                                      if (root == nullptr)
                                                          return;
                                                      root->fluidSolverWindow->resetData();
                                                  }),

                                          }), MainAxisAlignmentSpaceAround);
}
