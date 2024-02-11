#pragma once

#include <glm/glm.hpp>

namespace LibFluid
{

    struct Volume
    {
        glm::vec3 center;
        glm::vec3 distance_from_center;
    };

} // namespace LibFluid
