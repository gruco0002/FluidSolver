//
// Created by corbi on 02.12.2019.
//

#include <dependencies/cppgui/library/src/CPPGUI.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include "SimulationSettingsElement.hpp"
#include "RootElement.hpp"
#include "CustomTextBox.hpp"

cppgui::Element *FluidUI::SimulationSettingsElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;
    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto theme = stateContext.FindAncestor<Theme::ThemeData>();

    auto liste = std::vector<Element *>({
                                                new CustomTextBox("Timestep", GetState()->timestepText,
                                                                  [=](std::string newText) {
                                                                      GetState()->timestepText = newText;
                                                                      try {
                                                                          float value = std::stof(newText);
                                                                          if (value > 0.0f) {
                                                                              root->fluidSolverWindow->SetTimestep(
                                                                                      new FluidSolver::ConstantTimestep(value));
                                                                          }
                                                                      } catch (...) {

                                                                      }
                                                                      OnStateChanged();
                                                                  }),
                                                new CustomTextBox("Rest Density", GetState()->restDensityText,
                                                                  [=](std::string newText) {
                                                                      GetState()->restDensityText = newText;
                                                                      try {
                                                                          float value = std::stof(newText);
                                                                          if (value > 0.0f) {
                                                                              root->fluidSolverWindow->SetRestDensity(
                                                                                      value);
                                                                          }
                                                                      } catch (...) {

                                                                      }
                                                                      OnStateChanged();
                                                                  }),

                                        });

    return new Expander(new Text("Simulation"),
                        new Column(liste, MainAxisAlignmentStart, CrossAxisAlignmentStart,
                                   AxisSizeMin), GetState()->IsExpanded, [=](bool isExpanded) {
                GetState()->IsExpanded = isExpanded;
                OnStateChanged();
            });

}

cppgui::ElementState *FluidUI::SimulationSettingsElement::CreateState() {
    return new SimulationSettingsElementState();
}

bool FluidUI::SimulationSettingsElement::StateMatchElement(cppgui::ElementState *state) {
    return dynamic_cast<SimulationSettingsElementState *>(state) != nullptr;
}

FluidUI::SimulationSettingsElement::SimulationSettingsElementState *FluidUI::SimulationSettingsElement::GetState() {
    return dynamic_cast<SimulationSettingsElementState *>(state);
}

void FluidUI::SimulationSettingsElement::SimulationSettingsElementState::InitState(cppgui::StateContext stateContext) {
    auto root = stateContext.FindAncestor<RootElement::RootState>();
    timestepText = std::to_string(root->fluidSolverWindow->GetTimestep()->getCurrentTimestep());
    restDensityText = std::to_string(root->fluidSolverWindow->GetRestDensity());
}
