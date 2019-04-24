//
// Created by corbi on 24.04.2019.
//

#include "IKernel.hpp"

float FluidSolver::IKernel::GetKernelValue(glm::vec2 origin, glm::vec2 position, float particleSize) {
    return this->GetKernelValue(position - origin, particleSize);
}
