#include "UvSamplingMethod.hpp"

namespace LibFluid::Importer {

    std::vector<glm::vec3> UVSamplingMethod::generate_samples_for_triangle(const MeshData::Triangle& triangle) const {
        std::vector<glm::vec3> samples;

        const auto& origin = triangle.vertices[0];
        auto edge_1 = triangle.vertices[1] - triangle.vertices[0];
        auto edge_2 = triangle.vertices[2] - triangle.vertices[0];

        float u_step = 1.0f / (glm::length(edge_1) / particle_size);
        float v_step = 1.0f / (glm::length(edge_2) / particle_size);

        for (float u = 0.0f; u <= 1.0f; u += u_step) {
            for (float v = 0.0f; v <= 1.0f; v += v_step) {
                if (u + v > 1.0f) {
                    // the point lies outside the triangle
                    continue;
                }

                auto position = origin + u * edge_1 + v * edge_2;
                samples.push_back(position);
            }
        }

        // add corner points additionally
        samples.push_back(triangle.vertices[0]);
        samples.push_back(triangle.vertices[1]);
        samples.push_back(triangle.vertices[2]);

        return samples;
    }

} // namespace LibFluid::Importer