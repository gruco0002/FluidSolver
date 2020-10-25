#ifndef FLUIDSOLVER_FLUIDTYPES_HPP
#define FLUIDSOLVER_FLUIDTYPES_HPP

#include <cstdint>
#include "glm/glm.hpp"

namespace FluidSolver {

#ifdef WIN32
    // openmp on windows only supports signed integers

    using pIndex_t = int64_t;
    using pTag_t = int32_t;
#else
    using pIndex_t = size_t;
    using pTag_t = uint32_t;
#endif

    using pFloat = float;
    using vec2 = glm::vec2;

    struct Area {
        float left;
        float right;

        float bottom;
        float top;
    };

}

#endif //FLUIDSOLVER_FLUIDTYPES_HPP
