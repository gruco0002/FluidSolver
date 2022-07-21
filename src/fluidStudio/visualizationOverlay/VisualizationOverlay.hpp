#pragma once

#include "engine/graphics/Framebuffer.hpp"
#include "visualizationOverlay/OverlayCubeRenderer.hpp"
#include "visualizationOverlay/OverlayInstance.hpp"
#include <glm/glm.hpp>

namespace FluidStudio {
    class VisualizationOverlay {
      public:

        struct {
            glm::mat4 visualizer_view_matrix;
            glm::mat4 visualizer_projection_matrix;

            std::shared_ptr<OverlayInstance> overlay_instance = nullptr;
        } data;


        void render(float visualization_width, float visualization_height);

        void render_overlay_into_framebuffer(Engine::Graphics::Framebuffer* framebuffer);

        bool is_mouse_on_overlay() const;

        bool has_data_changed() const;

        void set_new_overlay_instance(std::shared_ptr<OverlayInstance> new_instance);

      private:
        bool mouse_on_overlay = false;
        bool data_changed = false;

        OverlayCubeRenderer overlay_cube_renderer;
    };

} // namespace FluidUi