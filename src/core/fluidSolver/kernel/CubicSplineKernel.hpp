#ifndef FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
#define FLUIDSOLVER_CUBICSPLINEKERNEL_HPP


#include "core/Compatibility.hpp"
#include "core/FluidInclude.hpp"

namespace FluidSolver
{
    class CubicSplineKernel {

      public:
        pFloat kernel_support;

        pFloat GetKernelValue(const vec2& position) const;

        vec2 GetKernelDerivativeValue(const vec2& position) const;

        pFloat GetKernelValue(const vec2& neighborPosition, const vec2& position) const;

        vec2 GetKernelDerivativeValue(const vec2& neighborPosition, const vec2& position) const;

        vec2 GetKernelDerivativeReversedValue(const vec2& neighborPosition, const vec2& position) const;

        Compatibility check();
    };
} // namespace FluidSolver

#endif // FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
