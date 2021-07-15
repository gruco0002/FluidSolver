#include "ParticleCollectionAlgorithm.hpp"

#include "FluidAssert.hpp"

namespace FluidSolver
{
    namespace ParticleCollectionAlgorithm
    {
        void Sort::adapt_collection(std::shared_ptr<ParticleCollection> collection)
        {
            if (!collection->is_type_present<SortInfo>())
            {
                collection->add_type<SortInfo>();
            }
        }

        void Sort::merge_sort(std::shared_ptr<ParticleCollection> collection, const key_function_t& key)
        {
            FLUID_ASSERT(collection->is_type_present<SortInfo>());

            precalculate_keys(collection, key);

            // define merge, left index is inclusive, right index is exclusive
            auto merge = [=, &collection](size_t left, size_t half, size_t right) {
                size_t leftIndex = left;
                size_t rightIndex = half;

                if (collection->get<SortInfo>(rightIndex - 1).key <= collection->get<SortInfo>(rightIndex).key)
                    return; // everything was already sorted

                while (leftIndex < half && rightIndex < right)
                {
                    if (collection->get<SortInfo>(leftIndex).key <= collection->get<SortInfo>(rightIndex).key)
                    {
                        leftIndex++; // the element is already in the right place
                    }
                    else
                    {
                        // move the item by continous swapping to the target, then adapt half and other indices
                        // accordingly
                        size_t movingIndex = rightIndex;
                        while (movingIndex > leftIndex)
                        {
                            collection->swap(movingIndex, movingIndex - 1);
                            movingIndex--;
                        }

                        rightIndex++;
                        half++;
                        leftIndex++;
                    }
                }
            };


            // define merge sort, left index is inclusive, right index is exclusive
            std::function<void(size_t, size_t)> mergesort = [=, &mergesort](size_t left, size_t right) {
                if (right - left <= 1)
                    return;

                size_t half = left + (right - left) / 2;
                mergesort(left, half);
                mergesort(half, right);

                merge(left, half, right);
            };

            // start mergesort
            mergesort(0, collection->size());
        }

        void Sort::insertion_sort(std::shared_ptr<ParticleCollection> collection, const key_function_t& key)
        {
            FLUID_ASSERT(collection->is_type_present<SortInfo>());

            precalculate_keys(collection, key);

            // start insertion sort
            for (size_t i = 1; i < collection->size(); i++)
            {
                auto& info = collection->get<SortInfo>(i);
                size_t j = i - 1;
                while (j >= 0 && collection->get<SortInfo>(j).key > info.key)
                {
                    collection->swap(j, j + 1);
                    j--;
                }
            }
        }

        void Sort::quick_sort(std::shared_ptr<ParticleCollection> collection, const key_function_t& key)
        {
            FLUID_ASSERT(collection->is_type_present<SortInfo>());

            precalculate_keys(collection, key);


            auto partition = [&](int64_t low, int64_t high) -> int64_t {
                auto pivot = collection->get<SortInfo>(high).key;

                int64_t i = low - 1;

                for (int64_t j = low; j < high; j++)
                {
                    auto value_at_j = collection->get<SortInfo>(j).key;
                    if (value_at_j < pivot)
                    {
                        i++;
                        collection->swap(i, j);
                    }
                }

                collection->swap(i + 1, high);
                return i + 1;
            };


            std::function<void(int64_t, int64_t)> quick_sort = [&](int64_t low, int64_t high) {
                if (low < high)
                {
                    auto partitioning_index = partition(low, high);

                    quick_sort(low, partitioning_index - 1);
                    quick_sort(partitioning_index + 1, high);
                }
            };

            quick_sort(0, collection->size() - 1);
        }

        void Sort::precalculate_keys(std::shared_ptr<ParticleCollection> collection, const Sort::key_function_t& key)
        {
            FLUID_ASSERT(collection->is_type_present<SortInfo>());
            for (size_t i = 0; i < collection->size(); i++)
            {
                collection->get<SortInfo>(i).key = key(collection, i);
            }
        }

    } // namespace ParticleCollectionAlgorithm
} // namespace FluidSolver
