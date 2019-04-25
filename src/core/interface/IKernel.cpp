//
// Created by corbi on 24.04.2019.
//

#include "IKernel.hpp"

float FluidSolver::IKernel::GetKernelValue(glm::vec2 origin, glm::vec2 position, float kernelSupport) {
    return this->GetKernelValue(position - origin, kernelSupport);
}

glm::vec2 FluidSolver::IKernel::GetKernelDerivativeValue(glm::vec2 origin, glm::vec2 position, float kernelSupport) {
    return this->GetKernelDerivativeValue(position - origin, kernelSupport);
}
