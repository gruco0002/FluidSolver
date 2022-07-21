#pragma once

#include <chrono>

namespace LibFluid {

    struct Timepoint {
        size_t timestep_number = 0;
        std::chrono::time_point<std::chrono::system_clock> system_time = {};
        float simulation_time = 0;

        float desired_time_step = 0;
        float actual_time_step = 0;
    };


} // namespace FluidSolver