#pragma once

#include "importer/methods/SamplingMethod.hpp"

namespace LibFluid::Importer
{
    class UVSamplingMethod final : public SamplingMethod
    {
      public:
        std::vector<glm::vec3> generate_samples_for_triangle(const MeshData::Triangle &triangle) const override;
    };
} // namespace LibFluid::Importer
