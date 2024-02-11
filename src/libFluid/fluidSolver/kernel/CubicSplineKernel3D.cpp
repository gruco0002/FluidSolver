#include "CubicSplineKernel3D.hpp"

#include "LibFluidAssert.hpp"
#include "LibFluidMath.hpp"

namespace LibFluid
{
    float CubicSplineKernel3D::GetKernelValue(const glm::vec3 &position) const
    {
        FLUID_ASSERT(h != 0.0f);
        FLUID_ASSERT(alpha != 0.0f);

        float q = glm::length(position) / h;

        float ret = 0.0f;
        if (q < 1.0f)
        {
            ret = Math::pow3(2.0f - q) - 4.0f * Math::pow3(1.0f - q);
        }
        else if (q < 2.0f)
        {
            ret = Math::pow3(2.0f - q);
        }
        else if (q >= 2.0f)
        {
            ret = 0.0f;
        }
        return alpha * ret;
    }

    glm::vec3 CubicSplineKernel3D::GetKernelDerivativeValue(const glm::vec3 &position) const
    {
        FLUID_ASSERT(h != 0.0f);
        FLUID_ASSERT(alpha != 0.0f);

        float length = glm::length(position);
        float q = length / h;

        glm::vec3 pre = glm::vec3(0.0f);
        if (length > std::numeric_limits<float>::epsilon())
            pre = h * position / length;

        glm::vec3 ret = glm::vec3(0.0f);
        // this is the reversed implementation
        if (q < 1.0f)
        {
            ret = pre * (-3.0f * Math::pow2(2.0f - q) + 12.0f * Math::pow2(1.0f - q));
        }
        else if (q < 2.0f)
        {
            ret = pre * (-3.0f * Math::pow2(2.0f - q));
        }
        else if (q >= 2.0f)
        {
            ret = glm::vec3(0.0f);
        }

        // reverse the reversed implementation, since this function should return a non reversed kernel derivative value
        return alpha * ret * -1.0f;
    }

    float CubicSplineKernel3D::GetKernelValue(const glm::vec3 &neighborPosition, const glm::vec3 &position) const
    {
        return this->GetKernelValue(position - neighborPosition);
    }

    glm::vec3 CubicSplineKernel3D::GetKernelDerivativeValue(const glm::vec3 &neighborPosition,
                                                            const glm::vec3 &position) const
    {
        return this->GetKernelDerivativeValue(position - neighborPosition);
    }

    glm::vec3 CubicSplineKernel3D::GetKernelDerivativeReversedValue(const glm::vec3 &neighborPosition,
                                                                    const glm::vec3 &position) const
    {
        return this->GetKernelDerivativeValue(neighborPosition, position) * -1.0f;
    }

    void CubicSplineKernel3D::initialize()
    {
        h = kernel_support / 2.0f;
        alpha = 1.0f / (4.0f * Math::PI * Math::pow3(h));
    }

    void CubicSplineKernel3D::create_compatibility_report(CompatibilityReport &report)
    {
        report.begin_scope(FLUID_NAMEOF(CubicSplineKernel3D));
        if (kernel_support <= 0.0f)
        {
            report.add_issue("Kernel support radius is smaller or equal to zero!");
        }
        report.end_scope();
    }
} // namespace LibFluid
