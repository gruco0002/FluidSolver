#include "Sampler.hpp"

#include "LibFluidMath.hpp"
#include <cmath>
#include <random>

namespace LibFluid::Raytracer {

    struct RandomGenerator {
        std::default_random_engine random_engine;
        std::uniform_real_distribution<float> uniform_distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

        float generate() {
            return uniform_distribution(random_engine);
        }
    };

    float Sampler::get_uniform_sampled_value() {
        static thread_local RandomGenerator generator;
        return generator.generate();
    }

    glm::vec2 Sampler::get_uniform_sampled_pair() {
        static thread_local RandomGenerator generator_1;
        static thread_local RandomGenerator generator_2;
        float zeta_one = generator_1.generate();
        float zeta_two = generator_2.generate();

        return {zeta_one, zeta_two};
    }


} // namespace LibFluid::Raytracer