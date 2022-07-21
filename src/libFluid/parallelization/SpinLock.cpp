#include "SpinLock.hpp"


namespace LibFluid::Parallelization {

    void SpinLock::lock() {
        for (;;) {
            if (!lock_bit.exchange(true, std::memory_order_acquire)) {
                break;
            }
            while (lock_bit.load(std::memory_order_relaxed)) {
                // pause for roughly a cycle
                asm volatile("nop" ::: "memory");
            }
        }
    }
    void SpinLock::unlock() {
        lock_bit.store(false, std::memory_order_release);
    }
} // namespace FluidSolver