#pragma once

#include "ParticleCollection.hpp"
#include "parallelization/StdParallelForEach.hpp"

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
                    pivot = key(collection, pivot_index);
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
                            current_element = key(collection, i);
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
                            current_element = key(collection, j);
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

        template <bool should_precalculate_keys = true, bool calculate_parallellized = true>
        void quick_sort_stable(std::shared_ptr<ParticleCollection> collection, const key_function_t& key)
        {
            using parallel = StdParallelForEach;

            if constexpr (should_precalculate_keys)
            {
                FLUID_ASSERT(collection->is_type_present<SortInfo>());
                precalculate_keys(collection, key);
            }


            std::vector<uint32_t> original_index;
            original_index.resize(collection->size());
            if constexpr (calculate_parallellized)
            {
                parallel::loop_for(0, original_index.size(), [&](size_t i) { original_index[i] = i; });
            }
            else
            {
                for (size_t i = 0; i < original_index.size(); i++)
                {
                    original_index[i] = i;
                }
            }

            auto three_way_partition = [&](int64_t low, int64_t high) -> std::tuple<int64_t, int64_t> {
                int64_t pivot_index = low + (high - low) / 2;
                // getting the pivot value
                uint64_t pivot_value;
                if constexpr (should_precalculate_keys)
                {
                    pivot_value = collection->get<SortInfo>(pivot_index).key;
                }
                else
                {
                    pivot_value = key(collection, pivot_index);
                }

                int64_t i, j;
                i = low;
                j = low;
                int64_t k = high;

                while (j <= k)
                {
                    uint64_t value_at_j;
                    if constexpr (should_precalculate_keys)
                    {
                        value_at_j = collection->get<SortInfo>(j).key;
                    }
                    else
                    {
                        value_at_j = key(collection, j);
                    }

                    if (value_at_j < pivot_value)
                    {
                        collection->swap(i, j);
                        std::swap(original_index[i], original_index[j]);
                        i++;
                        j++;
                    }
                    else if (value_at_j > pivot_value)
                    {
                        collection->swap(j, k);
                        std::swap(original_index[j], original_index[k]);
                        k--;
                    }
                    else
                    {
                        j++;
                    }
                }
                return {i, j};
            };

            std::function<void(int64_t, int64_t)> sort_by_original_index = [&](int64_t low, int64_t high) {
                if (high - low <= 0)
                {
                    // nothing to sort
                    return;
                }

                // insertion sort
                int i = low + 1;
                while (i <= high)
                {
                    int j = i;
                    while (j > low && original_index[j - 1] > original_index[j])
                    {
                        collection->swap(j, j - 1);
                        std::swap(original_index[j], original_index[j - 1]);
                        j--;
                    }
                    i++;
                }
            };


            std::function<void(int64_t, int64_t)> quick_sort = [&](int64_t low, int64_t high) {
                if (low >= high)
                    return;

                auto [p1_tmp, p2_tmp] = three_way_partition(low, high);
                auto p1 = p1_tmp;
                auto p2 = p2_tmp;
                if constexpr (!calculate_parallellized)
                {

                    quick_sort(low, p1 - 1);
                    sort_by_original_index(p1, p2 - 1);
                    quick_sort(p2, high);
                }
                else
                {
                    constexpr size_t MIN_ASYNC_SIZE = 10000;

                    if (p1 - low < MIN_ASYNC_SIZE || high - p2 < MIN_ASYNC_SIZE)
                    {
                        // one of the jobs is too small to allow for the overhead that would come with multi threading
                        // hence do the jobs normally
                        quick_sort(low, p1 - 1);
                        sort_by_original_index(p1, p2 - 1);
                        quick_sort(p2, high);
                    }
                    else
                    {
                        // both jobs are large enough to be split up on two threads

                        auto policy = std::launch::async;
                        auto lower_part_sorting = std::async(policy, [&]() { quick_sort(low, p1 - 1); });
                        auto upper_part_sorting = std::async(policy, [&]() { quick_sort(p2, high); });

                        sort_by_original_index(p1, p2 - 1);

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
