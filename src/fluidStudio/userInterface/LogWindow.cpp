#include "LogWindow.hpp"

#include "FluidSolverWindow.hpp"

#include <Log.hpp>
#include <imgui.h>


void FluidStudio::LogWindow::render()
{
    ImGui::Begin("Logs");
    auto entries = LibFluid::Log::get_entries();
    if (messages.size() < entries.size())
        messages.resize(entries.size());
    ImGui::Checkbox("Scroll to bottom", &scroll_to_bottom);
    ImGui::Separator();
    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

    for (int i = 0; i < entries.size(); i++)
    {
        auto& entry = entries[i];
        messages[i] = entry.human_readable();
        if (entry.type == LibFluid::Log::Type::TYPE_ERROR)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), messages[i].c_str());
        }
        else if (entry.type == LibFluid::Log::Type::TYPE_ERROR)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), messages[i].c_str());
        }
        else
        {
            ImGui::TextUnformatted(messages[i].c_str());
        }
    }
    ImGui::PopStyleVar();
    if (scroll_to_bottom)
        ImGui::SetScrollHereY();
    ImGui::EndChild();
    ImGui::End();
}

void FluidStudio::LogWindow::initialize()
{
}
