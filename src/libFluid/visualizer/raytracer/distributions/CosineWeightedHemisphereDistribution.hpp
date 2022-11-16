#pragma once

#include "visualizer/raytracer/Ray.hpp"

#include <glm/glm.hpp>

namespace LibFluid::Raytracer::Distributions::CosineWeightedHemisphereDistribution {

    Ray sample_ray(const glm::vec2& uniform_dist_samples, const glm::vec3& normalized_up);

}
