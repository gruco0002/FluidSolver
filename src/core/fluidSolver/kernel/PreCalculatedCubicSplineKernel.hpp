#ifndef FLUIDSOLVER_PRECALCULATEDCUBICSPLINEKERNEL_HPP
#define FLUIDSOLVER_PRECALCULATEDCUBICSPLINEKERNEL_HPP

#include <vector>
#include "IKernel.hpp"

namespace FluidSolver {
    class PreCalculatedCubicSplineKernel : public IKernel{
    public:
        IKernel *CreateCopy(float kernelSupport) override;

        explicit PreCalculatedCubicSplineKernel(float kernelSupport, size_t numberOfSamples = 1024);

        float GetKernelValue(glm::vec2 position) const override;

        glm::vec2 GetKernelDerivativeValue(glm::vec2 position) const override;

    private:

        size_t numberOfSamples;

        std::vector<float> kernelValues;
        std::vector<glm::vec2> kernelDerivativeValues;

        void PrecalculateValues();

    };
}


#endif //FLUIDSOLVER_PRECALCULATEDCUBICSPLINEKERNEL_HPP
