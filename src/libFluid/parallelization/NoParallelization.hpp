#pragma once

#include <functional>

namespace LibFluid {

    class NoParallelization {

      public:
        /**
		 * @brief Loops from (inclusive) to to (exclusive) and executes for each index i the function fn
		 *
         * @param from
		 * @param to
		 * @param fn
		 */
        static void loop_for(size_t from, size_t to, const std::function<void(size_t i)>& fn);

        static void loop_for(size_t from, size_t to, size_t step, const std::function<void(size_t i)>& fn);

        static float loop_for_max(size_t from, size_t to, const std::function<float(size_t i)>& fn);
    };

} // namespace FluidSolver
