#include "UiLayer.hpp"
#include "uiVersion/ImguiHelper.hpp"
#include <string>
#include <functional>
#include "uiVersion/FluidSolverWindow.hpp"


static void BeginSubsection(const std::string &name, const std::function<void()> &fnc) {
    const ImGuiTreeNodeFlags treeNodeFlags =
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
    ImGui::Separator();
    bool open = ImGui::TreeNodeEx((void *) name.c_str(), treeNodeFlags, name.c_str());
    ImGui::PopStyleVar();

    if (open) {
        fnc();
        ImGui::TreePop();
    }


}

void FluidUi::UiLayer::render() {

    ImGui::Begin("General");

    BeginSubsection("Simulation", [=]() {

        if (window->running)
            ImGui::TextColored(ImColor(0.1f, 0.8f, 0.1f), "Running");
        else
            ImGui::TextColored(ImColor(0.8f, 0.1f, 0.1f), "Paused");

        if (ImGui::Button("Run")) {
            this->window->running = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            this->window->running = false;
        }


    });

    BeginSubsection("Scenarios", [=]() {
        scenarios.render();
    });


    ImGui::End();

}

void FluidUi::UiLayer::initialize() {
    scenarios.window = window;
    scenarios.initialize();
}
