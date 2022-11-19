#pragma once

#include "visualizer/Image.hpp"
#include "visualizer/raytracer/RenderTarget.hpp"

namespace LibFluid::Raytracer {
    class ToneMapper {
      public:
        struct ToneMapperSettings {
            float exposure = 0.2f;

            float gamma = 2.2f;
            bool gamma_correction_enabled = true;

            enum class ToneMapperFunction {
                Exponential,
                Filmic
            } tone_mapper_function = ToneMapperFunction::Exponential;
        } settings;

        void map_render_target_to_image(const RenderTarget& render_target, Image& image);

      private:
        static float clamp(float value);

        static glm::vec3 clamp_radiance(const glm::vec3& value);

        float map_color_to_gamma_corrected_color(float color) const;

        glm::vec3 map_radiance_to_gamma_corrected_radiance(const glm::vec3& value) const;

        static uint8_t map_float_color_to_byte_color(float color);

        glm::vec3 map_radiance_to_clamped_radiance_using_tone_mapping(glm::vec3 value) const;

        Image::Color map_radiance_to_image_color(const glm::vec3& value) const;

      private:
        static float map_single_channel_with_filmic_tone_mapping(float hdr_value);

        static float map_single_channel_with_exponential_tone_mapping(float hdr_value);
    };
} // namespace LibFluid::Raytracer