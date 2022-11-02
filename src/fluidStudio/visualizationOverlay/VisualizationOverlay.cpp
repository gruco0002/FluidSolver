#include "VisualizationOverlay.hpp"

#include "ImguiHelper.hpp"
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

namespace FluidStudio {


    ImVec2 get_screen_position_by_world_position(const glm::vec3& world_position, const glm::mat4& view_projection_matrix, ImVec2 origin, ImVec2 size, bool* is_visible = nullptr) {
        glm::vec4 clip_space;
        clip_space = view_projection_matrix * glm::vec4(world_position, 1.0f);
        glm::vec4 ndc_space = clip_space / clip_space.w;

        if (is_visible != nullptr) {
            *is_visible = ndc_space.z <= 1.0f;
        }

        glm::vec4 screen_space = ndc_space * 0.5f;

        screen_space += glm::vec4(0.5f, 0.5f, 0.0f, 0.0f);
        screen_space.y = 1.f - screen_space.y;
        screen_space.x *= size.x;
        screen_space.y *= size.y;
        screen_space.x += origin.x;
        screen_space.y += origin.y;

        return ImVec2(screen_space.x, screen_space.y);
    }


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

        if (ImGuizmo::IsUsing()) {
            mouse_on_overlay = true;
        } else {
            mouse_on_overlay = false;
        }


        // render text if set
        auto display_text = data.overlay_instance->get_display_text();
        if (display_text != nullptr) {
            auto model_view_projection = data.visualizer_projection_matrix * data.visualizer_view_matrix * data.overlay_instance->get_matrix();
            bool is_visible = true;
            auto text_pos = get_screen_position_by_world_position(glm::vec3(0.0f), model_view_projection, {0, 0 + visualization_height}, {visualization_width, -visualization_height}, &is_visible);


            auto text_size = ImGui::CalcTextSize(display_text);
            text_pos.x -= text_size.x / 2;
            text_pos.y -= 130;

            if (is_visible) {
                {
                    auto p_min = pos;
                    p_min.x += text_pos.x;
                    p_min.y += text_pos.y;

                    p_min.x -= ImGui::GetStyle().FramePadding.x * 2.0f;
                    p_min.y -= ImGui::GetStyle().FramePadding.y * 2.0f;

                    auto p_max = p_min;
                    p_max.x += text_size.x;
                    p_max.y += text_size.y;

                    p_max.x += ImGui::GetStyle().FramePadding.x * 4.0f;
                    p_max.y += ImGui::GetStyle().FramePadding.y * 4.0f;

                    ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_Button]));
                }
                ImGui::SetCursorPos(text_pos);
                ImGui::Text("%s", display_text);
            }
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
} // namespace FluidStudio