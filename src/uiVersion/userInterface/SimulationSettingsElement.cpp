//
// Created by corbi on 02.12.2019.
//

#include <dependencies/cppgui/library/src/CPPGUI.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include <core/timestep/DynamicCFLTimestep.hpp>
#include "SimulationSettingsElement.hpp"
#include "RootElement.hpp"
#include "CustomTextBox.hpp"

cppgui::Element *FluidUI::SimulationSettingsElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;
    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto theme = stateContext.FindAncestor<Theme::ThemeData>();

    auto constant = dynamic_cast<FluidSolver::ConstantTimestep *>(root->fluidSolverWindow->GetTimestep());
    auto dynamicCFL = dynamic_cast<FluidSolver::DynamicCFLTimestep *>(root->fluidSolverWindow->GetTimestep());

    auto liste = std::vector<Element *>({

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

    liste.push_back(new Padding(new Text("Timestep Selection", theme->ControlFontSize), Padding::Border(0, 0, 5, 5)));

    liste.push_back(new Button(new Text("Constant Timestep"), [=]() {
        if (constant != nullptr)
            return;

        auto tmp = new FluidSolver::ConstantTimestep(0.001f);
        delete root->fluidSolverWindow->GetTimestep();
        root->fluidSolverWindow->SetTimestep(tmp);

        GetState()->OnTimestepChanged(root->fluidSolverWindow->GetTimestep());
        OnStateChanged();
    }));

    liste.push_back(new Button(new Text("Dynamic CFL Timestep"), [=]() {
        if (dynamicCFL != nullptr)
            return;

        auto tmp = new FluidSolver::DynamicCFLTimestep();
        delete root->fluidSolverWindow->GetTimestep();
        root->fluidSolverWindow->SetTimestep(tmp);

        GetState()->OnTimestepChanged(root->fluidSolverWindow->GetTimestep());
        OnStateChanged();
    }));


    liste.push_back(new Padding(new Text("Timestep Settings", theme->ControlFontSize), Padding::Border(0, 0, 5, 5)));
    if (GetState()->type == SimulationSettingsElementState::TimestepTypeConstant) {
        liste.push_back(new Text("Type: Constant Timestep"));
        liste.push_back(new Text(
                "Current Timestep: " + std::to_string(root->fluidSolverWindow->GetTimestep()->getCurrentTimestep())));
        liste.push_back(new CustomTextBox("Timestep", GetState()->timestepText,
                                          [=](std::string newText) {
                                              GetState()->timestepText = newText;
                                              try {
                                                  float value = std::stof(newText);
                                                  if (value > 0.0f) {

                                                      if (constant != nullptr)
                                                          constant->setTimestep(value);
                                                  }
                                              } catch (...) {

                                              }
                                              OnStateChanged();
                                          }));
    } else if (GetState()->type == SimulationSettingsElementState::TimestepTypeDynamicCFL) {
        liste.push_back(new Text("Type: Dynamic CFL Timestep"));
        liste.push_back(new Text(
                "Current Timestep: " + std::to_string(root->fluidSolverWindow->GetTimestep()->getCurrentTimestep())));

        liste.push_back(new CustomTextBox("Min. Timestep", GetState()->minTimestepText,
                                          [=](std::string newText) {
                                              GetState()->minTimestepText = newText;
                                              try {
                                                  float value = std::stof(newText);
                                                  if (value > 0.0f) {
                                                      if (dynamicCFL != nullptr)
                                                          dynamicCFL->setMinimumTimestep(value);
                                                  }
                                              } catch (...) {

                                              }
                                              OnStateChanged();
                                          }));

        liste.push_back(new CustomTextBox("Max. Timestep", GetState()->maxTimestepText,
                                          [=](std::string newText) {
                                              GetState()->maxTimestepText = newText;
                                              try {
                                                  float value = std::stof(newText);
                                                  if (value > 0.0f) {
                                                      if (dynamicCFL != nullptr)
                                                          dynamicCFL->setMaximumTimestep(value);
                                                  }
                                              } catch (...) {

                                              }
                                              OnStateChanged();
                                          }));

        liste.push_back(new CustomTextBox("CFL Number", GetState()->cflNumberText,
                                          [=](std::string newText) {
                                              GetState()->cflNumberText = newText;
                                              try {
                                                  float value = std::stof(newText);
                                                  if (value > 0.0f) {
                                                      if (dynamicCFL != nullptr)
                                                          dynamicCFL->setCflNumber(value);
                                                  }
                                              } catch (...) {

                                              }
                                              OnStateChanged();
                                          }));


    }


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
    FluidSolver::ITimestep *timestep = root->fluidSolverWindow->GetTimestep();

    OnTimestepChanged(timestep);

    restDensityText = std::to_string(root->fluidSolverWindow->GetRestDensity());
}

void FluidUI::SimulationSettingsElement::SimulationSettingsElementState::OnTimestepChanged(
        FluidSolver::ITimestep *timestep) {
    auto constant = dynamic_cast<FluidSolver::ConstantTimestep *>(timestep);
    auto dynamicCFL = dynamic_cast<FluidSolver::DynamicCFLTimestep *>(timestep);

    if (constant != nullptr) {
        type = TimestepTypeConstant;
        timestepText = std::to_string(constant->getTimestep());
    } else if (dynamicCFL != nullptr) {
        type = TimestepTypeDynamicCFL;
        minTimestepText = std::to_string(dynamicCFL->getMinimumTimestep());
        maxTimestepText = std::to_string(dynamicCFL->getMaximumTimestep());
        cflNumberText = std::to_string(dynamicCFL->getCflNumber());
    }
}
