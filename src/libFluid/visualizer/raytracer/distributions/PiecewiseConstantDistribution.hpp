#pragma once

#include <vector>

namespace LibFluid::Raytracer::Distributions {
    class PiecewiseConstantDistribution {
        // Based on the book 'Physically Based Rendering: From Theory To Implementation' [https://www.pbr-book.org/]
        // by Matt Pharr, Wenzel Jakob, and Greg Humphreys
        // on section 13.3.1 The Inversion Method / Example: Piecewise-Constant 1D Functions

      public:
        PiecewiseConstantDistribution();
        explicit PiecewiseConstantDistribution(std::vector<float> values);

        // samples a normalized index [0, 1) according to the distribution of the provided values
        float get_sample(float uniform_dist_random_sample, float& pdf_of_sample) const;
        float get_sample(float uniform_dist_random_sample, float& pdf_of_sample, size_t& discrete_sample) const;

        float get_pdf_for_sample(float sample) const;

        size_t get_discrete_sample(float uniform_dist_random_sample, float& pdf_of_sample) const;

        float get_pdf_for_discrete_sample(size_t sample) const;

        size_t size() const;

        const std::vector<float>& get_values() const;

        // also known as total weights or integral over the distribution function
        float get_normalized_sum_of_all_values() const;

      private:
        std::vector<float> values;
        std::vector<float> cdf;

        // sum of all values divided by the amount of values (values.size())
        float normalized_sum_of_all_values = 0.0f;


        void calculate_values();


        size_t find_largest_index_in_cdf_smaller_or_equal_than(float value) const;
    };
} // namespace LibFluid::Raytracer::Distributions
