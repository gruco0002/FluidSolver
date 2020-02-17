//
// Created by corbi on 03.01.2020.
//

#include "MainUi.hpp"

#include <uiVersion/FluidSolverWindow.hpp>
#include <imguiHelper.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include <core/timestep/DynamicCFLTimestep.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <core/statistics/CachedStatisticCollector.hpp>
#include <core/selection/NormalParticleSelection.hpp>
#include <core/selection/ParticleSelection.hpp>

FluidUI::MainUi::MainUi(FluidSolverWindow *window) : window(window) {}

void FluidUI::MainUi::Run() {
    //ImGui::ShowDemoWindow();

    SimulationControl();
    Scenario();
    Simulation();
    FluidSolver();
    VisualizerSettings();
    Statistics();
    Selection();
    DataLogger();

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
        // change resolution
        Visualizer_Width = 1920;
        Visualizer_Height = 1080;
        window->setVisualizerRenderTargetWidth(Visualizer_Width);
        window->setVisualizerRenderTargetHeight(Visualizer_Height);

        // set particle renderer
        auto tmp = new ParticleRenderer();
        delete window->GetVisualizer();
        window->SetVisualizer(tmp);
        pr = tmp;
        cv = nullptr;

    } else if (Visualizer_Selection == 1 && cv == nullptr) {
        // change resolution
        Visualizer_Width = 300;
        Visualizer_Height = 300;
        window->setVisualizerRenderTargetWidth(Visualizer_Width);
        window->setVisualizerRenderTargetHeight(Visualizer_Height);

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

        ImGui::Checkbox("Show Selection", &pr->showParticleSelection);

    } else if (cv != nullptr) {
        ImGui::Text("Continuous Visualizer");
        ImGui::InputFloat("Min. Render Density", &cv->MinimumRenderDensity);
        ImGui::ColorEdit3("Background Color", reinterpret_cast<float *>(&cv->ClearColor));
        ImGui::Checkbox("Show Selection", &cv->VisualizeParticleSelection);
    }

    ImGui::End();
}

void FluidUI::MainUi::Statistics() {
    ImGui::Begin("Statistics");


    ImGui::Columns(2);
    auto cached = dynamic_cast<FluidSolver::CachedStatisticCollector *>(window->GetStatisticCollector());

    if (cached != nullptr) {
        Statistics_yData.clear();
        for (size_t i = 0; i < Statistics_GraphSelection.size() && i < cached->getStats().size(); i++) {
            if (!Statistics_GraphSelection[i])
                continue;

            auto tmp = dynamic_cast<FluidSolver::CachedStatValue *>(cached->getStats()[i]);
            if (tmp != nullptr)
                Statistics_yData.push_back(tmp->FloatCache.data());

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

    Statistics_Names.clear();
    for (auto val: cached->getStats()) {
        Statistics_Names.push_back(val->Name);
    }
    Statistics_GraphSelection.resize(Statistics_Names.size());


    if (ImGui::ListBoxHeader("Data")) {

        for (size_t i = 0; i < Statistics_Names.size(); i++) {
            ImGui::Selectable(Statistics_Names[i].c_str(), (bool *) Statistics_GraphSelection.data() + i);
        }

        ImGui::ListBoxFooter();
    }

    ImGui::Columns();

    ImGui::End();
}

void FluidUI::MainUi::Selection() {
    auto selection = window->GetParticleSelection();

    auto all = dynamic_cast<FluidSolver::AllParticleSelection *>(selection);
    auto normal = dynamic_cast<FluidSolver::NormalParticleSelection *>(selection);
    auto custom = dynamic_cast<FluidSolver::ParticleSelection *>(selection);

    ImGui::Begin("Selection");

    if (ImGui::Button("Select All")) {
        auto tmp = new FluidSolver::AllParticleSelection();
        delete window->GetParticleSelection();
        window->SetParticleSelection(tmp);
        all = tmp;
        custom = nullptr;
        normal = nullptr;
    }

    if (ImGui::Button("Select Normal")) {
        auto tmp = new FluidSolver::NormalParticleSelection();
        delete window->GetParticleSelection();
        window->SetParticleSelection(tmp);
        all = nullptr;
        custom = nullptr;
        normal = tmp;
    }

    ImGui::Separator();

    if (all != nullptr) {
        ImGui::Text("Selected: All Particles");
    } else if (normal != nullptr) {
        ImGui::Text("Selected: Normal Particles");
    } else if (custom != nullptr) {
        ImGui::Text("Selected: %zu Particles", custom->GetSize());
    }

    ImGui::End();
}

void FluidUI::MainUi::DataLogger() {
    ImGui::Begin("Data Logger");

    auto logger = window->GetDataLogger();
    if (logger->hasStarted()) {
        ImGui::Text("Logger is running...");
        if (ImGui::Button("Stop Logger")) {
            logger->FinishLogging();
        }
        ImGui::Separator();
        ImGui::Text("Saving to:");
        ImGui::InputText("Log File", const_cast<char *>(logger->fileName.c_str()), logger->fileName.size(),
                         ImGuiInputTextFlags_ReadOnly);
    } else if (logger->isLogFinished()) {
        ImGui::Text("Logger is finished");
        if (ImGui::Button("Reset Logger")) {
            logger->ResetLogger();
        }
    } else {
        // logger is not finished and not started --> logger can be started
        ImGui::Text("Logger is ready to be started");
        if (ImGui::Button("Start Logger")) {
            logger->StartLogging();
        }
    }

    if (logger->isLogFinished() || !logger->hasStarted()) {
        // draw ui stuff
        ImGui::Separator();

        if (ImGui::Button("Infty")) {
            logger->MaxLogLengthInSimulationSeconds = std::numeric_limits<float>::infinity();
        }
        ImGui::SameLine();
        ImGui::InputFloat("Log length in seconds", &logger->MaxLogLengthInSimulationSeconds);


        ImGui::Checkbox("Always Flush", &logger->alwaysFlush);

        ImGui::InputText("Log File", const_cast<char *>(logger->fileName.c_str()), logger->fileName.size(),
                         ImGuiInputTextFlags_ReadOnly);

        if (ImGui::Button("Choose Location")) {

            std::string path = logger->fileName;
            if (path.empty()) path = ".";
            if (path.find('\\') != std::string::npos) {
                path = path.substr(0, path.find_last_of('\\'));
            } else if (path.find('/') != std::string::npos) {
                path = path.substr(0, path.find_last_of('/'));
            }

            ImGuiFileDialog::Instance()->OpenDialog("LogLocationKey", "Log Location", ".csv\0.txt\0\0",
                                                    path, "log");
            ImGuiFileDialog::Instance()->SetFilterColor(".csv", ImVec4(0, 0, 1, 1.0));
            ImGuiFileDialog::Instance()->SetFilterColor(".txt", ImVec4(1, 0, 1, 1.0));
        }

        // display and action if ok
        if (ImGuiFileDialog::Instance()->FileDialog("LogLocationKey")) {
            if (ImGuiFileDialog::Instance()->IsOk) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                // action
                logger->fileName = filePathName;
            }
            // close
            ImGuiFileDialog::Instance()->CloseDialog("LogLocationKey");
        }


    }


    ImGui::End();
}
