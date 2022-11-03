#pragma once

#include "fluidSolver/ParticleCollection.hpp"
#include "helpers/CompatibilityReport.hpp"

#include <memory>

#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"

namespace LibFluid {

    class TimestepGenerator : public Initializable, public Reportable {

      protected:
        float generated_timestep = 0.0f;

       static constexpr float MIN_ALLOWED_TIMESTEP = 0.000001f;

      public:
        struct TimestepParameters
        {
            std::shared_ptr<ParticleCollection> particle_collection = nullptr;
            float particle_size = 1.0f;
        } parameters;

        virtual void generate_next_timestep() = 0;

        float get_generated_timestep() const;

        virtual float get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) = 0;



    };
} // namespace FluidSolver
