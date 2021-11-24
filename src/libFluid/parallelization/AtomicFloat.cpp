#include "AtomicFloat.hpp"

#include <algorithm>

namespace FluidSolver::Parallelization {

    AtomicFloat::AtomicFloat(float value)
        : value(value) {
    }
    float AtomicFloat::get() {
        float result = 0.0f;
        lock.lock();
        result = value;
        lock.unlock();
    }
    void AtomicFloat::set(float value) {
        lock.lock();
        this->value = value;
        lock.unlock();
    }
    void AtomicFloat::add(float value) {
        lock.lock();
        this->value += value;
        lock.unlock();
    }
    void AtomicFloat::subtract(float value) {
        lock.lock();
        this->value -= value;
        lock.unlock();
    }
    void AtomicFloat::multiply(float value) {
        lock.lock();
        this->value *= value;
        lock.unlock();
    }
    void AtomicFloat::divide(float value) {
        lock.lock();
        this->value /= value;
        lock.unlock();
    }
    void AtomicFloat::max(float other) {
        lock.lock();
        this->value = std::max(this->value, other);
        lock.unlock();
    }
    void AtomicFloat::min(float other) {
        lock.lock();
        this->value = std::min(this->value, other);
        lock.unlock();
    }
} // namespace FluidSolver::Parallelization