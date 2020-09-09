#ifndef FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
#define FLUIDSOLVER_CUBICSPLINEKERNEL_HPP


#include <engine/libraries/glm/glm.hpp>

namespace FluidSolver {
    class CubicSplineKernel {

    public:
        float kernel_support;

        float GetKernelValue(glm::vec2 position) const;

        glm::vec2 GetKernelDerivativeValue(glm::vec2 position) const;

        float GetKernelValue(glm::vec2 neighborPosition, glm::vec2 position) const;

        glm::vec2 GetKernelDerivativeValue(glm::vec2 neighborPosition, glm::vec2 position) const;

        glm::vec2 GetKernelDerivativeReversedValue(glm::vec2 neighborPosition, glm::vec2 position) const;

    };
}

#endif //FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
