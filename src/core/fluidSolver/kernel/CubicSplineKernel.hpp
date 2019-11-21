//
// Created by corbi on 25.04.2019.
//

#ifndef FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
#define FLUIDSOLVER_CUBICSPLINEKERNEL_HPP

#include <core/fluidSolver/kernel/IKernel.hpp>

namespace  FluidSolver {
    class CubicSplineKernel : public IKernel{

    public:
        float GetKernelValue(glm::vec2 position, float kernelSupport) override;

        glm::vec2 GetKernelDerivativeValue(glm::vec2 position, float kernelSupport) override;

    };
}

#endif //FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
