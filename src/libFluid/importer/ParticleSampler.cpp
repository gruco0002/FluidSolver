#include "ParticleSampler.hpp"
#include <math.h>

namespace LibFluid::Importer {

    ParticleSampler::ParticleSampler(MeshData mesh_data, float particle_size)
        : mesh_data(std::move(mesh_data)), particle_size(particle_size) {
    }

    void ParticleSampler::generate_samples() {
        for (const auto& triangle : mesh_data.triangles) {
            add_samples_for_triangle(triangle);
        }

        reduce_samples();
    }

    const std::vector<glm::vec3>& ParticleSampler::get_samples() const {
        return samples;
    }

    void ParticleSampler::add_samples_for_triangle(const MeshData::Triangle& triangle) {
        if (triangle.get_area() <= std::numeric_limits<float>::epsilon()) {
            // the triangle has no area that we can sample
            return;
        }

        // add corners of triangle to samples
        samples.push_back(triangle.vertices[0]);
        samples.push_back(triangle.vertices[1]);
        samples.push_back(triangle.vertices[2]);

        // sample the area of the triangle
        auto minimum = triangle.get_minimum_coordinates();
        auto maximum = triangle.get_maximum_coordinates();

        minimum = get_grid_aligned_vector(minimum, true);
        maximum = get_grid_aligned_vector(maximum, false);

        for (float x = minimum.x; x <= maximum.x; x += particle_size) {
            for (float y = minimum.y; y <= maximum.y; y += particle_size) {
                for (float z = minimum.z; z <= maximum.z; z += particle_size) {
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
    }

    void ParticleSampler::reduce_samples() {
    }

    glm::vec3 ParticleSampler::get_grid_aligned_vector(const glm::vec3& value, bool subtract) const {
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