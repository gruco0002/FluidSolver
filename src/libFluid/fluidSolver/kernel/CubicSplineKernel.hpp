#ifndef FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
#define FLUIDSOLVER_CUBICSPLINEKERNEL_HPP


#include "Compatibility.hpp"
#include "FluidInclude.hpp"

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

        void initialize();

      private:
        float h = 0.0f;
        float alpha = 0.0f;
    };
} // namespace FluidSolver

#endif // FLUIDSOLVER_CUBICSPLINEKERNEL_HPP
