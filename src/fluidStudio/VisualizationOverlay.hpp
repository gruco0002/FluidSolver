#pragma once

#include <glm/glm.hpp>

namespace FluidUi {
    class VisualizationOverlay {

      public:

        struct {
            glm::mat4 visualizer_view_matrix;
            glm::mat4 visualizer_projection_matrix;


        } data;


        void render(float visualization_width, float visualization_height);

        bool is_mouse_on_overlay() const;

      private:

        bool mouse_on_overlay = false;





    };

}