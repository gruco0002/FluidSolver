#pragma once

#include "Compatibility.hpp"
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

        Compatibility check();
    };


} // namespace FluidSolver