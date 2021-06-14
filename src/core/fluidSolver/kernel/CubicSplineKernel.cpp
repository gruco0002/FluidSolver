#include "CubicSplineKernel.hpp"

#include <core/interface/Constants.hpp>
#include <limits>

float FluidSolver::CubicSplineKernel::GetKernelValue(const glm::vec2& position) const
{
    float h = kernel_support / 2.0f;
    float alpha = 5.0f / (14.0f * FS_PI * std::pow(h, 2.0f));
    float q = glm::length(position) / h;


    float ret = 0.0f;
    if (q < 1.0f)
    {
        ret = std::pow(2.0f - q, 3.0f) - 4.0f * std::pow(1.0f - q, 3.0f);
    }
    else if (q < 2.0f)
    {
        ret = std::pow(2.0f - q, 3.0f);
    }
    else if (q >= 2.0f)
    {
        ret = 0.0f;
    }
    return alpha * ret;
}

glm::vec2 FluidSolver::CubicSplineKernel::GetKernelDerivativeValue(const glm::vec2& position) const
{
    float h = kernel_support / 2.0f;
    float alpha = 5.0f / (14.0f * FS_PI * std::pow(h, 2.0f));
    float q = glm::length(position) / h;

    glm::vec2 pre = glm::vec2(0.0f);
    if (glm::length(position) > std::numeric_limits<float>::epsilon())
        pre = h * position / glm::length(position);


    glm::vec2 ret = glm::vec2(0.0f);
    // this is the reversed implementation
    if (q < 1.0f)
    {
        ret = pre * (-3.0f * std::pow(2.0f - q, 2.0f) + 12.0f * std::pow(1.0f - q, 2.0f));
    }
    else if (q < 2.0f)
    {
        ret = pre * (-3.0f * std::pow(2.0f - q, 2.0f));
    }
    else if (q >= 2.0f)
    {
        ret = glm::vec2(0.0f);
    }

    // reverse the reversed implementation, since this function should return a non reversed kernel derivative value
    return alpha * ret * -1.0f;
}


float FluidSolver::CubicSplineKernel::GetKernelValue(const glm::vec2& neighborPosition, const glm::vec2& position) const
{
    return this->GetKernelValue(position - neighborPosition);
}

glm::vec2 FluidSolver::CubicSplineKernel::GetKernelDerivativeValue(const glm::vec2& neighborPosition,
                                                                   const glm::vec2& position) const
{
    return this->GetKernelDerivativeValue(position - neighborPosition);
}

glm::vec2 FluidSolver::CubicSplineKernel::GetKernelDerivativeReversedValue(const glm::vec2& neighborPosition,
                                                                           const glm::vec2& position) const
{
    return this->GetKernelDerivativeValue(neighborPosition, position) * -1.0f;
}


FluidSolver::Compatibility FluidSolver::CubicSplineKernel::check()
{
    Compatibility c;
    if (kernel_support <= 0.0f)
    {
        c.add_issue({"CubicSplineKernel", "Kernel support radius is smaller or equal to zero!"});
    }
    return c;
}