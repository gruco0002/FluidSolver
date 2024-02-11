#pragma once

#include "helpers/CompatibilityReport.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"

#include <glm/glm.hpp>

namespace LibFluid
{

    class CubicSplineKernel3D : public Initializable, public Reportable
    {
      public:
        float kernel_support;

        float GetKernelValue(const glm::vec3 &position) const;

        glm::vec3 GetKernelDerivativeValue(const glm::vec3 &position) const;

        float GetKernelValue(const glm::vec3 &neighborPosition, const glm::vec3 &position) const;

        glm::vec3 GetKernelDerivativeValue(const glm::vec3 &neighborPosition, const glm::vec3 &position) const;

        glm::vec3 GetKernelDerivativeReversedValue(const glm::vec3 &neighborPosition, const glm::vec3 &position) const;

        void initialize() override;

        void create_compatibility_report(CompatibilityReport &report) override;

      private:
        float h = 0.0f;
        float alpha = 0.0f;
    };

} // namespace LibFluid
