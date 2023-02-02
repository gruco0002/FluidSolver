#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class VisualizerSettingsElement final : public UiElement {
      public:
        void update() override;

      private:
        void update_gl_renderer();
        void update_gl_renderer_3d();
        void update_continuous_visualizer();
        void update_raytracer();
        void update_simple_raytracer();

        void transfer_editor_view_to_new_visualizer();
    };
} // namespace FluidStudio
