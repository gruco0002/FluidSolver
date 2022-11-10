#include "Skybox.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer {

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
        // FIXME: check if this is the correct transformation (Mercator Projection)

        // get spherical coordinates
        float phi = glm::acos(normalized_direction.z);
        float theta = glm::atan(normalized_direction.y);

        // convert to image coordinates
        float x = phi * (float)skybox_image.width() / (2.0f * LibFluid::Math::PI);
        float y = (glm::log(glm::tan(LibFluid::Math::PI / 4.0f + theta / 2.0f))) * (float)skybox_image.height() / (2.0f * LibFluid::Math::PI);

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

} // namespace LibFluid::Raytracer