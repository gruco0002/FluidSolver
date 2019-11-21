//
// Created by corbi on 25.04.2019.
//

#include <core/interface/Constants.hpp>
#include "CubicSplineKernel.hpp"

float FluidSolver::CubicSplineKernel::GetKernelValue(glm::vec2 position, float kernelSupport) {
    float h = kernelSupport / 2.0f;
    float alpha = 5.0f / (14.0f * FS_PI * std::pow(h, 2.0f));
    float q = glm::length(position) / h;


    float ret = 0.0f;
    if (q < 1.0f) {
        ret = std::pow(2.0f - q, 3.0f) - 4.0f * std::pow(1.0f - q, 3.0f);
    } else if (q < 2.0f) {
        ret = std::pow(2.0f - q, 3.0f);
    } else if (q >= 2.0f) {
        ret = 0.0f;
    }
    return alpha * ret;

}

glm::vec2 FluidSolver::CubicSplineKernel::GetKernelDerivativeValue(glm::vec2 position, float kernelSupport) {
    float h = kernelSupport / 2.0f;
    float alpha = 5.0f / (14.0f * FS_PI * std::pow(h, 2.0f));
    float q = glm::length(position) / h;

    glm::vec2 pre = glm::vec2(0.0f);
    if (glm::length(position) != 0.0)
        pre = h * position / glm::length(position);


    glm::vec2 ret = glm::vec2(0.0f);
    // this is the reversed implementation
    if (q < 1.0f) {
        ret = pre * (-3.0f * std::pow(2.0f - q, 2.0f) + 12.0f * std::pow(1.0f - q, 2.0f));
    } else if (q < 2.0f) {
        ret = pre * (-3.0f * std::pow(2.0f - q, 2.0f));
    } else if (q >= 2.0f) {
        ret = glm::vec2(0.0f);
    }

    // reverse the reversed implementation, since this function should return a non reversed kernel derivative value
    return alpha * ret * -1.0f;
}