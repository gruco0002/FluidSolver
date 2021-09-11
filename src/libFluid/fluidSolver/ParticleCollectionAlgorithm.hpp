#pragma once

#include "ParticleCollection.hpp"

#include <functional>
#include <future>
#include <memory>

namespace FluidSolver::ParticleCollectionAlgorithm
{
    class Sort {
      public:
        using key_function_t = std::function<uint64_t(const std::shared_ptr<ParticleCollection>&, const pIndex_t)>;


        struct SortInfo
        {
            uint64_t key;
        };

        void adapt_collection(std::shared_ptr<ParticleCollection> collection);

        void merge_sort(std::shared_ptr<ParticleCollection>, const key_function_t& key);

        void insertion_sort(std::shared_ptr<ParticleCollection>, const key_function_t& key);

        template <bool should_precalculate_keys = true, bool calculate_parallellized = true>
        void quick_sort(std::shared_ptr<ParticleCollection> collection, const key_function_t& key)
        {
            if constexpr (should_precalculate_keys)
            {
                FLUID_ASSERT(collection->is_type_present<SortInfo>());
                precalculate_keys(collection, key);
            }


            auto partition = [&](int64_t low, int64_t high) -> int64_t {
                uint64_t pivot;
                uint64_t current_element;
                int64_t pivot_index = low + (high - low) / 2;

                if constexpr (should_precalculate_keys)
                {
                    pivot = collection->get<SortInfo>(pivot_index).key;
                }
                else
                {
                    pivot = key(pivot_index);
                }

                int64_t i = low - 1;
                int64_t j = high + 1;

                while (true)
                {
                    do
                    {
                        i++;
                        if constexpr (should_precalculate_keys)
                        {
                            current_element = collection->get<SortInfo>(i).key;
                        }
                        else
                        {
                            current_element = key(i);
                        }
                    } while (current_element < pivot);

                    do
                    {
                        j--;
                        if constexpr (should_precalculate_keys)
                        {
                            current_element = collection->get<SortInfo>(j).key;
                        }
                        else
                        {
                            current_element = key(j);
                        }
                    } while (current_element > pivot);


                    if (i >= j)
                    {
                        return j;
                    }

                    collection->swap(i, j);
                }
            };


            std::function<void(int64_t, int64_t)> quick_sort = [&](int64_t low, int64_t high) {
                FLUID_ASSERT(low >= 0);
                FLUID_ASSERT(high >= 0);

                if (low >= high)
                    return;

                auto partitioning_index = partition(low, high);
                if constexpr (!calculate_parallellized)
                {
                    quick_sort(low, partitioning_index);
                    quick_sort(partitioning_index + 1, high);
                }
                else
                {
                    constexpr size_t MIN_ASYNC_SIZE = 10000;

                    if (partitioning_index - low < MIN_ASYNC_SIZE || high - partitioning_index < MIN_ASYNC_SIZE)
                    {
                        // one of the jobs is too small to allow for the overhead that would come with multi threading
                        // hence do the jobs normally
                        quick_sort(low, partitioning_index);
                        quick_sort(partitioning_index + 1, high);
                    }
                    else
                    {
                        // both jobs are large enough to be split up on two threads

                        auto policy = std::launch::async;
                        auto lower_part_sorting = std::async(policy, [&]() { quick_sort(low, partitioning_index); });
                        auto upper_part_sorting =
                            std::async(policy, [&]() { quick_sort(partitioning_index + 1, high); });

                        lower_part_sorting.wait();
                        upper_part_sorting.wait();
                    }
                }
            };

            quick_sort(0, collection->size() - 1);
        }

      private:
        void precalculate_keys(std::shared_ptr<ParticleCollection>, const key_function_t& key);
    };


} // namespace FluidSolver::ParticleCollectionAlgorithm