#pragma once

#include "CompatibilityReport.hpp"
#include "fluidSolver/ParticleCollection.hpp"

#include <memory>

namespace FluidSolver
{

    class ITimestepGenerator {

      protected:
        float generated_timestep = 0.0f;

      public:
        struct TimestepParameters
        {
            std::shared_ptr<ParticleCollection> particle_collection = nullptr;
            float particle_size = 1.0f;
        } parameters;

        virtual void generate_next_timestep() = 0;

        float get_generated_timestep() const;

        virtual float get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) = 0;

        virtual ~ITimestepGenerator() = default;

        virtual void create_compatibility_report(CompatibilityReport &report) = 0;
    };
} // namespace FluidSolver
