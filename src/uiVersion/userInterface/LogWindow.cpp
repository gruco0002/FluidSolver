#include "LogWindow.hpp"

#include "uiVersion/FluidSolverWindow.hpp"

#include <imgui.h>

#include <core/Log.hpp>


void FluidUi::LogWindow::render()
{
	ImGui::Begin("Logs");
	auto entries = FluidSolver::Log::get_entries();
	if (messages.size() < entries.size()) messages.resize(entries.size());
	ImGui::Checkbox("Scroll to bottom", &scroll_to_bottom);
	ImGui::Separator();
	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

	for (int i = 0; i < entries.size(); i++) {
		auto& entry = entries[i];
		messages[i] = entry.human_readable();
		if (entry.type == FluidSolver::Log::Type::TYPE_ERROR) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), messages[i].c_str());
		}
		else if (entry.type == FluidSolver::Log::Type::TYPE_ERROR) {
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), messages[i].c_str());
		}
		else {
			ImGui::TextUnformatted(messages[i].c_str());
		}

	}
	ImGui::PopStyleVar();
	if (scroll_to_bottom)
		ImGui::SetScrollHere(1.0f);
	ImGui::EndChild();
	ImGui::End();
}

void FluidUi::LogWindow::initialize()
{

}
