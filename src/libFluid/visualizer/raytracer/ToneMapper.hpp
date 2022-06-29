#pragma once

#include "visualizer/Image.hpp"
#include "visualizer/raytracer/RenderTarget.hpp"

namespace FluidSolver::Raytracer {
    class ToneMapper {

      public:

        void map_render_target_to_image(const RenderTarget& render_target, Image& image);

    };
}