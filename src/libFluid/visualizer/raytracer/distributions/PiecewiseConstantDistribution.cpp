#include "PiecewiseConstantDistribution.hpp"

#include "LibFluidAssert.hpp"
#include "LibFluidMath.hpp"

#include <cmath>
#include <limits>

namespace LibFluid::Raytracer::Distributions {

    PiecewiseConstantDistribution::PiecewiseConstantDistribution(std::vector<float> values) {
        this->values = std::move(values);

        FLUID_ASSERT(!this->values.empty());

        calculate_values();
    }


    float PiecewiseConstantDistribution::get_sample(float uniform_dist_random_sample, float& pdf_of_sample) const {
        size_t unused;
        return get_sample(uniform_dist_random_sample, pdf_of_sample, unused);
    }

    float PiecewiseConstantDistribution::get_sample(float uniform_dist_random_sample, float& pdf_of_sample, size_t& discrete_sample) const {
        FLUID_ASSERT(uniform_dist_random_sample >= 0.0f);
        FLUID_ASSERT(uniform_dist_random_sample < 1.0f);

        size_t index = find_largest_index_in_cdf_smaller_or_equal_than(uniform_dist_random_sample);
        FLUID_ASSERT(index < cdf.size() - 1);
        FLUID_ASSERT(index < values.size());

        // set discrete sample
        discrete_sample = index;

        // calculate pdf of sample
        FLUID_ASSERT(Math::is_not_zero(normalized_sum_of_all_values));
        pdf_of_sample = values[index] / normalized_sum_of_all_values;

        // calculate remaining offset
        float remaining_part_of_random_sample = uniform_dist_random_sample - cdf[index];
        float cdf_difference = cdf[index + 1] - cdf[index];
        float remaining_offset;
        if (Math::is_not_zero(cdf_difference)) {
            remaining_offset = remaining_part_of_random_sample / cdf_difference;
        } else {
            remaining_offset = remaining_part_of_random_sample;
        }

        // calculate sample
        FLUID_ASSERT(size() != 0);
        float sample = (static_cast<float>(index) + remaining_offset) / static_cast<float>(size());

        return sample;
    }

    float PiecewiseConstantDistribution::get_pdf_for_sample(float sample) const {
        FLUID_ASSERT(sample >= 0.0f);
        FLUID_ASSERT(sample < 1.0f);

        size_t discrete_sample = std::floor(sample * static_cast<float>(values.size()));
        FLUID_ASSERT(discrete_sample < values.size());

        return get_pdf_for_discrete_sample(discrete_sample);
    }

    void PiecewiseConstantDistribution::calculate_values() {
        FLUID_ASSERT(size() != 0);

        // resize the cdf to the amount of values
        cdf.resize(values.size() + 1);

        float reciprocal_of_size = 1.0f / static_cast<float>(size());

        normalized_sum_of_all_values = 0.0f;
        for (size_t i = 0; i < values.size(); i++) {
            // the cdf will be normalized later on
            cdf[i] = normalized_sum_of_all_values;

            normalized_sum_of_all_values += values[i] * reciprocal_of_size;
        }
        cdf[values.size()] = normalized_sum_of_all_values;


        if (Math::is_not_zero(normalized_sum_of_all_values)) {
            // normalize cdf
            FLUID_ASSERT(Math::is_not_zero(normalized_sum_of_all_values));
            float reciprocal_of_sum_of_all_values = 1.0f / normalized_sum_of_all_values;
            for (float& cdf_value : cdf) {
                cdf_value *= reciprocal_of_sum_of_all_values;
            }
        } else {
            // the total value is zero, handle this case separately
            for (size_t i = 0; i < cdf.size(); i++) {
                cdf[i] = static_cast<float>(i) * reciprocal_of_size;
            }
        }
    }

    size_t PiecewiseConstantDistribution::find_largest_index_in_cdf_smaller_or_equal_than(float value) const {
        size_t left = 0;
        size_t right = cdf.size() - 1;

        while (left < right) {
            size_t middle = left + (right - left + 1) / 2;

            FLUID_ASSERT(middle > left);
            FLUID_ASSERT(middle <= right);
            FLUID_ASSERT(middle < cdf.size());

            // if the pivot is equal to the value
            if (cdf[middle] == value) {
                return middle;
            }

            if (cdf[middle] < value) {
                left = middle;
            } else {
                right = middle - 1;
            }
        }

        return left;
    }

    size_t PiecewiseConstantDistribution::get_discrete_sample(float uniform_dist_random_sample, float& pdf_of_sample) const {
        float continuous_sample = get_sample(uniform_dist_random_sample, pdf_of_sample);
        size_t sample = std::floor(continuous_sample * static_cast<float>(size()));
        FLUID_ASSERT(sample < values.size());
        return sample;
    }

    float PiecewiseConstantDistribution::get_pdf_for_discrete_sample(size_t sample) const {
        FLUID_ASSERT(sample < values.size());
        FLUID_ASSERT(size() != 0);
        if (Math::is_zero(normalized_sum_of_all_values)) {
            return 1.0f / static_cast<float>(size());
        }
        return values[sample] / (normalized_sum_of_all_values * static_cast<float>(size()));
    }
    size_t PiecewiseConstantDistribution::size() const {
        return values.size();
    }

    const std::vector<float>& PiecewiseConstantDistribution::get_values() const {
        return values;
    }

    float PiecewiseConstantDistribution::get_normalized_sum_of_all_values() const {
        return normalized_sum_of_all_values;
    }

    PiecewiseConstantDistribution::PiecewiseConstantDistribution() = default;
} // namespace LibFluid::Raytracer::Distributions