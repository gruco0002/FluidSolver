//
// Created by corbi on 03.01.2020.
//

#include "MainUi.hpp"

#include <uiVersion/FluidSolverWindow.hpp>
#include <imguiHelper.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include <core/timestep/DynamicCFLTimestep.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <core/CachedStatisticCollector.hpp>

FluidUI::MainUi::MainUi(FluidSolverWindow *window) : window(window) {}

void FluidUI::MainUi::Run() {
    //ImGui::ShowDemoWindow();

    SimulationControl();
    Scenario();
    Simulation();
    FluidSolver();
    VisualizerSettings();
    Statistics();

}

void FluidUI::MainUi::SimulationControl() {

    ImGui::Begin("Controls");


    if (ImGui::Button(window->Pause ? "Play" : "Pause")) {
        window->Pause = !window->Pause;
    }

    if (ImGui::Button("Reset")) {
        window->resetData();
    }

    ImGui::End();
}

void FluidUI::MainUi::Scenario() {
    ImGui::Begin("Scenario");

    ImGui::Text("Current Scenario:");
    ImGui::Text("Name: %s", window->GetScenario()->GetName().c_str());
    ImGui::Text("Particle Size: %f", window->GetScenario()->GetParticleSize());
    ImGui::Separator();
    ImGui::Text("Load Scenario");

    std::vector<const char *> scenarios;
    Scenario_Names.clear();
    int i = 0;
    for (auto scn : window->Scenarios) {

        Scenario_Names.push_back(scn->GetName());
        scenarios.push_back(Scenario_Names[Scenario_Names.size() - 1].c_str());
        if (Scenario_selectedScenario == i && Scenario_init) {
            // choose new scenario
            if (scn->GetName() != window->GetScenario()->GetName()) {
                // set new scenario
                window->SetScenario(scn);
            }
        }

        if (scn->GetName() == window->GetScenario()->GetName()) {
            Scenario_init = true;
        }
        if (!Scenario_init) {
            Scenario_selectedScenario++;
        }

        i++;
    }

    ImGui::ListBox("Scenarios", &Scenario_selectedScenario, scenarios.data(), scenarios.size());

    ImGui::End();
}

void FluidUI::MainUi::Simulation() {
    auto constant = dynamic_cast<FluidSolver::ConstantTimestep *>(window->GetTimestep());
    auto dynamic = dynamic_cast<FluidSolver::DynamicCFLTimestep *>(window->GetTimestep());

    ImGui::Begin("Simulation");

    if (!Simulation_Init) {
        Simulation_RestDensity = window->GetRestDensity();
        if (constant != nullptr) {
            Simulation_SelectedTimestep = 0;
        } else {
            Simulation_SelectedTimestep = 1;
        }
        Simulation_Init = true;
    }
    if (window->GetRestDensity() != Simulation_RestDensity) {
        window->SetRestDensity(Simulation_RestDensity);
    }


    ImGui::InputFloat("Rest Density", &Simulation_RestDensity);

    ImGui::Separator();

    static const char *selection[]{"Constant Timestep", "Dynamic CFL Timestep"};

    if (Simulation_SelectedTimestep == 0 && constant == nullptr) {
        // choose constant timestep
        auto tmp = new FluidSolver::ConstantTimestep(0.001f);
        delete window->GetTimestep();
        window->SetTimestep(tmp);
        constant = tmp;
        dynamic = nullptr;
    } else if (Simulation_SelectedTimestep == 1 && dynamic == nullptr) {
        // choose dynamic timestep
        auto tmp = new FluidSolver::DynamicCFLTimestep();
        delete window->GetTimestep();
        window->SetTimestep(tmp);
        dynamic = tmp;
        constant = nullptr;
    }

    ImGui::ListBox("Timestep", &Simulation_SelectedTimestep, selection, IM_ARRAYSIZE(selection));

    ImGui::Text("Current Timestep: %f", window->GetTimestep()->getCurrentTimestep());

    ImGui::Text("Settings");
    if (constant != nullptr) {
        // constant timestep settings
        ImGui::InputFloat("Timestep", &constant->Timestep);
    } else if (dynamic != nullptr) {
        // dynamic timestep settings
        ImGui::InputFloat("Min. Timestep", &dynamic->MinimumTimestep, 0, 0, "%.6f");
        ImGui::InputFloat("Max. Timestep", &dynamic->MaximumTimestep, 0, 0, "%.6f");
        ImGui::InputFloat("CFL Number", &dynamic->CFLNumber, 0, 0, "%.6f");

    }


    ImGui::End();
}

void FluidUI::MainUi::FluidSolver() {

    auto simple = dynamic_cast<FluidSolver::SPHFluidSolver *>(window->GetFluidSolver());
    auto iisph = dynamic_cast<FluidSolver::IISPHFluidSolver *>(window->GetFluidSolver());

    if (!FluidSolver_Init) {
        if (simple != nullptr) {
            FluidSolver_SelectedSolver = 0;
        } else {
            FluidSolver_SelectedSolver = 1;
        }

        FluidSolver_Init = true;
    }


    ImGui::Begin("Fluid Solver");

    static const char *selection[]{"Simple SPH Fluid Solver", "IISPH Fluid Solver"};
    if (FluidSolver_SelectedSolver == 0 && simple == nullptr) {
        auto tmp = new FluidSolver::SPHFluidSolver();
        delete window->GetFluidSolver();
        window->SetFluidSolver(tmp);
        simple = tmp;
        iisph = nullptr;
    } else if (FluidSolver_SelectedSolver == 1 && iisph == nullptr) {
        auto tmp = new FluidSolver::IISPHFluidSolver();
        delete window->GetFluidSolver();
        window->SetFluidSolver(tmp);
        simple = nullptr;
        iisph = tmp;
    }

    ImGui::ListBox("Solver", &FluidSolver_SelectedSolver, selection, IM_ARRAYSIZE(selection));


    if (simple != nullptr) {
        ImGui::InputFloat("Stiffness (k)", &simple->StiffnessK);
        ImGui::InputFloat("Viscosity", &simple->Viscosity);
    } else if (iisph != nullptr) {
        ImGui::InputInt("Min. Iterations", reinterpret_cast<int *>(&iisph->MinNumberOfIterations));
        ImGui::InputInt("Max. Iterations", reinterpret_cast<int *>(&iisph->MaxNumberOfIterations));
        ImGui::InputFloat("Max. Density Error", &iisph->MaxDensityErrorAllowed, 0, 0, "%.6f");
        ImGui::InputFloat("Gamma", &iisph->Gamma);
        ImGui::InputFloat("Omega", &iisph->Omega);
        ImGui::InputFloat("Viscosity", &iisph->Viscosity);

        ImGui::Separator();
        ImGui::Text("Last Iteration Count: %zu", iisph->getLastIterationCount());
        ImGui::Text("Last Predicted Density Error: %f", iisph->getLastPredictedDensityError());
        ImGui::Text("Max Predicted Density Error: %f", iisph->getMaxPredictedDensityErrorReached());

    }

    ImGui::End();

}

void FluidUI::MainUi::VisualizerSettings() {

    auto pr = dynamic_cast<ParticleRenderer *>(window->GetVisualizer());
    auto cv = dynamic_cast<ContinousVisualizerOpenGL *>(window->GetVisualizer());

    if (!Visualizer_Init) {
        Visualizer_Width = window->getVisualizerRenderTargetWidth();
        Visualizer_Height = window->getVisualizerRenderTargetHeight();
        if (pr != nullptr) {
            Visualizer_Selection = 0;
        } else if (cv != nullptr) {
            Visualizer_Selection = 1;
        }
        Visualizer_Init = true;
    }

    if (window->getVisualizerRenderTargetWidth() != Visualizer_Width && Visualizer_Width <= 16000) {
        window->setVisualizerRenderTargetWidth(Visualizer_Width);
    }
    if (window->getVisualizerRenderTargetHeight() != Visualizer_Height && Visualizer_Height <= 16000) {
        window->setVisualizerRenderTargetHeight(Visualizer_Height);
    }

    ImGui::Begin("Visualizer Settings");

    ImGui::InputInt("Width (px)", reinterpret_cast<int *>(&Visualizer_Width));
    ImGui::InputInt("Height (px)", reinterpret_cast<int *>(&Visualizer_Height));

    static const char *visSelection[]{"Particle Renderer", "Continuous Visualizer"};

    if (Visualizer_Selection == 0 && pr == nullptr) {
        // set particle renderer
        auto tmp = new ParticleRenderer();
        delete window->GetVisualizer();
        window->SetVisualizer(tmp);
        pr = tmp;
        cv = nullptr;

    } else if (Visualizer_Selection == 1 && cv == nullptr) {
        // set continuous renderer
        auto tmp = new ContinousVisualizerOpenGL();
        delete window->GetVisualizer();
        window->SetVisualizer(tmp);
        pr = nullptr;
        cv = tmp;
    }

    ImGui::ListBox("Visualizer", &Visualizer_Selection, visSelection, IM_ARRAYSIZE(visSelection));

    ImGui::Separator();

    if (pr != nullptr) {

        ImGui::Text("Particle Renderer");

        static const char *selection[]{"Velocity", "Acceleration", "Mass", "Pressure", "Density"};
        ImGui::Combo("Value", (int *) &pr->colorSelection, selection, IM_ARRAYSIZE(selection));

        ImGui::InputFloat("Bottom Value", &pr->bottomValue, 0, 0, "%.6f");
        ImGui::ColorEdit3("Bottom Color", reinterpret_cast<float *>(&pr->bottomColor));

        ImGui::InputFloat("Top Value", &pr->topValue, 0, 0, "%.6f");
        ImGui::ColorEdit3("Top Color", reinterpret_cast<float *>(&pr->topColor));

        ImGui::Separator();

        ImGui::ColorEdit3("Boundary Color", reinterpret_cast<float *>(&pr->boundaryParticleColor));
        ImGui::ColorEdit3("Background Color", reinterpret_cast<float *>(&pr->backgroundClearColor));
    } else if (cv != nullptr) {
        ImGui::Text("Continuous Visualizer");
        ImGui::InputFloat("Min. Render Density", &cv->MinimumRenderDensity);
        ImGui::ColorEdit3("Background Color", reinterpret_cast<float *>(&cv->ClearColor));
    }

    ImGui::End();
}

void FluidUI::MainUi::Statistics() {
    ImGui::Begin("Statistics");

    static const char *visSelection[]{"Average Density", "Energy", "Potential Energy", "Kinetic Energy",
                                      "Max. Velocity", "CFL Number", "Dead Particles", "Boundary Particles",
                                      "Normal Particles"};

    ImGui::Columns(2);
    auto cached = dynamic_cast<FluidSolver::CachedStatisticCollector *>(window->GetStatisticCollector());

    if (cached != nullptr) {
        Statistics_yData.clear();
        for (size_t i = 0; i < IM_ARRAYSIZE(visSelection); i++) {
            if (!Statistics_GraphSelection[i])
                continue;

            switch (i) {
                case 0:
                    Statistics_yData.push_back(cached->getAverageDensityCache().data());
                    break;
                case 1:
                    Statistics_yData.push_back(cached->getEnergyCache().data());
                    break;
                case 2:
                    Statistics_yData.push_back(cached->getPotentialEnergyCache().data());
                    break;
                case 3:
                    Statistics_yData.push_back(cached->getKineticEnergyCache().data());
                    break;
                case 4:
                    Statistics_yData.push_back(cached->getMaximumVelocityCache().data());
                    break;
                case 5:
                    Statistics_yData.push_back(cached->getCflNumberCache().data());
                    break;
                case 6:
                    Statistics_yData.push_back(cached->getDeadParticleCountCache().data());
                    break;
                case 7:
                    Statistics_yData.push_back(cached->getBoundaryParticleCountCache().data());
                    break;
                case 8:
                    Statistics_yData.push_back(cached->getNormalParticleCountCache().data());
                    break;
                default:
                    break;
            }


        }


        if (!Statistics_yData.empty()) {
            ImGui::PlotConfig conf;
            conf.values.xs = cached->getTimestepCache().data(); // this line is optional
            conf.values.ys_list = Statistics_yData.data();
            conf.values.ys_count = Statistics_yData.size();
            conf.values.count = cached->getCurrentCacheDataSize();
            conf.scale.min = Statistics_ScaleMin;
            conf.scale.max = Statistics_ScaleMax;
            conf.tooltip.show = true;
            conf.tooltip.format = "x=%.8f, y=%.8f";
            conf.grid_x.show = true;
            conf.grid_y.show = true;
            conf.frame_size = ImGui::GetContentRegionMax();
            conf.frame_size.x -= 20.0f;
            conf.frame_size.y -= 30.0f;
            conf.line_thickness = 2.f;
            ImGui::Plot("Data Visualization", conf);
        } else {
            ImGui::Text("No data to show");
        }
    }

    ImGui::NextColumn();

    ImGui::Text("Chart settings");
    ImGui::InputFloat("Scale Min", &Statistics_ScaleMin, 0, 0, "%.6f");
    ImGui::InputFloat("Scale Max", &Statistics_ScaleMax, 0, 0, "%.6f");


    if (ImGui::ListBoxHeader("Data")) {

        for (size_t i = 0; i < IM_ARRAYSIZE(visSelection); i++) {
            ImGui::Selectable(visSelection[i], &Statistics_GraphSelection[i]);
        }

        ImGui::ListBoxFooter();
    }

    ImGui::Columns();

    ImGui::End();
}
