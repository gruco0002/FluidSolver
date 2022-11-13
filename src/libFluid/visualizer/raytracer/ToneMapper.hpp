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

        static LightValue clamp_light_value(const LightValue& value);

        float map_color_to_gamma_corrected_color(float color) const;

        LightValue map_light_value_to_gamma_corrected_light_value(const LightValue& value) const;

        static uint8_t map_float_color_to_byte_color(float color);

        LightValue map_light_value_to_clamped_light_value_using_tone_mapping(LightValue value) const;

        Image::Color map_light_value_to_image_color(const LightValue& value) const;

      private:
        static float map_single_channel_with_filmic_tone_mapping(float hdr_value);

        static float map_single_channel_with_exponential_tone_mapping(float hdr_value);
    };
} // namespace FluidSolver::Raytracer