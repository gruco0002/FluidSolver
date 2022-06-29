#include "ToneMapper.hpp"

#include "FluidAssert.hpp"

namespace FluidSolver::Raytracer {

    float ToneMapper::map_single_channel_with_filmic_tone_mapping(float hdr_value) {
        // ACES tonemapping curve
        // https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
        // http://duikerresearch.com/2015/09/filmic-tonemapping-ea-2006/
        // http://duikerresearch.com/2015/09/filmic-tonemapping-for-real-time-rendering/
        // http://filmicworlds.com/blog/filmic-tonemapping-with-piecewise-power-curves/
        float a = 2.51f;
        float b = 0.03f;
        float c = 2.43f;
        float d = 0.59f;
        float e = 0.14f;
        return clamp((hdr_value * (a * hdr_value + b)) /
                (hdr_value * (c * hdr_value + d) + e));
    }

    float ToneMapper::clamp(float value) {
        if (value < 0.0f)
            return 0.0f;
        if (value > 1.0f)
            return 1.0f;
        return value;
    }

    float ToneMapper::map_color_to_gamma_corrected_color(float color) const {
        float gamma_power_term = 1.0f / settings.gamma;
        return powf(color, gamma_power_term);
    }

    uint8_t ToneMapper::map_float_color_to_byte_color(float color) {
        return (uint8_t)(clamp(color) * 255.0f);
    }

    float ToneMapper::map_single_channel_with_exponential_tone_mapping(float hdr_value) {
        return 1.0f - std::exp(-hdr_value);
    }

    LightValue ToneMapper::map_light_value_to_clamped_light_value_using_tone_mapping(LightValue value) const {
        switch (settings.tone_mapper_function) {
            case ToneMapperSettings::ToneMapperFunction::Exponential: {
                LightValue new_value;

                new_value.r = map_single_channel_with_exponential_tone_mapping(value.r);
                new_value.g = map_single_channel_with_exponential_tone_mapping(value.g);
                new_value.b = map_single_channel_with_exponential_tone_mapping(value.b);

                return clamp_light_value(new_value);
            }
            case ToneMapperSettings::ToneMapperFunction::Filmic: {
                LightValue new_value;

                new_value.r = map_single_channel_with_filmic_tone_mapping(value.r);
                new_value.g = map_single_channel_with_filmic_tone_mapping(value.g);
                new_value.b = map_single_channel_with_filmic_tone_mapping(value.b);

                return clamp_light_value(new_value);
            }
        }
    }
    LightValue ToneMapper::clamp_light_value(const LightValue& value) {
        return {clamp(value.r), clamp(value.g), clamp(value.b)};
    }

    void ToneMapper::map_render_target_to_image(const RenderTarget& render_target, Image& image) {
        FLUID_ASSERT(render_target.get_width() == image.width(), "Render target and output image dimensions have to match!");
        FLUID_ASSERT(render_target.get_height() == image.height(), "Render target and output image dimensions have to match!");

        for (size_t y = 0; y < render_target.get_height(); y++) {
            for (size_t x = 0; x < render_target.get_width(); x++) {
                image.set(x, y, map_light_value_to_image_color(render_target.get(x, y)));
            }
        }
    }

    Image::Color ToneMapper::map_light_value_to_image_color(const LightValue& value) const {
        LightValue res = value;

        // apply exposure
        res.mul(settings.exposure);

        // apply tone mapping
        res = map_light_value_to_clamped_light_value_using_tone_mapping(res);

        // apply gamma correction if activated
        if (settings.gamma_correction_enabled) {
            res = map_light_value_to_gamma_corrected_light_value(res);
        }

        // switch to byte representation
        Image::Color c;
        c.r = map_float_color_to_byte_color(res.r);
        c.g = map_float_color_to_byte_color(res.g);
        c.b = map_float_color_to_byte_color(res.b);
        c.a = 255;

        return c;
    }
    LightValue ToneMapper::map_light_value_to_gamma_corrected_light_value(const LightValue& value) const {
        return {map_color_to_gamma_corrected_color(value.r), map_color_to_gamma_corrected_color(value.g), map_color_to_gamma_corrected_color(value.b)};
    }


} // namespace FluidSolver::Raytracer