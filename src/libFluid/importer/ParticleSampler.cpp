#include "ParticleSampler.hpp"

#include "importer/methods/GridSamplingMethod.hpp"

#include <cmath>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include "LibFluidAssert.hpp"


namespace LibFluid::Importer {


    ParticleSampler::ParticleSampler() {
        sampling_method = std::make_shared<GridSamplingMethod>();
    }

    std::vector<glm::vec3> ParticleSampler::generate_samples(const MeshData& mesh_data, float particle_size) const {
        std::vector<glm::vec3> samples;

        // sample all triangles
        FLUID_ASSERT(sampling_method != nullptr, "Sampling method must not be null!");
        sampling_method->particle_size = particle_size;
        for (const auto& triangle : mesh_data.triangles) {
            auto triangle_samples = sampling_method->generate_samples_for_triangle(triangle);
            samples.insert(samples.end(), triangle_samples.begin(), triangle_samples.end());
        }

        // initialize random number generators
        std::mt19937 g(1234);
        if (use_random_seed_for_shuffling) {
            std::random_device d;
            g = std::mt19937(d());
        }

        // reduce the samples
        for (const auto& reduction_method : reduction_methods) {
            reduction_method->particle_size = particle_size;

            if (shuffle_before_each_reduction) {
                // shuffle data around to avoid patterns
                std::shuffle(samples.begin(), samples.end(), g);
            }

            samples = reduction_method->reduce_samples(samples);
        }

        return samples;
    }

} // namespace LibFluid::Importer