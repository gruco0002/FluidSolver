#pragma once

#include <cstdlib>
namespace FluidSolver::Raytracer {

    struct LightValue{
        float r;
        float g;
        float b;
    };

    class RenderTarget {


      public:

        void clear();
        void set_size(size_t width, size_t height);

        size_t get_width() const;
        size_t get_height() const;

    };


}