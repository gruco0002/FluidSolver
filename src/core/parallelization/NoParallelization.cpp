#include "NoParallelization.hpp"

void FluidSolver::NoParallelization::loop_for(size_t from, size_t to, const std::function<void(size_t i)>& fn)
{
	for (size_t i = from; i < to; i++) {
		fn(i);
	}
}

void FluidSolver::NoParallelization::loop_for(size_t from, size_t to, size_t step, const std::function<void(size_t i)>& fn)
{
	for (size_t i = from; i < to; i += step) {
		fn(i);
	}
}
