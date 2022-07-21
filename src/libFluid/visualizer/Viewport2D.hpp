#pragma once

namespace LibFluid {

    struct Viewport2D
    {
        float left = -10.0f;
        float top = 10.0f;

        float right = 10.0f;
        float bottom = -10.0f;

        inline float width()
        {
            return right - left;
        }
        inline float height()
        {
            return top - bottom;
        }
    };


} // namespace FluidSolver