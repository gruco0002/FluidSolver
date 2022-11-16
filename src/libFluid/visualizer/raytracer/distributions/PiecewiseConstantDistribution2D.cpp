#include "PiecewiseConstantDistribution2D.hpp"

#include "LibFluidAssert.hpp"

#include <algorithm>
#include <limits>

namespace LibFluid::Raytracer::Distributions {

    PiecewiseConstantDistribution2D::PiecewiseConstantDistribution2D(std::vector<float> values, size_t width, size_t height) {
        this->values = std::move(values);
        this->width = width;
        this->height = height;

        FLUID_ASSERT(width > 0);
        FLUID_ASSERT(height > 0);
        FLUID_ASSERT(this->values.size() == width * height);

        calculate_values();
    }

    glm::vec2 PiecewiseConstantDistribution2D::get_sample(const glm::vec2& uniform_dist_random_sample_pair, float& pdf_of_sample) const {
        float pdf_of_sample_y;
        size_t discrete_sample;
        float sample_y = marginal_distribution.get_sample(uniform_dist_random_sample_pair.y, pdf_of_sample_y, discrete_sample);

        FLUID_ASSERT(discrete_sample < conditional_distributions.size());

        float pdf_of_sample_x;
        float sample_x = conditional_distributions[discrete_sample].get_sample(uniform_dist_random_sample_pair.x, pdf_of_sample_x);

        pdf_of_sample = pdf_of_sample_x * pdf_of_sample_y;

        return glm::vec2(sample_x, sample_y);
    }

    float PiecewiseConstantDistribution2D::get_pdf_for_sample(const glm::vec2& sample) const {
        FLUID_ASSERT(!conditional_distributions.empty());
        FLUID_ASSERT(conditional_distributions[0].size() == width);
        FLUID_ASSERT(marginal_distribution.size() == height);

        if (marginal_distribution.get_normalized_sum_of_all_values() <= std::numeric_limits<float>::epsilon()) {
            return 1.0f / static_cast<float>(width * height);
        }

        auto discrete_sample_x = std::clamp<size_t>(static_cast<size_t>(sample.x * static_cast<float>(width)), 0, width - 1);
        auto discrete_sample_y = std::clamp<size_t>(static_cast<size_t>(sample.y * static_cast<float>(height)), 0, height - 1);

        FLUID_ASSERT(discrete_sample_y < conditional_distributions.size());
        FLUID_ASSERT(conditional_distributions[discrete_sample_y].get_values().size() < discrete_sample_x);
        FLUID_ASSERT(marginal_distribution.get_normalized_sum_of_all_values() > 0.0f);

        return conditional_distributions[discrete_sample_y].get_values()[discrete_sample_x] / marginal_distribution.get_normalized_sum_of_all_values();
    }

    void PiecewiseConstantDistribution2D::calculate_values() {
        // create conditional distributions out of the rows
        for (size_t y = 0; y < height; y++) {
            auto row_start = values.begin() + y * width;
            auto row_end = row_start + width;

            // get row as vector
            std::vector<float> row(row_start, row_end);

            // add piecewise constant distribution for row to conditional distributions
            conditional_distributions.emplace_back(PiecewiseConstantDistribution(row));
        }


        // create marginal distribution
        std::vector<float> marginal_values;
        marginal_values.reserve(height); // reserve space to avoid unnecessary allocations
        for (const auto& conditional_distribution : conditional_distributions) {
            marginal_values.push_back(conditional_distribution.get_normalized_sum_of_all_values());
        }
        marginal_distribution = PiecewiseConstantDistribution(marginal_values);
    }

    PiecewiseConstantDistribution2D::PiecewiseConstantDistribution2D() = default;
} // namespace LibFluid::Raytracer::Distributions