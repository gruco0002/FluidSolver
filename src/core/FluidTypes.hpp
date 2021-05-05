#pragma once

#include "glm/glm.hpp"

#include <cstdint>

namespace FluidSolver
{


    using pIndex_t = size_t;
    using pTag_t = uint32_t;

    using pFloat = float;
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;

    struct Area
    {
        float left;
        float right;

        float bottom;
        float top;
    };

} // namespace FluidSolver
