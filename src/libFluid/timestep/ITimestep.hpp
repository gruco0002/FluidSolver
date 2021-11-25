#pragma once

#include "Compatibility.hpp"
#include "fluidSolver/ParticleCollection.hpp"

#include <chrono>
#include <memory>

namespace FluidSolver
{

    struct Timepoint
    {
        size_t timestep_number = 0;
        std::chrono::time_point<std::chrono::system_clock> system_time = {};
        float simulation_time = 0;

        float desired_time_step = 0;
        float actual_time_step = 0;

        static float get_non_cfl_validating_timestep(float max_velocity, float particle_size);
    };

    class ITimestep {

      protected:
        float current_timestep = 0.0f;

      public:
        struct TimestepParameters
        {
            std::shared_ptr<ParticleCollection> particle_collection = nullptr;
            float particle_size = 1.0f;
        } parameters;

        virtual void calculate_current_timestep() = 0;

        float get_current_timestep() const;

        virtual ~ITimestep() = default;

        virtual Compatibility check() = 0;
    };
} // namespace FluidSolver
