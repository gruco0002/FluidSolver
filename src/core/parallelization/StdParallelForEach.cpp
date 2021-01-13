#include "StdParallelForEach.hpp"

#include <algorithm>
#include <execution>

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

	SizeTIterator(size_t index) : index(index)
	{

	}

	const size_t& operator*() const
	{
		return index;
	}

	const void operator++() {
		++index;
	}

	bool operator!=(const SizeTIterator& lhs) const
	{
		return index != lhs.index;
	}

	const size_t operator+(const SizeTIterator& lhs) const
	{
		return index + lhs.index;
	}

	size_t operator-(const SizeTIterator& lhs) const
	{
		return index - lhs.index;
	}

};


void FluidSolver::StdParallelForEach::loop_for(size_t from, size_t to, const std::function<void(size_t i)>& fn)
{
#ifndef __clang__
	std::for_each(std::execution::par, SizeTIterator(from), SizeTIterator(to), fn);
#else
    #warning Apple Clang Compiler does not support StdParallelForEach. Currently using non parallel implementation instead.
    for(size_t i = from; i < to; i++) fn(i);
#endif
}

void FluidSolver::StdParallelForEach::loop_for(size_t from, size_t to, size_t step, const std::function<void(size_t i)>& fn)
{
#ifndef __clang__
	size_t steps = ((to - 1) - from) / step + 1;
    std::for_each(std::execution::par, SizeTIterator(0), SizeTIterator(steps), [&fn, from, step](size_t i) {fn(from + i * step); });
#else
    #warning Apple Clang Compiler does not support StdParallelForEach. Currently using non parallel implementation instead.
    for(size_t i = from; i < to; i += step) fn(i);
#endif
}
