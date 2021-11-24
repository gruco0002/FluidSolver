#pragma once

#include "parallelization/SpinLock.hpp"

namespace FluidSolver::Parallelization {
    class AtomicFloat {
      public:
        AtomicFloat(float value);

        float get();
        void set(float value);

        void add(float value);
        void subtract(float value);
        void multiply(float value);
        void divide(float value);

        void max(float other);
        void min(float other);

      private:
        float value = 0.0f;
        SpinLock lock;
    };
} // namespace FluidSolver::Parallelization
