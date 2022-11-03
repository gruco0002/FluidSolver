#pragma once

#include "helpers/CompatibilityReport.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"

#include <glm/glm.hpp>

namespace LibFluid {
    class CubicSplineKernel : public Initializable, public Reportable {
      public:
        float kernel_support;

        float GetKernelValue(const glm::vec2& position) const;

        glm::vec2 GetKernelDerivativeValue(const glm::vec2& position) const;

        float GetKernelValue(const glm::vec2& neighborPosition, const glm::vec2& position) const;

        glm::vec2 GetKernelDerivativeValue(const glm::vec2& neighborPosition, const glm::vec2& position) const;

        glm::vec2 GetKernelDerivativeReversedValue(const glm::vec2& neighborPosition, const glm::vec2& position) const;

        void create_compatibility_report(CompatibilityReport& report) override;

        void initialize() override;

      private:
        float h = 0.0f;
        float alpha = 0.0f;
    };
} // namespace LibFluid
