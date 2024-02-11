#pragma once

#include "visualizer/raytracer/distributions/PiecewiseConstantDistribution.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace LibFluid::Raytracer::Distributions
{
    class PiecewiseConstantDistribution2D
    {
        // Based on the book 'Physically Based Rendering: From Theory To Implementation' [https://www.pbr-book.org/]
        // by Matt Pharr, Wenzel Jakob, and Greg Humphreys
        // on section 13.6.7 Piecewise-Constant 2D Distributions

      public:
        PiecewiseConstantDistribution2D();

        PiecewiseConstantDistribution2D(std::vector<float> values, size_t width, size_t height);

        // samples a normalized index pair [0, 1)x[0, 1) according to the distribution of the provided values
        glm::vec2 get_sample(const glm::vec2 &uniform_dist_random_sample_pair, float &pdf_of_sample) const;

        float get_pdf_for_sample(const glm::vec2 &sample) const;

      private:
        std::vector<float> values;
        size_t width = 0;
        size_t height = 0;

        void calculate_values();

        std::vector<PiecewiseConstantDistribution> conditional_distributions;
        PiecewiseConstantDistribution marginal_distribution;
    };
} // namespace LibFluid::Raytracer::Distributions
