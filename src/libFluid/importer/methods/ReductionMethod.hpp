#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace LibFluid::Importer {
    class ReductionMethod {
      public:
        float particle_size = 1.0f;

        virtual std::vector<glm::vec3> reduce_samples(const std::vector<glm::vec3>& samples) const = 0;
    };
} // namespace LibFluid::Importer