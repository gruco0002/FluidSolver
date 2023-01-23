#pragma once

#include "importer/methods/SamplingMethod.hpp"

namespace LibFluid::Importer {
    class GridSamplingMethod final : public SamplingMethod {
      public:
        std::vector<glm::vec3> generate_samples_for_triangle(const MeshData::Triangle& triangle) const override;

        float cell_size_factor = 0.5f;

      private:
        glm::vec3 get_grid_aligned_vector(const glm::vec3& value, bool subtract) const;
    };
} // namespace LibFluid::Importer
