#include "VseGlParticleRenderer.hpp"

#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"
#include "visualizer/GLParticleRenderer.hpp"

namespace FluidStudio
{

    bool VseGlParticleRenderer::can_serialize(const std::shared_ptr<LibFluid::ISimulationVisualizer> &visualizer) const
    {
        return LibFluid::Serialization::dynamic_pointer_is<LibFluid::GLParticleRenderer>(visualizer);
    }
    nlohmann::json VseGlParticleRenderer::serialize_visualizer(
        const std::shared_ptr<LibFluid::ISimulationVisualizer> &visualizer,
        LibFluid::Serialization::SerializationContext &context) const
    {
        auto r = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(visualizer);
        FLUID_ASSERT(r != nullptr, "Called with invalid subtype of visualizer!");

        nlohmann::json node;

        node["type"] = "gl-particle-renderer";

        // default parameters
        node["viewport"]["left"] = r->settings.viewport.left;
        node["viewport"]["right"] = r->settings.viewport.right;
        node["viewport"]["top"] = r->settings.viewport.top;
        node["viewport"]["bottom"] = r->settings.viewport.bottom;
        node["render-target"]["width"] = r->parameters.render_target.width;
        node["render-target"]["height"] = r->parameters.render_target.height;
        node["enabled"] = r->parameters.enabled;

        // custom parameters for the particle renderer
        node["settings"]["top"]["value"] = r->settings.topValue;
        node["settings"]["top"]["color"] = r->settings.topColor;
        node["settings"]["bottom"]["value"] = r->settings.bottomValue;
        node["settings"]["bottom"]["color"] = r->settings.bottomColor;
        node["settings"]["value-selection"] = (int)r->settings.colorSelection;
        node["settings"]["colors"]["boundary"] = r->settings.boundaryParticleColor;
        node["settings"]["colors"]["background"] = r->settings.backgroundClearColor;
        node["settings"]["show-memory-location"] = r->settings.showMemoryLocation;

        return node;
    }

    bool VseGlParticleRenderer::can_deserialize(const nlohmann::json &node) const
    {
        auto type = node["type"].get<std::string>();
        return type == "gl-particle-renderer";
    }
    std::shared_ptr<LibFluid::ISimulationVisualizer> VseGlParticleRenderer::deserialize_visualizer(
        const nlohmann::json &node, LibFluid::Serialization::SerializationContext &context) const
    {
        if (!LibFluid::GLRenderer::is_opengl_available())
        {
            context.add_issue("Visualizer is not supported in this context. OpenGL was not initialized but visualizer "
                              "requires OpenGL!");
            return nullptr;
        }

        auto r = std::make_shared<LibFluid::GLParticleRenderer>();

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

        // custom paramters for the particle renderer
        r->settings.topValue = node["settings"]["top"]["value"].get<float>();
        r->settings.topColor = node["settings"]["top"]["color"].get<glm::vec4>();
        r->settings.bottomValue = node["settings"]["bottom"]["value"].get<float>();
        r->settings.bottomColor = node["settings"]["bottom"]["color"].get<glm::vec4>();
        r->settings.colorSelection =
            (LibFluid::GLParticleRenderer::Settings::ColorSelection)node["settings"]["value-selection"].get<int>();
        r->settings.boundaryParticleColor = node["settings"]["colors"]["boundary"].get<glm::vec4>();
        r->settings.backgroundClearColor = node["settings"]["colors"]["background"].get<glm::vec4>();
        r->settings.showMemoryLocation = node["settings"]["show-memory-location"].get<bool>();

        return r;
    }
} // namespace FluidStudio
