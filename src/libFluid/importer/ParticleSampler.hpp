#pragma once

#include "fluidSolver/Particle.hpp"
#include "importer/MeshData.hpp"
#include "importer/methods/ReductionMethod.hpp"
#include "importer/methods/SamplingMethod.hpp"

#include <memory>

namespace LibFluid::Importer
{
    class ParticleSampler
    {
      public:
        explicit ParticleSampler();

        std::vector<glm::vec3> generate_samples(const MeshData &mesh_data, float particle_size) const;

        std::shared_ptr<SamplingMethod> sampling_method;

        std::vector<std::shared_ptr<ReductionMethod>> reduction_methods;

        bool shuffle_before_each_reduction = false;
        bool use_random_seed_for_shuffling = false;
    };

} // namespace LibFluid::Importer
