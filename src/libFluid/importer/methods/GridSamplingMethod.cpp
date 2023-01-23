#include "GridSamplingMethod.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid::Importer {
    std::vector<glm::vec3> GridSamplingMethod::generate_samples_for_triangle(const MeshData::Triangle& triangle) const {
        if (triangle.get_area() <= std::numeric_limits<float>::epsilon()) {
            // the triangle has no area that we can sample
            return {};
        }

        std::vector<glm::vec3> samples;

        // add corners of triangle to samples
        samples.push_back(triangle.vertices[0]);
        samples.push_back(triangle.vertices[1]);
        samples.push_back(triangle.vertices[2]);

        // sample the area of the triangle
        auto minimum = triangle.get_minimum_coordinates();
        auto maximum = triangle.get_maximum_coordinates();

        minimum = get_grid_aligned_vector(minimum, true);
        maximum = get_grid_aligned_vector(maximum, false);

        // correcting for possible floating point inaccuracy in the for loops below
        minimum -= glm::vec3(particle_size);
        maximum += glm::vec3(particle_size);

        for (float x = minimum.x; x <= maximum.x; x += particle_size * cell_size_factor) {
            for (float y = minimum.y; y <= maximum.y; y += particle_size * cell_size_factor) {
                for (float z = minimum.z; z <= maximum.z; z += particle_size * cell_size_factor) {
                    glm::vec3 sample_position(x, y, z);

                    auto point_on_triangle = triangle.get_closest_point_on_triangle(sample_position);

                    // FIXME: check if the division by 2 is correct
                    if (Math::abs(sample_position.x - point_on_triangle.x) > particle_size / 2.0f) {
                        continue; // point was moved outside our virtual grid cell
                    }
                    if (Math::abs(sample_position.y - point_on_triangle.y) > particle_size / 2.0f) {
                        continue; // point was moved outside our virtual grid cell
                    }
                    if (Math::abs(sample_position.z - point_on_triangle.z) > particle_size / 2.0f) {
                        continue; // point was moved outside our virtual grid cell
                    }

                    samples.push_back(point_on_triangle);
                }
            }
        }

        return samples;
    }

    glm::vec3 GridSamplingMethod::get_grid_aligned_vector(const glm::vec3& value, bool subtract) const {
        float x_mod = std::fmod(value.x, particle_size);
        float y_mod = std::fmod(value.y, particle_size);
        float z_mod = std::fmod(value.z, particle_size);

        // correcting for negative modulo result
        x_mod = x_mod < 0.0f ? particle_size + x_mod : x_mod;
        y_mod = y_mod < 0.0f ? particle_size + y_mod : y_mod;
        z_mod = z_mod < 0.0f ? particle_size + z_mod : z_mod;

        if (subtract) {
            return {
                    value.x - x_mod,
                    value.y - y_mod,
                    value.z - z_mod,
            };
        } else {
            return {
                    value.x + (particle_size - x_mod),
                    value.y + (particle_size - y_mod),
                    value.z + (particle_size - z_mod),
            };
        }
    }
} // namespace LibFluid::Importer
