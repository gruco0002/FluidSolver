#include "PreCalculatedCubicSplineKernel.hpp"

float FluidSolver::PreCalculatedCubicSplineKernel::GetKernelValue(glm::vec2 position) const {
    return 0;
}

glm::vec2
FluidSolver::PreCalculatedCubicSplineKernel::GetKernelDerivativeValue(glm::vec2 position) const {
    return glm::vec2();
}

FluidSolver::PreCalculatedCubicSplineKernel::PreCalculatedCubicSplineKernel(float kernelSupport, size_t numberOfSamples)
        : IKernel(kernelSupport), numberOfSamples(numberOfSamples) {

    PrecalculateValues();

}

void FluidSolver::PreCalculatedCubicSplineKernel::PrecalculateValues() {



}
