#include "VisualizerSerializer.hpp"

#include "serialization/extensions/VisualizerSerializerExtension.hpp"
#include "serialization/helpers/Base64.hpp"
#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"
#include "visualizer/ContinuousVisualizer.hpp"
#include "visualizer/raytracer/FluidRaytracer3D.hpp"
#include "visualizer/raytracer/SimpleRaytracer.hpp"

namespace LibFluid::Serialization
{

    nlohmann::json VisualizerSerializer::serialize(std::shared_ptr<ISimulationVisualizer> visualizer)
    {
        if (visualizer == nullptr)
        {
            nlohmann::json node;
            node["type"] = "no-visualizer";
            return node;
        }
        else if (dynamic_pointer_is<ContinuousVisualizer>(visualizer))
        {
            return serialize_continuous_visualizer(visualizer);
        }
        else if (dynamic_pointer_is<Raytracer::FluidRaytracer3D>(visualizer))
        {
            return serialize_raytracer(visualizer);
        }
        else if (dynamic_pointer_is<Raytracer::SimpleRaytracer>(visualizer))
        {
            return serialize_simple_raytracer(visualizer);
        }

        // query the extensions
        for (const auto &ext : serializer_extensions().visualizer_serializer_extensions)
        {
            if (ext->can_serialize(visualizer))
            {
                return ext->serialize_visualizer(visualizer, context());
            }
        }

        context().add_issue("Encountered unknown visualizer type!");

        return {};
    }

    std::shared_ptr<ISimulationVisualizer> VisualizerSerializer::deserialize(const nlohmann::json &node)
    {
        auto type = node["type"].get<std::string>();

        if (type == "no-visualizer")
        {
            return nullptr;
        }
        else if (type == "continuous")
        {
            return deserialize_continuous_visualizer(node);
        }
        else if (type == "raytracer")
        {
            return deserialize_raytracer(node);
        }
        else if (type == "simple-raytracer")
        {
            return deserialize_simple_raytracer(node);
        }

        // query the extensions
        for (const auto &ext : serializer_extensions().visualizer_serializer_extensions)
        {
            if (ext->can_deserialize(node))
            {
                return ext->deserialize_visualizer(node, context());
            }
        }

        context().add_issue("Encountered unknown visualizer type!");
        return nullptr;
    }

    nlohmann::json VisualizerSerializer::serialize_continuous_visualizer(
        std::shared_ptr<ISimulationVisualizer> visualizer)
    {
        auto r = std::dynamic_pointer_cast<ContinuousVisualizer>(visualizer);
        FLUID_ASSERT(r != nullptr, "Passed invalid visualizer type!");

        nlohmann::json node;

        node["type"] = "continuous";

        // default parameters
        node["viewport"]["left"] = r->settings.viewport.left;
        node["viewport"]["right"] = r->settings.viewport.right;
        node["viewport"]["top"] = r->settings.viewport.top;
        node["viewport"]["bottom"] = r->settings.viewport.bottom;
        node["render-target"]["width"] = r->parameters.render_target.width;
        node["render-target"]["height"] = r->parameters.render_target.height;
        node["enabled"] = r->parameters.enabled;

        // custom parameters for the continuous visualizer
        node["settings"]["background"] = r->settings.clear_color;
        node["settings"]["minimum-density"] = r->settings.minimum_render_density;

        return node;
    }

    std::shared_ptr<ISimulationVisualizer> VisualizerSerializer::deserialize_continuous_visualizer(
        const nlohmann::json &node)
    {
        auto r = std::make_shared<ContinuousVisualizer>();

        // default parameters
        r->settings.viewport.left = node["viewport"]["left"].get<float>();
        r->settings.viewport.right = node["viewport"]["right"].get<float>();
        r->settings.viewport.top = node["viewport"]["top"].get<float>();
        r->settings.viewport.bottom = node["viewport"]["bottom"].get<float>();
        r->parameters.render_target.width = node["render-target"]["width"].get<size_t>();
        r->parameters.render_target.height = node["render-target"]["height"].get<size_t>();
        if (node.contains("enabled"))
        {
            r->parameters.enabled = node["enabled"].get<bool>();
        }
        else
        {
            r->parameters.enabled = true;
        }

        // custom parameters for the continuous visualizer
        r->settings.clear_color = node["settings"]["background"].get<Image::Color>();
        r->settings.minimum_render_density = node["settings"]["minimum-density"].get<float>();

        return r;
    }

    nlohmann::json VisualizerSerializer::serialize_raytracer(std::shared_ptr<ISimulationVisualizer> visualizer)
    {
        auto r = std::dynamic_pointer_cast<LibFluid::Raytracer::FluidRaytracer3D>(visualizer);
        FLUID_ASSERT(r != nullptr, "Called with invalid subtype of visualizer!");

        nlohmann::json node;

        node["type"] = "raytracer";

        // default parameters
        node["render-target"]["width"] = r->parameters.render_target.width;
        node["render-target"]["height"] = r->parameters.render_target.height;
        node["enabled"] = r->parameters.enabled;

        // custom parameters for the raytracer
        node["settings"]["camera"]["location"] = r->camera.settings.position;
        node["settings"]["camera"]["up"] = r->camera.settings.view_up;
        node["settings"]["camera"]["direction"] = r->camera.settings.view_direction;
        node["settings"]["camera"]["field-of-view-x"] = r->camera.settings.field_of_view_x;
        node["settings"]["camera"]["flip-y"] = r->camera.settings.flip_y;
        node["settings"]["camera"]["samples-per-pixel"] = r->camera.sample_settings.amount_of_samples;

        node["settings"]["tone-mapping"]["exposure"] = r->tone_mapper.settings.exposure;
        node["settings"]["tone-mapping"]["gamma"] = r->tone_mapper.settings.gamma;
        node["settings"]["tone-mapping"]["gamma-correction-enabled"] = r->tone_mapper.settings.gamma_correction_enabled;

        switch (r->tone_mapper.settings.tone_mapper_function)
        {
        case Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Exponential:
            node["settings"]["tone-mapping"]["function"] = "exponential";
            break;
        case Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Filmic:
            node["settings"]["tone-mapping"]["function"] = "filmic";
            break;
        }

        node["settings"]["surface"]["fraction-of-rest-density"] =
            r->accelerator.surface_density_as_fraction_of_rest_density;

        if (r->skybox.skybox_image.width() == 0 || r->skybox.skybox_image.height() == 0)
        {
            // empty skybox -> create null object
            node["settings"]["skybox"]["image"] = nlohmann::json();
        }
        else
        {
            // serialize the hdr image to base64
            auto skybox_image_data = r->skybox.skybox_image.get_as_hdr_file_data();
            auto skybox_image_data_as_base_64_string = Base64::encode_to_base_64(skybox_image_data);
            node["settings"]["skybox"]["image"] = skybox_image_data_as_base_64_string;
        }

        node["settings"]["debug"]["output-normals"] = r->settings.output_normals_of_first_hit;

        return node;
    }

    std::shared_ptr<ISimulationVisualizer> VisualizerSerializer::deserialize_raytracer(const nlohmann::json &node)
    {
        auto r = std::make_shared<Raytracer::FluidRaytracer3D>();

        // default data
        r->parameters.render_target.width = node["render-target"]["width"].get<size_t>();
        r->parameters.render_target.height = node["render-target"]["height"].get<size_t>();
        if (node.contains("enabled"))
        {
            r->parameters.enabled = node["enabled"].get<bool>();
        }
        else
        {
            r->parameters.enabled = true;
        }

        // custom parameters for the raytracer
        r->camera.settings.position = node["settings"]["camera"]["location"].get<glm::vec3>();
        r->camera.settings.view_up = node["settings"]["camera"]["up"].get<glm::vec3>();
        r->camera.settings.view_direction = node["settings"]["camera"]["direction"].get<glm::vec3>();
        r->camera.settings.field_of_view_x = node["settings"]["camera"]["field-of-view-x"].get<float>();
        r->camera.settings.flip_y = node["settings"]["camera"]["flip-y"].get<bool>();
        r->camera.sample_settings.amount_of_samples = node["settings"]["camera"]["samples-per-pixel"].get<size_t>();

        r->tone_mapper.settings.exposure = node["settings"]["tone-mapping"]["exposure"].get<float>();
        r->tone_mapper.settings.gamma = node["settings"]["tone-mapping"]["gamma"].get<float>();
        r->tone_mapper.settings.gamma_correction_enabled =
            node["settings"]["tone-mapping"]["gamma-correction-enabled"].get<bool>();

        auto tone_mapping_function = node["settings"]["tone-mapping"]["function"].get<std::string>();
        if (tone_mapping_function == "exponential")
        {
            r->tone_mapper.settings.tone_mapper_function =
                Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Exponential;
        }
        else if (tone_mapping_function == "filmic")
        {
            r->tone_mapper.settings.tone_mapper_function =
                Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Filmic;
        }
        else
        {
            // unhandled
            context().add_issue("Unhandled tone mapping function: " + tone_mapping_function);
        }

        r->accelerator.surface_density_as_fraction_of_rest_density =
            node["settings"]["surface"]["fraction-of-rest-density"].get<float>();

        if (!node["settings"]["skybox"]["image"].is_null())
        {
            // deserialize the base64 encoded hdr image file
            auto skybox_image_data_as_base_64_string = node["settings"]["skybox"]["image"].get<std::string>();
            auto skybox_image_data = Base64::decode_from_base_64(skybox_image_data_as_base_64_string);
            r->skybox.skybox_image.load_from_hdr_file_data(skybox_image_data);
        }

        if (node.contains("/settings/debug/output-normals"_json_pointer))
        {
            r->settings.output_normals_of_first_hit = node["settings"]["debug"]["output-normals"].get<bool>();
        }
        else
        {
            r->settings.output_normals_of_first_hit = false;
        }

        return r;
    }

    nlohmann::json VisualizerSerializer::serialize_simple_raytracer(std::shared_ptr<ISimulationVisualizer> visualizer)
    {
        auto r = std::dynamic_pointer_cast<LibFluid::Raytracer::SimpleRaytracer>(visualizer);
        FLUID_ASSERT(r != nullptr, "Called with invalid subtype of visualizer!");

        nlohmann::json node;

        node["type"] = "simple-raytracer";

        // default parameters
        node["render-target"]["width"] = r->parameters.render_target.width;
        node["render-target"]["height"] = r->parameters.render_target.height;
        node["enabled"] = r->parameters.enabled;

        // custom parameters for the raytracer
        node["settings"]["camera"]["location"] = r->camera.settings.position;
        node["settings"]["camera"]["up"] = r->camera.settings.view_up;
        node["settings"]["camera"]["direction"] = r->camera.settings.view_direction;
        node["settings"]["camera"]["field-of-view-x"] = r->camera.settings.field_of_view_x;
        node["settings"]["camera"]["flip-y"] = r->camera.settings.flip_y;

        node["settings"]["tone-mapping"]["exposure"] = r->tone_mapper.settings.exposure;
        node["settings"]["tone-mapping"]["gamma"] = r->tone_mapper.settings.gamma;
        node["settings"]["tone-mapping"]["gamma-correction-enabled"] = r->tone_mapper.settings.gamma_correction_enabled;

        switch (r->tone_mapper.settings.tone_mapper_function)
        {
        case Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Exponential:
            node["settings"]["tone-mapping"]["function"] = "exponential";
            break;
        case Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Filmic:
            node["settings"]["tone-mapping"]["function"] = "filmic";
            break;
        }

        node["settings"]["surface"]["fraction-of-rest-density"] =
            r->accelerator.surface_density_as_fraction_of_rest_density;

        node["settings"]["fluid-color"] = r->settings.fluid_color;
        node["settings"]["boundary-color"] = r->settings.boundary_color;
        node["settings"]["background-color"] = r->settings.background_color;
        node["settings"]["ambient-strength"] = r->settings.ambient_strength;
        node["settings"]["light-direction"] = r->settings.light_direction;

        if (r->settings.output == Raytracer::SimpleRaytracer::Settings::Output::Normal)
        {
            node["settings"]["output-mode"] = "normal";
        }
        else
        {
            node["settings"]["output-mode"] = "color";
        }

        return node;
    }

    std::shared_ptr<ISimulationVisualizer> VisualizerSerializer::deserialize_simple_raytracer(
        const nlohmann::json &node)
    {
        auto r = std::make_shared<Raytracer::SimpleRaytracer>();

        // default data
        r->parameters.render_target.width = node["render-target"]["width"].get<size_t>();
        r->parameters.render_target.height = node["render-target"]["height"].get<size_t>();
        if (node.contains("enabled"))
        {
            r->parameters.enabled = node["enabled"].get<bool>();
        }
        else
        {
            r->parameters.enabled = true;
        }

        // custom parameters for the raytracer
        r->camera.settings.position = node["settings"]["camera"]["location"].get<glm::vec3>();
        r->camera.settings.view_up = node["settings"]["camera"]["up"].get<glm::vec3>();
        r->camera.settings.view_direction = node["settings"]["camera"]["direction"].get<glm::vec3>();
        r->camera.settings.field_of_view_x = node["settings"]["camera"]["field-of-view-x"].get<float>();
        r->camera.settings.flip_y = node["settings"]["camera"]["flip-y"].get<bool>();
        r->camera.sample_settings.amount_of_samples = 1;

        r->tone_mapper.settings.exposure = node["settings"]["tone-mapping"]["exposure"].get<float>();
        r->tone_mapper.settings.gamma = node["settings"]["tone-mapping"]["gamma"].get<float>();
        r->tone_mapper.settings.gamma_correction_enabled =
            node["settings"]["tone-mapping"]["gamma-correction-enabled"].get<bool>();

        auto tone_mapping_function = node["settings"]["tone-mapping"]["function"].get<std::string>();
        if (tone_mapping_function == "exponential")
        {
            r->tone_mapper.settings.tone_mapper_function =
                Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Exponential;
        }
        else if (tone_mapping_function == "filmic")
        {
            r->tone_mapper.settings.tone_mapper_function =
                Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Filmic;
        }
        else
        {
            // unhandled
            context().add_issue("Unhandled tone mapping function: " + tone_mapping_function);
        }

        r->accelerator.surface_density_as_fraction_of_rest_density =
            node["settings"]["surface"]["fraction-of-rest-density"].get<float>();

        r->settings.fluid_color = node["settings"]["fluid-color"].get<glm::vec3>();
        r->settings.boundary_color = node["settings"]["boundary-color"].get<glm::vec3>();
        r->settings.background_color = node["settings"]["background-color"].get<glm::vec3>();
        r->settings.ambient_strength = node["settings"]["ambient-strength"].get<float>();
        r->settings.light_direction = node["settings"]["light-direction"].get<glm::vec3>();

        auto mode_str = node["settings"]["output-mode"].get<std::string>();
        if (mode_str == "normal")
        {
            r->settings.output = Raytracer::SimpleRaytracer::Settings::Output::Normal;
        }
        else
        {
            r->settings.output = Raytracer::SimpleRaytracer::Settings::Output::Color;
        }

        return r;
    }
} // namespace LibFluid::Serialization
