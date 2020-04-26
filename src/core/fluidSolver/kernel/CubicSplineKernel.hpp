#ifndef FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
#define FLUIDSOLVER_CUBICSPLINEKERNEL_HPP

#include <core/fluidSolver/kernel/IKernel.hpp>

namespace  FluidSolver {
    class CubicSplineKernel : public IKernel{

    public:
        CubicSplineKernel(float kernelSupport);

        float GetKernelValue(glm::vec2 position) const override;

        glm::vec2 GetKernelDerivativeValue(glm::vec2 position) const override;

    };
}

#endif //FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
