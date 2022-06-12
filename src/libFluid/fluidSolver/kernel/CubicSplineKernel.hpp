#pragma once


#include "CompatibilityReport.hpp"
#include "FluidInclude.hpp"

#include "Initializable.hpp"
#include "Reportable.hpp"

namespace FluidSolver
{
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
