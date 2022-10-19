#pragma once


#include "LibFluidTypes.hpp"
#include "helpers/CompatibilityReport.hpp"

#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"

namespace LibFluid {
    class CubicSplineKernel : public Initializable, public Reportable {

      public:
        pFloat kernel_support;

        pFloat GetKernelValue(const vec2& position) const;

        vec2 GetKernelDerivativeValue(const vec2& position) const;

        pFloat GetKernelValue(const vec2& neighborPosition, const vec2& position) const;

        vec2 GetKernelDerivativeValue(const vec2& neighborPosition, const vec2& position) const;

        vec2 GetKernelDerivativeReversedValue(const vec2& neighborPosition, const vec2& position) const;

        void create_compatibility_report(CompatibilityReport &report) override;

        void initialize() override;

      private:
        float h = 0.0f;
        float alpha = 0.0f;
    };
} // namespace FluidSolver
