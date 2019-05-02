//
// Created by corbi on 24.04.2019.
//

#include "IKernel.hpp"

float FluidSolver::IKernel::GetKernelValue(glm::vec2 neighborPosition, glm::vec2 position, float kernelSupport) {
    return this->GetKernelValue(position - neighborPosition, kernelSupport);
}

glm::vec2
FluidSolver::IKernel::GetKernelDerivativeValue(glm::vec2 neighborPosition, glm::vec2 position, float kernelSupport) {
    return this->GetKernelDerivativeValue(position - neighborPosition, kernelSupport);
}

glm::vec2 FluidSolver::IKernel::GetKernelDerivativeReversedValue(glm::vec2 neighborPosition, glm::vec2 position,
                                                                 float kernelSupport) {
    return this->GetKernelDerivativeValue(neighborPosition, position, kernelSupport) * -1.0f;
}
