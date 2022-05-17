#pragma once

#include "CompatibilityReport.hpp"
#include "FluidInclude.hpp"

namespace FluidSolver
{

    class CubicSplineKernel3D {
      public:
        pFloat kernel_support;

        pFloat GetKernelValue(const vec3& position) const;

        vec3 GetKernelDerivativeValue(const vec3& position) const;

        pFloat GetKernelValue(const vec3& neighborPosition, const vec3& position) const;

        vec3 GetKernelDerivativeValue(const vec3& neighborPosition, const vec3& position) const;

        vec3 GetKernelDerivativeReversedValue(const vec3& neighborPosition, const vec3& position) const;

        void initialize();

        void create_compatibility_report(CompatibilityReport &report);

      private:
        float h = 0.0f;
        float alpha = 0.0f; 
     
    };


} // namespace FluidSolver