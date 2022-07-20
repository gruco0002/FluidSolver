#pragma once

#include "engine/graphics/Framebuffer.hpp"
#include "visualizationOverlay/OverlayCubeRenderer.hpp"
#include <glm/glm.hpp>

namespace FluidUi {
    class VisualizationOverlay {

      public:

        VisualizationOverlay();

        struct {
            glm::mat4 visualizer_view_matrix;
            glm::mat4 visualizer_projection_matrix;


        } data;


        void render(float visualization_width, float visualization_height);

        void render_overlay_into_framebuffer(Engine::Graphics::Framebuffer* framebuffer);

        bool is_mouse_on_overlay() const;

        bool has_data_changed() const;

      private:

        bool mouse_on_overlay = false;

        OverlayCubeRenderer overlay_renderer;

        glm::mat4 matrix;

        bool data_changed = false;



    };

}