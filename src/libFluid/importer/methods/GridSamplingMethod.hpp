#pragma once

#include "importer/methods/SamplingMethod.hpp"

namespace LibFluid::Importer {
    class GridSamplingMethod final : public SamplingMethod {
      public:
        std::vector<glm::vec3> generate_samples_for_triangle(const MeshData::Triangle& triangle) const override;

      private:
        glm::vec3 get_grid_aligned_vector(const glm::vec3& value, bool subtract) const;
    };
} // namespace LibFluid::Importer
