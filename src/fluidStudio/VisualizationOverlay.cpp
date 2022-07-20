#include "VisualizationOverlay.hpp"

#include "ImguiHelper.hpp"
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

namespace FluidUi {

    void VisualizationOverlay::render(float visualization_width, float visualization_height) {
        static glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f));


        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        auto pos = ImGui::GetWindowPos();

        // since we have flipped rendering use that
        ImGuizmo::SetRect(pos.x, pos.y + visualization_height, visualization_width, -visualization_height);

        ImGuizmo::DrawCubes(glm::value_ptr(data.visualizer_view_matrix), glm::value_ptr(data.visualizer_projection_matrix), glm::value_ptr(matrix), 1);
        // ImGuizmo::DrawGrid(glm::value_ptr(data.visualizer_view_matrix), glm::value_ptr(data.visualizer_projection_matrix),glm::value_ptr(matrix), 2.0f);
        ImGuizmo::Manipulate(glm::value_ptr(data.visualizer_view_matrix), glm::value_ptr(data.visualizer_projection_matrix), ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE, ImGuizmo::MODE::LOCAL, glm::value_ptr(matrix));


        if (ImGuizmo::IsOver() || ImGuizmo::IsUsing()) {
            mouse_on_overlay = true;
        } else {
            mouse_on_overlay = false;
        }
    }
    bool VisualizationOverlay::is_mouse_on_overlay() const {
        return mouse_on_overlay;
    }
} // namespace FluidUi