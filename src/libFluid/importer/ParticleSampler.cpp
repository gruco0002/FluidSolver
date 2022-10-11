#include "ParticleSampler.hpp"
#include <cmath>
#include <unordered_map>
#include <unordered_set>

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

        // correcting for possible floating point inaccuracy in the for loops below
        minimum -= glm::vec3(particle_size);
        maximum += glm::vec3(particle_size);

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
        reduce_duplicates();
        reduce_too_close_ones();
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

    void ParticleSampler::reduce_duplicates() {
        struct SetFunctionsForGlmVec3 {
            size_t operator()(const glm::vec3& v) const {
                return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
            }

            bool operator()(const glm::vec3& a, const glm::vec3& b) const {
                return a.x == b.x && a.y == b.y && a.z == b.z;
            }
        };

        std::unordered_set<glm::vec3, SetFunctionsForGlmVec3, SetFunctionsForGlmVec3> sample_set(samples.begin(), samples.end());

        std::vector<glm::vec3> reduced(sample_set.begin(), sample_set.end());
        samples = std::move(reduced);
    }

    void ParticleSampler::reduce_too_close_ones() {
        // removes samples that are considered too close to other samples
        // what is too close: less than half the particle size
        // approach: greedy, the first encountered sample is being taken, too close ones to the taken samples are skipped

        struct Accelerator {
            struct Cell {
                int x;
                int y;
                int z;
            };
            struct MapFunctionsForCell {
                size_t operator()(const Cell& v) const {
                    return std::hash<int>()(v.x) ^ std::hash<int>()(v.y) ^ std::hash<int>()(v.z);
                }

                bool operator()(const Cell& a, const Cell& b) const {
                    return a.x == b.x && a.y == b.y && a.z == b.z;
                }
            };

            float cell_size;
            float min_distance_required;
            std::vector<glm::vec3> all_samples;
            std::unordered_map<Cell, std::vector<glm::vec3>, MapFunctionsForCell, MapFunctionsForCell> grid;

            explicit Accelerator(float particle_size) {
                cell_size = particle_size;
                min_distance_required = particle_size / 2.0f;
            }

            void add_sample(const glm::vec3& sample) {
                if (are_samples_closer_than_min_distance(sample)) {
                    // ignore
                    return;
                }

                // add to final output
                all_samples.push_back(sample);

                // add to grid structure
                auto cell = get_cell_for_sample(sample);
                grid[cell].push_back(sample);
            }

            Cell get_cell_for_sample(const glm::vec3& sample) const {
                float x = sample.x / cell_size;
                float y = sample.y / cell_size;
                float z = sample.z / cell_size;

                int cell_x = static_cast<int>(std::floorf(x));
                int cell_y = static_cast<int>(std::floorf(y));
                int cell_z = static_cast<int>(std::floorf(z));

                return {cell_x, cell_y, cell_z};
            }

            bool are_samples_closer_than_min_distance(const glm::vec3& to_check) {
                Cell center_cell = get_cell_for_sample(to_check);
                std::array<Cell, 27> cells_to_check = {
                        center_cell,
                        {center_cell.x, center_cell.y, center_cell.z + 1},
                        {center_cell.x, center_cell.y, center_cell.z - 1},
                        {center_cell.x, center_cell.y + 1, center_cell.z},
                        {center_cell.x, center_cell.y + 1, center_cell.z + 1},
                        {center_cell.x, center_cell.y + 1, center_cell.z - 1},
                        {center_cell.x, center_cell.y - 1, center_cell.z},
                        {center_cell.x, center_cell.y - 1, center_cell.z + 1},
                        {center_cell.x, center_cell.y - 1, center_cell.z - 1},

                        {center_cell.x + 1, center_cell.y, center_cell.z},
                        {center_cell.x + 1, center_cell.y, center_cell.z + 1},
                        {center_cell.x + 1, center_cell.y, center_cell.z - 1},
                        {center_cell.x + 1, center_cell.y + 1, center_cell.z},
                        {center_cell.x + 1, center_cell.y + 1, center_cell.z + 1},
                        {center_cell.x + 1, center_cell.y + 1, center_cell.z - 1},
                        {center_cell.x + 1, center_cell.y - 1, center_cell.z},
                        {center_cell.x + 1, center_cell.y - 1, center_cell.z + 1},
                        {center_cell.x + 1, center_cell.y - 1, center_cell.z - 1},

                        {center_cell.x - 1, center_cell.y, center_cell.z},
                        {center_cell.x - 1, center_cell.y, center_cell.z + 1},
                        {center_cell.x - 1, center_cell.y, center_cell.z - 1},
                        {center_cell.x - 1, center_cell.y + 1, center_cell.z},
                        {center_cell.x - 1, center_cell.y + 1, center_cell.z + 1},
                        {center_cell.x - 1, center_cell.y + 1, center_cell.z - 1},
                        {center_cell.x - 1, center_cell.y - 1, center_cell.z},
                        {center_cell.x - 1, center_cell.y - 1, center_cell.z + 1},
                        {center_cell.x - 1, center_cell.y - 1, center_cell.z - 1}};


                for (auto& cell : cells_to_check) {
                    const auto& samples_in_cell = grid[cell];
                    for (const auto& sample : samples_in_cell) {
                        float distance = glm::length(sample - to_check);
                        if (distance < min_distance_required) {
                            return true;
                        }
                    }
                }

                return false;
            }


        } accelerator(particle_size);

        for (const auto& sample : samples) {
            accelerator.add_sample(sample);
        }

        samples = std::move(accelerator.all_samples);
    }
} // namespace LibFluid::Importer