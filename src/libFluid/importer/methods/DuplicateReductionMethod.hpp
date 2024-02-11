#pragma once

#include "importer/methods/ReductionMethod.hpp"

namespace LibFluid::Importer
{
    class DuplicateReductionMethod final : public ReductionMethod
    {
      public:
        std::vector<glm::vec3> reduce_samples(const std::vector<glm::vec3> &samples) const override;
    };
} // namespace LibFluid::Importer
