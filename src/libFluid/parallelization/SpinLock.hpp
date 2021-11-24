#pragma once

#include <atomic>

namespace FluidSolver::Parallelization {
    class SpinLock {
      public:
        void lock();
        void unlock();

      private:
        std::atomic<bool> lock_bit = {false};
    };

} // namespace FluidSolver
