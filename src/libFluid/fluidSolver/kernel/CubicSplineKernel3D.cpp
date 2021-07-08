#include "CubicSplineKernel3D.hpp"


FluidSolver::pFloat FluidSolver::CubicSplineKernel3D::GetKernelValue(const vec3& position) const
{
    float q = glm::length(position) / h;

    float ret = 0.0f;
    if (q < 1.0f)
    {
        ret = pow3(2.0f - q) - 4.0f * pow3(1.0f - q);
    }
    else if (q < 2.0f)
    {
        ret = pow3(2.0f - q);
    }
    else if (q >= 2.0f)
    {
        ret = 0.0f;
    }
    return alpha * ret;
}

FluidSolver::vec3 FluidSolver::CubicSplineKernel3D::GetKernelDerivativeValue(const vec3& position) const
{

    float length = glm::length(position);
    float q = length / h;

    vec3 pre = vec3(0.0f);
    if (length > std::numeric_limits<float>::epsilon())
        pre = h * position / length;


    vec3 ret = vec3(0.0f);
    // this is the reversed implementation
    if (q < 1.0f)
    {
        ret = pre * (-3.0f * pow2(2.0f - q) + 12.0f * pow2(1.0f - q));
    }
    else if (q < 2.0f)
    {
        ret = pre * (-3.0f * pow2(2.0f - q));
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

void FluidSolver::CubicSplineKernel3D::initialize()
{
    h = kernel_support / 2.0f;
    alpha = 1.0f / (4.0f * FS_PI * std::pow(h, 3.0f));
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