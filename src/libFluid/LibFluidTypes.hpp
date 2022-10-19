#pragma once

#include "glm/glm.hpp"

#include <cstdint>

namespace LibFluid {


    using pIndex_t = size_t;
    using pTag_t = uint32_t;

    using pFloat = float;
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;


    namespace Constants {
        constexpr float kernel_support_factor = 2.0f;
    }

} // namespace LibFluid
