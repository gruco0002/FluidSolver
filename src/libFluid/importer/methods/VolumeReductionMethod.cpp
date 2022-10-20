#include "VolumeReductionMethod.hpp"

#include "LibFluidMath.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"

#include <array>
#include <unordered_map>

namespace LibFluid::Importer {

    class Grid {
      private:
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

      private:
        struct SampleState {
            glm::vec3 position;
            float current_volume = 0.0f;
        };

        float cell_size;
        float ideal_volume;
        float min_volume;
        float max_volume;

        std::unordered_map<Cell, std::vector<SampleState>, MapFunctionsForCell, MapFunctionsForCell> grid;
        std::vector<glm::vec3> choosen_samples;

        CubicSplineKernel3D kernel;

      public:
        explicit Grid(float particle_size, float min_volume_factor, float max_volume_factor) {
            cell_size = particle_size * Math::kernel_support_factor;

            kernel.kernel_support = particle_size * Math::kernel_support_factor;
            kernel.initialize();

            ideal_volume = calculate_ideal_volume(particle_size);
            min_volume = ideal_volume * min_volume_factor;
            max_volume = ideal_volume * max_volume_factor;
        }

        void add_sample(const glm::vec3& new_sample_candidate) {
            auto cells_to_check = get_cells_around_position(new_sample_candidate);

            // flags for the sample candidate
            bool avoids_undersampling = false;
            bool leads_to_oversampling_of_one_or_more = false;
            bool leads_to_oversampling_of_all = true;

            // calculate volume and flags for the case that the candidate gets added
            float volume_reciprocal_of_sample_candidate = kernel.GetKernelValue(glm::vec3(0.0f));
            for (auto& cell : cells_to_check) {
                const auto& samples_in_cell = grid[cell];
                for (const auto& sample_state : samples_in_cell) {
                    float contribution = kernel.GetKernelValue(sample_state.position, new_sample_candidate);
                    if (contribution <= 0.0f) {
                        continue;
                    }

                    volume_reciprocal_of_sample_candidate += contribution;

                    if (sample_state.current_volume > max_volume) {
                        // this sample has a volume that is too large
                        // which means in its neighborhood are too few samples
                        // hence adding the new sample candidate would help to decrease the volume
                        avoids_undersampling = true;
                    }

                    float new_volume_of_sample = 1.0f / (1.0f / sample_state.current_volume + contribution);
                    if (new_volume_of_sample < min_volume) {
                        // by adding the new sample candidate this sample would
                        // have a volume below our threshold which means that
                        // there would be too many samples in its neighborhood
                        leads_to_oversampling_of_one_or_more = true;
                    } else {
                        // by adding the new sample candidate this sample would
                        // not have a volume below our threshold
                        // this means that the candidate does not cause oversampling
                        // of all its neighbors
                        leads_to_oversampling_of_all = false;
                    }
                }
            }

            float volume_of_sample_candidate = 1.0f / volume_reciprocal_of_sample_candidate;

            bool should_add_candidate = false;
            if (avoids_undersampling) {
                // adding the candidate would improve an undersampled particle
                should_add_candidate = true;
            } else if (volume_of_sample_candidate > max_volume) {
                // the candidate itself would be undersampled and is therefore a good candidate
                should_add_candidate = true;
            } else if (volume_of_sample_candidate >= min_volume) {
                // the candidate itself would not be oversampled
                should_add_candidate = true;
            }

            if (!should_add_candidate) {
                return;
            }

            // add to grid structure
            auto cell = get_cell_for_sample(new_sample_candidate);
            grid[cell].push_back({new_sample_candidate, volume_of_sample_candidate});

            if (volume_of_sample_candidate <= max_volume) {
                // the sample candidate is already below the max volume threshold and can therefore directly
                // be added to the result list
                choosen_samples.push_back(new_sample_candidate);
            }

            // add contributions to neighbors
            for (auto& cell : cells_to_check) {
                auto& samples_in_cell = grid[cell];
                for (auto& sample_state : samples_in_cell) {
                    float contribution = kernel.GetKernelValue(new_sample_candidate, sample_state.position);
                    if (contribution <= 0.0f) {
                        continue;
                    }

                    // calculate the new volume of the sample
                    float new_volume_of_sample = 1.0f / (1.0f / sample_state.current_volume + contribution);

                    if (sample_state.current_volume > max_volume && new_volume_of_sample <= max_volume) {
                        // the sample volume changed such that the sample volume came below the threshold
                        // this means that we can add the sample to our result list
                        choosen_samples.push_back(sample_state.position);
                    }

                    // update the samples volume
                    sample_state.current_volume = new_volume_of_sample;
                }
            }
        }

        std::vector<glm::vec3>& get_choosen_samples() {
            return choosen_samples;
        }

      private:
        float calculate_ideal_volume(float particle_size) const {
            float volume_reciprocal = 0.0f;

            // calculating the volume of a perfectly sampled plane
            for (int y = -2; y <= 2; y++) {
                for (int z = -2; z <= 2; z++) {
                    const auto position = glm::vec3(0.0f, (float)y, (float)z) * particle_size;
                    volume_reciprocal += kernel.GetKernelValue(position);
                }
            }

            return 1.0f / volume_reciprocal;
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

        std::array<Cell, 27> get_cells_around_position(const glm::vec3& position) const {
            Cell center_cell = get_cell_for_sample(position);
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

            return cells_to_check;
        }
    };


    std::vector<glm::vec3> VolumeReductionMethod::reduce_samples(const std::vector<glm::vec3>& samples) const {
        // Idea: We add particles as long as their volume does not fall below a certain value
        // But we always add particles if one of their neighbors has a volume above a certain value (too few samples in its area)

        Grid grid(particle_size, min_volume_factor, max_volume_factor);

        for (const auto& sample : samples) {
            grid.add_sample(sample);
        }

        return std::move(grid.get_choosen_samples());
    }
} // namespace LibFluid::Importer