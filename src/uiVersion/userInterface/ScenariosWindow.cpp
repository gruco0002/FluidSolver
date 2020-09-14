#include "ScenariosWindow.hpp"
#include "core/FluidAssert.hpp"
#include "../FluidSolverWindow.hpp"
#include "../ImguiHelper.hpp"

#include <filesystem>

void ScenariosWindow::render() {
    FLUID_ASSERT(initialized)
    FLUID_ASSERT(window != nullptr)

    if (ImGui::Begin("Scenarios")) {
        ImGui::ListBox("Scenarios", &current_item, scenarios.data(), scenarios.size());
        if (ImGui::Button("(Re)Load Scenario")) {
            load();
        }
    }
    ImGui::End();
}

void ScenariosWindow::initialize() {
    FLUID_ASSERT(window != nullptr)
    FLUID_ASSERT(initialized == false)
    initialized = true;

    update_path();
}

void ScenariosWindow::update_path() {

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

void ScenariosWindow::load() {
    FLUID_ASSERT(scenarios.size() > 0)
    FLUID_ASSERT(window != nullptr)
    FLUID_ASSERT(current_item >= 0)
    FLUID_ASSERT(current_item < scenarios.size())
    std::string toLoad = path + scenarios[current_item];
    window->load_scenario(toLoad);
}

ScenariosWindow::~ScenariosWindow() {
    for (auto ptr: scenarios)
        delete[] ptr;
}
