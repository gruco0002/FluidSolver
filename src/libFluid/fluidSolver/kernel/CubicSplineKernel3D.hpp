#pragma once

#include "LibFluidTypes.hpp"
#include "helpers/CompatibilityReport.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"

namespace LibFluid {

    class CubicSplineKernel3D : public Initializable, public Reportable {
      public:
        pFloat kernel_support;

        pFloat GetKernelValue(const vec3& position) const;

        vec3 GetKernelDerivativeValue(const vec3& position) const;

        pFloat GetKernelValue(const vec3& neighborPosition, const vec3& position) const;

        vec3 GetKernelDerivativeValue(const vec3& neighborPosition, const vec3& position) const;

        vec3 GetKernelDerivativeReversedValue(const vec3& neighborPosition, const vec3& position) const;

        void initialize() override;

        void create_compatibility_report(CompatibilityReport& report) override;

      private:
        float h = 0.0f;
        float alpha = 0.0f;
    };


} // namespace LibFluid