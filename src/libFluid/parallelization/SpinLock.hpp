#pragma once

#include <atomic>

namespace LibFluid::Parallelization {
    class SpinLock {
      public:
        void lock();
        void unlock();

      private:
        std::atomic<bool> lock_bit = {false};
    };

} // namespace FluidSolver
