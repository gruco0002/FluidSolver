#pragma once

#include <functional>

namespace FluidSolver {
	class StdParallelForEach
	{

	public:

		static void loop_for(size_t from, size_t to, const std::function<void(size_t i)>& fn);

		static void loop_for(size_t from, size_t to, size_t step, const std::function<void(size_t i)>& fn);


	};


}

