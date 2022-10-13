#include "VisualizerSerializer.hpp"

#include "serialization/extensions/VisualizerSerializerExtension.hpp"
#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"
#include "visualizer/ContinousVisualizer.hpp"

namespace LibFluid::Serialization {

    nlohmann::json VisualizerSerializer::serialize(std::shared_ptr<ISimulationVisualizer> visualizer) {
        if (visualizer == nullptr) {
            nlohmann::json node;
            node["type"] = "no-visualizer";
            return node;
        } else if (dynamic_pointer_is<ContinousVisualizer>(visualizer)) {
            return serialize_continuous_visualizer(visualizer);
        }

        // query the extensions
        for (const auto& ext : serializer_extensions().visualizer_serializer_extensions) {
            if (ext->can_serialize(visualizer)) {
                return ext->serialize_visualizer(visualizer, context());
            }
        }

        context().add_issue("Encountered unknown visualizer type!");

        return {};
    }
    nlohmann::json VisualizerSerializer::serialize_continuous_visualizer(std::shared_ptr<ISimulationVisualizer> visualizer) {
        auto r = std::dynamic_pointer_cast<ContinousVisualizer>(visualizer);
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

        // custom parameters for the continuous visualizer
        node["settings"]["background"] = r->settings.clear_color;
        node["settings"]["minimum-density"] = r->settings.minimum_render_density;

        return node;
    }
} // namespace LibFluid::Serialization