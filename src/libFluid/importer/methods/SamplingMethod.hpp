#pragma once

#include "importer/MeshData.hpp"
#include <vector>

namespace LibFluid::Importer
{

    class SamplingMethod
    {
      public:
        float particle_size = 1.0f;

        virtual std::vector<glm::vec3> generate_samples_for_triangle(const MeshData::Triangle &triangle) const = 0;
    };

} // namespace LibFluid::Importer
