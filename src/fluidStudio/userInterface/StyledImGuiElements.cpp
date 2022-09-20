#include "StyledImGuiElements.hpp"

#include "ImguiHelper.hpp"

namespace FluidStudio::StyledImGuiElements {

    bool slim_tree_node(const char* name, void* ptr_id) {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_FramePadding;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4, 4});
        ImGui::Separator();
        if (ptr_id == nullptr)
            ptr_id = (void*)name;
        bool open = ImGui::TreeNodeEx(ptr_id, treeNodeFlags, name);
        ImGui::PopStyleVar();

        return open;
    }


} // namespace FluidStudio::StyledImGuiElements
