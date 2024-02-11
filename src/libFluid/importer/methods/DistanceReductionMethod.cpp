#include "DistanceReductionMethod.hpp"

#include <array>
#include <unordered_map>

namespace LibFluid::Importer
{

    std::vector<glm::vec3> DistanceReductionMethod::reduce_samples(const std::vector<glm::vec3> &samples) const
    {
        // removes samples that are considered too close to other samples
        // what is too close: less than half the particle size
        // approach: greedy, the first encountered sample is being taken, too close ones to the taken samples are
        // skipped

        struct Accelerator
        {
            struct Cell
            {
                int x;
                int y;
                int z;
            };
            struct MapFunctionsForCell
            {
                size_t operator()(const Cell &v) const
                {
                    return std::hash<int>()(v.x) ^ std::hash<int>()(v.y) ^ std::hash<int>()(v.z);
                }

                bool operator()(const Cell &a, const Cell &b) const
                {
                    return a.x == b.x && a.y == b.y && a.z == b.z;
                }
            };

            float cell_size;
            float min_distance_required;
            std::vector<glm::vec3> all_samples;
            std::unordered_map<Cell, std::vector<glm::vec3>, MapFunctionsForCell, MapFunctionsForCell> grid;

            explicit Accelerator(float particle_size)
            {
                cell_size = particle_size;
                min_distance_required = particle_size / 2.0f;
            }

            void add_sample(const glm::vec3 &sample)
            {
                if (are_samples_closer_than_min_distance(sample))
                {
                    // ignore
                    return;
                }

                // add to final output
                all_samples.push_back(sample);

                // add to grid structure
                auto cell = get_cell_for_sample(sample);
                grid[cell].push_back(sample);
            }

            Cell get_cell_for_sample(const glm::vec3 &sample) const
            {
                float x = sample.x / cell_size;
                float y = sample.y / cell_size;
                float z = sample.z / cell_size;

                int cell_x = static_cast<int>(std::floorf(x));
                int cell_y = static_cast<int>(std::floorf(y));
                int cell_z = static_cast<int>(std::floorf(z));

                return {cell_x, cell_y, cell_z};
            }

            bool are_samples_closer_than_min_distance(const glm::vec3 &to_check)
            {
                Cell center_cell = get_cell_for_sample(to_check);
                std::array<Cell, 27> cells_to_check = {center_cell,
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

                for (auto &cell : cells_to_check)
                {
                    const auto &samples_in_cell = grid[cell];
                    for (const auto &sample : samples_in_cell)
                    {
                        float distance = glm::length(sample - to_check);
                        if (distance < min_distance_required)
                        {
                            return true;
                        }
                    }
                }

                return false;
            }

        } accelerator(particle_size);

        for (const auto &sample : samples)
        {
            accelerator.add_sample(sample);
        }

        return std::move(accelerator.all_samples);
    }
} // namespace LibFluid::Importer
