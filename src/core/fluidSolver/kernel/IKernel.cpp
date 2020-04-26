#include "IKernel.hpp"

float FluidSolver::IKernel::GetKernelValue(glm::vec2 neighborPosition, glm::vec2 position) const {
    return this->GetKernelValue(position - neighborPosition);
}

glm::vec2
FluidSolver::IKernel::GetKernelDerivativeValue(glm::vec2 neighborPosition, glm::vec2 position) const {
    return this->GetKernelDerivativeValue(position - neighborPosition);
}

glm::vec2 FluidSolver::IKernel::GetKernelDerivativeReversedValue(glm::vec2 neighborPosition, glm::vec2 position) const {
    return this->GetKernelDerivativeValue(neighborPosition, position) * -1.0f;
}

FluidSolver::IKernel::IKernel(float kernelSupport) : KernelSupport(kernelSupport) {}

FluidSolver::IKernel::~IKernel() {

}

float FluidSolver::IKernel::GetKernelSupport() const {
    return KernelSupport;
}
