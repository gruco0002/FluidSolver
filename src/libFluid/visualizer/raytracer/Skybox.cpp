#include "Skybox.hpp"

#include "LibFluidAssert.hpp"
#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer {

    inline glm::vec3 normalized_cartesian_from_polar_direction(const glm::vec2& polar) {
        // NOTE: This function is different compared to known math definitions as it
        // uses the y-axis as "up". Most definitions use the z-axis as "up".
        return {
                std::sin(polar.x) * std::cos(polar.y),
                std::cos(polar.x),
                std::sin(polar.x) * std::sin(polar.y)};
    }

    inline glm::vec2 polar_from_normalized_cartesian_direction(const glm::vec3& cartesian) {
        // NOTE: This function is different compared to known math definitions as it
        // uses the y-axis as "up". Most definitions use the z-axis as "up".
        auto theta = std::acos(cartesian.y);
        auto phi = std::atan2(cartesian.z, cartesian.x);
        phi = phi < 0.0f ? phi + (Math::PI * 2.0f) : phi;
        return {theta, phi};
    }

    LightValue Skybox::get_light_value_by_direction(const glm::vec3& normalized_direction) const {
        if (skybox_image.width() == 0 || skybox_image.height() == 0) {
            // return default color
            return LightValue(2.0f);
        }

        // map direction to pixel coordinate
        auto pixel_coordinate = direction_to_pixel_coordinate(normalized_direction);

        return get_interpolated_color(pixel_coordinate);
    }

    glm::vec2 Skybox::direction_to_pixel_coordinate(const glm::vec3& normalized_direction) const {
        // get spherical coordinates
        auto polar = polar_from_normalized_cartesian_direction(normalized_direction);
        auto theta = polar.x;
        auto phi = polar.y;

        // determine position on image
        auto uv = glm::vec2(phi * (1.0f / (2.0f * Math::PI)), theta * (1.0f / Math::PI));

        float x = uv.x * (float)skybox_image.width();
        float y = uv.y * (float)skybox_image.height();


        return {x, y};
    }

    LightValue Skybox::get_interpolated_color(const glm::vec2& position) const {
        // FIXME: check if the bilinear interpolation is implemented correctly

        // linear interpolation
        size_t x = std::floor(position.x);
        size_t y = std::floor(position.y);
        size_t x_up = std::ceil(position.x);
        size_t y_up = std::ceil(position.y);

        // correct for equal values
        if (x_up == x) {
            x_up += 1;
        }
        if (y_up == y) {
            y_up += 1;
        }

        // correct sample locations
        x = x % skybox_image.width();
        x_up = x_up % skybox_image.width();
        y = y % skybox_image.height();
        y_up = y_up % skybox_image.height();


        // calculate factors for samples
        float dump;

        float x_up_factor = std::modf(position.x, &dump);
        float y_up_factor = std::modf(position.y, &dump);
        float x_factor = 1.0f - x_up_factor;
        float y_factor = 1.0f - y_up_factor;

        // sample data
        LightValue result;
        {
            auto sample = skybox_image.get(x, y);
            sample.mul(x_factor * y_factor);
            result.add(sample);
        }
        {
            auto sample = skybox_image.get(x_up, y);
            sample.mul(x_up_factor * y_factor);
            result.add(sample);
        }
        {
            auto sample = skybox_image.get(x, y_up);
            sample.mul(x_factor * y_up_factor);
            result.add(sample);
        }
        {
            auto sample = skybox_image.get(x_up, y_up);
            sample.mul(x_up_factor * y_up_factor);
            result.add(sample);
        }

        return result;
    }


    void Skybox::prepare() {
        if (skybox_image.width() == 0 || skybox_image.height() == 0) {
            return;
        }

        std::vector<float> skybox_luminance_distribution_values;
        size_t size = skybox_image.width() * skybox_image.height();
        skybox_luminance_distribution_values.resize(size);

        // TODO: Box filter

        for (size_t i = 0; i < size; i++) {
            auto x = i % skybox_image.width();
            auto y = i / skybox_image.width();

            FLUID_ASSERT(y < skybox_image.height());

            auto radiance_value = skybox_image.get(x, y);
            auto luminance = glm::dot(glm::vec3(radiance_value.r, radiance_value.g, radiance_value.b), glm::vec3(0.299f, 0.587f, 0.144f));

            // TODO: explain with comment
            float theta = Math::PI * (static_cast<float>(y) + 0.5f) / static_cast<float>(skybox_image.height());
            float sinus_theta = std::sin(theta);

            skybox_luminance_distribution_values[i] = luminance * sinus_theta;
        }

        skybox_image_distribution = Distributions::PiecewiseConstantDistribution2D(skybox_luminance_distribution_values, skybox_image.width(), skybox_image.height());
    }


    glm::vec3 Skybox::sample_normalized_direction(const glm::vec2& uniform_dist_random_sample_pair, float& pdf) const {
        float uv_sample_pdf;
        auto uv_sample = skybox_image_distribution.get_sample(uniform_dist_random_sample_pair, uv_sample_pdf);

        auto phi = uv_sample.x * 2.0f * Math::PI;
        auto theta = uv_sample.y * Math::PI;

        float sinus_theta = std::sin(theta);

        if (Math::abs(sinus_theta) <= std::numeric_limits<float>::epsilon()) {
            pdf = 0.0f;
        } else {
            // TODO: explain with a comment
            pdf = uv_sample_pdf / (2.0f * Math::pow2(Math::PI) * sinus_theta);
        }

        auto cartesian = normalized_cartesian_from_polar_direction({theta, phi});
        return cartesian;
    }

} // namespace LibFluid::Raytracer