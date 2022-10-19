#pragma once

#include "importer/methods/ReductionMethod.hpp"

namespace LibFluid::Importer {

    class VolumeReductionMethod final : public ReductionMethod {
      public:
        float min_volume_factor = 0.95f;
        float max_volume_factor = 1.5f;

        std::vector<glm::vec3> reduce_samples(const std::vector<glm::vec3>& samples) const override;
    };

} // namespace LibFluid::Importer
