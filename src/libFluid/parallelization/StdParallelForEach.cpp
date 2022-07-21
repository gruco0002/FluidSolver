#include "StdParallelForEach.hpp"

#include <algorithm>
#include <execution>

#ifdef __clang__
    #include <tbb/tbb.h>
#endif

#include "parallelization/AtomicFloat.hpp"

class SizeTIterator {
  private:
    size_t index;

  public:
    using T = size_t;
    using iterator = SizeTIterator;
    using const_iterator = SizeTIterator;
    using difference_type = int64_t;
    using size_type = size_t;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

    SizeTIterator() = default;

    SizeTIterator(size_t index)
        : index(index) {
    }

    const size_t& operator*() const {
        return index;
    }

    const void operator++() {
        ++index;
    }

    bool operator!=(const SizeTIterator& lhs) const {
        return index != lhs.index;
    }

    const size_t operator+(const SizeTIterator& lhs) const {
        return index + lhs.index;
    }

    size_t operator-(const SizeTIterator& lhs) const {
        return index - lhs.index;
    }

    bool operator<(const SizeTIterator& lhs) {
        return index < lhs.index;
    }

    const bool operator<(const SizeTIterator& lhs) const {
        return index < lhs.index;
    }
};


void LibFluid::StdParallelForEach::loop_for(size_t from, size_t to, const std::function<void(size_t i)>& fn) {
    size_t chunked_to = from + (to - from) / chunk_size;
    if ((to - from) % chunk_size != 0)
        chunked_to++;

#ifndef __clang__
    std::for_each(std::execution::par, SizeTIterator(from), SizeTIterator(chunked_to), [&fn, to](size_t i) {
        size_t base = i * chunk_size;
        for (uint8_t j = 0; j < chunk_size && base + j < to; j++) {
            fn(base + j);
        }
    });
#else
    tbb::parallel_for(size_t(from), chunked_to, [&fn, to](size_t i) {
        size_t base = i * chunk_size;
        for (uint8_t j = 0; j < chunk_size && base + j < to; j++) {
            fn(base + j);
        }
    });
#endif
}

void LibFluid::StdParallelForEach::loop_for(size_t from, size_t to, size_t step,
        const std::function<void(size_t i)>& fn) {
    size_t steps = ((to - 1) - from) / step + 1;
#ifndef __clang__
    std::for_each(std::execution::par, SizeTIterator(0), SizeTIterator(steps),
            [&fn, from, step](size_t i) { fn(from + i * step); });
#else
    tbb::parallel_for(size_t(0), steps, [&fn, from, step](size_t i) { fn(from + i * step); });
#endif
}
float LibFluid::StdParallelForEach::loop_for_max(size_t from, size_t to, const std::function<float(size_t)>& fn) {
    size_t chunked_to = from + (to - from) / chunk_size;
    if ((to - from) % chunk_size != 0)
        chunked_to++;

    Parallelization::AtomicFloat atomic_result(std::numeric_limits<float>::lowest());

#ifndef __clang__
    std::for_each(std::execution::par, SizeTIterator(from), SizeTIterator(chunked_to), [&fn, to, &atomic_result](size_t i) {
        float result = std::numeric_limits<float>::lowest();

        size_t base = i * chunk_size;
        for (uint8_t j = 0; j < chunk_size && base + j < to; j++) {
            result = std::max(result, fn(base + j));
        }

        atomic_result.max(result);
    });
#else
    tbb::parallel_for(size_t(from), chunked_to, [&fn, to, &atomic_result](size_t i) {
        float result = std::numeric_limits<float>::lowest();

        size_t base = i * chunk_size;
        for (uint8_t j = 0; j < chunk_size && base + j < to; j++) {
            result = std::max(result, fn(base + j));
        }

        atomic_result.max(result);
    });
#endif

    return atomic_result.get();
}
