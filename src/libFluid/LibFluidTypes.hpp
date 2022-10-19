#pragma once

#include "glm/glm.hpp"

#include <cstdint>

namespace LibFluid {


    using pIndex_t = size_t;
    using pTag_t = uint32_t;

    using pFloat = float;
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;

    struct Area {
        float left;
        float right;

        float bottom;
        float top;
    };

    struct Volume {
        glm::vec3 center;
        glm::vec3 distance_from_center;
    };

    namespace Constants {
        constexpr float kernel_support_factor = 2.0f;
    }

} // namespace FluidSolver
