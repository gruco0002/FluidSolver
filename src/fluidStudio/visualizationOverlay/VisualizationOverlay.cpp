#include "VisualizationOverlay.hpp"

#include "ImguiHelper.hpp"
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

namespace FluidUi {

    void VisualizationOverlay::render(float visualization_width, float visualization_height) {
        if (data.overlay_instance == nullptr) {
            return;
        }

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        auto pos = ImGui::GetWindowPos();

        // since we have flipped rendering use that
        ImGuizmo::SetRect(pos.x, pos.y + visualization_height, visualization_width, -visualization_height);

        auto matrix = data.overlay_instance->get_matrix();

        auto mode = ImGuizmo::OPERATION::TRANSLATE;
        switch (data.overlay_instance->get_allowed_transforms()) {
            case OverlayInstance::AllowedTransforms::Translate:
                mode = ImGuizmo::OPERATION::TRANSLATE;
                break;
            case OverlayInstance::AllowedTransforms::Scale:
                mode = ImGuizmo::OPERATION::SCALE;
                break;
            case OverlayInstance::AllowedTransforms::TranslateAndScale:
                mode = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE;
                break;
        }

        // ImGuizmo::DrawCubes(glm::value_ptr(data.visualizer_view_matrix), glm::value_ptr(data.visualizer_projection_matrix), glm::value_ptr(matrix), 1);
        // ImGuizmo::DrawGrid(glm::value_ptr(data.visualizer_view_matrix), glm::value_ptr(data.visualizer_projection_matrix),glm::value_ptr(matrix), 2.0f);
        if (ImGuizmo::Manipulate(glm::value_ptr(data.visualizer_view_matrix), glm::value_ptr(data.visualizer_projection_matrix), mode, ImGuizmo::MODE::WORLD, glm::value_ptr(matrix))) {
            // data changed
            data_changed = true;
            data.overlay_instance->set_matrix(matrix);
        }

        if (ImGuizmo::IsOver() || ImGuizmo::IsUsing()) {
            mouse_on_overlay = true;
        } else {
            mouse_on_overlay = false;
        }
    }
    bool VisualizationOverlay::is_mouse_on_overlay() const {
        return mouse_on_overlay;
    }


    void VisualizationOverlay::render_overlay_into_framebuffer(Engine::Graphics::Framebuffer* framebuffer) {
        data_changed = false;

        if (data.overlay_instance == nullptr) {
            return;
        }

        if (data.overlay_instance->get_display() == OverlayInstance::Display::Cube) {
            const auto matrix = data.overlay_instance->get_matrix();
            overlay_cube_renderer.data.framebuffer = framebuffer;
            overlay_cube_renderer.data.view_matrix = data.visualizer_view_matrix;
            overlay_cube_renderer.data.projection_matrix = data.visualizer_projection_matrix;

            overlay_cube_renderer.render(matrix);
        }
    }


    bool VisualizationOverlay::has_data_changed() const {
        return data_changed;
    }
    void VisualizationOverlay::set_new_overlay_instance(std::shared_ptr<OverlayInstance> new_instance) {
        data.overlay_instance = new_instance;
        data_changed = true;
    }
} // namespace FluidUi