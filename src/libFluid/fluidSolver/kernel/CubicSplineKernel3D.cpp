#include "CubicSplineKernel3D.hpp"


FluidSolver::pFloat FluidSolver::CubicSplineKernel3D::GetKernelValue(const vec3& position) const
{
    float h = kernel_support / 2.0f;
    float alpha = 1.0f / (4.0f * FS_PI * std::pow(h, 3.0f));
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

FluidSolver::vec3 FluidSolver::CubicSplineKernel3D::GetKernelDerivativeValue(const vec3& position) const
{
    float h = kernel_support / 2.0f;
    float alpha = 1.0f / (4.0f * FS_PI * std::pow(h, 3.0f));
    float q = glm::length(position) / h;

    vec3 pre = vec3(0.0f);
    if (glm::length(position) > std::numeric_limits<float>::epsilon())
        pre = h * position / glm::length(position);


    vec3 ret = vec3(0.0f);
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
        ret = vec3(0.0f);
    }

    // reverse the reversed implementation, since this function should return a non reversed kernel derivative value
    return alpha * ret * -1.0f;
}


FluidSolver::pFloat FluidSolver::CubicSplineKernel3D::GetKernelValue(const vec3& neighborPosition,
                                                                     const vec3& position) const
{
    return this->GetKernelValue(position - neighborPosition);
}


FluidSolver::vec3 FluidSolver::CubicSplineKernel3D::GetKernelDerivativeValue(const vec3& neighborPosition,
                                                                             const vec3& position) const
{
    return this->GetKernelDerivativeValue(position - neighborPosition);
}


FluidSolver::vec3 FluidSolver::CubicSplineKernel3D::GetKernelDerivativeReversedValue(const vec3& neighborPosition,
                                                                                     const vec3& position) const
{
    return this->GetKernelDerivativeValue(neighborPosition, position) * -1.0f;
}

FluidSolver::Compatibility FluidSolver::CubicSplineKernel3D::check()
{
    Compatibility c;
    if (kernel_support <= 0.0f)
    {
        c.add_issue({"CubicSplineKernel3D", "Kernel support radius is smaller or equal to zero!"});
    }
    return c;
}