#include "SpinLock.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace LibFluid::Parallelization {

    void SpinLock::lock() {
        for (;;) {
            if (!lock_bit.exchange(true, std::memory_order_acquire)) {
                break;
            }
            while (lock_bit.load(std::memory_order_relaxed)) {
                // pause for roughly a cycle
#ifndef _MSC_VER
                asm volatile("nop" ::: "memory");
#else
                __nop();
#endif
            }
        }
    }
    void SpinLock::unlock() {
        lock_bit.store(false, std::memory_order_release);
    }
} // namespace FluidSolver