#include "CubicSplineKernel.hpp"

#include "LibFluidAssert.hpp"
#include "LibFluidMath.hpp"

#include <limits>

namespace LibFluid
{
    float CubicSplineKernel::GetKernelValue(const glm::vec2 &position) const
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

    glm::vec2 CubicSplineKernel::GetKernelDerivativeValue(const glm::vec2 &position) const
    {
        FLUID_ASSERT(h != 0.0f);
        FLUID_ASSERT(alpha != 0.0f);

        float length = glm::length(position);
        float q = length / h;

        glm::vec2 pre = glm::vec2(0.0f);
        if (length > std::numeric_limits<float>::epsilon())
            pre = h * position / length;

        glm::vec2 ret = glm::vec2(0.0f);
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
            ret = glm::vec2(0.0f);
        }

        // reverse the reversed implementation, since this function should return a non reversed kernel derivative value
        return alpha * ret * -1.0f;
    }

    float CubicSplineKernel::GetKernelValue(const glm::vec2 &neighborPosition, const glm::vec2 &position) const
    {
        return this->GetKernelValue(position - neighborPosition);
    }

    glm::vec2 CubicSplineKernel::GetKernelDerivativeValue(const glm::vec2 &neighborPosition,
                                                          const glm::vec2 &position) const
    {
        return this->GetKernelDerivativeValue(position - neighborPosition);
    }

    glm::vec2 CubicSplineKernel::GetKernelDerivativeReversedValue(const glm::vec2 &neighborPosition,
                                                                  const glm::vec2 &position) const
    {
        return this->GetKernelDerivativeValue(neighborPosition, position) * -1.0f;
    }

    void CubicSplineKernel::create_compatibility_report(CompatibilityReport &report)
    {
        report.begin_scope(FLUID_NAMEOF(CubicSplineKernel));
        if (kernel_support <= 0.0f)
        {
            report.add_issue("Kernel support radius is smaller or equal to zero!");
        }

        report.end_scope();
    }

    void CubicSplineKernel::initialize()
    {
        h = kernel_support / 2.0f;
        alpha = 5.0f / (14.0f * Math::PI * Math::pow2(h));
    }

} // namespace LibFluid
