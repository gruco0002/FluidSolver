#include "ScenariosWindow.hpp"
#include "core/FluidAssert.hpp"
#include "../FluidSolverWindow.hpp"
#include "../ImguiHelper.hpp"

#include <filesystem>

void FluidUi::ScenariosWindow::render() {
    FLUID_ASSERT(initialized)
    FLUID_ASSERT(window != nullptr)

    ImGui::ListBox("Scenarios", &current_item, scenarios.data(), scenarios.size());
    if (window->asynchronous_simulation) {
        if (window->running || !window->is_done_working()) {
            ImGui::TextColored(ImColor(0.8f, 0.1f, 0.1f), "Scenario Loading not possible");
            ImGui::TextWrapped(
                    "Since you are running the simulation in asynchronous mode, you cannot load a scenario if the simulation is running or has not stopped running!");
        }
    }
    if (ImGui::Button("(Re)Load Scenario") &&
        (!window->asynchronous_simulation || (!window->running && window->is_done_working()))) {
        load();
    }

}

void FluidUi::ScenariosWindow::initialize() {
    FLUID_ASSERT(window != nullptr)
    FLUID_ASSERT(initialized == false)
    initialized = true;

    update_path();
}

void FluidUi::ScenariosWindow::update_path() {

    for (auto ptr: scenarios)
        delete[] ptr;
    scenarios.clear();
    current_item = 0;

    namespace fs = std::filesystem;

    for (const auto &entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            auto filename = entry.path().filename().string();
            char *tmp = new char[filename.size() + 1];
            memcpy(tmp, filename.c_str(), filename.size());
            tmp[filename.size()] = 0;
            scenarios.push_back(tmp);
        }
    }

}

void FluidUi::ScenariosWindow::load() {
    FLUID_ASSERT(scenarios.size() > 0)
    FLUID_ASSERT(window != nullptr)
    FLUID_ASSERT(current_item >= 0)
    FLUID_ASSERT(current_item < scenarios.size())
    std::string toLoad = path + scenarios[current_item];
    window->load_scenario(toLoad);
}

FluidUi::ScenariosWindow::~ScenariosWindow() {
    for (auto ptr: scenarios)
        delete[] ptr;
}
