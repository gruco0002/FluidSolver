//
// Created by corbi on 02.12.2019.
//

#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include "FluidSolverElement.hpp"
#include "RootElement.hpp"
#include "CustomTextBox.hpp"

void FluidUI::FluidSolverElement::FluidSolverElementState::InitState(cppgui::StateContext stateContext) {
    ElementState::InitState(stateContext);
    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto fluidSolver = root->fluidSolverWindow->GetFluidSolver();
    this->FluidSolverChanged(fluidSolver);
}

void FluidUI::FluidSolverElement::FluidSolverElementState::FluidSolverChanged(FluidSolver::IFluidSolver *fluidSolver) {


    auto simple = dynamic_cast<FluidSolver::SPHFluidSolver *>(fluidSolver);
    auto iisph = dynamic_cast<FluidSolver::IISPHFluidSolver *>(fluidSolver);
    if (simple != nullptr) {
        this->Type = FluidSolverTypeSimpleSPH;
        this->stiffnessString = std::to_string(simple->StiffnessK);
        this->viscosityString = std::to_string(simple->Viscosity);
    } else if (iisph != nullptr) {
        this->Type = FluidSolverTypeIISPH;
        this->gammaString = std::to_string(iisph->Gamma);
        this->omegaString = std::to_string(iisph->Omega);
        this->maxDensityErrorAllowedString = std::to_string(iisph->MaxDensityErrorAllowed);
        this->minIterationsString = std::to_string(iisph->MinNumberOfIterations);
        this->maxIterationsString = std::to_string(iisph->MaxNumberOfIterations);
        this->viscosityString = std::to_string(iisph->Viscosity);
    } else {
        throw cppgui::GuiException("Fluid solver not supported in gui");
    }
}

cppgui::Element *FluidUI::FluidSolverElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;

    std::vector<Element *> liste;
    auto theme = stateContext.FindAncestor<Theme::ThemeData>();
    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto fluidSolver = root->fluidSolverWindow->GetFluidSolver();

    auto simple = dynamic_cast<FluidSolver::SPHFluidSolver *>(fluidSolver);
    auto iisph = dynamic_cast<FluidSolver::IISPHFluidSolver *>(fluidSolver);

    liste.push_back(new Padding(new Text("Solvers", theme->ControlFontSize), Padding::Border(0, 0, 0, 5)));
    liste.push_back(new Button(new Text("Simple SPH Fluid Solver"), [=]() {
        if (simple != nullptr)
            return;

        auto sphFluidSolver = new FluidSolver::SPHFluidSolver();
        sphFluidSolver->StiffnessK = 100000.0f;
        sphFluidSolver->Viscosity = 5.0f;
        root->fluidSolverWindow->SetFluidSolver(sphFluidSolver);
        delete iisph;

        GetState()->FluidSolverChanged(root->fluidSolverWindow->GetFluidSolver());
        OnStateChanged();

    }));
    liste.push_back(new Button(new Text("IISPH Fluid Solver"), [=]() {
        if (iisph != nullptr)
            return;

        auto isphFluidSolver = new FluidSolver::IISPHFluidSolver();
        isphFluidSolver->Omega = 0.5f;
        isphFluidSolver->Gamma = 0.7f;
        isphFluidSolver->MaxDensityErrorAllowed = 0.001f;
        isphFluidSolver->MinNumberOfIterations = 5;
        root->fluidSolverWindow->SetFluidSolver(isphFluidSolver);
        delete simple;

        GetState()->FluidSolverChanged(root->fluidSolverWindow->GetFluidSolver());
        OnStateChanged();
    }));

    liste.push_back(new Padding(new Text("Current", theme->ControlFontSize), Padding::Border(0, 0, 0, 5)));
    if (GetState()->Type == FluidSolverElementState::FluidSolverTypeSimpleSPH) {
        liste.push_back(new Text("Name: Simple SPH Fluid Solver"));
        liste.push_back(
                new CustomTextBox("Stiffness (k)", GetState()->stiffnessString, [=](std::string newText) {
                    GetState()->stiffnessString = newText;
                    try {
                        float value = std::stof(newText);
                        if (value > 0.0f) {
                            simple->StiffnessK = value;
                        }
                    } catch (...) {

                    }
                    OnStateChanged();
                }));
        liste.push_back(
                new CustomTextBox("Viscosity", GetState()->viscosityString, [=](std::string newText) {
                    GetState()->viscosityString = newText;
                    try {
                        float value = std::stof(newText);
                        if (value >= 0.0f) {
                            simple->Viscosity = value;
                        }
                    } catch (...) {

                    }
                    OnStateChanged();
                }));

    } else if (GetState()->Type == FluidSolverElementState::FluidSolverTypeIISPH) {
        liste.push_back(new Text("Name: IISPH Fluid Solver"));
        liste.push_back(new Text("Last Iteration Count: " + std::to_string(iisph->getLastIterationCount())));
        liste.push_back(new Text("Last Predicted Density Error: " + std::to_string(iisph->getLastPredictedDensityError())));
        liste.push_back(
                new CustomTextBox("Min. Iterations", GetState()->minIterationsString, [=](std::string newText) {
                    GetState()->minIterationsString = newText;
                    try {
                        int value = std::stoi(newText);
                        if (value >= 0) {
                            iisph->MinNumberOfIterations = value;
                        }
                    } catch (...) {

                    }
                    OnStateChanged();
                }));

        liste.push_back(
                new CustomTextBox("Max. Iterations", GetState()->maxIterationsString, [=](std::string newText) {
                    GetState()->maxIterationsString = newText;
                    try {
                        int value = std::stoi(newText);
                        if (value >= 0) {
                            iisph->MaxNumberOfIterations = value;
                        }
                    } catch (...) {

                    }
                    OnStateChanged();
                }));

        liste.push_back(
                new CustomTextBox("Max. Density Error", GetState()->maxDensityErrorAllowedString,
                                  [=](std::string newText) {
                                      GetState()->maxDensityErrorAllowedString = newText;
                                      try {
                                          float value = std::stof(newText);
                                          if (value != 0.0f) {
                                              iisph->MaxDensityErrorAllowed = value;
                                          }
                                      } catch (...) {

                                      }
                                      OnStateChanged();
                                  }));

        liste.push_back(
                new CustomTextBox("Gamma", GetState()->gammaString, [=](std::string newText) {
                    GetState()->gammaString = newText;
                    try {
                        float value = std::stof(newText);
                        if (value >= 0.0f) {
                            iisph->Gamma = value;
                        }
                    } catch (...) {

                    }
                    OnStateChanged();
                }));

        liste.push_back(
                new CustomTextBox("Omega", GetState()->omegaString, [=](std::string newText) {
                    GetState()->omegaString = newText;
                    try {
                        float value = std::stof(newText);
                        if (value >= 0.0f) {
                            iisph->Omega = value;
                        }
                    } catch (...) {

                    }
                    OnStateChanged();
                }));
        liste.push_back(
                new CustomTextBox("Viscosity", GetState()->viscosityString, [=](std::string newText) {
                    GetState()->viscosityString = newText;
                    try {
                        float value = std::stof(newText);
                        if (value >= 0.0f) {
                            iisph->Viscosity = value;
                        }
                    } catch (...) {

                    }
                    OnStateChanged();
                }));
    }


    return new Expander(new Text("FluidSolver"),
                        new Column(liste, MainAxisAlignmentStart, CrossAxisAlignmentStart,
                                   AxisSizeMin), GetState()->IsExpanded, [=](bool isExpanded) {
                GetState()->IsExpanded = isExpanded;
                OnStateChanged();
            });
}

cppgui::ElementState *FluidUI::FluidSolverElement::CreateState() {
    return new FluidSolverElementState();
}

bool FluidUI::FluidSolverElement::StateMatchElement(cppgui::ElementState *state) {
    return dynamic_cast<FluidSolverElementState *>(state) != nullptr;
}

FluidUI::FluidSolverElement::FluidSolverElementState *FluidUI::FluidSolverElement::GetState() {
    return dynamic_cast<FluidSolverElementState *>(state);
}
